using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;

public class MenuSceneManager : LogicManagerBase<MenuSceneManager>
{
    public AudioClip BGAudio;
    public AudioClip BGAudio2;
    
    protected override void RegisterEventListener()
    {
        
    }

    protected override void CancelEventListener()
    {
        
    }

    private void Start()
    {
        UIManager.Instance.Show<UI_MenuSceneMainWindow>();
        InvokeRepeating(nameof(PlayFireAudio), 0.2f, BGAudio2.length);
        AudioManager.Instance.PlayBGAudio(BGAudio);
    }

    private void PlayFireAudio()
    {
        AudioManager.Instance.PlayOnShot(BGAudio2, Vector3.zero, 1, false);
    }
}
