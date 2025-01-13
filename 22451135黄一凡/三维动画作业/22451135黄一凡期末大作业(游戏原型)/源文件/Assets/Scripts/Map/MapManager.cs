using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UIElements;

public class MapManager : SingletonMono<MapManager>
{
    #region 运行时的变量
    [SerializeField] private MeshCollider meshCollider;
    private MapGenerator mapGenerator;
    private Transform viewer;            // 观察者
    private Vector3 lastViewerPos = Vector3.one * -1;
    private Dictionary<Vector2Int, MapChunkController> mapChunkDic;  // 玩家已有的地图块
    private float updateChunkTime = 1.0f;
    private bool canUpdateChunk = true;
    private float mapSizeOnWorld;       // 在地图中实际的地图整体尺寸
    private float chunkSizeOnWorld;     // 在世界中实际的地图块尺寸 单位米
    private List<MapChunkController> lastVisibleChunkList = new();
    #endregion

    #region 配置
    private MapConfig mapConfig;  // 地图配置
    public MapConfig MapConfig => mapConfig;

    private Dictionary<MapVertexType, List<int>> spawnMapObjectConfigDic;
    private Dictionary<MapVertexType, List<int>> spawnAIConfigDic;
    #endregion

    #region 存档
    private MapInitData mapInitData;
    private MapData mapData;
    #endregion

    #region 导航相关
    [SerializeField] private NavMeshSurface navMeshSurface;
    public void BakeNavMesh()
    {
        navMeshSurface.BuildNavMesh();
    }
    #endregion
    
    public void Init()
    {
        StartCoroutine(DoInit());
        EventManager.AddEventListener(EventName.SaveGame, OnGameSave);
    }
    
    private IEnumerator DoInit()
    {
        // 确定存档
        mapInitData = ArchiveManager.Instance.MapInitData;
        mapData = ArchiveManager.Instance.MapData;
        
        // 确定配置
        mapConfig = ConfigManager.Instance.GetConfig<MapConfig>(ConfigName.Map);
        Dictionary<int, ConfigBase> tempDic = ConfigManager.Instance.GetConfigs(ConfigName.MapObject);
        spawnMapObjectConfigDic = new();
        spawnMapObjectConfigDic.Add(MapVertexType.Forest, new ());
        spawnMapObjectConfigDic.Add(MapVertexType.Marsh, new ());
        foreach (var item in tempDic)
        {
            MapVertexType mapVertexType = (item.Value as MapObjectConfig).MapVertexType;
            if (mapVertexType == MapVertexType.None) continue;
            spawnMapObjectConfigDic[mapVertexType].Add(item.Key);
        }
        
        tempDic = ConfigManager.Instance.GetConfigs(ConfigName.AI);
        spawnAIConfigDic = new();
        spawnAIConfigDic.Add(MapVertexType.Forest, new ());
        spawnAIConfigDic.Add(MapVertexType.Marsh, new ());
        foreach (var item in tempDic)
        {
            MapVertexType mapVertexType = (item.Value as AIConfig).MapVertexType;
            if (mapVertexType == MapVertexType.None) continue;
            spawnAIConfigDic[mapVertexType].Add(item.Key);
        }

        mapGenerator = new MapGenerator(mapConfig, mapInitData, mapData, spawnMapObjectConfigDic, spawnAIConfigDic);
        mapGenerator.GenerateMapData();
        mapChunkDic = new();
        chunkSizeOnWorld = mapConfig.mapChunkSize * mapConfig.cellSize;
        mapSizeOnWorld = chunkSizeOnWorld * mapInitData.mapSize;
        // 生成地面碰撞体的网格
        meshCollider.sharedMesh = GenerateGroundMesh(mapSizeOnWorld, mapSizeOnWorld);
        // 烘焙导航网格
        BakeNavMesh();
        int mapChunkCount = mapData.MapChunkIndexList.Count;
        // 已经有存档了
        if (mapChunkCount > 0)
        {
            GameSceneManager.Instance.SetProgressMapChunkCount(mapChunkCount);
            // 根据存档去恢复整个地图的状态
            for (int i = 0; i < mapChunkCount; i++)
            {
                Serialization_Vector2 chunkIndex = mapData.MapChunkIndexList[i];
                MapChunkData chunkData = ArchiveManager.Instance.GetMapChunkData(chunkIndex);
                GenerateMapChunk(chunkIndex.ConvertToVector2Int(), chunkData).gameObject.SetActive(false);
                for (int j = 0; j < 5; j++) yield return null;
            }
        }
        // 新存档
        else
        {
            GameSceneManager.Instance.SetProgressMapChunkCount(GetMapChunkCountOnGameInit());
            // 当前观察者所在的地图块
            Vector2Int currChunkIndex = GetMapChunkIndexByWorldPosition(viewer.position);
            int startX = currChunkIndex.x - mapConfig.viewDistance;
            int startY = currChunkIndex.y - mapConfig.viewDistance;
            // 开启需要显示的地图块
            for (int x = 0; x < 2*mapConfig.viewDistance+1; x++)
            {
                for (int y = 0; y < 2*mapConfig.viewDistance+1; y++)
                {
                    Vector2Int chunkIndex = new Vector2Int(startX + x, startY + y);
                    GenerateMapChunk(chunkIndex, null);
                    for (int j = 0; j < 5; j++) yield return null;
                }
            }
        }
        DoUpdateVisibleChunk();
        // 打开并关闭一次地图UI，初始化
        ShowMapUI();
        CloseMapUI();
    }

