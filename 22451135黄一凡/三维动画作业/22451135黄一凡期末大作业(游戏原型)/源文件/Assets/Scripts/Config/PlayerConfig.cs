using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using Sirenix.OdinInspector;
using UnityEngine.Serialization;

[CreateAssetMenu(fileName = "玩家设置",menuName = "Config/玩家设置")]
public class PlayerConfig : ConfigBase
{
    #region 角色配置
    [FoldoutGroup("角色配置"), LabelText("旋转速度")]
    public float RotateSpeed = 10;
    [FoldoutGroup("角色配置"), LabelText("移动速度")]
    public float MoveSpeed = 4;
    [FoldoutGroup("角色配置"), LabelText("最大生命值")]
    public float MaxHp = 100;
    [FoldoutGroup("角色配置"), LabelText("最大饥饿值")]
    public float MaxHungry = 100;
    [FormerlySerializedAs("hungryReduceSpeed")] [FormerlySerializedAs("HungryDampSpeed")] [FoldoutGroup("角色配置"), LabelText("饥饿值衰减速度")]
    public float HungryReduceSpeed = 0.25f;
    [FormerlySerializedAs("HpDampSpeedOnHungryIsZero")] [FoldoutGroup("角色配置"), LabelText("生命值衰减速度")]
    public float HpReduceSpeedOnHungryIsZero = 2;
    [FoldoutGroup("角色配置"), LabelText("走路音效")]
    public AudioClip[] FootstepAudioClips;
    [FoldoutGroup("角色配置"), LabelText("默认动画状态机")]
    public RuntimeAnimatorController NormalAnimatorController;
    #endregion

    #region 其它
    [FoldoutGroup("其它"), LabelText("音效配置")]
    public Dictionary<AudioType, AudioClip> AudioClipDic;
    
    #endregion
}
