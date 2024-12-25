using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 地图初始化数据
/// </summary>
[Serializable]
public class MapInitData
{
    public int mapSize;
    public int mapSeed;
    public int spawnSeed;
    public float marshLimit;
}

/// <summary>
/// 地图数据
/// </summary>
[Serializable]
public class MapData
{
    public ulong CurrentID = 1;
    // 当前玩家去过的所有地图块（已经生成过的地图块）
    public List<Serialization_Vector2> MapChunkIndexList = new ();
}

/// <summary>
/// 地图块数据
/// </summary>
[Serializable]
public class MapChunkData
{
    public Serialization_Dic<ulong, MapObjectData> MapObjectDic;
    public Serialization_Dic<ulong, MapObjectData> AIDataDic;

    [NonSerialized] public List<MapVertex> ForestVertexList;
    [NonSerialized] public List<MapVertex> MarshVertexList;
}

/// <summary>
/// 地图块上一个对象的数据
/// </summary>
[Serializable]
public class MapObjectData
{
    // 唯一的身份标识
    public ulong ID;
    // 当前的配置
    public int ConfigID;
    // 剩余的腐烂天数
    public int DestroyDays;
    // 坐标
    private Serialization_Vector3 position;
    public Vector3 Position
    {
        get => position.ConvertToVector3();
        set => position = value.ConvertToSVector3();
    }
}
