using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using Sirenix.OdinInspector;

/// <summary>
/// 掉落配置
/// </summary>
[CreateAssetMenu(fileName = "掉落配置", menuName = "Config/掉落配置")]
public class LootConfig : ConfigBase
{
    [LabelText("掉落配置列表")] public List<LootConfigModel> Configs;

    public void GenerateMapObject(MapChunkController mapChunk, Vector3 position)
    {
        for (int i = 0; i < Configs.Count; i++)
        {
            // 根据概率决定是否实例化
            int randomValue = Random.Range(1, 101);
            if (randomValue < Configs[i].Probability)
            {
                // 生成掉落物体
                MapManager.Instance.SpawnMapObject(mapChunk, Configs[i].LootObjectConfigID, position, false);
            }
        }
    }
}

public class LootConfigModel
{
    [LabelText("掉落物体ID")] public int LootObjectConfigID;
    [LabelText("掉落概率%")] public int Probability;
}
