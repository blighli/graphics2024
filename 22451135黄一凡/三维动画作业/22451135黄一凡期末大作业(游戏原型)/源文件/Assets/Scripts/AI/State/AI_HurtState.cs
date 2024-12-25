using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AI_HurtState : AIStateBase
{
    public override void Enter()
    {
        // 播放动画
        AI.PlayAnimation("Hurt");
        // 播放受伤音效
        AI.PlayAudio("Hurt");
        // 添加受伤结束的动画事件
        AI.AddAnimationEvent("HurtOver", HurtOver);
    }

    public override void Exit()
    {
        AI.RemoveAnimationEvent("HurtOver", HurtOver);
    }

    private void HurtOver()
    {
        // 切换到追击
        AI.ChangeState(AIState.Pursue);
    }
}
