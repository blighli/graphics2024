using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

public class Spider_PursueState : AI_PursueState
{
    protected Spider_Controller spider;
    public override void Init(IStateMachineOwner owner, int stateType, StateMachine stateMachine)
    {
        base.Init(owner, stateType, stateMachine);
        spider = owner as Spider_Controller;
    }

    public override void Enter()
    {
        AI.NavMeshAgent.enabled = true;
        AI.PlayAnimation("Run");
        // 添加脚步声事件
        AI.AddAnimationEvent("Footstep", Footstep);
        AI.NavMeshAgent.speed = spider.RunSpeed;
    }

    public override void Update()
    {
        if (GameSceneManager.Instance.IsGameOver == false)
        {
            float distance = Vector3.Distance(AI.transform.position, Player_Controller.Instance.transform.position);
            if (distance < AI.Radius+AI.AttackDistance)
            {
                AI.ChangeState(AIState.Attack);
            }
            else
            {
                AI.NavMeshAgent.SetDestination(Player_Controller.Instance.transform.position);
                AI.SavePosition();
                // 如果远离则变为待机状态
                if (distance > spider.RetreatDistance)
                {
                    AI.ChangeState(AIState.Idle);
                    return;
                }
                // 检测AI的所属地图块
                CheckAndTransferMapChunk();
            }
        }
    }

    public override void Exit()
    {
        base.Exit();
        AI.NavMeshAgent.speed = spider.WalkSpeed;
    }
}
