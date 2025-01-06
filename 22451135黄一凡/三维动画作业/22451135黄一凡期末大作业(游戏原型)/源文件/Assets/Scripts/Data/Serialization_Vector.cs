using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

[Serializable]
public struct Serialization_Vector3
{
    public float x, y, z;

    public Serialization_Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public override string ToString()
    {
        return $"({x},{y},{z})";
    }
}

[Serializable]
public struct Serialization_Vector2
{
    public float x, y;

    public Serialization_Vector2(float x, float y)
    {
        this.x = x;
        this.y = y;
    }

    public override string ToString()
    {
        return $"({x},{y})";
    }
}

public static class SerializationVectorExtensions
{
    /// <summary>
    /// SV3 转 Unity V3
    /// </summary>
    public static Vector3 ConvertToVector3(this Serialization_Vector3 sv3)
    {
        return new Vector3(sv3.x, sv3.y, sv3.z);
    }
    /// <summary>
    /// V3 转 SV3
    /// </summary>
    public static Serialization_Vector3 ConvertToSVector3(this Vector3 v3)
    {
        return new Serialization_Vector3(v3.x, v3.y, v3.z);
    }
    
    /// <summary>
    /// SV2 转 Unity V2
    /// </summary>
    public static Vector2 ConvertToVector2(this Serialization_Vector2 sv2)
    {
        return new Vector2(sv2.x, sv2.y);
    }
    /// <summary>
    /// SV2 转 Unity V2
    /// </summary>
    public static Vector2Int ConvertToVector2Int(this Serialization_Vector2 sv2)
    {
        return new Vector2Int((int)sv2.x, (int)sv2.y);
    }
    /// <summary>
    /// V2 转 SV2
    /// </summary>
    public static Serialization_Vector2 ConvertToSVector2(this Vector2 v2)
    {
        return new Serialization_Vector2(v2.x, v2.y);
    }
    /// <summary>
    /// V2Int 转 SV2
    /// </summary>
    public static Serialization_Vector2 ConvertToSVector2(this Vector2Int v2)
    {
        return new Serialization_Vector2(v2.x, v2.y);
    }
}