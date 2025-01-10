using System;
using UnityEngine;
using UnityEngine.UI;
using JKFrame;
using UnityEngine.EventSystems;

[Pool]
public class UI_ItemSlot : MonoBehaviour
{
    [SerializeField] private Image bgImg;
    [SerializeField] private Image iconImg;
    [SerializeField] private Text countText;
    
    public ItemData ItemData { get; private set; }  // 格子里面的数据
    public int Index { get; private set; }          // 第几个格子
    private UI_InventoryWindowBase ownerWindow;         // 宿主窗口：物品栏或仓库

    private Transform slotTransform;
    private Transform iconTransform;

    public static UI_ItemSlot CurrentMouseEnterSlot;
    public static UI_ItemSlot WeaponSlot;

    private Func<int, AudioType> onUseAction;
    private void Start()
    {
        slotTransform = transform;
        iconTransform = iconImg.transform;
        // 鼠标交互事件
        this.OnMouseEnter(MouseEnter);
        this.OnMouseExit(MouseExit);
        this.OnBeginDrag(BeginDrag);
        this.OnDrag(Drag);
        this.OnEndDrag(EndDrag);
        UITool.BindMouseEffect(this);
    }

    private void OnEnable()
    {
        this.OnUpdate(CheckMouseRightClick);
    }

    private void OnDisable()
    {
        this.RemoveUpdate(CheckMouseRightClick);
    }

    private void CheckMouseRightClick()
    {
        if (ItemData == null || onUseAction == null) return;
        // 当 当前格子为选中状态并且按下了右键，则目的是使用物品
        if (isMouseStay && Input.GetMouseButtonDown(1))
        {
            // 根据使用的情况来播放音效
            AudioType resultAudioType = onUseAction.Invoke(Index);
            ProjectTool.PlayAudio(resultAudioType);
        }
    }

    public void Init(int index, UI_InventoryWindowBase ownerWindow, Func<int, AudioType> onUseAction = null)
    {
        this.Index = index;
        this.ownerWindow = ownerWindow;
        this.onUseAction = onUseAction;
        bgImg.sprite = ownerWindow.bgSprites[0];
    }

    public void InitData(ItemData itemData = null)
    {
        ItemData = itemData;
        if (ItemData == null)
        {
            // 外框设置为白色，隐藏数量、Icon
            bgImg.color = Color.white;
            countText.gameObject.SetActive(false);
            iconImg.sprite = null;
            iconImg.gameObject.SetActive(false);
            return;
        }
        // 有数据的情况
        countText.gameObject.SetActive(true);
        iconImg.gameObject.SetActive(true);
        iconImg.sprite = ItemData.Config.Icon;  // 根据格子里面的数据 的配置来显示图片
        UpdateCountTextView();
    }

    public void UpdateCountTextView()
    {
        // 根据不同的物品类型显示不同的效果
        switch (ItemData.Config.ItemType)
        {
            case ItemType.Weapon:
                bgImg.color = Color.white;
                countText.text = (ItemData.ItemTypeData as ItemWeaponData).Durability.ToString() + "%";
                break;
            case ItemType.Consumable:
                bgImg.color = new Color(0, 1, 0, 0.5f);
                countText.text = (ItemData.ItemTypeData as ItemConsumableData).Count.ToString();
                break;
            case ItemType.Material:
                bgImg.color = Color.white;
                countText.text = (ItemData.ItemTypeData as ItemMaterialData).Count.ToString();
                break;
        }
    }
    
    #region 鼠标交互

    private bool isMouseStay = false;
    private void MouseEnter(PointerEventData arg1, object[] arg2)
    {
        GameManager.Instance.SetCursorState(CursorState.Handle);
        bgImg.sprite = ownerWindow.bgSprites[1];
        isMouseStay = true;
        CurrentMouseEnterSlot = this;
    }

    private void MouseExit(PointerEventData arg1, object[] arg2)
    {
        GameManager.Instance.SetCursorState(CursorState.Normal);
        bgImg.sprite = ownerWindow.bgSprites[0];
        isMouseStay = false;
        CurrentMouseEnterSlot = null;
    }

    private void BeginDrag(PointerEventData arg1, object[] arg2)
    {
        if (ItemData == null) return;
        iconTransform.SetParent(UIManager.Instance.DragLayer);
    }

