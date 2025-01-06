using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using Random = UnityEngine.Random;

/// <summary>
/// 地图生成工具
/// </summary>
public class MapGenerator
{
    #region 运行时的逻辑
    private MapGrid mapGrid;           // 地图逻辑网格、顶点数据
    private Material marshMaterial;
    private Mesh chunkMesh;
    private int mapObjectForestWeightTotal;
    private int mapObjectMarshWeightTotal;
    private int AIForestWeightTotal;
    private int AIMarshWeightTotal;
    #endregion

    #region 配置
    private Dictionary<MapVertexType, List<int>> mapObjectConfigDic;
    private Dictionary<MapVertexType, List<int>> AIConfigDic;
    private MapConfig mapConfig;
    #endregion

    #region 存档
    private MapInitData mapInitData;
    private MapData mapData;
    #endregion

    public MapGenerator(MapConfig mapConfig,MapInitData mapInitData, MapData mapData, Dictionary<MapVertexType, List<int>> mapObjectConfigDic, Dictionary<MapVertexType, List<int>> AIConfigDic)
    {
        this.mapConfig = mapConfig;
        this.mapInitData = mapInitData;
        this.mapData = mapData;
        this.mapObjectConfigDic = mapObjectConfigDic;
        this.AIConfigDic = AIConfigDic;
    }
    
