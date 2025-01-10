using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_Dead : PlayerStateBase
{
    public override void Enter()
    {
        player.PlayAnimation("Dead");
        player.Collider.enabled = false;
    }
}
