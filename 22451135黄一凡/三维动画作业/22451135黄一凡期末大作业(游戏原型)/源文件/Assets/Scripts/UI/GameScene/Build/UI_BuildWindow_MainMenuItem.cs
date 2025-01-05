using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// UI 建造窗口 一级菜单选项
/// </summary>
public class UI_BuildWindow_MainMenuItem : MonoBehaviour
{
    [SerializeField] private Image bgImage;
    [SerializeField] private Button button;
    [SerializeField] private Image iconImage;
    [SerializeField] private Sprite[] bgSprites;    // 默认和选中时不同的精灵图
    
    public BuildType MenuType { get; private set; }
    private UI_BuildWindow ownerWindow;

    public void Init(BuildType buildType, UI_BuildWindow ownerWindow)
    {
        MenuType = buildType;
        this.ownerWindow = ownerWindow;
        UITool.BindMouseEffect(this);
        button.onClick.AddListener(OnClick);
        OnUnSelect();
    }

    private void OnClick()
    {
        ownerWindow.SelectMainMenuItem(this);
    }

    public void OnSelect()
    {
        bgImage.sprite = bgSprites[1];
    }

    public void OnUnSelect()
    {
        bgImage.sprite = bgSprites[0];
    }
}
