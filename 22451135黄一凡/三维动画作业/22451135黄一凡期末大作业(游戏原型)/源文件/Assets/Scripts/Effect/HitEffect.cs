using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

public class HitEffect : MonoBehaviour
{
    private void OnParticleSystemStopped()
    {
        this.JKGameObjectPushPool();
    }
}
