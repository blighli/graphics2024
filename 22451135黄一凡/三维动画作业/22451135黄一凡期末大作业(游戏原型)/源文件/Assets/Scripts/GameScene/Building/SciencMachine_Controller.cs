using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// 科学机器 控制器
/// </summary>
public class SciencMachine_Controller : BuildingBase
{
    public override void Init(MapChunkController mapChunk, ulong id, bool isFromBuild)
    {
        base.Init(mapChunk, id, isFromBuild);
        if (isFromBuild)
        {
            // 只有建造成功时，才同步科技数据
            ScienceManager.Instance.AddScience(27);
        }
    }
}
