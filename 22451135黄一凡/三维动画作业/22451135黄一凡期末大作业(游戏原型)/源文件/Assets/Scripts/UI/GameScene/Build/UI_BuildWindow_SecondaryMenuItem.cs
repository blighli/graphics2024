using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// 合成窗口的二级菜单选项
/// </summary>
public class UI_BuildWindow_SecondaryMenuItem : MonoBehaviour
{
    [SerializeField] private Image bgImage;
    [SerializeField] private Button button;
    [SerializeField] private Image iconImage;
    [SerializeField] private Sprite[] bgSprites;

    public BuildConfig BuildConfig { get; private set; }
    private UI_BuildWindow_SecondaryMenu ownerWindow;

    private void Start()
    {
        UITool.BindMouseEffect(this);
        button.onClick.AddListener(OnClick);
    }

    public void Init(BuildConfig buildConfig, UI_BuildWindow_SecondaryMenu ownerWindow, bool isMeetCondition)
    {
        this.ownerWindow = ownerWindow;
        BuildConfig = buildConfig;
        if (buildConfig.BuildType == BuildType.Weapon)
        {
            iconImage.sprite = ConfigManager.Instance.GetConfig<ItemConfig>(ConfigName.Item, buildConfig.TargetID).Icon;
        }
        else
        {
            iconImage.sprite = ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, buildConfig.TargetID).MapIconSprite;
        }
        if (isMeetCondition) iconImage.color = Color.white;
        else iconImage.color = Color.black;
        UnSelect();
    }
    
    private void OnClick()
    {
        ownerWindow.SelectSecondaryMenuItem(this);
    }

    public void Select()
    {
        bgImage.sprite = bgSprites[1];
    }

    public void UnSelect()
    {
        bgImage.sprite = bgSprites[0];
    }
}
