using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using JKFrame;
using UnityEngine;
using UnityEngine.AI;
using Random = UnityEngine.Random;

public class MapChunkController : MonoBehaviour
{
    public Vector2Int ChunkIndex { get; private set; }
    public Vector3 CenterPosition { get; private set; }
    
    public bool IsAllForest { get; private set; }
    public MapChunkData MapChunkData { get; private set; }
    // MapChunkData里面需要销毁的地图对象
    private Dictionary<ulong, MapObjectData> wantDestroyMapObjectDic;
    // 引用实际在地图上的GameObject
    private Dictionary<ulong, MapObjectBase> mapObjectDic;
    // 引用实际在地图上的AI的GameObject
    private Dictionary<ulong, AIBase> AIObjectDic;
    public bool IsInitialized { get; private set; } = false;
    private bool isActive = false;
    public void Init(Vector2Int chunkIndex, Vector3 centerPosition, bool isAllForest, MapChunkData mapChunkData)
    {
        ChunkIndex = chunkIndex;
        CenterPosition = centerPosition;
        MapChunkData = mapChunkData;
        mapObjectDic = new(MapChunkData.MapObjectDic.Dictionary.Count());
        AIObjectDic = new(MapChunkData.AIDataDic.Dictionary.Count());
        IsAllForest = isAllForest;
        IsInitialized = true;
        wantDestroyMapObjectDic = new();
        foreach (var item in mapChunkData.MapObjectDic.Dictionary.Values)
        {
            if (item.DestroyDays > 0) wantDestroyMapObjectDic.Add(item.ID, item);
        }
        EventManager.AddEventListener(EventName.OnMorn, OnMorn);
        EventManager.AddEventListener(EventName.SaveGame, OnGameSave);
    }

    public void SetActive(bool active)
    {
        if (isActive != active)
        {
            isActive = active;
            gameObject.SetActive(isActive);
            // 从对象池中获取所有物体
            if (isActive)
            {
                // 处理地图对象
                foreach (var item in MapChunkData.MapObjectDic.Dictionary.Values)
                {
                    InstantiateMapObject(item, false);
                }
                // 处理AI对象
                foreach (var item in MapChunkData.AIDataDic.Dictionary.Values)
                {
                    InstantiateAIObject(item);
                }
            }
            // 把所有物体放回对象池
            else
            {
                foreach (var item in mapObjectDic)
                {
                    item.Value.JKGameObjectPushPool();
                }
                foreach (var item in AIObjectDic)
                {
                    item.Value.Destroy();
                }
                mapObjectDic.Clear();
                AIObjectDic.Clear();
            }
        }
    }

