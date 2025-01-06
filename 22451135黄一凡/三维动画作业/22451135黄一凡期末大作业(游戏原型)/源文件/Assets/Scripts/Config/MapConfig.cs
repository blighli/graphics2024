using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using Sirenix.OdinInspector;

/// <summary>
/// 地图物品的生成配置
/// </summary>
[CreateAssetMenu(fileName = "地图配置",menuName = "Config/地图配置")]
public class MapConfig : ConfigBase
{
    [LabelText("一个地图块有多少个格子")]
    public int mapChunkSize;
    [LabelText("一个格子多少米")]
    public float cellSize;
    [LabelText("噪音间隙")]
    public float noiseLacunarity;
    [LabelText("地图材质")]
    public Material mapMaterial;
    [LabelText("森林贴图")]
    public Texture2D forestTexture;
    [LabelText("沼泽贴图")]
    public Texture2D[] marshTextures;
    [LabelText("玩家可视距离，单位是Chunk")]
    public int viewDistance;
    [LabelText("地图早晨刷新概率")]
    public int refreshProbability;

    [Header("AI")]
    [LabelText("地图块AI数量限制")]
    public int maxAIOnChunk;
    [LabelText("地图块森林/沼泽生成AI的最小顶点数")]
    public int GenerateAIMinVertexCountOnChunk;
}