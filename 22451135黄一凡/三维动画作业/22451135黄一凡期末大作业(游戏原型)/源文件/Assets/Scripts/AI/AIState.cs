using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// AI状态枚举
/// </summary>
public enum AIState
{
    None,
    Idle,
    Patrol,
    Hurt,
    Pursue,
    Attack,
    Dead
}
