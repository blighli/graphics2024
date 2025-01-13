using JKFrame;
using UnityEngine;

public class AI_AttackState : AIStateBase
{
    public override void Enter()
    {
        // 随机播放一个攻击动作
        int index = Random.Range(1, 3);
        AI.PlayAnimation("Attack_"+index.ToString());
        AI.transform.LookAt(Player_Controller.Instance.transform);
        AI.PlayAudio("Attack");
        AI.AddAnimationEvent("StartHit", StartHit);
        AI.AddAnimationEvent("StopHit", StopHit);
        AI.AddAnimationEvent("AttackOver", AttackOver);
        AI.Weapon.OnTriggerStay(CheckHitOnTriggerStay);
    }

    public override void Exit()
    {
        AI.RemoveAnimationEvent("StartHit", StartHit);
        AI.RemoveAnimationEvent("StopHit", StopHit);
        AI.RemoveAnimationEvent("AttackOver", AttackOver);
        AI.Weapon.RemoveTriggerStay(CheckHitOnTriggerStay);
    }

    // 开启伤害
    private void StartHit()
    {
        AI.Weapon.gameObject.SetActive(true);
    }

    private bool isAttacked = false;
    // 武器的伤害检测
    private void CheckHitOnTriggerStay(Collider other, object[] args)
    {
        // 避免一次攻击产生多次伤害
        if (isAttacked) return;
        if (other.gameObject.CompareTag("Player"))
        {
            isAttacked = true;
            AI.PlayAudio("Hit");
            Player_Controller.Instance.Hurt(AI.AttackValue);
        }
    }
    
    // 关闭伤害
    private void StopHit()
    {
        isAttacked = false;
        AI.Weapon.gameObject.SetActive(false);
    }
    // 攻击结束
    private void AttackOver()
    {
        AI.ChangeState(AIState.Pursue);
    }
}
