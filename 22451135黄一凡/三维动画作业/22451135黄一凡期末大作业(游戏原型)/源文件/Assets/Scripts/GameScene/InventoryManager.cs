using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

public class InventoryManager : SingletonMono<InventoryManager>
{
    private UI_MainInventoryWindow mainInventoryWindow;     // 快捷栏
    public void Init()
    {
        mainInventoryWindow = UIManager.Instance.Show<UI_MainInventoryWindow>();
        mainInventoryWindow.InitData();
        EventManager.AddEventListener(EventName.SaveGame, OnGameSave);
    }

    #region 主背包
    public int GetMainInventoryItemCount(int configID)
    {
        return mainInventoryWindow.GetItemCount(configID);
    }

    public void UpdateMainInventoryItemsForBuild(BuildConfig buildConfig)
    {
        mainInventoryWindow.UpdateItemsForBuild(buildConfig);
    }

    public bool AddMainInventoryItem(int itemConfigID)
    {
        return mainInventoryWindow.AddItem(itemConfigID);
    }

    public bool AddMainInventoryItemAndPlayAudio(int itemConfigID)
    {
        return mainInventoryWindow.AddItemAndPlayAudio(itemConfigID);
    }
    #endregion

    #region 储物箱背包

    public void OpenStorageBoxWindow(StorageBox_Controller storageBox, InventoryData inventoryData, Vector2Int size)
    {
        ProjectTool.PlayAudio(AudioType.Bag);
        UIManager.Instance.Close<UI_StorageBoxInventoryWindow>();
        UIManager.Instance.Show<UI_StorageBoxInventoryWindow>().Init(storageBox, inventoryData, size);
    }

    #endregion
    private void OnGameSave()
    {
        ArchiveManager.Instance.SaveMainInventoryData();
    }
}
