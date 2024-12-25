using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using Sirenix.OdinInspector;

/// <summary>
/// 物品类型
/// </summary>
public enum ItemType
{
    [LabelText("装备")] Weapon,
    [LabelText("消耗品")] Consumable,
    [LabelText("材料")] Material
}

/// <summary>
/// 武器类型
/// </summary>
public enum WeaponType
{
    [LabelText("斧头")] Axe,
    [LabelText("稿")] PickAxe,
    [LabelText("镰刀")] Sickle
}

/// <summary>
/// 物品配置
/// </summary>
[CreateAssetMenu(menuName = "Config/物品配置")]
public class ItemConfig : ConfigBase
{
    [LabelText("名称")] public string Name;
    [LabelText("类型"),OnValueChanged(nameof(OnItemTypeChanged))] public ItemType ItemType;
    [LabelText("地图物品ID")] public int MapObjectConfigID;
    [LabelText("描述"),MultiLineProperty] public string Description;
    [LabelText("图标")] public Sprite Icon;
    [LabelText("类型专属信息")] public IItemTypeInfo ItemTypeInfo;

    // 当类型修改的时候，自动生成同等类型应有的专属信息
    private void OnItemTypeChanged()
    {
        switch (ItemType)
        {
            case ItemType.Weapon:
                ItemTypeInfo = new ItemWeaponInfo();
                break;
            case ItemType.Consumable:
                ItemTypeInfo = new ItemConsumableInfo();
                break;
            case ItemType.Material:
                ItemTypeInfo = new ItemMaterialInfo();
                break;
        }
    }
}

/// <summary>
/// 物品类型信息的接口
/// </summary>
public interface IItemTypeInfo { };

/// <summary>
/// 武器类型信息
/// </summary>
[Serializable]
public class ItemWeaponInfo : IItemTypeInfo
{
    [LabelText("武器类型")] public WeaponType WeaponType;
    [LabelText("玩家手里的预制体")] public GameObject PrefabOnPlayer;
    [LabelText("武器坐标")] public Vector3 PositionOnPlayer;
    [LabelText("武器旋转")] public Vector3 RotationOnPlayer;
    [LabelText("动画状态机")] public AnimatorOverrideController AnimatorController;
    [LabelText("攻击力")] public float AttackValue;
    [LabelText("攻击损耗")] public float AttackDurabilityCost;
    [LabelText("攻击距离")] public float AttackDistance;
    [LabelText("攻击音效")] public AudioClip AttackAudio;
    [LabelText("击打音效")] public AudioClip HitAudio;
    [LabelText("击打特效")] public GameObject HitEffect;
}

/// <summary>
/// 可堆叠的物品类似数据基类
/// </summary>
[Serializable]
public abstract class PileItemTypeInfoBase
{
    public int MaxCount;
}

/// <summary>
/// 消耗品类型信息
/// </summary>
[Serializable]
public class ItemConsumableInfo : PileItemTypeInfoBase, IItemTypeInfo
{
    [LabelText("恢复生命值")] public float RecoverHP;
    [LabelText("恢复饥饿值")] public float RecoverHungry;
}

/// <summary>
/// 材料类型信息
/// </summary>
[Serializable]
public class ItemMaterialInfo : PileItemTypeInfoBase, IItemTypeInfo
{
    
}