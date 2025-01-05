using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using UnityEngine.UI;

[UIElement(false, "UI/UI_MainInfoWindow", 0)]
public class UI_MainInfoWindow : UI_WindowBase
{
    [SerializeField] private Image TimeState_Img;
    [SerializeField] private Sprite[] TimeStateSprites;
    [SerializeField] private Text DayNum_Text;
    [SerializeField] private Image Hp_Img;
    [SerializeField] private Image Hungry_Img;

    private PlayerConfig playerConfig;

    public override void Init()
    {
        playerConfig = ConfigManager.Instance.GetConfig<PlayerConfig>(ConfigName.Player);
    }

    protected override void RegisterEventListener()
    {
        base.RegisterEventListener();
        EventManager.AddEventListener<bool>(EventName.UpdateTimeState, UpdateTimeState);
        EventManager.AddEventListener<int>(EventName.UpdateDayNum, UpdateDayNum);
        EventManager.AddEventListener<float>(EventName.UpdatePlayerHP, UpdatePlayerHP);
        EventManager.AddEventListener<float>(EventName.UpdatePlayerHungry, UpdatePlayerHungry);
    }

    protected override void CancelEventListener()
    {
        base.CancelEventListener();
        EventManager.RemoveEventListener<bool>(EventName.UpdateTimeState, UpdateTimeState);
        EventManager.RemoveEventListener<int>(EventName.UpdateDayNum, UpdateDayNum);
        EventManager.RemoveEventListener<float>(EventName.UpdatePlayerHP, UpdatePlayerHP);
        EventManager.RemoveEventListener<float>(EventName.UpdatePlayerHungry, UpdatePlayerHungry);
    }

    private void UpdateTimeState(bool isSun)
    {
        TimeState_Img.sprite = TimeStateSprites[isSun ? 0 : 1];
    }

    private void UpdateDayNum(int dayNum)
    {
        DayNum_Text.text = "Day " + (dayNum + 1);
    }
    
    private void UpdatePlayerHP(float hp)
    {
        Hp_Img.fillAmount = hp / playerConfig.MaxHp;
    }

    private void UpdatePlayerHungry(float hungry)
    {
        Hungry_Img.fillAmount = hungry / playerConfig.MaxHungry;
    }
}
