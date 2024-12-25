using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spider_Controller : AIBase
{
    [SerializeField] private float walkSpeed = 3;
    [SerializeField] private float runSpeed = 4;
    [SerializeField] private float retreatDistance = 4;

    public float WalkSpeed => walkSpeed;
    public float RunSpeed => runSpeed;
    public float RetreatDistance => retreatDistance;

    public override void ChangeState(AIState state)
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
                StateMachine.ChangeState<Spider_PursueState>((int)state);
                break;
            case AIState.Attack:
                StateMachine.ChangeState<AI_AttackState>((int)state);
                break;
            case AIState.Dead:
                StateMachine.ChangeState<AI_DeadState>((int)state);
                break;
        }
    }
}
