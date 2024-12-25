using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

/// <summary>
/// 数据存档管理器
/// </summary>
public class ArchiveManager : Singleton<ArchiveManager>
{
    public ArchiveManager()
    {
        LoadSaveData();
    }
    public PlayerTransformData PlayerTransformData { get; private set; }
    public PlayerMainData PlayerMainData { get; private set; }
    public MapInitData MapInitData { get; private set; }
    public MapData MapData { get; private set; }
    public Serialization_Dic<ulong, IMapObjectTypeData> MapObjectTypeDataDic { get; private set; }
    public MainInventoryData MainInventoryData { get; private set; }
    public TimeData TimeData { get; private set; }
    public ScienceData ScienceData { get; private set; }
    
    /// <summary>
    /// 是否有存档
    /// </summary>
    public bool HaveArchive { get; private set; }

    /// <summary>
    /// 加载存档系统里面的数据
    /// </summary>
    public void LoadSaveData()
    {
        // 单存档机制，只有0号存档
        SaveItem saveItem = SaveManager.GetSaveItem(0);
        HaveArchive = saveItem != null;
    }

    public void CleanArchive()
    {
        // 清空当前存档
        SaveManager.Clear();
        LoadSaveData();
    }
    
    public void CreateNewArchive(int mapSize, int mapSeed, int spawnSeed, float marshLimit)
    {
        // 清空当前存档
        SaveManager.Clear();
        // 创建存档信息
        
        //1.要在SaveManager中创建一个SaveItem
        SaveManager.CreateSaveItem();
        HaveArchive = true;
        //2.保存我们自己的初始化数据
        
        // 地图初始化数据
        MapInitData = new MapInitData()
        {
            mapSize = mapSize,
            mapSeed = mapSeed,
            spawnSeed = spawnSeed,
            marshLimit = marshLimit
        };
        SaveManager.SaveObject(MapInitData);

        MapConfig mapConfig = ConfigManager.Instance.GetConfig<MapConfig>(ConfigName.Map);
        float mapSizeOnWorld = mapSize * mapConfig.mapChunkSize * mapConfig.cellSize;
        // 玩家数据
        // 玩家的位置信息
        PlayerTransformData = new PlayerTransformData()
        {
            Position = new Vector3(mapSizeOnWorld / 2, 0, mapSizeOnWorld / 2),
            Rotation = Vector3.zero
        };
        SavePlayerTransformData();
        
        PlayerConfig playerConfig = ConfigManager.Instance.GetConfig<PlayerConfig>(ConfigName.Player);
        PlayerMainData = new PlayerMainData()
        {
            Hp = playerConfig.MaxHp,
            Hungry = playerConfig.MaxHungry
        };
        SavePlayerMainData();
        
        // 地图数据
        MapData = new MapData();
        MapObjectTypeDataDic = new();
        SaveMapData();
        
        // 背包数据，写死，就是14个
        MainInventoryData = new MainInventoryData(14);
        #region 测试数据

        MainInventoryData.ItemDatas[0] = ItemData.CreateItemData(0);
        (MainInventoryData.ItemDatas[0].ItemTypeData as ItemMaterialData).Count = 5;
        
        MainInventoryData.ItemDatas[1] = ItemData.CreateItemData(1);
        
        MainInventoryData.ItemDatas[2] = ItemData.CreateItemData(2);
        (MainInventoryData.ItemDatas[2].ItemTypeData as ItemWeaponData).Durability = 60;
        
        MainInventoryData.ItemDatas[3] = ItemData.CreateItemData(3);
        (MainInventoryData.ItemDatas[3].ItemTypeData as ItemConsumableData).Count = 10;

        MainInventoryData.ItemDatas[4] = ItemData.CreateItemData(4);
        MainInventoryData.ItemDatas[5] = ItemData.CreateItemData(5);
        
        #endregion
        SaveMainInventoryData();
        
        // 时间数据
        TimeConfig timeConfig = ConfigManager.Instance.GetConfig<TimeConfig>(ConfigName.Time);
        TimeData = new TimeData()
        {
            StateIndex = 0,
            CalculateTime = timeConfig.timeStateConfigs[0].durationTime,
            DayNum = 0
        };
        SaveTimeData();
        
        // 科技数据
        ScienceData = new ScienceData();
        SaveScienceData();
    }

