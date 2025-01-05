using System.Collections;
using System.Collections.Generic;
using JKFrame;
using Sirenix.OdinInspector;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.AI;

/// <summary>
/// 地图物品类型
/// </summary>
public enum MapObjectType
{
    Tree,
    Stone,
    Bush,
    Material,
    Consumable,
    Weapon,
    Building
}

/// <summary>
/// 地图对象基类
/// </summary>
public abstract class MapObjectBase : MonoBehaviour
{
    [SerializeField] private MapObjectType objectType;
    public MapObjectType ObjectType { get => objectType; }
    // 交互距离
    [SerializeField] protected float touchDistance;
    public float TouchDistance { get => touchDistance; }
    // 能否采摘
    [SerializeField] protected bool canPickUp;
    public bool CanPickUp { get => canPickUp; }
    // -1表示无效
    [SerializeField] protected int pickUpItemConfigID = -1;
    public int PickUpItemConfigID { get => pickUpItemConfigID; }
    
    protected MapChunkController mapChunk;  // 当前所在地图块
    protected ulong id;

    public virtual void Init(MapChunkController mapChunk, ulong id, bool isFromBuild)
    {
        this.mapChunk = mapChunk;
        this.id = id;
    }

    /// <summary>
    /// 从地图上移除
    /// </summary>
    public virtual void RemoveOnMap()
    {
        mapChunk.RemoveMapObject(id);
    }
    
    /// <summary>
    /// 当被捡起来
    /// </summary>
    public virtual int OnPickUp()
    {
        RemoveOnMap();
        return pickUpItemConfigID;
    }

    #region Editor

#if UNITY_EDITOR
    [Button]
    public void AddNavMeshObstacle()
    {
        NavMeshObstacle obstacle = transform.AddComponent<NavMeshObstacle>();
        BoxCollider boxCollider = GetComponent<BoxCollider>();
        CapsuleCollider capsuleCollider = GetComponent<CapsuleCollider>();
        if (boxCollider != null)
        {
            obstacle.shape = NavMeshObstacleShape.Box;
            obstacle.center = boxCollider.center;
            obstacle.size = boxCollider.size;
            obstacle.carving = true;
        }
        else if (capsuleCollider != null)
        {
            obstacle.shape = NavMeshObstacleShape.Capsule;
            obstacle.center = capsuleCollider.center;
            obstacle.height = capsuleCollider.height;
            obstacle.radius = capsuleCollider.radius;
            obstacle.carving = true;
        }
    }
#endif

    #endregion
}