    /// <summary>
    /// 生成地图数据，主要是所有地图块都通用的数据
    /// </summary>
    public void GenerateMapData()
    {
        // 生成噪声图
        // 应用地图种子
        Random.InitState(mapInitData.mapSeed);
        float[,] noiseMap = GenerateNoiseMap(mapInitData.mapSize * mapConfig.mapChunkSize, mapInitData.mapSize * mapConfig.mapChunkSize, mapConfig.noiseLacunarity);
        // 生成网格数据
        mapGrid = new MapGrid(mapInitData.mapSize * mapConfig.mapChunkSize, mapInitData.mapSize * mapConfig.mapChunkSize, mapConfig.cellSize);
        // 确认网格 格子的贴图索引
        mapGrid.CalculateMapVertexType(noiseMap, mapInitData.marshLimit);
        // 初始化默认材质的尺寸
        mapConfig.mapMaterial.mainTexture = mapConfig.forestTexture;
        mapConfig.mapMaterial.SetTextureScale("_MainTex", new Vector2(mapConfig.cellSize * mapConfig.mapChunkSize, mapConfig.cellSize * mapConfig.mapChunkSize));
        // 实例化一个沼泽材质
        marshMaterial = new Material(mapConfig.mapMaterial);
        marshMaterial.SetTextureScale("_MainTex", Vector2.one);
        
        chunkMesh = GenerateMapMesh(mapConfig.mapChunkSize, mapConfig.mapChunkSize, mapConfig.cellSize);
        Random.InitState(mapInitData.spawnSeed);

        List<int> temps = mapObjectConfigDic[MapVertexType.Forest];
        for (int i = 0; i < temps.Count; i++)
        {
            mapObjectForestWeightTotal += ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, temps[i])
                .Probability;
        }
        temps = mapObjectConfigDic[MapVertexType.Marsh];
        for (int i = 0; i < temps.Count; i++)
        {
            mapObjectMarshWeightTotal += ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, temps[i])
                .Probability;
        }
        
        temps = AIConfigDic[MapVertexType.Forest];
        for (int i = 0; i < temps.Count; i++)
        {
            AIForestWeightTotal += ConfigManager.Instance.GetConfig<AIConfig>(ConfigName.AI, temps[i])
                .Probability;
        }
        temps = AIConfigDic[MapVertexType.Marsh];
        for (int i = 0; i < temps.Count; i++)
        {
            AIMarshWeightTotal += ConfigManager.Instance.GetConfig<AIConfig>(ConfigName.AI, temps[i])
                .Probability;
        }
    }

    #region 地图块
    /// <summary>
    /// 生成地图块
    /// </summary>
    public MapChunkController GenerateMapChunk(Vector2Int chunkIndex, Transform parent, MapChunkData mapChunkData, Action callBackForMapTexture)
    {
        // 生成地图块物体
        GameObject mapChunkObj = new GameObject("Chunk_" + chunkIndex.ToString());
        MapChunkController mapChunk = mapChunkObj.AddComponent<MapChunkController>();
        // 生成Mesh
        mapChunkObj.AddComponent<MeshFilter>().mesh = chunkMesh;

        bool allForest;
        // 生成地图块的贴图
        Texture2D mapTexture;
        this.StartCoroutine(GenerateMapTexture(chunkIndex, (tex, isAllForest) =>
        {
            allForest = isAllForest;
            if (isAllForest)
            {
                mapChunkObj.AddComponent<MeshRenderer>().sharedMaterial = mapConfig.mapMaterial;
            }
            else
            {
                mapTexture = tex;
                Material material = new Material(marshMaterial);
                material.mainTexture = tex;
                mapChunkObj.AddComponent<MeshRenderer>().material = material;
            }
            callBackForMapTexture?.Invoke();
            // 确定坐标
            Vector3 position =
                new Vector3(chunkIndex.x * mapConfig.mapChunkSize * mapConfig.cellSize, 0, chunkIndex.y * mapConfig.mapChunkSize * mapConfig.cellSize);
            mapChunk.transform.position = position;
            mapChunkObj.transform.SetParent(parent);
            
            // 如果没有指定地图块数据，说明是新建的，需要生成默认数据
            if (mapChunkData == null)
            {
                // 生成场景物体数据
                mapChunkData = GenerateMapChunkData(chunkIndex);
                // 生成后进行持久化保存
                ArchiveManager.Instance.AddAndSaveMapChunkData(chunkIndex, mapChunkData);
            }
            else
            {
                // 恢复VertexList
                RecoverMapChunkData(chunkIndex, mapChunkData);
            }
            
            mapChunk.Init(chunkIndex,
                position + new Vector3((mapConfig.mapChunkSize * mapConfig.cellSize) / 2, 0, (mapConfig.mapChunkSize * mapConfig.cellSize) / 2),
                allForest, mapChunkData);
            
            // 如果目前游戏没有完成初始化，要告诉GameSceneManager更新进度
            if (GameSceneManager.Instance.IsInitialized == false)
            {
                GameSceneManager.Instance.OnGenerateMapChunkSucceed();
            }
        }));
        
        return mapChunk;
    }
    
    private Mesh GenerateMapMesh(int height, int width, float cellSize)
    {
        Mesh mesh = new Mesh();
        // 确定顶点在哪里
        mesh.vertices = new Vector3[]
        {
            new Vector3(0, 0, 0),
            new Vector3(0, 0, height * cellSize),
            new Vector3(width * cellSize, 0, height * cellSize),
            new Vector3(width * cellSize, 0, 0),
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
        // 计算法线
        mesh.RecalculateNormals();
        return mesh;
    }

    private float[,] GenerateNoiseMap(int width, int height, float lacunarity)
    {
        lacunarity += 0.1f;
        // 这里的噪声图是为了顶点服务的
        float[,] noiseMap = new float[width, height];
        float offsetX = Random.Range(-10000f, 10000f);
        float offsetY = Random.Range(-10000f, 10000f);

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                noiseMap[x, y] = Mathf.PerlinNoise(x * lacunarity + offsetX, y * lacunarity + offsetY);
            }
        }
        return noiseMap;
    }

    /// <summary>
    /// 分帧-生成地图贴图
    /// 如果这个地图块完全是森林，直接返回森林贴图
    /// </summary>
    private IEnumerator GenerateMapTexture(Vector2Int chunkIndex, Action<Texture2D, bool> callBack)
    {
        // 当前地图块的偏移量 找到这个地图块具体的每一个格子
        int cellOffsetX = chunkIndex.x * mapConfig.mapChunkSize + 1;
        int cellOffsetY = chunkIndex.y * mapConfig.mapChunkSize + 1;
        
        // 是不是一张完整的森林地图块
        bool isAllForest = true;
        // 检查是否只有森林类型的格子
        for (int y = 0; y < mapConfig.mapChunkSize; y++)
        {
            if (isAllForest == false) break;
            for (int x = 0; x < mapConfig.mapChunkSize; x++)
            {
                MapCell cell = mapGrid.GetCell(x + cellOffsetX, y + cellOffsetY);
                if (cell != null && cell.TextureIndex != 0)
                {
                    isAllForest = false;
                    break;
                }
            }
        }
        Texture2D mapTexture = null;
        if (isAllForest == false)
        {
            // 贴图都是矩形
            int textureCellSize = mapConfig.forestTexture.width;
            // 整个地图的宽高
            int textureSize = mapConfig.mapChunkSize * textureCellSize;
            mapTexture = new Texture2D(textureSize, textureSize, TextureFormat.RGB24, false);
            // 遍历每一个格子
            for (int y = 0; y < mapConfig.mapChunkSize; y++)
            {
                yield return null;
                int pixelOffsetY = y * textureCellSize;
                for (int x = 0; x < mapConfig.mapChunkSize; x++)
                {
                    int pixelOffsetX = x * textureCellSize;
                    int textureIndex = mapGrid.GetCell(x+cellOffsetX, y + cellOffsetY).TextureIndex - 1;
                    // 绘制每一个格子内的像素
                    for (int y1 = 0; y1 < textureCellSize; y1++)
                    {
                        for (int x1 = 0; x1 < textureCellSize; x1++)
                        {
                            // 设置某个像素点的颜色
                            // 确定是森林还是沼泽
                            // 这个地方是森林或者这个地方是沼泽但是是透明的
                            // 都需要绘制groundTexture同位置的像素颜色
                            if (textureIndex < 0)
                            {
                                Color color = mapConfig.forestTexture.GetPixel(x1, y1);
                                mapTexture.SetPixel(x1 + pixelOffsetX, y1 + pixelOffsetY, color);
                            }
                            else
                            {
                                Color color = mapConfig.marshTextures[textureIndex].GetPixel(x1, y1);
                                if (color.a < 1f)
                                {
                                    mapTexture.SetPixel(x1 + pixelOffsetX, y1 + pixelOffsetY, mapConfig.forestTexture.GetPixel(x1, y1));
                                }
                                else
                                {
                                    mapTexture.SetPixel(x1 + pixelOffsetX, y1 + pixelOffsetY, color);
                                }
                            }
                        }
                    }
                }
            }
            
            mapTexture.filterMode = FilterMode.Point;
            mapTexture.wrapMode = TextureWrapMode.Clamp;
            mapTexture.Apply();
        }
        callBack?.Invoke(mapTexture, isAllForest);
    }

    /// <summary>
    /// 生成一个地图对象的数据
    /// </summary>
    public MapObjectData GenerateMapObjectData(int mapObjectConfigID, Vector3 pos)
    {
        MapObjectData mapObjectData = null;
        MapObjectConfig mapObjectConfig = ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, mapObjectConfigID);
        if (mapObjectConfig.IsEmpty == false)
        {
            mapObjectData = GenerateMapObjectData(mapObjectConfigID, pos, mapObjectConfig.DestroyDays);
        }
        return mapObjectData;
    }

    /// <summary>
    /// 通过权重获取一个地图对象配置ID
    /// </summary>
    private int GetMapObjectConfigIDForWeight(MapVertexType mapVertexType)
    {
        List<int> configIDs = mapObjectConfigDic[mapVertexType];
                
        // 确定权重的组合
        int weightTotal = mapVertexType == MapVertexType.Forest
            ? mapObjectForestWeightTotal
            : mapObjectMarshWeightTotal;
        
        int randValue = Random.Range(1, weightTotal + 1);
        float temp = 0;
        for (int i = 0; i < configIDs.Count; i++)
        {
            temp += ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, configIDs[i])
                .Probability;
            if (randValue < temp)
            {
                return configIDs[i];
            }
        }
        return 0;
    }
    
    /// <summary>
    /// 通过权重获取一个AI配置ID
    /// </summary>
    private int GetAIConfigIDForWeight(MapVertexType mapVertexType)
    {
        List<int> configIDs = AIConfigDic[mapVertexType];
                
        // 确定权重的组合
        int weightTotal = mapVertexType == MapVertexType.Forest
            ? AIForestWeightTotal
            : AIMarshWeightTotal;
        
        int randValue = Random.Range(1, weightTotal + 1);
        float temp = 0;
        for (int i = 0; i < configIDs.Count; i++)
        {
            temp += ConfigManager.Instance.GetConfig<AIConfig>(ConfigName.AI, configIDs[i])
                .Probability;
            if (randValue < temp)
            {
                return configIDs[i];
            }
        }
        return 0;
    }
    
    /// <summary>
    /// 生成各种地图对象，为地图块初始化准备
    /// </summary>
    private MapChunkData GenerateMapChunkData(Vector2Int chunkIndex)
    {
        MapChunkData mapChunkData = new();
        mapChunkData.MapObjectDic = new();
        mapChunkData.AIDataDic = new();
        mapChunkData.ForestVertexList = new(mapConfig.mapChunkSize * mapConfig.mapChunkSize);
        mapChunkData.MarshVertexList = new(mapConfig.mapChunkSize * mapConfig.mapChunkSize);
        
        int offsetX = chunkIndex.x * mapConfig.mapChunkSize;
        int offsetY = chunkIndex.y * mapConfig.mapChunkSize;
        
        // 生成地图对象
        for (int x = 1; x < mapConfig.mapChunkSize; x++)
        {
            for (int y = 1; y < mapConfig.mapChunkSize; y++)
            {
                MapVertex mapVertex = mapGrid.GetVertex(x + offsetX, y + offsetY);

                if (mapVertex.VertexType == MapVertexType.Forest)
                {
                    mapChunkData.ForestVertexList.Add(mapVertex);
                }
                else if (mapVertex.VertexType == MapVertexType.Marsh)
                {
                    mapChunkData.MarshVertexList.Add(mapVertex);
                }
                
                int configID = GetMapObjectConfigIDForWeight(mapVertex.VertexType);
                MapObjectConfig objectConfig =
                    ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, configID);
                if (objectConfig.IsEmpty == false)
                {
                    Vector3 position = mapVertex.Position + new Vector3(Random.Range(-mapConfig.cellSize / 2, mapConfig.cellSize / 2), 0,
                        Random.Range(-mapConfig.cellSize / 2, mapConfig.cellSize / 2));
                    mapVertex.MapObjectID = mapData.CurrentID;
                    mapChunkData.MapObjectDic.Dictionary.Add(mapData.CurrentID, GenerateMapObjectData(configID, position, objectConfig.DestroyDays));
                }
            }
        }
        
        List<MapObjectData> aiDataList = GenerateAIObjectDataList(mapChunkData);
        for (int i = 0; i < aiDataList.Count; i++)
        {
            mapChunkData.AIDataDic.Dictionary.Add(aiDataList[i].ID, aiDataList[i]);
        }
        return mapChunkData;
    }

    /// <summary>
    /// 恢复地图块数据
    /// 目前只恢复了VertexList了
    /// </summary>
    private void RecoverMapChunkData(Vector2Int chunkIndex, MapChunkData mapChunkData)
    {
        mapChunkData.ForestVertexList = new(mapConfig.mapChunkSize * mapConfig.mapChunkSize);
        mapChunkData.MarshVertexList = new(mapConfig.mapChunkSize * mapConfig.mapChunkSize);
        int offsetX = chunkIndex.x * mapConfig.mapChunkSize;
        int offsetY = chunkIndex.y * mapConfig.mapChunkSize;
        for (int x = 1; x < mapConfig.mapChunkSize; x++)
        {
            for (int y = 1; y < mapConfig.mapChunkSize; y++)
            {
                MapVertex mapVertex = mapGrid.GetVertex(x + offsetX, y + offsetY);

                if (mapVertex.VertexType == MapVertexType.Forest)
                {
                    mapChunkData.ForestVertexList.Add(mapVertex);
                }
                else if (mapVertex.VertexType == MapVertexType.Marsh)
                {
                    mapChunkData.MarshVertexList.Add(mapVertex);
                }
            }
        }
    }
    
    private List<MapObjectData> mapObjectDataList = new();
    /// <summary>
    /// 为地图块刷新生成地图对象列表
    /// 可能为null
    /// </summary>
    public List<MapObjectData> GenerateMapObjectDataListOnMapChunkRefresh(Vector2Int chunkIndex)
    {
        mapObjectDataList.Clear();
        int offsetX = chunkIndex.x * mapConfig.mapChunkSize;
        int offsetY = chunkIndex.y * mapConfig.mapChunkSize;
        for (int x = 1; x < mapConfig.mapChunkSize; x++)
        {
            for (int y = 1; y < mapConfig.mapChunkSize; y++)
            {
                // 如果概率没命中，这一个顶点不刷新
                if (Random.Range(0, mapConfig.refreshProbability) != 0) continue;
                
                MapVertex mapVertex = mapGrid.GetVertex(x + offsetX, y + offsetY);
                
                if (mapVertex.MapObjectID != 0) continue;
                
                int configID = GetMapObjectConfigIDForWeight(mapVertex.VertexType);
                MapObjectConfig objectConfig = ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, configID);
                if (objectConfig.IsEmpty == false)
                {
                    Vector3 position = mapVertex.Position + new Vector3(Random.Range(-mapConfig.cellSize / 2, mapConfig.cellSize / 2), 0,
                        Random.Range(-mapConfig.cellSize / 2, mapConfig.cellSize / 2));
                    mapVertex.MapObjectID = mapData.CurrentID;
                    mapObjectDataList.Add(GenerateMapObjectData(configID, position, objectConfig.DestroyDays));
                }
            }
        }
        return mapObjectDataList;
    }

    /// <summary>
    /// 为地图块刷新生成AI列表
    /// </summary>
    public List<MapObjectData> GenerateAIObjectDataList(MapChunkData mapChunkData)
    {
        mapObjectDataList.Clear();
        // 最多生成的数量
        int max = mapConfig.maxAIOnChunk - mapChunkData.AIDataDic.Dictionary.Count;
        if (mapChunkData.ForestVertexList.Count > mapConfig.GenerateAIMinVertexCountOnChunk)
        {
            for (int i = 0; i < max; i++)
            {
                int configID = GetAIConfigIDForWeight(MapVertexType.Forest);
                AIConfig config = ConfigManager.Instance.GetConfig<AIConfig>(ConfigName.AI, configID);
                if (config.IsEmpty == false)
                {
                    mapObjectDataList.Add(GenerateMapObjectData(configID, Vector3.zero, -1));
                    max--;
                }
            }
        }
        if (mapChunkData.MarshVertexList.Count > mapConfig.GenerateAIMinVertexCountOnChunk)
        {
            for (int i = 0; i < max; i++)
            {
                int configID = GetAIConfigIDForWeight(MapVertexType.Marsh);
                AIConfig config = ConfigManager.Instance.GetConfig<AIConfig>(ConfigName.AI, configID);
                if (config.IsEmpty == false)
                {
                    mapObjectDataList.Add(GenerateMapObjectData(configID, Vector3.zero, -1));
                }
            }
        }
        return mapObjectDataList;
    }

    /// <summary>
    /// 生成一个地图对象数据
    /// </summary>
    private MapObjectData GenerateMapObjectData(int mapObjectConfigID, Vector3 position, int destroyDays)
    {
        MapObjectData mapObjectData = PoolManager.Instance.GetObject<MapObjectData>();
        mapObjectData.ConfigID = mapObjectConfigID;
        mapObjectData.ID = mapData.CurrentID;
        mapData.CurrentID += 1;
        mapObjectData.Position = position;
        mapObjectData.DestroyDays = destroyDays;
        return mapObjectData;
    }
    #endregion
    
    
}