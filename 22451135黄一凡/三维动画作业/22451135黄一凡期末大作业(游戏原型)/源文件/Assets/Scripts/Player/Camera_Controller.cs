using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

public class Camera_Controller : SingletonMono<Camera_Controller>
{
    private Transform mTransform;
    [SerializeField] private Transform target;  // 跟随目标
    [SerializeField] private Vector3 offset;    // 跟随偏移量
    [SerializeField] private float moveSpeed;   // 跟随速度
    [SerializeField] private new Camera camera;   // 相机
    public Camera Camera { get => camera; }
    
    private Vector2 positionXScope; // X的范围
    private Vector2 positionZScope; // Z的范围

    public void Init(float mapSizeOnWorld)
    {
        mTransform = transform;
        InitPositionScope(mapSizeOnWorld);
    }

    // 初始化坐标范围
    private void InitPositionScope(float mapSizeOnWorld)
    {
        positionXScope = new Vector2(5, mapSizeOnWorld - 5);
        positionZScope = new Vector2(-1, mapSizeOnWorld - 5);
    }

    private void LateUpdate()
    {
        if (target!=null)
        {
            Vector3 targetPosition = target.position + offset;
            targetPosition.x = Mathf.Clamp(targetPosition.x, positionXScope.x, positionXScope.y);
            targetPosition.z = Mathf.Clamp(targetPosition.z, positionZScope.x, positionZScope.y);
            mTransform.position = Vector3.Lerp(transform.position, targetPosition, Time.deltaTime * moveSpeed);
        }
    }
}
