using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 玩家状态
/// </summary>
public enum PlayerState
{
    Idle,
    Move,
    Attack,
    Hurt,
    Dead
}

/// <summary>
/// 玩家控制器
/// </summary>
public class Player_Controller : SingletonMono<Player_Controller>, IStateMachineOwner
{
    [SerializeField] private Player_Model playerModel;
    [SerializeField] private Animator animator;
    [SerializeField] private new Collider collider;
    public CharacterController characterController;
    private StateMachine stateMachine;
    
    public Transform playerTransform { get; private set; }
    private PlayerConfig playerConfig;
    public float rotateSpeed { get => playerConfig.RotateSpeed; }
    public float moveSpeed { get => playerConfig.MoveSpeed;}
    public Vector2 positionXScope { get; private set; } // X的范围
    public Vector2 positionZScope { get; private set; } // Z的范围

    public Collider Collider => collider;
    public bool CanUseItem { get; private set; } = true;
    public bool CanPickUpItem { get; private set; } = true;

    #region 存档相关数据

    private PlayerTransformData playerTransformData;
    private PlayerMainData playerMainData;

    #endregion

    #region 初始化
    public void Init(float mapSizeOnWorld)
    {
        // 确定配置
        playerConfig = ConfigManager.Instance.GetConfig<PlayerConfig>(ConfigName.Player);
        // 确定存档
        playerTransformData = ArchiveManager.Instance.PlayerTransformData;
        playerMainData = ArchiveManager.Instance.PlayerMainData;
        
        playerModel.Init(PlayAudioOnFootstep, OnStartHit, OnStopHit, OnAttackOverHit, OnHurtOver, OnDeadOver);
        playerTransform = transform;
        stateMachine = ResManager.Load<StateMachine>();
        stateMachine.Init(this);
        // 初始状态为待机
        stateMachine.ChangeState<Player_Idle>((int)PlayerState.Idle);
        InitPositionScope(mapSizeOnWorld);
        
        // 初始化存档相关的数据
        playerTransform.localPosition = playerTransformData.Position;
        playerTransform.localRotation = Quaternion.Euler(playerTransformData.Rotation);
        
        // 触发初始化相关事件
        TriggerUpdateHPEvent();
        TriggerUpdateHungryEvent();
        
        EventManager.AddEventListener(EventName.SaveGame, OnGameSave);
    }

    // 初始化坐标范围
    private void InitPositionScope(float mapSizeOnWorld)
    {
        positionXScope = new Vector2(1, mapSizeOnWorld - 1);
        positionZScope = new Vector2(1, mapSizeOnWorld - 1);
    }
    #endregion

    #region 辅助/工具函数
    public void PlayAudioOnFootstep(int index)
    {
        AudioManager.Instance.PlayOnShot(playerConfig.FootstepAudioClips[index], playerTransform.position, 0.5f);
    }
    
    public void ChangeState(PlayerState playerState)
    {
        switch (playerState)
        {
            case PlayerState.Idle:
                canAttack = true;
                CanUseItem = true;
                CanPickUpItem = true;
                stateMachine.ChangeState<Player_Idle>((int)playerState);
                break;
            case PlayerState.Move:
                canAttack = true;
                CanUseItem = true;
                CanPickUpItem = false;
                stateMachine.ChangeState<Player_Move>((int)playerState);
                break;
            case PlayerState.Attack:
                canAttack = false;
                CanUseItem = false;
                CanPickUpItem = false;
                stateMachine.ChangeState<Player_Attack>((int)playerState);
                break;
            case PlayerState.Hurt:
                canAttack = false;
                CanUseItem = false;
                CanPickUpItem = false;
                stateMachine.ChangeState<Player_Hurt>((int)playerState);
                break;
            case PlayerState.Dead:
                canAttack = false;
                CanUseItem = false;
                CanPickUpItem = false;
                stateMachine.ChangeState<Player_Dead>((int)playerState);
                break;
        }
    }
    
    /// <summary>
    /// 播放动画
    /// </summary>
    public void PlayAnimation(string animationName, float fixedTime = 0.25f)
    {
        animator.CrossFadeInFixedTime(animationName, fixedTime);
    }
    #endregion

