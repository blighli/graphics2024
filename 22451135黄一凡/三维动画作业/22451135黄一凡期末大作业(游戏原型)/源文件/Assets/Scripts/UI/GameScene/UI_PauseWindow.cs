using JKFrame;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[UIElement(false, "UI/UI_PauseWindow", 4)]
public class UI_PauseWindow : UI_WindowBase
{
    [SerializeField] Button continueButton;
    [SerializeField] Button quitButton;

    public override void Init()
    {
        continueButton.onClick.AddListener(ContinueButtonClick);
        quitButton.onClick.AddListener(QuitButtonClick);
    }

    private void ContinueButtonClick()
    {
        GameSceneManager.Instance.UnPauseGame();
    }
    private void QuitButtonClick()
    {
        GameSceneManager.Instance.CloseAndSave();
    }
}