    /// <summary>
    /// 加载当前存档-继续游戏
    /// </summary>
    public void LoadCurrentArchive()
    {
        MapInitData = SaveManager.LoadObject<MapInitData>();
        PlayerTransformData = SaveManager.LoadObject<PlayerTransformData>();
        MapData = SaveManager.LoadObject<MapData>();
        MapObjectTypeDataDic = SaveManager.LoadObject<Serialization_Dic<ulong, IMapObjectTypeData>>();
        MainInventoryData = SaveManager.LoadObject<MainInventoryData>();
        TimeData = SaveManager.LoadObject<TimeData>();
        PlayerMainData = SaveManager.LoadObject<PlayerMainData>();
        ScienceData = SaveManager.LoadObject<ScienceData>();
    }
    
    /// <summary>
    /// 保存玩家位置信息至磁盘
    /// </summary>
    public void SavePlayerTransformData()
    {
        SaveManager.SaveObject(PlayerTransformData);
    }

    public void SavePlayerMainData()
    {
        SaveManager.SaveObject(PlayerMainData);
    }
    
    /// <summary>
    /// 保存地图数据存档至磁盘
    /// </summary>
    public void SaveMapData()
    {
        SaveMapObjectTypeData();
        SaveManager.SaveObject(MapData);
    }

    /// <summary>
    /// 添加一个地图块数据
    /// </summary>
    public void AddAndSaveMapChunkData(Vector2Int chunkIndex, MapChunkData mapChunkData)
    {
        Serialization_Vector2 index = chunkIndex.ConvertToSVector2();
        MapData.MapChunkIndexList.Add(index);
        SaveMapData();
        
        // 保存一个地图块
        SaveManager.SaveObject(mapChunkData, "Map_" + index.ToString());
    }

    /// <summary>
    /// 保存一个地图块数据
    /// </summary>
    public void SaveMapChunkData(Vector2Int chunkIndex, MapChunkData mapChunkData)
    {
        Serialization_Vector2 index = chunkIndex.ConvertToSVector2();
        SaveManager.SaveObject(mapChunkData, "Map_" + index.ToString());
    }

    public MapChunkData GetMapChunkData(Serialization_Vector2 chunkIndex)
    {
        return SaveManager.LoadObject<MapChunkData>("Map_" + chunkIndex.ToString());
    }

    /// <summary>
    /// 保存物品栏数据
    /// </summary>
    public void SaveMainInventoryData()
    {
        SaveManager.SaveObject(MainInventoryData);
    }

    /// <summary>
    /// 保存时间数据
    /// </summary>
    public void SaveTimeData()
    {
        SaveManager.SaveObject(TimeData);
    }

    /// <summary>
    /// 保存科技数据
    /// </summary>
    public void SaveScienceData()
    {
        SaveManager.SaveObject(ScienceData);
    }

    /// <summary>
    /// 获取地图对象的类型数据
    /// </summary>
    public IMapObjectTypeData GetMapObjectTypeData(ulong id)
    {
        return MapObjectTypeDataDic.Dictionary[id];
    }

    /// <summary>
    /// 尝试获取地图对象的类型数据
    /// </summary>
    public bool TryGetMapObjectTypeData(ulong id, out IMapObjectTypeData mapObjectTypeData)
    {
        return MapObjectTypeDataDic.Dictionary.TryGetValue(id, out mapObjectTypeData);
    }
    
    /// <summary>
    /// 添加地图对象的类型数据
    /// </summary>
    public void AddMapObjectTypeData(ulong mapObjectID, IMapObjectTypeData mapObjectTypeData)
    {
        MapObjectTypeDataDic.Dictionary.Add(mapObjectID, mapObjectTypeData);
    }

    /// <summary>
    /// 移除地图对象的类型数据
    /// </summary>
    public void RemoveMapObjectTypeData(ulong mapObjectID)
    {
        MapObjectTypeDataDic.Dictionary.Remove(mapObjectID);
    }

    public void SaveMapObjectTypeData()
    {
        SaveManager.SaveObject(MapObjectTypeDataDic);
    }
}