    #region 核心数值
    // 计算饥饿值
    private void CalculateHungryOnUpdate()
    {
        if (playerMainData.Hungry > 0)
        {
            playerMainData.Hungry -= Time.deltaTime * playerConfig.HungryReduceSpeed;
            if (playerMainData.Hungry < 0) playerMainData.Hungry = 0;
            // 更新UI
            TriggerUpdateHungryEvent();
        }
        else
        {
            if (playerMainData.Hp > 0)
            {
                playerMainData.Hp -= Time.deltaTime * playerConfig.HpReduceSpeedOnHungryIsZero;
                if (playerMainData.Hp <= 0)
                {
                    playerMainData.Hp = 0;
                    ChangeState(PlayerState.Dead);
                }
                TriggerUpdateHPEvent();
            }
        }
    }

    private void TriggerUpdateHPEvent()
    {
        EventManager.EventTrigger(EventName.UpdatePlayerHP, playerMainData.Hp);
    }
    
    private void TriggerUpdateHungryEvent()
    {
        EventManager.EventTrigger(EventName.UpdatePlayerHungry, playerMainData.Hungry);
    }

    public void RecoverHP(float value)
    {
        playerMainData.Hp = Mathf.Clamp(playerMainData.Hp + value, 0, playerConfig.MaxHp);
        TriggerUpdateHPEvent();
    }

    public void RecoverHungry(float value)
    {
        playerMainData.Hungry = Mathf.Clamp(playerMainData.Hungry + value, 0, playerConfig.MaxHungry);
        TriggerUpdateHungryEvent();
    }
    #endregion

    #region 武器相关

    private ItemData currentWeaponItemData;
    private GameObject currentWeaponGameObject;
    public void ChangeWeapon(ItemData newWeapon)
    {
        if (currentWeaponItemData == newWeapon) return;
        if (currentWeaponItemData != null)
        {
            // 塞进对象池是基于GameObject.name的，所以不能同名
            currentWeaponGameObject.JKGameObjectPushPool();
        }
        if (newWeapon != null)
        {
            ItemWeaponInfo newWeaponInfo = newWeapon.Config.ItemTypeInfo as ItemWeaponInfo;
            currentWeaponGameObject =
                PoolManager.Instance.GetGameObject(newWeaponInfo.PrefabOnPlayer, playerModel.WeaponRoot);
            currentWeaponGameObject.transform.localPosition = newWeaponInfo.PositionOnPlayer;
            currentWeaponGameObject.transform.localRotation = Quaternion.Euler(newWeaponInfo.RotationOnPlayer);
            animator.runtimeAnimatorController = newWeaponInfo.AnimatorController;
        }
        else
        {
            animator.runtimeAnimatorController = playerConfig.NormalAnimatorController;
        }
        // 由于动画是基于状态机驱动的 如果不重新激活一次状态机，动画会错误（比如在移动过程中，突然切换AnimatorController会不播放走路动画）
        stateMachine.ChangeState<Player_Idle>((int)PlayerState.Idle, true);
        currentWeaponItemData = newWeapon;
    }

    #endregion

    #region 战斗、伐木采摘
    private bool canAttack = true;
    public Quaternion attackDir { get; private set; }
    private List<MapObjectBase> lastAttackedMapObjectList = new();
    private MapObjectBase lastHitMapObject;
    