    private void Drag(PointerEventData arg1, object[] arg2)
    {
        if (ItemData == null) return;
        GameManager.Instance.SetCursorState(CursorState.Handle);
        iconTransform.position = arg1.position;
    }

    private void EndDrag(PointerEventData arg1, object[] arg2)
    {
        if (ItemData == null) return;
        
        // 格子和建筑物的交互
        if (onUseAction!=null && InputManager.Instance.CheckSlotEndDragOnBuilding(ItemData.ConfigID))
        {
            ResetIcon();
            ProjectTool.PlayAudio(AudioType.Bag);
            ownerWindow.DiscardItem(Index);
            return;
        }
        
        // 虽然松手了，但是鼠标不在某个格子里面
        if (CurrentMouseEnterSlot == null)
        {
            GameManager.Instance.SetCursorState(CursorState.Normal);
        }
        // 当前拖拽中的Icon归位
        ResetIcon();
        // 如果还是自己
        if (CurrentMouseEnterSlot == this) return;
        
        // 物品丢弃
        if (CurrentMouseEnterSlot == null)
        {
            // 如果目标没有格子，但是是某个UI物体，无视之
            // if (EventSystem.current.IsPointerOverGameObject()) return;
            
            // 射线去检测有没有除了Mask以外的任何UI物体
            if (InputManager.Instance.CheckMouseOnUI()) return;
            // 防止玩家将物品扔到大型物体上方
            if (InputManager.Instance.CheckMouseOnBigMapObject()) return;
            if (InputManager.Instance.GetMouseWorldPositionOnGround(arg1.position,out Vector3 worldPos))
            {
                // 在地面生成物品
                worldPos.y = 1;
                MapManager.Instance.SpawnMapObject(ItemData.Config.MapObjectConfigID, worldPos, false);
                ProjectTool.PlayAudio(AudioType.Bag);
                // 丢弃一件物品
                ownerWindow.DiscardItem(Index);
            }
            return;
        }
        
        // 当前格子是武器格子
        if (this == WeaponSlot)
        {
            // 目标格子是空的，播放卸下武器的音效
            if (CurrentMouseEnterSlot.ItemData == null)
            {
                ProjectTool.PlayAudio(AudioType.TakeDownWeapon);
                SwapSlotItem(this, CurrentMouseEnterSlot);
            }
            else if (CurrentMouseEnterSlot.ItemData.Config.ItemType == ItemType.Weapon)
            {
                ProjectTool.PlayAudio(AudioType.TakeUpWeapon);
                SwapSlotItem(this, CurrentMouseEnterSlot);
            }
            else
            {
                ProjectTool.PlayAudio(AudioType.Fail);
                UIManager.Instance.AddTips("必须得是装备才行！");
            }
        }
        else
        {
            // 检查类型能不能拖入装备栏
            if (CurrentMouseEnterSlot == WeaponSlot)
            {
                if (ItemData.Config.ItemType != ItemType.Weapon)
                {
                    ProjectTool.PlayAudio(AudioType.Fail);
                    UIManager.Instance.AddTips("必须得是装备才行！");
                }
                else
                {
                    ProjectTool.PlayAudio(AudioType.TakeUpWeapon);
                    Debug.Log("可以装备物品:" + ItemData.Config.Name);
                    SwapSlotItem(this, CurrentMouseEnterSlot);
                }
            }
            else
            {
                // 普通格子和普通格子物品交换
                SwapSlotItem(this, CurrentMouseEnterSlot);
                ProjectTool.PlayAudio(AudioType.Bag);
            }
        }
    }

    private void ResetIcon()
    {
        iconTransform.SetParent(slotTransform);
        iconTransform.localPosition = Vector3.zero;
        iconTransform.localScale = Vector3.one;
    }
    
    public static void SwapSlotItem(UI_ItemSlot slot1, UI_ItemSlot slot2)
    {
        ItemData itemData1 = slot1.ItemData;
        ItemData itemData2 = slot2.ItemData;
        // 窗口、存档的数据交换
        // 有可能是跨窗口交换，比如快捷栏和仓库
        slot1.ownerWindow.SetItem(slot1.Index, itemData2);
        slot2.ownerWindow.SetItem(slot2.Index, itemData1);
    }
    
    #endregion
}
