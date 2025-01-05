using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

public class BuildManager : SingletonMono<BuildManager>
{
    [SerializeField] private float virtualCellSize = 0.25f;
    [SerializeField] private LayerMask buildLayerMask;
    private Dictionary<string, IBuilding> buildPreviewGameObjectDic = new();
    public void Init()
    {
        // 初始化建造面板
        UIManager.Instance.Show<UI_BuildWindow>();
        EventManager.AddEventListener<BuildConfig>(EventName.BuildBuilding, BuildBuilding);
    }

    private void BuildBuilding(BuildConfig buildConfig)
    {
        StartCoroutine(DOBuildBuilding(buildConfig));
    }

    IEnumerator DOBuildBuilding(BuildConfig buildConfig)
    {
        // 进入建造状态
        UIManager.Instance.DisableUIRaycaster();
        InputManager.Instance.SetCheckState(false);
        // 生成预览物品
        GameObject prefab = ConfigManager.Instance.GetConfig<MapObjectConfig>(ConfigName.MapObject, buildConfig.TargetID).Prefab;
        if (buildPreviewGameObjectDic.TryGetValue(prefab.name, out IBuilding previewBuilding))
        {
            previewBuilding.GameObject.SetActive(true);
        }
        else
        {
            previewBuilding = Instantiate(prefab, transform).GetComponent<IBuilding>();
            previewBuilding.InitOnPreview();
            buildPreviewGameObjectDic.Add(prefab.name, previewBuilding);
        }
        
        while (true)
        {
            // 取消建造
            if (Input.GetMouseButtonDown(1))
            {
                previewBuilding.GameObject.SetActive(false);
                UIManager.Instance.EnableUIRaycaster();
                InputManager.Instance.SetCheckState(true);
                yield break;
            }
            // 预览物体跟随鼠标
            if (InputManager.Instance.GetMouseWorldPositionOnGround(Input.mousePosition, out Vector3 mouseWorldPos))
            {
                // 把鼠标的坐标转换为虚拟格子的坐标
                Vector3 virtualCellPos = mouseWorldPos;
                // Mathf.RoundToInt(mouseWorldPos.x / virtualCellSize) 代表第几个格子
                virtualCellPos.x = Mathf.RoundToInt(mouseWorldPos.x / virtualCellSize) * virtualCellSize;
                virtualCellPos.z = Mathf.RoundToInt(mouseWorldPos.z / virtualCellSize) * virtualCellSize;
            
                previewBuilding.GameObject.transform.position = virtualCellPos;
            }
            bool isOverlap = true;
            // 碰撞检测 
            if (previewBuilding.Collider is BoxCollider)
            {
                BoxCollider boxCollider = (BoxCollider)previewBuilding.Collider;
                isOverlap = Physics.CheckBox(boxCollider.transform.position + boxCollider.center, boxCollider.size / 2,
                    boxCollider.transform.rotation, buildLayerMask);
            }
            else if (previewBuilding.Collider is CapsuleCollider)
            {
                CapsuleCollider capsuleCollider = (CapsuleCollider)previewBuilding.Collider;
                Vector3 colliderCenterPos = capsuleCollider.transform.position + capsuleCollider.center;
                Vector3 startPos = colliderCenterPos;
                Vector3 endPos = colliderCenterPos;
                startPos.y = colliderCenterPos.y - (capsuleCollider.height / 2) + capsuleCollider.radius;
                endPos.y = colliderCenterPos.y + (capsuleCollider.height / 2) - capsuleCollider.radius;
                isOverlap = Physics.CheckCapsule(startPos, endPos, capsuleCollider.radius, buildLayerMask);
            }
            else if (previewBuilding.Collider is SphereCollider)
            {
                SphereCollider sphereCollider = (SphereCollider)previewBuilding.Collider;
                isOverlap = Physics.CheckSphere(sphereCollider.transform.position + sphereCollider.center,
                    sphereCollider.radius, buildLayerMask);
            }
            // 如果可以建造材质球为绿色，否则为红色
            if(isOverlap) previewBuilding.SetColorOnPreview(true);
            else
            {
                previewBuilding.SetColorOnPreview(false);
                // 确定建造-根据配置扣除物资
                if (Input.GetMouseButtonDown(0))
                {
                    previewBuilding.GameObject.SetActive(false);
                    UIManager.Instance.EnableUIRaycaster();
                    InputManager.Instance.SetCheckState(true);
                    // 放置建筑物
                    MapManager.Instance.SpawnMapObject(buildConfig.TargetID, previewBuilding.GameObject.transform.position, true);
                    // 物资的消耗
                    InventoryManager.Instance.UpdateMainInventoryItemsForBuild(buildConfig);
                    yield break;
                }
            }
            yield return null;
        }
    }
}
