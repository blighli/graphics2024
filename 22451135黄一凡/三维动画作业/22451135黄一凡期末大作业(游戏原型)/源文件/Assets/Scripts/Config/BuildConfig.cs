using System.Collections;
using System.Collections.Generic;
using JKFrame;
using Sirenix.OdinInspector;
using UnityEngine;

[CreateAssetMenu(fileName = "建造与合成配置",menuName = "Config/建造与合成配置")]
public class BuildConfig : ConfigBase
{
    [LabelText("合成类型")] public BuildType BuildType;
    [LabelText("前置科技")] public List<int> PreconditionScienceIDList;
    [LabelText("合成条件")] public List<BuildConfigCondition> BuildConfigConditionList = new();
    [LabelText("合成产物")] public int TargetID;

    public bool CheckBuildConfigCondition()
    {
        for (int j = 0; j < BuildConfigConditionList.Count; j++)
        {
            int currentCount = InventoryManager.Instance.GetMainInventoryItemCount(BuildConfigConditionList[j].ItemId);
            // 检查当前数量是否满足这个条件
            if (currentCount < BuildConfigConditionList[j].Count)
            {
                return false;
            }
        }
        return true;
    }
}

public class BuildConfigCondition
{
    [LabelText("物品ID"), HorizontalGroup] public int ItemId;
    [LabelText("物品数量"), HorizontalGroup] public int Count;
}