    public void UpdateViewer(Transform player)
    {
        this.viewer = player;
    }
    
    private void Update()
    {
        if (GameSceneManager.Instance.IsInitialized == false) return;
        
        UpdateVisibleChunk();

        if (Input.GetKeyDown(KeyCode.M))
        {
            if (isShowMaping) CloseMapUI();
            else ShowMapUI();
            isShowMaping = !isShowMaping;
        }

        if (isShowMaping)
        {
            UpdateMapUI();
        }
    }

    private Mesh GenerateGroundMesh(float height, float width)
    {
        Mesh mesh = new Mesh();
        // 确定顶点在哪里
        mesh.vertices = new Vector3[]
        {
            new Vector3(0, 0, 0),
            new Vector3(0, 0, height),
            new Vector3(width, 0, height),
            new Vector3(width, 0, 0),
        };
        // 确定哪些点形成三角形
        mesh.triangles = new int[]
        {
            0, 1, 2,
            0, 2, 3
        };
        mesh.uv = new Vector2[]
        {
            new Vector2(0, 0),
            new Vector2(0, 1),
            new Vector2(1, 1),
            new Vector2(1, 0),
        };
        return mesh;
    }

    #region 地图块相关
    // 根据观察者的位置来刷新哪些地图块可以看到
    private void UpdateVisibleChunk()
    {
        // 如果观察者没有移动过，不需要刷新
        if (viewer.position == lastViewerPos) return;
        lastViewerPos = viewer.position;
        
        // 更新地图UI的坐标
        if (isShowMaping) mapUI.UpdatePivot(viewer.position);
        
        // 如果时间没到，不允许更新
        if (canUpdateChunk == false) return;

        DoUpdateVisibleChunk();
    }

