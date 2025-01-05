using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_Model : MonoBehaviour
{
    [SerializeField] private Transform weaponRoot;
    public Transform WeaponRoot { get => weaponRoot; }
    
    private Action<int> footstepAction;
    private Action startHitAction;
    private Action stopHitAction;
    private Action attackOverAction;
    private Action hurtOverAction;
    private Action deadOverAction;
    
    public void Init(Action<int> footstepAction, Action startHitAction, Action stopHitAction, Action attackOverAction, Action hurtOverAction, Action deadOverAction)
    {
        this.footstepAction = footstepAction;
        this.startHitAction = startHitAction;
        this.stopHitAction = stopHitAction;
        this.attackOverAction = attackOverAction;
        this.hurtOverAction = hurtOverAction;
        this.deadOverAction = deadOverAction;
    }

    #region 动画事件
    // 脚步声
    private void Footstep(int index)
    {
        footstepAction?.Invoke(index);
    }
    // 开始有伤害
    private void StartHit()
    {
        startHitAction?.Invoke();
    }
    // 没有伤害
    private void StopHit()
    {
        stopHitAction?.Invoke();
    }
    // 攻击结束
    private void AttackOver()
    {
        attackOverAction?.Invoke();
    }
    // 受伤结束
    private void HurtOver()
    {
        hurtOverAction?.Invoke();
    }
    // 死亡结束
    private void DeadOver()
    {
        deadOverAction?.Invoke();
    }
    #endregion
}
