using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

/// <summary>
/// 科技存档数据
/// </summary>
[Serializable]
public class ScienceData
{
    public List<int> UnlockScienceList = new(10);
    
    /// <summary>
    /// 检测科技是否解锁
    /// </summary>
    public bool CheckUnlock(int ID)
    {
        return UnlockScienceList.Contains(ID);
    }
    
    /// <summary>
    /// 添加科技
    /// </summary>
    public void AddScience(int ID)
    {
        if (!UnlockScienceList.Contains(ID))
        {
            UnlockScienceList.Add(ID);
        }
    }
}