    /// <summary>
    /// 当选择地图对象时
    /// </summary>
    public void OnSelectMapObject(RaycastHit hitInfo, bool isMouseButtonDown)
    {
        if (hitInfo.collider.TryGetComponent(out MapObjectBase mapObject))
        {
            float dis = Vector3.Distance(playerTransform.position, mapObject.transform.position);
            // 不在交互范围内
            if (dis > mapObject.TouchDistance)
            {
                if (isMouseButtonDown)
                {
                    UIManager.Instance.AddTips("离近一点哦！");
                    ProjectTool.PlayAudio(AudioType.Fail);
                }
                return;
            }
            // 判断拾取
            if (mapObject.CanPickUp)
            {
                if (CanPickUpItem == false) return;
                if (!isMouseButtonDown) return;
                lastHitMapObject = null;
                // 判断背包已满
                // 获取捡到的东西
                int itemConfigID = mapObject.PickUpItemConfigID;
                if (itemConfigID != -1)
                {
                    // 如果数据添加到背包成功，则销毁地图物体
                    if (InventoryManager.Instance.AddMainInventoryItem(itemConfigID))
                    {
                        mapObject.OnPickUp();
                        // 播放捡东西的动画
                        PlayAnimation("PickUp");
                        ProjectTool.PlayAudio(AudioType.Bag);
                    }
                    else
                    {
                        if (isMouseButtonDown)
                        {
                            UIManager.Instance.AddTips("背包已经满了！");
                            ProjectTool.PlayAudio(AudioType.Fail);
                        }
                    }
                }
                return;
            }
            if (!canAttack) return;
            if (lastHitMapObject != mapObject && !isMouseButtonDown) return;
            lastHitMapObject = mapObject;
            // 判断攻击
            // 根据玩家选中的地图对象类型以及当前角色的武器来判断做什么
            switch (mapObject.ObjectType)
            {
                case MapObjectType.Tree:
                    // 允许在两米内挥斧头砍树
                    if (!CheckHitMapObject(mapObject, WeaponType.Axe) && isMouseButtonDown)
                    {
                        UIManager.Instance.AddTips("你需要装备斧头！");
                        ProjectTool.PlayAudio(AudioType.Fail);
                    }
                    break;
                case MapObjectType.Stone:
                    if (!CheckHitMapObject(mapObject, WeaponType.PickAxe) && isMouseButtonDown)
                    {
                        UIManager.Instance.AddTips("你需要装备镐！");
                        ProjectTool.PlayAudio(AudioType.Fail);
                    }
                    break;
                case MapObjectType.Bush:
                    if (!CheckHitMapObject(mapObject, WeaponType.Sickle) && isMouseButtonDown)
                    {
                        UIManager.Instance.AddTips("你需要装备镰刀！");
                        ProjectTool.PlayAudio(AudioType.Fail);
                    }
                    break;
            }
            return;
        }
        if (canAttack && currentWeaponItemData != null && hitInfo.collider.TryGetComponent(out AIBase aiObject))
        {
            float dis = Vector3.Distance(playerTransform.position, aiObject.transform.position);
            ItemWeaponInfo itemWeaponInfo = currentWeaponItemData.Config.ItemTypeInfo as ItemWeaponInfo;
            // 交互距离：武器的长度 + AI的半径
            if (dis < itemWeaponInfo.AttackDistance + aiObject.Radius)
            {
                // 计算方向
                attackDir = Quaternion.LookRotation(aiObject.transform.position - transform.position);
                // 播放攻击音效
                AudioManager.Instance.PlayOnShot(itemWeaponInfo.AttackAudio, transform.position);
                // 切换状态
                ChangeState(PlayerState.Attack);
            }
        }
    }
    
    // 检测击打地图物体
    public bool CheckHitMapObject(MapObjectBase mapObject, WeaponType weaponType)
    {
        if (currentWeaponItemData == null) return false;
        ItemWeaponInfo itemWeaponInfo = currentWeaponItemData.Config.ItemTypeInfo as ItemWeaponInfo;
        // 有武器且符合要求
        if (itemWeaponInfo.WeaponType == weaponType)
        {
            // 计算方向
            attackDir = Quaternion.LookRotation(mapObject.transform.position - transform.position);
            // 播放攻击音效
            AudioManager.Instance.PlayOnShot(itemWeaponInfo.AttackAudio, transform.position);
            // 切换状态
            ChangeState(PlayerState.Attack);
            return true;
        }
        return false;
    }

    public void Hurt(float damage)
    {
        if (playerMainData.Hp <= 0) return;
        playerMainData.Hp -= damage;
        if (playerMainData.Hp <= 0)
        {
            playerMainData.Hp = 0;
            TriggerUpdateHPEvent();
            ChangeState(PlayerState.Dead);
        }
        else
        {
            TriggerUpdateHPEvent();
            ChangeState(PlayerState.Hurt);
        }
    }
    
