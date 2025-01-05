using JKFrame;
using UnityEngine;
using Sirenix.OdinInspector;

[CreateAssetMenu(fileName = "地图物体配置",menuName = "Config/地图物体配置")]
public class MapObjectConfig : ConfigBase
{
    [LabelText("空的 不生成物品")] public bool IsEmpty = false;
    [LabelText("所在的地图顶点类型")] public MapVertexType MapVertexType;
    [LabelText("生成的预制体")] public GameObject Prefab;
    [LabelText("Icon")] public Sprite MapIconSprite;
    [LabelText("UI地图Icon尺寸")] public float IconSize = 1;
    [LabelText("销毁天数")] public int DestroyDays = -1;
    [LabelText("生成概率 权重类型")] public int Probability;
    [LabelText("描述"),MultiLineProperty] public string Description;
}
