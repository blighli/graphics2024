using System.Collections;
using JKFrame;
using UnityEngine;

/// <summary>
/// AI的待机状态
/// </summary>
public class AI_IdleState : AIStateBase
{
    private Coroutine goPatrolCoroutine;
    public override void Enter()
    {
        // 播放待机动画
        AI.PlayAnimation("Idle");
        // 待机一段时间后去巡逻
        goPatrolCoroutine = MonoManager.Instance.StartCoroutine(GoPatrol());
        // 有一定概率猪叫
        if (Random.Range(0,15) == 0)
        {
            AI.PlayAudio("Idle", 1f);
        }
    }

    public override void Update()
    {
        if (AI.HostileDistance > 0 && GameSceneManager.Instance.IsGameOver == false)
        {
            // 判断敌对距离
            if (Vector3.Distance(AI.transform.position, Player_Controller.Instance.playerTransform.position) <
                AI.HostileDistance)
            {
                AI.ChangeState(AIState.Pursue);
            }
        }
    }

    IEnumerator GoPatrol()
    {
        // 随机待机0-6秒
        yield return CoroutineTool.WaitForSeconds(Random.Range(0, 6f));
        AI.ChangeState(AIState.Patrol);
    }

    public override void Exit()
    {
        if (goPatrolCoroutine != null)
        {
            MonoManager.Instance.StopCoroutine(goPatrolCoroutine);
            goPatrolCoroutine = null;
        }
    }
}