    private void OnHurtOver()
    {
        // 切换状态到待机
        ChangeState(PlayerState.Idle);
    }
    
    private void OnDeadOver()
    {
        // 角色死亡，游戏结束
        GameSceneManager.Instance.GameOver();
    }
    #endregion
    
    private void Update()
    {
        if (GameSceneManager.Instance.IsInitialized == false) return;
        CalculateHungryOnUpdate();
    }

    // 让武器开始伤害检测
    private void OnStartHit()
    {
        attackSucceedCount = 0;
        currentWeaponGameObject.transform.OnTriggerEnter(OnWeaponTriggerEnter);
    }

    // 让武器停止伤害检测
    public void OnStopHit()
    {
        currentWeaponGameObject.transform.RemoveTriggerEnter(OnWeaponTriggerEnter);
        lastAttackedMapObjectList.Clear();
    }
    // 整个攻击状态的结束
    private void OnAttackOverHit()
    {
        // 攻击命中几次，就损耗几次
        for (int i = 0; i < attackSucceedCount; i++)
        {
            // 武器损耗
            EventManager.EventTrigger(EventName.PlayerWeaponAttackSucceed);
        }
        // 切换状态到待机
        ChangeState(PlayerState.Idle);
    }
    
    // 攻击成功的数量
    private int attackSucceedCount;
    private void OnWeaponTriggerEnter(Collider other, object[] arg2)
    {
        // 对方是地图对象才有意义
        if (other.TryGetComponent(out HitMapObjectBase mapObject))
        {
            // 已经攻击过的，防止二次伤害
            if (lastAttackedMapObjectList.Contains(mapObject)) return;
            lastAttackedMapObjectList.Add(mapObject);
            // 检测对方是什么类型 以及 自己手上是什么武器
            switch (mapObject.ObjectType)
            {
                case MapObjectType.Tree:
                    CheckMapObjectHurt(mapObject, WeaponType.Axe);
                    break;
                case MapObjectType.Stone:
                    CheckMapObjectHurt(mapObject, WeaponType.PickAxe);
                    break;
                case MapObjectType.Bush:
                    CheckMapObjectHurt(mapObject, WeaponType.Sickle);
                    break;
            }
        }
        else if (other.TryGetComponent(out AIBase aiObject))
        {
            ItemWeaponInfo itemWeaponInfo = currentWeaponItemData.Config.ItemTypeInfo as ItemWeaponInfo;
            AudioManager.Instance.PlayOnShot(itemWeaponInfo.HitAudio, transform.position);
            GameObject effect = PoolManager.Instance.GetGameObject(itemWeaponInfo.HitEffect);
            effect.transform.position = other.ClosestPoint(currentWeaponGameObject.transform.position);
            // 受伤
            aiObject.Hurt(itemWeaponInfo.AttackValue);
            attackSucceedCount += 1;
        }
    }

    /// <summary>
    /// 检测地图对象能否受伤
    /// </summary>
    private void CheckMapObjectHurt(HitMapObjectBase hitMapObject, WeaponType weaponType)
    {
        ItemWeaponInfo itemWeaponInfo = currentWeaponItemData.Config.ItemTypeInfo as ItemWeaponInfo;
        if (itemWeaponInfo.WeaponType == weaponType)
        {
            // 受伤
            AudioManager.Instance.PlayOnShot(itemWeaponInfo.HitAudio, transform.position);
            hitMapObject.Hurt(itemWeaponInfo.AttackValue);
            attackSucceedCount += 1;
        }
    }
    
    private void OnGameSave()
    {
        // 把存档数据实际写入磁盘
        playerTransformData.Position = playerTransform.localPosition;
        playerTransformData.Rotation = playerTransform.localRotation.eulerAngles;
        ArchiveManager.Instance.SavePlayerTransformData();
        ArchiveManager.Instance.SavePlayerMainData();
    }

    private void OnDestroy()
    {
        if (stateMachine != null)
        {
            stateMachine.Destory();
            stateMachine = null;
        }
    }
}
