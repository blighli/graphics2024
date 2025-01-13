using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

/// <summary>
/// 科技管理器
/// </summary>
public class ScienceManager : SingletonMono<ScienceManager>
{
    private ScienceData scienceData;

    public void Init()
    {
        scienceData = ArchiveManager.Instance.ScienceData;
        EventManager.AddEventListener(EventName.SaveGame, OnGameSave);
    }

    public bool CheckUnlock(int ID)
    {
        return scienceData.CheckUnlock(ID);
    }
    
    public void AddScience(int ID)
    {
        scienceData.AddScience(ID);
    }

    private void OnGameSave()
    {
        ArchiveManager.Instance.SaveScienceData();
    }
}
