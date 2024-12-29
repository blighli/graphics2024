using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 玩家状态基类
/// 抽象出所有玩家状态所需要的共同 字段、函数等
/// </summary>
public class PlayerStateBase : StateBase
{
    protected Player_Controller player;
    public override void Init(IStateMachineOwner owner, int stateType, StateMachine stateMachine)
    {
        base.Init(owner, stateType, stateMachine);
        player = owner as Player_Controller;
    }
}
