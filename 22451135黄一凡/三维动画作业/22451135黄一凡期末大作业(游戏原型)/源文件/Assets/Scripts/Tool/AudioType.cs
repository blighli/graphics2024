using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Sirenix.OdinInspector;

/// <summary>
/// 音效类型
/// </summary>
public enum AudioType
{
    [LabelText("玩家无法使用")] PlayerCannot,
    [LabelText("拿起武器")] TakeUpWeapon,
    [LabelText("放下武器")] TakeDownWeapon,
    [LabelText("使用消耗品成功")] ConsumablesOK,
    [LabelText("使用消耗品失败")] ConsumablesFail,
    [LabelText("背包")] Bag,
    [LabelText("通用失败")] Fail
}
