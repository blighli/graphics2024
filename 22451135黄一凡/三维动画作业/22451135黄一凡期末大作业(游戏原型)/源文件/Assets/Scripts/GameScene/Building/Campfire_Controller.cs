using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

/// <summary>
/// 篝火控制器
/// </summary>
public class Campfire_Controller : BuildingBase
{
    [SerializeField] private new Light light;
    [SerializeField] private GameObject fire;
    [SerializeField] private AudioSource audioSource;
    private CampfireConfig campfireConfig;
    private CampfireData campfireData;
    private bool isOnGround;
    
    public override void OnPreview()
    {
        isOnGround = false;
        // 关闭粒子和火焰效果
        SetLight(0);
    }

    public override void Init(MapChunkController mapChunk, ulong id, bool isFromBuild)
    {
        base.Init(mapChunk, id, isFromBuild);
        campfireConfig = ConfigManager.Instance.GetConfig<CampfireConfig>(ConfigName.Campfire);

        if (isFromBuild)
        {
            campfireData = new CampfireData();
            campfireData.CurrentFuel = campfireConfig.DefaultFuelValue;
            ArchiveManager.Instance.AddMapObjectTypeData(id, campfireData);
        }
        else
        {
            campfireData = ArchiveManager.Instance.GetMapObjectTypeData(id) as CampfireData;
        }
        SetLight(campfireData.CurrentFuel);
        isOnGround = true;
    }

    private void Update()
    {
        if (GameSceneManager.Instance.IsInitialized == false) return;
        if (isOnGround)
        {
            UpdateFuel();
        }
    }

    private void UpdateFuel()
    {
        if (campfireData.CurrentFuel == 0) return;
        // 减少燃料
        campfireData.CurrentFuel = Mathf.Clamp(campfireData.CurrentFuel - TimeManager.Instance.timeScale * Time.deltaTime * campfireConfig.BurningSpeed, 0,
            campfireConfig.MaxFuelValue);
        SetLight(campfireData.CurrentFuel);
    }

    private void SetLight(float fuelValue)
    {
        light.gameObject.SetActive(fuelValue!=0);
        fire.SetActive(fuelValue!=0);
        audioSource.gameObject.SetActive(fuelValue!=0);
        if (fuelValue != 0)
        {
            float value = fuelValue / campfireConfig.MaxFuelValue;
            // 设置亮度
            light.intensity = Mathf.Lerp(0, campfireConfig.MaxLightIntensity, value);
            // 设置范围
            light.range = Mathf.Lerp(0, campfireConfig.MaxLightRange, value);
            // 设置篝火音量
            audioSource.volume = value;
        }
    }

    public override bool OnSlotEndDragSelect(int itemID)
    {
        // 燃烧
        if (campfireConfig.TryGetFuelValueByItemID(itemID, out float fuelValue))
        {
            campfireData.CurrentFuel =
                Mathf.Clamp(campfireData.CurrentFuel + fuelValue, 0, campfireConfig.MaxFuelValue);
            SetLight(campfireData.CurrentFuel);
            return true;
        }
        // 烧烤
        if (campfireConfig.TryGetBakedItemByItemID(itemID, out int bakedItemID))
        {
            // 检查燃料
            if (campfireData.CurrentFuel <= 0)
            {
                UIManager.Instance.AddTips("需要点燃篝火");
                return false;
            }
            // 给玩家添加一个物品
            InventoryManager.Instance.AddMainInventoryItemAndPlayAudio(bakedItemID);
            return true;
        }
        return false;
    }
}
