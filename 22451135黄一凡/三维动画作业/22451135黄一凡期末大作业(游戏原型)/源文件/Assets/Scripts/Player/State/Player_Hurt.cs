using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_Hurt : PlayerStateBase
{
    public override void Enter()
    {
        player.PlayAnimation("Hurt");
    }
}
