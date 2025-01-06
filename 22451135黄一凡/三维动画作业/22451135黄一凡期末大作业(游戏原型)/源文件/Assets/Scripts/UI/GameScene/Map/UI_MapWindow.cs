using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// UI地图窗口
/// </summary>
[UIElement(true, "UI/UI_MapWindow", 4)]
public class UI_MapWindow : UI_WindowBase
{
    [SerializeField] private RectTransform content;     // 所有地图块，Icon显示的父物体
    private float contentSize;

    [SerializeField] private GameObject mapItemPrefab;  // 单个地图块在UI中的预制体
    [SerializeField] private GameObject mapIconPrefab;  // 单个Icon在UI中的预制体
    [SerializeField] private RectTransform playerIcon;  // 玩家所在位置的Icon
    
    private Dictionary<ulong, Image> mapObjectIconDic = new();                                  // 所有的地图物体Icon字典
    private Dictionary<Vector2Int, Image> mapChunkImageDic = new();                                  // 所有的地图物体Icon字典

    private float mapChunkImageSize;      // UI地图块的尺寸
    private int mapChunkSize;             // 一个地图块有多少个格子
    private float mapSizeOnWorld;   // 3D地图在世界中的坐标
    private Sprite forestSprite;    // 森林地块的精灵

    private float minScale;             // 最小的缩小倍数
    private float maxScale = 10;        // 最大的放大倍数

    public override void Init()
    {
        transform.Find("Scroll View").GetComponent<ScrollRect>().onValueChanged.AddListener(UpdatePlayerIconPos);
    }

    private void Update()
    {
        float scroll = Input.GetAxis("Mouse ScrollWheel");
        if (scroll != 0)
        {
            float newScale = Mathf.Clamp(content.localScale.x + scroll, minScale, maxScale);
            content.localScale = new Vector3(newScale, newScale, 0);
        }
    }

    /// <summary>
    /// 初始化地图
    /// </summary>
    /// <param name="mapSize">一个地图一行或一列有多少个Image/Chunk</param>
    /// <param name="mapSizeOnWorld">地图在世界中一行或一列有多大</param>
    /// <param name="forestTexture">森林的贴图</param>
    public void InitMap(float mapSize, int mapChunkSize, float mapSizeOnWorld, Texture2D forestTexture)
    {
        this.mapSizeOnWorld = mapSizeOnWorld;
        forestSprite = CreateMapSprite(forestTexture);
        this.mapChunkSize = mapChunkSize;
        
        // 内容尺寸
        contentSize = mapSizeOnWorld * 10;
        content.sizeDelta = new Vector2(contentSize, contentSize);
        content.localScale = new Vector3(maxScale, maxScale, 1);
        // 一个UI地图块的尺寸
        mapChunkImageSize = contentSize / mapSize;
        minScale = 1050f / contentSize;
    }

    /// <summary>
    /// 更新中心点，为了鼠标缩放的时候，中心点是玩家现在的坐标
    /// </summary>
    /// <param name="viewerPosition"></param>
    public void UpdatePivot(Vector3 viewerPosition)
    {
        float x = viewerPosition.x / mapSizeOnWorld;
        float y = viewerPosition.z / mapSizeOnWorld;
        // 修改Content后会导致Scroll Rect组件的 当值修改事件=>UpdatePlayerIconPos
        content.pivot = new Vector2(x, y);
    }

    public void UpdatePlayerIconPos(Vector2 value)
    {
        // 玩家的Icon放在Content的中心点
        playerIcon.anchoredPosition3D = content.anchoredPosition3D;
    }
    
    /// <summary>
    /// 添加一个地图块
    /// </summary>
    public void AddMapChunk(Vector2Int chunkIndex, Serialization_Dic<ulong, MapObjectData> mapObjectDic, Texture2D texture = null)
    {
        RectTransform mapChunkRect = Instantiate(mapIconPrefab, content).GetComponent<RectTransform>();
        // 确定地图块的Image的坐标和宽高
        mapChunkRect.anchoredPosition = new Vector2(chunkIndex.x * mapChunkImageSize, chunkIndex.y * mapChunkImageSize);
        mapChunkRect.sizeDelta = new Vector2(mapChunkImageSize, mapChunkImageSize);
        Image mapChunkImage = mapChunkRect.GetComponent<Image>();
        mapChunkImageDic.Add(chunkIndex, mapChunkImage);
        // 森林的情况
        if (texture == null)
        {
            mapChunkImage.type = Image.Type.Tiled;
            // 设置贴瓷砖的比例，要在一个Image中显示，这个地图块所包含的格子数量
            // 贴图和Image的比例
            float ratio = forestSprite.texture.width / mapChunkImageSize;
            // 一个地图块上有多少个格子
            mapChunkImage.pixelsPerUnitMultiplier = mapChunkSize * ratio;
            mapChunkImage.sprite = forestSprite;
        }
        else
        {
            mapChunkImage.sprite = CreateMapSprite(texture);
        }
        // 添加物体的ICON
        foreach (var item in mapObjectDic.Dictionary.Values)
        {
            AddMapObjectIcon(item);
        }
    }
    
    /// <summary>
    /// 生成地图精灵
    /// </summary>
    private Sprite CreateMapSprite(Texture2D texture)
    {
        return Sprite.Create(texture, new Rect(0, 0, texture.width, texture.height), new Vector2(0.5f, 0.5f));
    }

    /// <summary>
    /// 移除地图对象Icon
    /// </summary>
    public void RemoveMapObjectIcon(ulong mapObjectID)
    {
        if (mapObjectIconDic.TryGetValue(mapObjectID, out Image iconImg))
        {
            // 把Icon游戏物体扔回对象池
            iconImg.JKGameObjectPushPool();
            mapObjectIconDic.Remove(mapObjectID);
        }
    }

    /// <summary>
    /// 添加一个地图对象Icon
    /// </summary>
    public void AddMapObjectIcon(MapObjectData mapObjectData)
    {
        MapObjectConfig config =
            ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, mapObjectData.ConfigID);
        if (config.MapIconSprite == null || config.IconSize <= 0) return;
        GameObject go = PoolManager.Instance.GetGameObject(mapIconPrefab, content);
        Image iconImg = go.GetComponent<Image>();
        iconImg.sprite = config.MapIconSprite;
        iconImg.transform.localScale = Vector3.one * config.IconSize;
        float x = mapObjectData.Position.x * 10;       // 因为整个Content的尺寸在初始化已经*10，所以Icon也需要乘上同样的系数
        float y = mapObjectData.Position.z * 10;
        go.GetComponent<RectTransform>().anchoredPosition = new Vector2(x, y);
        mapObjectIconDic.Add(mapObjectData.ID, iconImg);    // 保存进字典
    }

    // 回收窗口资源
    public void ResetWindow()
    {
        foreach (var value in mapObjectIconDic.Values)
        {
            value.JKGameObjectPushPool();
        }
        foreach (var image in mapChunkImageDic.Values)
        {
            Destroy(image.gameObject);
        }
        mapObjectIconDic.Clear();
        mapChunkImageDic.Clear();
    }
}