    private void DoUpdateVisibleChunk()
    {
        // 当前观察者所在的地图块
        Vector2Int currChunkIndex = GetMapChunkIndexByWorldPosition(viewer.position);
        
        // 关闭全部不需要显示的地图块
        for (int i = lastVisibleChunkList.Count - 1; i >= 0; i--)
        {
            Vector2Int chunkIndex = lastVisibleChunkList[i].ChunkIndex;
            if (Mathf.Abs(chunkIndex.x - currChunkIndex.x) > mapConfig.viewDistance ||
                Mathf.Abs(chunkIndex.y - currChunkIndex.y) > mapConfig.viewDistance)
            {
                lastVisibleChunkList[i].SetActive(false);
                lastVisibleChunkList.RemoveAt(i);
            }
        }

        int startX = currChunkIndex.x - mapConfig.viewDistance;
        int startY = currChunkIndex.y - mapConfig.viewDistance;
        // 开启需要显示的地图块
        for (int x = 0; x < 2*mapConfig.viewDistance+1; x++)
        {
            for (int y = 0; y < 2*mapConfig.viewDistance+1; y++)
            {
                Vector2Int chunkIndex = new Vector2Int(startX + x, startY + y);
                // 在地图字典中，也就是之前加载过，但是不一定加载完成了，因为贴图会在协程中执行，执行完成后才算初始化完毕
                if (mapChunkDic.TryGetValue(chunkIndex, out MapChunkController chunk))
                {
                    // 上一次显示的地图列表中并不包含这个地图块且同时它已经完成了初始化
                    if (lastVisibleChunkList.Contains(chunk) == false && chunk.IsInitialized)
                    {
                        lastVisibleChunkList.Add(chunk);
                        chunk.SetActive(true);
                    }
                }
                // 之前没有加载
                else
                {
                    chunk = GenerateMapChunk(chunkIndex, null);
                }
            }
        }
        canUpdateChunk = false;
        Invoke(nameof(ResetCanUpdateChunkFlag), updateChunkTime);
    }

    /// <summary>
    /// 根据世界坐标获取地图块的索引
    /// </summary>
    private Vector2Int GetMapChunkIndexByWorldPosition(Vector3 worldPosition)
    {
        int x = Mathf.Clamp(Mathf.FloorToInt(worldPosition.x / chunkSizeOnWorld), 1, mapInitData.mapSize);
        int y = Mathf.Clamp(Mathf.FloorToInt(worldPosition.z / chunkSizeOnWorld), 1, mapInitData.mapSize);
        return new Vector2Int(x, y);
    }

    /// <summary>
    /// 根据世界坐标获取地图块
    /// </summary>
    public MapChunkController GetMapChunkByWorldPosition(Vector3 worldPosition)
    {
        return mapChunkDic[GetMapChunkIndexByWorldPosition(worldPosition)];
    }

    private int GetMapChunkCountOnGameInit()
    {
        // 当前观察者所在的地图块
        int res = 0;
        Vector2Int currChunkIndex = GetMapChunkIndexByWorldPosition(viewer.position);
        int startX = currChunkIndex.x - mapConfig.viewDistance;
        int startY = currChunkIndex.y - mapConfig.viewDistance;
        
        for (int x = 0; x < 2 * mapConfig.viewDistance + 1; x++)
        {
            for (int y = 0; y < 2 * mapConfig.viewDistance + 1; y++)
            {
                int indexX = startX + x;
                int indexY = startY + y;
                // 检查坐标的合法性
                if (indexX > mapInitData.mapSize - 1 || indexY > mapInitData.mapSize - 1) continue;
                if (indexX < 0 || indexY < 0) continue;
                res++;
            }
        }
        return res;
    }
    
    /// <summary>
    /// 生成地图块
    /// </summary>
    private MapChunkController GenerateMapChunk(Vector2Int index, MapChunkData mapChunkData = null)
    {
        // 检查坐标的合法性
        if (index.x > mapInitData.mapSize - 1 || index.y > mapInitData.mapSize - 1) return null;
        if (index.x < 0 || index.y < 0) return null;
        MapChunkController chunk =
            mapGenerator.GenerateMapChunk(index, transform, mapChunkData, () => mapUIUpdateChunkIndexList.Add(index));
        mapChunkDic.Add(index, chunk);
        return chunk;
    }

    private void ResetCanUpdateChunkFlag()
    {
        canUpdateChunk = true;
    }

