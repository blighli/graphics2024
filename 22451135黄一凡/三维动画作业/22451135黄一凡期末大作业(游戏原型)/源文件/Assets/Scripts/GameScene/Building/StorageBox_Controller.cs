using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StorageBox_Controller : BuildingBase
{
    private StorageBoxData storageBoxData;
    [SerializeField] private Vector2Int UIWindowGridSize;
    public override void Init(MapChunkController mapChunk, ulong id, bool isFromBuild)
    {
        base.Init(mapChunk, id, isFromBuild);
        if (isFromBuild)
        {
            storageBoxData = new StorageBoxData(UIWindowGridSize.x * UIWindowGridSize.y);
            ArchiveManager.Instance.AddMapObjectTypeData(id, storageBoxData);
        }
        else
        {
            storageBoxData = ArchiveManager.Instance.GetMapObjectTypeData(id) as StorageBoxData;
        }
    }

    public override void OnSelect()
    {
        // 打开储物箱UI窗口
        InventoryManager.Instance.OpenStorageBoxWindow(this, storageBoxData.InventoryData, UIWindowGridSize);
    }
}
