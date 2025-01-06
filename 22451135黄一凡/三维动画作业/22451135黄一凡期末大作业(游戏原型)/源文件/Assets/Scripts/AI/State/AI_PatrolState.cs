using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// AI的巡逻状态
/// </summary>
public class AI_PatrolState : AIStateBase
{
    private Vector3 target;
    public override void Enter()
    {
        AI.NavMeshAgent.enabled = true;
        target = AI.GetAIRandomPoint();
        AI.PlayAnimation("Move");
        AI.NavMeshAgent.SetDestination(target);
        // 添加脚步声事件
        AI.AddAnimationEvent("Footstep", Footstep);
        
    }

    public override void Update()
    {
        AI.SavePosition();
        if (AI.HostileDistance > 0 && GameSceneManager.Instance.IsGameOver == false)
        {
            // 判断敌对距离
            if (Vector3.Distance(AI.transform.position, Player_Controller.Instance.playerTransform.position) <
                AI.HostileDistance)
            {
                AI.ChangeState(AIState.Pursue);
                return;
            }
        }
        // 检测是否到达目标
        if (Vector3.Distance(AI.transform.position, target) < 0.5f)
        {
            AI.ChangeState(AIState.Idle);
        }
    }

    public override void Exit()
    {
        AI.NavMeshAgent.enabled = false;
        // 移除脚步声事件
        AI.RemoveAnimationEvent("Footstep", Footstep);
    }
    
    private void Footstep()
    {
        int index = Random.Range(1, 3);
        AI.PlayAudio("Footstep"+index.ToString(),0.5f);
    }
}