    /// <summary>
    /// 为地图块刷新生成地图对象列表
    /// 可能为null
    /// </summary>
    public List<MapObjectData> SpawnMapObjectDataOnMapChunkRefresh(Vector2Int chunkIndex)
    {
        return mapGenerator.GenerateMapObjectDataListOnMapChunkRefresh(chunkIndex);
    }
    
    /// <summary>
    /// 为地图块刷新生成AI列表
    /// </summary>
    public List<MapObjectData> SpawnAIObjectDataOnMapChunkRefresh(MapChunkData mapChunkData)
    {
        return mapGenerator.GenerateAIObjectDataList(mapChunkData);
    }
    #endregion

    #region 地图UI相关
    private bool mapUIInitialized = false;
    private bool isShowMaping = false;
    private List<Vector2Int> mapUIUpdateChunkIndexList = new List<Vector2Int>();    // 待更新的列表
    private UI_MapWindow mapUI;
    
    // 显示地图UI
    private void ShowMapUI()
    {
        mapUI = UIManager.Instance.Show<UI_MapWindow>();
        if (!mapUIInitialized)
        {
            mapUI.InitMap(mapInitData.mapSize, mapConfig.mapChunkSize, mapSizeOnWorld, mapConfig.forestTexture);
            mapUIInitialized = true;
        }
        // 更新
        UpdateMapUI();
    }

    private void UpdateMapUI()
    {
        for (int i = 0; i < mapUIUpdateChunkIndexList.Count; i++)
        {
            Vector2Int chunkIndex = mapUIUpdateChunkIndexList[i];
            Texture2D texture = null;
            MapChunkController mapChunk = mapChunkDic[chunkIndex];
            if (mapChunkDic[chunkIndex].IsAllForest == false)
            {
                texture = (Texture2D)mapChunk.GetComponent<MeshRenderer>().material.mainTexture;
            }
            mapUI.AddMapChunk(chunkIndex,mapChunk.MapChunkData.MapObjectDic, texture);
        }
        mapUIUpdateChunkIndexList.Clear();
        mapUI.UpdatePivot(viewer.position);
    }
    
    // 关闭地图UI
    private void CloseMapUI()
    {
        UIManager.Instance.Close<UI_MapWindow>();
    }
    #endregion

    #region 地图对象相关
    /// <summary>
    /// 移除一个地图对象
    /// </summary>
    public void RemoveMapObject(ulong id)
    {
        // 处理Icon
        if (mapUI != null) mapUI.RemoveMapObjectIcon(id);
    }

    /// <summary>
    /// 生成一个地图对象
    /// </summary>
    public void SpawnMapObject(int mapObjectConfigID, Vector3 pos, bool isFromBuild)
    {
        Vector2Int chunkIndex = GetMapChunkIndexByWorldPosition(pos);
        SpawnMapObject(mapChunkDic[chunkIndex], mapObjectConfigID, pos, isFromBuild);
    }
    
    /// <summary>
    /// 生成一个地图对象
    /// </summary>
    public void SpawnMapObject(MapChunkController mapChunkController, int mapObjectConfigID, Vector3 pos, bool isFromBuild)
    {
        // 生成数据
        MapObjectData mapObjectData = mapGenerator.GenerateMapObjectData(mapObjectConfigID, pos);
        if (mapObjectData == null) return;
        // 交给地图块
        mapChunkController.AddMapObject(mapObjectData, isFromBuild);
        // 处理Icon
        if (mapUI != null) mapUI.AddMapObjectIcon(mapObjectData);
    }
    #endregion

    private void OnGameSave()
    {
        ArchiveManager.Instance.SaveMapData();
    }
    
    // 保留当前场景中的资源
    public void OnCloseGameScene()
    {
        // 地图UI
        mapUI.ResetWindow();
        // 地图块
        foreach (var value in mapChunkDic.Values)
        {
            value.OnCloseGameScene();
        }
    }
}
