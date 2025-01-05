using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 玩家攻击状态
/// </summary>
public class Player_Attack : PlayerStateBase
{
    public Quaternion attackDir;
    public override void Enter()
    {
        attackDir = player.attackDir;
        player.PlayAnimation("Attack");
    }

    public override void Update()
    {
        // 旋转到攻击方向
        player.playerTransform.localRotation = Quaternion.Slerp(player.playerTransform.localRotation, attackDir,
            Time.deltaTime * player.rotateSpeed * 2);
    }

    public override void Exit()
    {
        player.OnStopHit();
    }
}
