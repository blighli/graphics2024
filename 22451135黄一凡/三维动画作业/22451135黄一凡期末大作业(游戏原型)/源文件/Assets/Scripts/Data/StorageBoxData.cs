using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class StorageBoxData : IMapObjectTypeData
{
    private InventoryData inventoryData;

    public InventoryData InventoryData { get => inventoryData; }

    public StorageBoxData(int itemCount)
    {
        inventoryData = new InventoryData(itemCount);
    }
}
