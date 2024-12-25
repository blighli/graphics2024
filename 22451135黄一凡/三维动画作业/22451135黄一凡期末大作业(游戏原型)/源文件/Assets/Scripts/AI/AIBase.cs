using System;
using System.Collections.Generic;
using JKFrame;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.AI;

public abstract class AIBase : SerializedMonoBehaviour,IStateMachineOwner
{
    #region 组件
    [SerializeField] protected Animator animator;
    [SerializeField] protected Collider inputCheckCollider;
    public Collider InputCheckCollider => inputCheckCollider;
    [SerializeField] protected NavMeshAgent navMeshAgent;
    public NavMeshAgent NavMeshAgent => navMeshAgent;
    [SerializeField] protected Transform weapon;
    public Transform Weapon => weapon;
    #endregion

    #region 参数
    protected float hp;
    [SerializeField] protected MapVertexType mapVertexType;   // AI活动的地图类型范围
    [SerializeField] protected float maxHP = 10;
    [SerializeField] protected float attackDistance = 0.5f;
    [SerializeField] protected float attackValue = 10f;
    [SerializeField] protected float radius;
    [SerializeField] protected int lootConfigID = -1;
    [SerializeField] protected float hostileDistance = -1;
    public float AttackDistance => attackDistance;
    public float AttackValue => attackValue;
    public float Radius => radius;
    public float HostileDistance => hostileDistance;
    #endregion

    #region 数据
    protected AIState currentState;
    protected StateMachine stateMachine;
    public StateMachine StateMachine
    {
        get
        {
            if (stateMachine == null)
            {
                stateMachine = PoolManager.Instance.GetObject<StateMachine>();
                StateMachine.Init(this);
            }
            return stateMachine;
        }
    }
    [SerializeField] protected Dictionary<string, AudioClip> audioDic = new();
    protected MapChunkController mapChunk;
    public MapChunkController MapChunk => mapChunk;
    private MapObjectData aiData;
    public MapObjectData AIData => aiData;
    #endregion
    
    public virtual void Init(MapChunkController mapChunk, MapObjectData aiData)
    {
        this.mapChunk = mapChunk;
        this.aiData = aiData;
        transform.position = aiData.Position;
        hp = maxHP;
        ChangeState(AIState.Idle);
    }

    public virtual void InitOnTransfer(MapChunkController mapChunk)
    {
        this.mapChunk = mapChunk;
    }
    
    public virtual void ChangeState(AIState state)
    {
        currentState = state;
        switch (state)
        {
            case AIState.Idle:
                StateMachine.ChangeState<AI_IdleState>((int)state);
                break;
            case AIState.Patrol:
                StateMachine.ChangeState<AI_PatrolState>((int)state);
                break;
            case AIState.Hurt:
                StateMachine.ChangeState<AI_HurtState>((int)state, true);
                break;
            case AIState.Pursue:
                StateMachine.ChangeState<AI_PursueState>((int)state);
                break;
            case AIState.Attack:
                StateMachine.ChangeState<AI_AttackState>((int)state);
                break;
            case AIState.Dead:
                StateMachine.ChangeState<AI_DeadState>((int)state);
                break;
            default:
                throw new ArgumentOutOfRangeException(nameof(state), state, null);
        }
    }
    
    public void PlayAnimation(string animationName, float fixedTime = 0.25f)
    {
        animator.CrossFadeInFixedTime(animationName, fixedTime);
    }

    public void PlayAudio(string audioName, float volumeScale = 1)
    {
        if (audioDic.TryGetValue(audioName, out AudioClip audioClip))
        {
            AudioManager.Instance.PlayOnShot(audioClip, transform.position, volumeScale);
        }
    }
    
    public Vector3 GetAIRandomPoint()
    {
        return mapChunk.GetAIRandomPoint(mapVertexType);
    }

    /// <summary>
    /// 保存坐标
    /// </summary>
    public void SavePosition()
    {
        aiData.Position = transform.position;
    }

    public virtual void Hurt(float damage)
    {
        if (hp <= 0) return;
        hp -= damage;
        if (hp<=0)
        {
            ChangeState(AIState.Dead);
        }
        else
        {
            ChangeState(AIState.Hurt);
        }
    }
    
    /// <summary>
    /// 仅考虑自身游戏物体的销毁，而不考虑存档层面的问题
    /// </summary>
    public void Destroy()
    {
        this.JKGameObjectPushPool();
        currentState = AIState.None;
        stateMachine.Stop();
    }
    
    /// <summary>
    /// 死亡
    /// 数据层面、游戏物体都要销毁
    /// </summary>
    public void Dead()
    {
        // 告诉地图块移除自己
        mapChunk.RemoveAIObject(aiData.ID);
        // 物品掉落逻辑
        if (lootConfigID == -1) return;
        LootConfig lootConfig = ConfigManager.Instance.GetConfig<LootConfig>(ConfigName.Loot, lootConfigID);
        if (lootConfig != null) lootConfig.GenerateMapObject(mapChunk, transform.position + new Vector3(0, 1, 0));
    }
    #region 动画事件
    private Dictionary<string, Action> animationEventDic = new(5);
    private void AnimationEvent(string eventName)
    {
        if (animationEventDic.TryGetValue(eventName, out Action action))
        {
            action?.Invoke();
        }
    }

    public void AddAnimationEvent(string eventName, Action action)
    {
        if (animationEventDic.TryGetValue(eventName, out Action _action))
        {
            _action += action;
        }
        else
        {
            animationEventDic.Add(eventName, action);
        }
    }

    public void RemoveAnimationEvent(string eventName, Action action)
    {
        if (animationEventDic.TryGetValue(eventName, out Action _action))
        {
            _action -= action;
        }
    }
    
    public void RemoveAnimationEvent(string eventName)
    {
        animationEventDic.Remove(eventName);
    }
    
    public void ClearAllAnimationEvent()
    {
        animationEventDic.Clear();
    }
    #endregion
}
