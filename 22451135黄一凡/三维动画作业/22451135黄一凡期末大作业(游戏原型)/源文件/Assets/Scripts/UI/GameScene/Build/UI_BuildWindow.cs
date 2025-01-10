using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

[UIElement(false, "UI/UI_BuildWindow", 1)]
public class UI_BuildWindow : UI_WindowBase
{   
    // 所有的一级菜单选项
    [SerializeField] private UI_BuildWindow_MainMenuItem[] mainMenuItems;
    [SerializeField] private UI_BuildWindow_SecondaryMenu secondaryMenu;
    private UI_BuildWindow_MainMenuItem currentMenuItem;
    private bool isTouch = false;
    
    public override void Init()
    {
        // 初始化一级菜单全部选项
        for (int i = 0; i < mainMenuItems.Length; i++)
        {
            mainMenuItems[i].Init((BuildType)i, this);
        }
        secondaryMenu.Init();
    }

    private void Update()
    {
        if (isTouch)
        {
            if (RectTransformUtility.RectangleContainsScreenPoint(transform as RectTransform, Input.mousePosition) == false)
            {
                isTouch = false;
                CloseMenu();
                currentMenuItem.OnUnSelect();
            }
        }
    }

    private void CloseMenu()
    {
        secondaryMenu.Close();
    }
    
    /// <summary>
    /// 选择某一个一级菜单选项
    /// </summary>
    public void SelectMainMenuItem(UI_BuildWindow_MainMenuItem newMenuItem)
    {
        if (currentMenuItem != null) currentMenuItem.OnUnSelect();
        currentMenuItem = newMenuItem;
        currentMenuItem.OnSelect();
        // 开启二级菜单
        secondaryMenu.Show(newMenuItem.MenuType);
        isTouch = true;
    }
}
