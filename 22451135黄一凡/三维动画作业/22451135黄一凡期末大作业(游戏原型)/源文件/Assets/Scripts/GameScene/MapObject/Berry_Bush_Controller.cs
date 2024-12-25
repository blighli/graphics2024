using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

/// <summary>
/// 浆果控制器
/// </summary>
public class Berry_Bush_Controller : Bush_Controller,IBuilding
{
    [SerializeField] private MeshRenderer meshRenderer;
    [SerializeField] private Material[] materials;      // 0是有果子，1是没果子
    [SerializeField] private int berryGrowDayNum = 2;       // 浆果成长天数
    private Berry_BushTypeData typeData;

    #region 建筑物
    [SerializeField] private new Collider collider;
    private List<Material> materialList;
    public GameObject GameObject => gameObject;
    public Collider Collider => collider;
    public List<Material> MaterialList { get=>materialList; set => materialList = value; }
    public void OnPreview() { }

    #endregion
    
    public override void Init(MapChunkController mapChunk, ulong id, bool isFromBuild)
    {
        base.Init(mapChunk, id, isFromBuild);
        // 尝试获取类型数据的存档
        if (ArchiveManager.Instance.TryGetMapObjectTypeData(id, out IMapObjectTypeData tempData))
        {
            typeData = tempData as Berry_BushTypeData;
        }
        else
        {
            typeData = new Berry_BushTypeData();
            ArchiveManager.Instance.AddMapObjectTypeData(id, typeData);
        }
        if (isFromBuild)
        {
            // 来自建筑物建造的情况下，直接视为刚刚采摘（这件事情也需要持久化）
            typeData.lastPickUpDayNum = TimeManager.Instance.CurrentDayNum;
        }
        CheckAndSetState();
        EventManager.AddEventListener(EventName.OnMorn, OnMorn);
    }

    public override int OnPickUp()
    {
        meshRenderer.sharedMaterial = materials[1];
        canPickUp = false;
        typeData.lastPickUpDayNum = TimeManager.Instance.CurrentDayNum;
        return pickUpItemConfigID;
    }

    private void CheckAndSetState()
    {
        // 有没有采摘过
        if (typeData.lastPickUpDayNum == -1)
        {
            meshRenderer.sharedMaterial = materials[0];
            canPickUp = true;
        }
        else
        {
            // 根据时间决定状态
            if (TimeManager.Instance.CurrentDayNum - typeData.lastPickUpDayNum >= berryGrowDayNum)
            {
                meshRenderer.sharedMaterial = materials[0];
                canPickUp = true;
            }
            else
            {
                meshRenderer.sharedMaterial = materials[1];
                canPickUp = false;
            }
        }
    }
    
    private void OnMorn()
    {
        if (canPickUp == false) CheckAndSetState();
    }
}
