using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// 玩家位置相关的数据
/// </summary>
[Serializable]
public class PlayerTransformData
{
    // 坐标
    private Serialization_Vector3 position;
    public Vector3 Position
    {
        get => position.ConvertToVector3();
        set => position = value.ConvertToSVector3();
    }
    
    // 旋转
    private Serialization_Vector3 rotation;
    public Vector3 Rotation
    {
        get => rotation.ConvertToVector3();
        set => rotation = value.ConvertToSVector3();
    }
}

[Serializable]
public class PlayerMainData
{
    public float Hp;
    public float Hungry;
}
