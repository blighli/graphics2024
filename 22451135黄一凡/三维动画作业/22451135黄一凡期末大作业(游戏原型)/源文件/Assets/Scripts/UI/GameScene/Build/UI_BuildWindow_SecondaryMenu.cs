using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// 合成窗口的二级菜单
/// </summary>
public class UI_BuildWindow_SecondaryMenu : MonoBehaviour
{
    [SerializeField] private Transform itemParent;
    [SerializeField] private GameObject itemPrefab;
    [SerializeField] private UI_BuildWindow_BuildPanel buildPanel;
    
    private Dictionary<BuildType, List<BuildConfig>> buildConfigDic;
    private UI_BuildWindow_SecondaryMenuItem currentSecondaryMenuItem;
    private List<UI_BuildWindow_SecondaryMenuItem> secondaryMenuItemList;
    private List<BuildConfig> meetTheConditionList;         // 满足条件的配置
    private List<BuildConfig> failToMeetTheConditionList;   // 不满足条件的配置
    private BuildType currentBuildType;
    
    public void Init()
    {
        buildConfigDic = new Dictionary<BuildType, List<BuildConfig>>(3);
        buildConfigDic.Add(BuildType.Weapon, new List<BuildConfig>());
        buildConfigDic.Add(BuildType.Building, new List<BuildConfig>());
        buildConfigDic.Add(BuildType.Crop, new List<BuildConfig>());
        Dictionary<int, ConfigBase> buildConfigs = ConfigManager.Instance.GetConfigs(ConfigName.Build);
        foreach (ConfigBase config in buildConfigs.Values)
        {
            BuildConfig buildConfig = (BuildConfig)config;
            buildConfigDic[buildConfig.BuildType].Add(buildConfig);
        }

        secondaryMenuItemList = new(10);
        meetTheConditionList = new();
        failToMeetTheConditionList = new();
        // 初始化三级窗口
        buildPanel.Init(this);
        Close();
    }

    public void Show(BuildType buildType)
    {
        currentBuildType = buildType;
        // 旧列表中的所有选项先放进对象池
        for (int i = 0; i < secondaryMenuItemList.Count; i++)
        {
            secondaryMenuItemList[i].JKGameObjectPushPool();
        }
        secondaryMenuItemList.Clear();
        // 当前类型的配置列表
        List<BuildConfig> buildConfigList = buildConfigDic[buildType];
        
        meetTheConditionList.Clear();
        failToMeetTheConditionList.Clear();
        
        for (int i = 0; i < buildConfigList.Count; i++)
        {
            // 科技判断
            bool scienceUnlock = true;
            if (buildConfigList[i].PreconditionScienceIDList != null)
            {
                for (int j = 0; j < buildConfigList[i].PreconditionScienceIDList.Count; j++)
                {
                    if (!ScienceManager.Instance.CheckUnlock(buildConfigList[i].PreconditionScienceIDList[j]))
                    {
                        scienceUnlock = false;
                    }
                }
            }

            if (scienceUnlock)
            {
                bool isMeet = buildConfigList[i].CheckBuildConfigCondition();
                if (isMeet) meetTheConditionList.Add(buildConfigList[i]);
                else failToMeetTheConditionList.Add(buildConfigList[i]);
            }
        }
        
        // 对配置进行分类，满足条件/不满足条件
        for (int i = 0; i < meetTheConditionList.Count; i++)
        {
            AddSecondaryMenuItem(meetTheConditionList[i],true);
        }
        for (int i = 0; i < failToMeetTheConditionList.Count; i++)
        {
            AddSecondaryMenuItem(failToMeetTheConditionList[i],false);
        }
        gameObject.SetActive(true);
    }

    public void RefreshView()
    {
        Show(currentBuildType);
        for (int i = 0; i < secondaryMenuItemList.Count; i++)
        {
            if (secondaryMenuItemList[i].BuildConfig == currentSecondaryMenuItem.BuildConfig)
            {
                secondaryMenuItemList[i].Select();
            }
        }
    }
    
    /// <summary>
    /// 添加一个二级菜单选项
    /// </summary>
    private void AddSecondaryMenuItem(BuildConfig buildConfig, bool isMeetCondition)
    {
        // 从对象池中获取菜单选项
        UI_BuildWindow_SecondaryMenuItem menuItem =
            PoolManager.Instance.GetGameObject<UI_BuildWindow_SecondaryMenuItem>(itemPrefab, itemParent);
        secondaryMenuItemList.Add(menuItem);
        menuItem.Init(buildConfig, this, isMeetCondition);
    }
    
    public void SelectSecondaryMenuItem(UI_BuildWindow_SecondaryMenuItem newSecondaryMenuItem)
    {
        if(currentSecondaryMenuItem!=null) currentSecondaryMenuItem.UnSelect();
        currentSecondaryMenuItem = newSecondaryMenuItem;
        currentSecondaryMenuItem.Select();
        // 显示三级窗口
        buildPanel.Show(newSecondaryMenuItem.BuildConfig);
    }

    public void Close()
    {
        buildPanel.Close();
        gameObject.SetActive(false);
    }
}
