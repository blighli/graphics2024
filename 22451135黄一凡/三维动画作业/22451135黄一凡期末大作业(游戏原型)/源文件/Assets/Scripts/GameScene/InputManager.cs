using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using UnityEngine.EventSystems;
using UnityEngine.Serialization;

/// <summary>
/// 输入管理器
/// </summary>
public class InputManager : SingletonMono<InputManager>
{
    private List<RaycastResult> raycastResultList = new List<RaycastResult>();
    [SerializeField] private LayerMask bigMapObjectLayer;       // 大型地图对象层
    [SerializeField] private LayerMask mouseMapObjectLayer;     // 鼠标可交互地图对象层
    [SerializeField] private LayerMask groundLayer;             // 地面层
    [SerializeField] private LayerMask buildingLayer;             // 地面层
    private bool wantCheck = false;                             // 需要检测
    
    /// <summary>
    /// 初始化
    /// </summary>
    public void Init()
    {
        SetCheckState(true);
    }

    private void Update()
    {
        if (GameSceneManager.Instance.IsInitialized == false) return;
        CheckSelectMapObject();
    }

    public void SetCheckState(bool wantCheck)
    {
        this.wantCheck = wantCheck;
    }

    /// <summary>
    /// 检查选中的地图物体
    /// </summary>
    private void CheckSelectMapObject()
    {
        if (wantCheck == false) return;
        bool mouseButtonDown = Input.GetMouseButtonDown(0);
        bool mouseButton = Input.GetMouseButton(0);
        if (mouseButtonDown || mouseButton)
        {
            // 如果检测到UI则无视
            if (CheckMouseOnUI()) return;
            // 射线检测地图上的3D物体
            Ray ray = Camera_Controller.Instance.Camera.ScreenPointToRay(Input.mousePosition);
            if (Physics.Raycast(ray,out RaycastHit hitInfo,100,mouseMapObjectLayer))
            {
                Player_Controller.Instance.OnSelectMapObject(hitInfo, mouseButtonDown);
            }
            // 检测建筑物
            if (mouseButtonDown && Physics.Raycast(ray,out hitInfo,100,buildingLayer))
            {
                BuildingBase building = hitInfo.collider.GetComponent<BuildingBase>();
                // 距离的验证
                if (building.TouchDistance > 0)
                {
                    if (Vector3.Distance(Player_Controller.Instance.playerTransform.position, building.transform.position) < building.TouchDistance)
                    {
                        building.OnSelect();
                    }
                    else
                    {
                        UIManager.Instance.AddTips("离近一点！");
                        ProjectTool.PlayAudio(AudioType.Fail);
                    }
                }
            }
        }
    }
    
    public bool CheckMouseOnUI()
    {
        PointerEventData pointerEventData = new PointerEventData(EventSystem.current);
        pointerEventData.position = Input.mousePosition;
        
        // 射线去检测有没有除了Mask以外的任何UI物体
        EventSystem.current.RaycastAll(pointerEventData, raycastResultList);
        for (int i = 0; i < raycastResultList.Count; i++)
        {
            RaycastResult raycastResult = raycastResultList[i];
            // 是UI同时还不是Mask作用的物体
            if (raycastResult.gameObject.TryGetComponent<RectTransform>(out var temp)&&
                raycastResult.gameObject.name != "Mask")
            {
                raycastResultList.Clear();
                return true;
            }
        }
        raycastResultList.Clear();
        return false;
    }

    /// <summary>
    /// 检查鼠标是否在大型地图对象上方
    /// </summary>
    public bool CheckMouseOnBigMapObject()
    {
        return Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), 1000, bigMapObjectLayer);
    }
    
    public bool GetMouseWorldPositionOnGround(Vector3 mousePos, out Vector3 worldPos)
    {
        if (Physics.Raycast(Camera.main.ScreenPointToRay(mousePos), out RaycastHit hitInfo, 1000, groundLayer))
        {
            worldPos = hitInfo.point;
            return true;
        }
        worldPos = Vector3.zero;
        return false;
    }

    public bool CheckSlotEndDragOnBuilding(int itemID)
    {
        Ray ray = Camera_Controller.Instance.Camera.ScreenPointToRay(Input.mousePosition);
        if (Physics.Raycast(ray, out RaycastHit hitInfo, 100, buildingLayer))
        {
            BuildingBase building = hitInfo.collider.GetComponent<BuildingBase>();
            return building.OnSlotEndDragSelect(itemID);
        }
        return false;
    }
}
