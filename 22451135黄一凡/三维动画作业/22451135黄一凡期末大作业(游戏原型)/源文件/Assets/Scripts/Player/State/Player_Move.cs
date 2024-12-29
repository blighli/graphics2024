using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 玩家移动状态
/// </summary>
public class Player_Move : PlayerStateBase
{
    private CharacterController characterController;

    public override void Init(IStateMachineOwner owner, int stateType, StateMachine stateMachine)
    {
        base.Init(owner, stateType, stateMachine);
        characterController = player.characterController;
    }

    public override void Enter()
    {
        player.PlayAnimation("Move");
    }

    public override void Update()
    {
        float h = Input.GetAxis("Horizontal");
        float v = Input.GetAxis("Vertical");
        if (h == 0 && v == 0)
        {
            player.ChangeState(PlayerState.Idle);
            return;
        }

        Vector3 inputDir = new Vector3(h, 0, v);
        // 朝向计算
        Quaternion targetQua = Quaternion.LookRotation(inputDir);
        player.playerTransform.localRotation = Quaternion.Slerp(player.playerTransform.localRotation, targetQua,
            Time.deltaTime * player.rotateSpeed);

        // 检查地图边界
        if (player.playerTransform.position.x < player.positionXScope.x && h < 0
            || player.playerTransform.position.x > player.positionXScope.y && h > 0)
        {
            inputDir.x = 0;
        }
        if (player.playerTransform.position.z < player.positionZScope.x && v < 0
            || player.playerTransform.position.z > player.positionZScope.y && v > 0)
        {
            inputDir.z = 0;
        }
        characterController.Move(Time.deltaTime * player.moveSpeed * inputDir);
    }
}