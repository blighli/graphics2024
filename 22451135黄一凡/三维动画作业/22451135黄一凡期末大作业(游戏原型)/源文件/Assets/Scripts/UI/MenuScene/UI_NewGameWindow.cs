using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;
using UnityEngine.UI;

[UIElement(false,"UI/UI_NewGameWindow",1)]
public class UI_NewGameWindow : UI_WindowBase
{
    [SerializeField] private Slider mapSize_Slider;
    [SerializeField] private InputField mapSeed_InputField;
    [SerializeField] private InputField spawnSeed_InputField;
    [SerializeField] private Slider marshLimit_Slider;
    [SerializeField] private Button back_Button;
    [SerializeField] private Button startGame_Button;

    public override void Init()
    {
        back_Button.onClick.AddListener(Close);
        startGame_Button.onClick.AddListener(StartGame);
        back_Button.BindMouseEffect();
        startGame_Button.BindMouseEffect();
    }

    public override void OnClose()
    {
        base.OnClose();
        back_Button.RemoveMouseEffect();
        startGame_Button.RemoveMouseEffect();
        UIManager.Instance.Show<UI_MenuSceneMainWindow>();
    }

    private void StartGame()
    {
        int mapSize = (int)mapSize_Slider.value;
        // 如果玩家不输入种子的值，则随机
        int mapSeed = string.IsNullOrEmpty(mapSeed_InputField.text)
            ? Random.Range(int.MinValue, int.MaxValue)
            : int.Parse(mapSeed_InputField.text);
        int spawnSeed = string.IsNullOrEmpty(spawnSeed_InputField.text)
            ? Random.Range(int.MinValue, int.MaxValue)
            : int.Parse(spawnSeed_InputField.text);
        float marshLimit = marshLimit_Slider.value;
        UIManager.Instance.CloseAll();
        //建立新的存档并且开始游戏
        GameManager.Instance.CreateNewArchive_EnterGame(mapSize, mapSeed, spawnSeed, marshLimit);
    }
}
