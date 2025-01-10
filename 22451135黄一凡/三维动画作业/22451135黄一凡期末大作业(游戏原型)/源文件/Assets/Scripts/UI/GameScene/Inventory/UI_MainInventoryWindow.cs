using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

[UIElement(false, "UI/UI_MainInventoryWindow", 1)]
public class UI_MainInventoryWindow : UI_InventoryWindowBase
{
    private MainInventoryData mainInventoryData;
    [SerializeField] private UI_ItemSlot weaponSlot;// 装备槽
    
    private void Update()
    {
        #region 测试
        // if (Input.GetKeyDown(KeyCode.Alpha0)) AddItemAndPlayAudio(0);
        // if (Input.GetKeyDown(KeyCode.Alpha1)) AddItemAndPlayAudio(1);
        // if (Input.GetKeyDown(KeyCode.Alpha2)) AddItemAndPlayAudio(2);
        // if (Input.GetKeyDown(KeyCode.Alpha3)) AddItemAndPlayAudio(3);
        // if (Input.GetKeyDown(KeyCode.Alpha4)) AddItemAndPlayAudio(4);
        // if (Input.GetKeyDown(KeyCode.Alpha5)) AddItemAndPlayAudio(5);
        #endregion
    }

    public override void Init()
    {
        base.Init();
        // 就算窗口被隐藏，这个事件也要持续监听
        EventManager.AddEventListener(EventName.PlayerWeaponAttackSucceed, OnPlayerWeaponAttackSucceed);
    }

    public void InitData()
    {
        // 确定存档数据
        inventoryData = ArchiveManager.Instance.MainInventoryData;
        mainInventoryData = inventoryData as MainInventoryData;
        // 初始化格子数据
        InitSlotData();
        // 初始化玩家手中的武器
        Player_Controller.Instance.ChangeWeapon(mainInventoryData.WeaponSlotItemData);
    }

    protected override void InitSlotData()
    {
        // 给普通格子初始化数据
        for (int i = 0; i < inventoryData.ItemDatas.Length; i++)
        {
            slots[i].Init(i, this, UseItem);
            slots[i].InitData(inventoryData.ItemDatas[i]);
        }
        // 给武器格子初始化数据
        UI_ItemSlot.WeaponSlot = weaponSlot;
        weaponSlot.Init(inventoryData.ItemDatas.Length, this, UseItem);
        weaponSlot.InitData(mainInventoryData.WeaponSlotItemData);
    }
    
    /// <summary>
    /// 当玩家使用武器攻击成功后
    /// </summary>
    private void OnPlayerWeaponAttackSucceed()
    {
        if (mainInventoryData.WeaponSlotItemData == null) return;
        ItemWeaponData weaponData = mainInventoryData.WeaponSlotItemData.ItemTypeData as ItemWeaponData;
        ItemWeaponInfo weaponInfo = mainInventoryData.WeaponSlotItemData.Config.ItemTypeInfo as ItemWeaponInfo;
        weaponData.Durability -= weaponInfo.AttackDurabilityCost;
        if (weaponData.Durability <= 0)
        {
            // 损坏
            mainInventoryData.RemoveWeaponItem();
            // 武器槽拿掉这个武器
            weaponSlot.InitData(null);
            // 通知玩家卸掉武器
            Player_Controller.Instance.ChangeWeapon(null);
        }
        else
        {
            // 更新耐久度UI
            weaponSlot.UpdateCountTextView();
        }
    }
    
    /// <summary>
    /// 移除一个物品并且不考虑堆积的情况
    /// </summary>
    protected override void RemoveItem(int index)
    {
        // 是武器
        if (index == inventoryData.ItemDatas.Length)
        {
            mainInventoryData.RemoveWeaponItem();
            weaponSlot.InitData(null);
        }
        // 普通格子
        else
        {
            base.RemoveItem(index);
        }
    }

    public override void DiscardItem(int index)
    {
        // 武器槽直接丢弃
        if (index == slots.Count)
        {
            RemoveItem(index);
        }
        else
        {
            base.DiscardItem(index);
        }
    }

    public override void SetItem(int index, ItemData itemData)
    {
        // 是武器
        if (index == mainInventoryData.ItemDatas.Length)
        {
            mainInventoryData.SetWeaponItem(itemData);
            weaponSlot.InitData(itemData);
            // 将武器数据同步给玩家
            Player_Controller.Instance.ChangeWeapon(itemData);
        }
        else
        {
            base.SetItem(index, itemData);
        }
    }
    
    /// <summary>
    /// 使用物品
    /// </summary>
    public AudioType UseItem(int index)
    {
        // 判断玩家的状态能否使用物品
        if (Player_Controller.Instance.CanUseItem == false) return AudioType.PlayerCannot;
        // 拿下武器
        if (index == slots.Count)
        {
            int emptySlotIndex = GetEmptySlot();
            if (emptySlotIndex > 0)
            {
                UI_ItemSlot.SwapSlotItem(weaponSlot, slots[emptySlotIndex]);
                return AudioType.TakeDownWeapon;
            }
            else
            {
                return AudioType.Fail;
            }
        }
        ItemData itemData = slots[index].ItemData;
        switch (itemData.Config.ItemType)
        {
            case ItemType.Weapon:
                UI_ItemSlot.SwapSlotItem(slots[index], weaponSlot);
                return AudioType.TakeUpWeapon;
            case ItemType.Consumable:
                ItemConsumableInfo info = itemData.Config.ItemTypeInfo as ItemConsumableInfo;
                if (info.RecoverHP != 0) Player_Controller.Instance.RecoverHP(info.RecoverHP);
                if (info.RecoverHungry != 0) Player_Controller.Instance.RecoverHungry(info.RecoverHungry);
                PileItemTypeDataBase typeData = itemData.ItemTypeData as PileItemTypeDataBase;
                typeData.Count -= 1;
                if (typeData.Count == 0)
                {
                    RemoveItem(index);
                }
                else
                {
                    slots[index].UpdateCountTextView();
                }
                return AudioType.ConsumablesOK;
            default:
                return AudioType.Fail;
        }
    }
    
    public void UpdateItemsForBuild(BuildConfig buildConfig)
    {
        for (int i = 0; i < buildConfig.BuildConfigConditionList.Count; i++)
        {
            UpdateItemForBuild(buildConfig.BuildConfigConditionList[i]);
        }
    }

    private void UpdateItemForBuild(BuildConfigCondition buildConfigCondition)
    {
        int count = buildConfigCondition.Count;
        for (int i = 0; i < inventoryData.ItemDatas.Length; i++)
        {
            ItemData itemData = inventoryData.ItemDatas[i];
            if (itemData != null && itemData.ConfigID == buildConfigCondition.ItemId)
            {
                if (itemData.ItemTypeData is PileItemTypeDataBase)
                {
                    PileItemTypeDataBase pileItemTypeData = itemData.ItemTypeData as PileItemTypeDataBase;
                    int quantity = pileItemTypeData.Count - count;
                    if (quantity > 0)
                    {
                        pileItemTypeData.Count -= count;
                        slots[i].UpdateCountTextView();
                        return;
                    }
                    else
                    {
                        count -= pileItemTypeData.Count;
                        RemoveItem(i);
                        if (count == 0) return;
                    }
                }
                else
                {
                    count -= 1;
                    RemoveItem(i);
                    if (count == 0) return; 
                }
            }
        }
        Debug.LogError("出Bug啦");
        
    }
}
