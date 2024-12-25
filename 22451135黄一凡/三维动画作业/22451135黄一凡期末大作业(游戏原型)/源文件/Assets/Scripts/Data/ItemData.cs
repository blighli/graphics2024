using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 物品的动态数据
/// </summary>
[Serializable]
public class ItemData
{
    public int ConfigID;
    public IItemTypeData ItemTypeData;
    
    public ItemConfig Config
    {
        get => ConfigManager.Instance.GetConfig<ItemConfig>(ConfigName.Item, ConfigID);
    }

    public static ItemData CreateItemData(int configID)
    {
        ItemData itemData = new ItemData();
        itemData.ConfigID = configID;
        // 根据物品的实际类型来创建符合类型的动态数据
        switch (itemData.Config.ItemType)
        {
            case ItemType.Weapon:
                itemData.ItemTypeData = new ItemWeaponData()
                {
                    Durability = 100
                };
                break;
            case ItemType.Consumable:
                itemData.ItemTypeData = new ItemConsumableData()
                {
                    Count = 1
                };
                break;
            case ItemType.Material:
                itemData.ItemTypeData = new ItemMaterialData()
                {
                    Count = 1
                };
                break;
        }
        return itemData;
    }
}

/// <summary>
/// 物品类型数据的接口
/// </summary>
public interface IItemTypeData { }

[Serializable]
public class ItemWeaponData : IItemTypeData
{
    public float Durability;  // 耐久度 默认为100
}

[Serializable]
public abstract class PileItemTypeDataBase
{
    public int Count;
}

[Serializable]
public class ItemConsumableData : PileItemTypeDataBase, IItemTypeData
{
    
}

[Serializable]
public class ItemMaterialData : PileItemTypeDataBase, IItemTypeData
{
    
}