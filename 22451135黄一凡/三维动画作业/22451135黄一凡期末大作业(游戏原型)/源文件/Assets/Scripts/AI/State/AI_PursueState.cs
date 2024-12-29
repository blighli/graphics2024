using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// AI追击状态
/// </summary>
public class AI_PursueState : AIStateBase
{
    public override void Enter()
    {
        AI.NavMeshAgent.enabled = true;
        AI.PlayAnimation("Move");
        // 添加脚步声事件
        AI.AddAnimationEvent("Footstep", Footstep);
    }

    public override void Update()
    {
        if (GameSceneManager.Instance.IsGameOver == false)
        {
            if (Vector3.Distance(AI.transform.position,Player_Controller.Instance.transform.position)<AI.Radius+AI.AttackDistance)
            {
                AI.ChangeState(AIState.Attack);
            }
            else
            {
                AI.NavMeshAgent.SetDestination(Player_Controller.Instance.transform.position);
                AI.SavePosition();
                // 检测AI的所属地图块
                CheckAndTransferMapChunk();
            }
        }
    }

    protected void CheckAndTransferMapChunk()
    {
        // 通关AI所在的坐标的地图块 和 AI归属的地图块作比较
        MapChunkController newMapChunk = MapManager.Instance.GetMapChunkByWorldPosition(AI.transform.position);
        if (newMapChunk != AI.MapChunk)
        {
            // 从当前地图块移除
            AI.MapChunk.RemoveAIObjectOnTransfer(AI.AIData.ID);
            // 加入新的地图块
            newMapChunk.AddAIObjectFromTransfer(AI.AIData, AI);
        }
    }
    
    public override void Exit()
    {
        AI.NavMeshAgent.enabled = false;
        // 移除脚步声事件
        AI.RemoveAnimationEvent("Footstep", Footstep);
    }
    
    protected void Footstep()
    {
        int index = Random.Range(1, 3);
        AI.PlayAudio("Footstep"+index.ToString(),0.5f);
    }
}
