using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using UnityEngine.UI;

/// <summary>
/// 储物箱UI窗口
/// </summary>
[UIElement(true, "UI/UI_StorageBoxInventoryWindow", 1)]
public class UI_StorageBoxInventoryWindow : UI_InventoryWindowBase
{
    [SerializeField] private Button closeButton;
    [SerializeField] private Transform itemParent;
    private StorageBox_Controller storageBox;
    public override void Init()
    {
        base.Init();
        slots = new List<UI_ItemSlot>(10);
        closeButton.onClick.AddListener(CloseButtonClick);
    }

    public void Init(StorageBox_Controller storageBox, InventoryData inventoryData, Vector2Int size)
    {
        this.inventoryData = inventoryData;
        this.storageBox = storageBox;
        SetWindowSize(size);
        // 生成格子
        for (int i = 0; i < inventoryData.ItemDatas.Length; i++)
        {
            UI_ItemSlot slot = ResManager.Load<UI_ItemSlot>("UI/UI_ItemSlot", itemParent);
            slot.Init(i, this);
            slot.InitData(inventoryData.ItemDatas[i]);
            slots.Add(slot);
        }
    }

    private void Update()
    {
        if (Player_Controller.Instance != null)
        {
            if (Vector3.Distance(Player_Controller.Instance.transform.position, storageBox.transform.position) > storageBox.TouchDistance)
            {
                CloseButtonClick();
            }
        }
    }

    private void SetWindowSize(Vector2Int size)
    {
        // 宽度 = 两边15＋中间格子区域（一个格子100）
        // 高度 = 上边15＋中间格子区域（一个格子100）+底部15
        RectTransform rectTransform = transform as RectTransform;
        rectTransform.sizeDelta = new Vector2(30 + size.x * 100, 65 + size.y * 100);
    }

    private void CloseButtonClick()
    {
        ProjectTool.PlayAudio(AudioType.Bag);
        Close();
    }
    
    public override void OnClose()
    {
        base.OnClose();
        for (int i = 0; i < slots.Count; i++)
        {
            slots[i].JKGameObjectPushPool();
        }
        slots.Clear();
        inventoryData = null;
    }
}
