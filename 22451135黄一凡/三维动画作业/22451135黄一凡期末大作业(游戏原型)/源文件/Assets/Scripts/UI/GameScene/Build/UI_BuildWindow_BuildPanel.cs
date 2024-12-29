using System.Collections;
using System.Collections.Generic;
using JKFrame;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class UI_BuildWindow_BuildPanel : MonoBehaviour
{
    [SerializeField] private UI_BuildWindow_BuildPanelItem[] buildPanelItems;
    [SerializeField] private Text descriptionText;
    [SerializeField] private Button buildButton;
    private BuildConfig buildConfig;
    private UI_BuildWindow_SecondaryMenu ownerWindow;

    public void Init(UI_BuildWindow_SecondaryMenu ownerWindow)
    {
        this.ownerWindow = ownerWindow;
        buildButton.onClick.AddListener(BuildButtonClick);
        Close();
    }

    private void BuildButtonClick()
    {
        if (buildConfig.BuildType == BuildType.Weapon)
        {
            if (InventoryManager.Instance.AddMainInventoryItemAndPlayAudio(buildConfig.TargetID))
            {
                // TODO:根据建造配置，减少消耗的材料
                InventoryManager.Instance.UpdateMainInventoryItemsForBuild(buildConfig);
                // TODO:刷新当前状态
                RefreshView();
            }
            else UIManager.Instance.AddTips("背包已满，无法建造！");
        }
        else
        {
            // 进入建造模式
            EventManager.EventTrigger(EventName.BuildBuilding, buildConfig);
            ownerWindow.Close();
        }
    }

    public void Show(BuildConfig buildConfig)
    {
        this.buildConfig = buildConfig;
        // 显示合成需要的物品
        for (int i = 0; i < buildConfig.BuildConfigConditionList.Count; i++)
        {
            int id = buildConfig.BuildConfigConditionList[i].ItemId;
            int currentCount = InventoryManager.Instance.GetMainInventoryItemCount(id);
            int needCount = buildConfig.BuildConfigConditionList[i].Count;
            buildPanelItems[i].Show(id, currentCount, needCount);
        }

        if (buildConfig.BuildType == BuildType.Weapon)
        {
            descriptionText.text = ConfigManager.Instance.GetConfig<ItemConfig>(ConfigName.Item, buildConfig.TargetID).Description;
        }
        else
        {
            descriptionText.text = ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, buildConfig.TargetID).Description;
        }
        buildButton.interactable = buildConfig.CheckBuildConfigCondition();
        gameObject.SetActive(true);
    }

    public void RefreshView()
    {
        Show(buildConfig);
        ownerWindow.RefreshView();
    }
    
    public void Close()
    {
        for (int i = 0; i < buildPanelItems.Length; i++)
        {
            buildPanelItems[i].Close();
        }
        gameObject.SetActive(false);
    }
}
