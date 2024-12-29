using System.Collections;
using System.Collections.Generic;
using JKFrame;
using Sirenix.OdinInspector;
using UnityEngine;


[CreateAssetMenu(fileName = "篝火配置", menuName = "Config/篝火配置")]
public class CampfireConfig : ConfigBase
{
    [LabelText("默认燃料数值")] public float DefaultFuelValue;
    [LabelText("上限燃料数值")] public float MaxFuelValue;
    [LabelText("燃烧速度（每秒消耗）")] public float BurningSpeed;
    [LabelText("最大灯光亮度")] public float MaxLightIntensity;
    [LabelText("最大灯光范围")] public float MaxLightRange;
    [LabelText("物品和燃料对照表")] public Dictionary<int, float> ItemFuelDic;
    [LabelText("烧烤物品对照表")] public Dictionary<int, int> BakeDic;

    public bool TryGetFuelValueByItemID(int itemID, out float fuelValue)
    {
        return ItemFuelDic.TryGetValue(itemID, out fuelValue);
    }
    
    public bool TryGetBakedItemByItemID(int itemID, out int bakedItemID)
    {
        return BakeDic.TryGetValue(itemID, out bakedItemID);
    }
}