    #region 地图对象
    private void InstantiateMapObject(MapObjectData mapObjectData, bool isFromBuild)
    {
        MapObjectConfig config = ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, mapObjectData.ConfigID);
        GameObject go = PoolManager.Instance.GetGameObject(config.Prefab, transform);
        go.transform.position = mapObjectData.Position;
        // 测试逻辑，如果获取到了，则加入，否则无视
        if (go.TryGetComponent<MapObjectBase>(out MapObjectBase mapObject))
        {
            mapObject.Init(this, mapObjectData.ID, isFromBuild);
            mapObjectDic.Add(mapObjectData.ID,mapObject);
        }
    }
    
    /// <summary>
    /// 移除一个地图对象
    /// </summary>
    public void RemoveMapObject(ulong mapObjectID)
    {
        // 数据层面移除
        // 数据放进对象池
        MapChunkData.MapObjectDic.Dictionary.Remove(mapObjectID,out MapObjectData mapObjectData);
        mapObjectData.JKObjectPushPool();
        // 自身显示层面移除
        if (mapObjectDic.TryGetValue(mapObjectID, out MapObjectBase mapObjectBase))
        {
            // 把游戏物体放进对象池
            mapObjectBase.JKGameObjectPushPool();
            mapObjectDic.Remove(mapObjectID);
        }
        // 通知上级，主要做UI地图移除
        MapManager.Instance.RemoveMapObject(mapObjectID);
    }

    /// <summary>
    /// 添加一个地图对象
    /// 1.玩家丢弃一个物品
    /// 2.早晨刷新地图对象
    /// </summary>
    public void AddMapObject(MapObjectData mapObjectData, bool isFromBuild)
    {
        // 添加存档数据
        MapChunkData.MapObjectDic.Dictionary.Add(mapObjectData.ID, mapObjectData);
        if (mapObjectData.DestroyDays > 0) wantDestroyMapObjectDic.Add(mapObjectData.ID, mapObjectData);
        // 实例化物体
        if (isActive)
        {
            InstantiateMapObject(mapObjectData, isFromBuild);
        }
    }
    #endregion

    #region AI
    /// <summary>
    /// 添加一个AI
    /// 1.早晨刷新地图对象
    /// </summary>
    public void AddAIObject(MapObjectData aiData)
    {
        // 添加存档数据
        MapChunkData.AIDataDic.Dictionary.Add(aiData.ID, aiData);
        // 实例化物体
        if (isActive)
        {
            InstantiateAIObject(aiData);
        }
    }
    private void InstantiateAIObject(MapObjectData aiData)
    {
        AIConfig config = ConfigManager.Instance.GetConfig<AIConfig>(ConfigName.AI, aiData.ConfigID);
        if (config.Prefab == null) return;
        GameObject go = PoolManager.Instance.GetGameObject(config.Prefab, transform);
        if (aiData.Position == Vector3.zero)
        {
            aiData.Position = GetAIRandomPoint(config.MapVertexType);
        }
        if (go.TryGetComponent(out AIBase aiObj))
        {
            aiObj.Init(this, aiData);
            AIObjectDic.Add(aiData.ID,aiObj);
        }
    }

    public Vector3 GetAIRandomPoint(MapVertexType mapVertexType)
    {
        List<MapVertex> vertexList = null;
        if (mapVertexType == MapVertexType.Forest)
        {
            // 如果格子不够，依然用另一个类型
            if (MapChunkData.ForestVertexList.Count < MapManager.Instance.MapConfig.GenerateAIMinVertexCountOnChunk)
                vertexList = MapChunkData.MarshVertexList;
            else vertexList = MapChunkData.ForestVertexList;
        }
        else if (mapVertexType == MapVertexType.Marsh)
        {
            if (MapChunkData.MarshVertexList.Count < MapManager.Instance.MapConfig.GenerateAIMinVertexCountOnChunk)
                vertexList = MapChunkData.ForestVertexList;
            else vertexList = MapChunkData.MarshVertexList;
        }
        int index = Random.Range(0, vertexList.Count);
        // 确保AI可以到达这个位置
        if (NavMesh.SamplePosition(vertexList[index].Position, out NavMeshHit hitInfo,1,NavMesh.AllAreas))
        {
            return hitInfo.position;
        }
        // 递归查找下一个坐标
        return GetAIRandomPoint(mapVertexType);
    }

    /// <summary>
    /// 因为迁移删除一个AI对象
    /// 只删除本地持有的数据，不做实际的对象删除、处理
    /// </summary>
    public void RemoveAIObjectOnTransfer(ulong aiObjectID)
    {
        MapChunkData.AIDataDic.Dictionary.Remove(aiObjectID);
        AIObjectDic.Remove(aiObjectID);
    }
    
    /// <summary>
    /// AI的游戏物体、数据存档等都要移除
    /// </summary>
    /// <param name="aiObjectID"></param>
    public void RemoveAIObject(ulong aiObjectID)
    {
        MapChunkData.AIDataDic.Dictionary.Remove(aiObjectID, out MapObjectData aiData);
        aiData.JKObjectPushPool();
        if (AIObjectDic.Remove(aiObjectID, out AIBase aiObject))
        {
            aiObject.Destroy();
        }
    }
    
    /// <summary>
    /// 因为迁移添加一个AI对象
    /// </summary>
    public void AddAIObjectFromTransfer(MapObjectData aiObjectData, AIBase aiObject)
    {
        MapChunkData.AIDataDic.Dictionary.Add(aiObjectData.ID, aiObjectData);
        AIObjectDic.Add(aiObjectData.ID, aiObject);
        aiObject.transform.SetParent(transform);
        aiObject.InitOnTransfer(this);
    }
    
    #endregion
    
    // 执行销毁的地图对象列表
    private static List<ulong> doDestroyMapObjectList = new(20);
    /// <summary>
    /// 早晨的时候刷新地图对象
    /// </summary>
    private void OnMorn()
    {
        // 遍历可能要销毁的地图对象，做时间计算
        foreach (var item in wantDestroyMapObjectDic.Values)
        {
            item.DestroyDays -= 1;
            if (item.DestroyDays == 0)
            {
                doDestroyMapObjectList.Add(item.ID);
            }
        }
        for (int i = 0; i < doDestroyMapObjectList.Count; i++)
        {
            RemoveMapObject(doDestroyMapObjectList[i]);
        }
        doDestroyMapObjectList.Clear();
        
        List<MapObjectData> mapObjectDataList = MapManager.Instance.SpawnMapObjectDataOnMapChunkRefresh(ChunkIndex);
        for (int i = 0; i < mapObjectDataList.Count; i++)
        {
            AddMapObject(mapObjectDataList[i], false);
        }
        
        // 三天刷新一次AI
        if (TimeManager.Instance.CurrentDayNum % 3 == 0)
        {
            mapObjectDataList = MapManager.Instance.SpawnAIObjectDataOnMapChunkRefresh(MapChunkData);
            for (int i = 0; i < mapObjectDataList.Count; i++)
            {
                AddAIObject(mapObjectDataList[i]);
            }
        }
    }
    
    private void OnGameSave()
    {
        ArchiveManager.Instance.SaveMapChunkData(ChunkIndex, MapChunkData);
    }

    public void OnCloseGameScene()
    {
        SetActive(false);
    }
}
