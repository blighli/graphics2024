using System;
using System.Collections;
using System.Collections.Generic;
using JKFrame;
using UnityEngine;
using Sirenix.OdinInspector;

/// <summary>
/// 时间管理器
/// </summary>
public class TimeManager : LogicManagerBase<TimeManager>
{
    [SerializeField] private Light mainLight;
    [Range(0,30)] public float timeScale = 1;
    private TimeConfig timeConfig;
    private TimeData timeData;
    private int nextIndex;
    public int CurrentDayNum { get => timeData.DayNum; }
    
    protected override void RegisterEventListener()
    {
        
    }

    protected override void CancelEventListener()
    {
        
    }

    public void Init()
    {
        timeData = ArchiveManager.Instance.TimeData;
        timeConfig = ConfigManager.Instance.GetConfig<TimeConfig>(ConfigName.Time);
        InitState();
        EventManager.AddEventListener(EventName.SaveGame, OnGameSave);
    }

    private void Update()
    {
        if (GameSceneManager.Instance.IsInitialized == false) return;
        UpdateTime();
    }

    // 首次应用存档数据时，需要根据数据做初始的设置
    private void InitState()
    {
        // 设置初始的nextIndex
        nextIndex = timeData.StateIndex + 1 >= timeConfig.timeStateConfigs.Length ? 0 : timeData.StateIndex + 1;
        RenderSettings.fog = timeConfig.timeStateConfigs[timeData.StateIndex].fog;
        // 背景音乐
        if (timeConfig.timeStateConfigs[timeData.StateIndex].bgAudioClip!=null)
            StartCoroutine(ChangeBGAudio(timeConfig.timeStateConfigs[timeData.StateIndex].bgAudioClip));
        // 发送是否为白天的事件
        EventManager.EventTrigger<bool>(EventName.UpdateTimeState, timeData.StateIndex <= 1);
        // 发送当前是第几天的事件
        EventManager.EventTrigger<int>(EventName.UpdateDayNum, timeData.DayNum);
    }

    private void UpdateTime()
    {
        timeData.CalculateTime -= Time.deltaTime * timeScale;
        // 计算阳光并且得到阳光相关的设置
        if (!timeConfig.timeStateConfigs[timeData.StateIndex].CheckAndCalTime(timeData.CalculateTime,timeConfig.timeStateConfigs[nextIndex],out Quaternion quaternion, out Color color, out float sunIntensity))
        {
            EnterNextState();
        }
        SetLight(sunIntensity, quaternion, color);
    }

    // 进入新的时间状态
    private void EnterNextState()
    {
        // 切换到下一状态
        timeData.StateIndex = nextIndex;
        // 检查边界，超过就从0开始
        nextIndex = timeData.StateIndex + 1 >= timeConfig.timeStateConfigs.Length ? 0 : timeData.StateIndex + 1;
        // 如果现在是早上，也就是currentStateIndex==0，那么意味着天数需要加1
        if (timeData.StateIndex == 0)
        {
            timeData.DayNum++;
            // 发送当前是第几天的事件
            EventManager.EventTrigger<int>(EventName.UpdateDayNum, timeData.DayNum);
            // 发送当前是早晨的事件
            EventManager.EventTrigger(EventName.OnMorn);
        }
        timeData.CalculateTime = timeConfig.timeStateConfigs[timeData.StateIndex].durationTime;
        // 雾
        RenderSettings.fog = timeConfig.timeStateConfigs[timeData.StateIndex].fog;
        // 背景音乐
        if (timeConfig.timeStateConfigs[timeData.StateIndex].bgAudioClip!=null) 
            StartCoroutine(ChangeBGAudio(timeConfig.timeStateConfigs[timeData.StateIndex].bgAudioClip));
        // 发送是否为白天的事件
        EventManager.EventTrigger<bool>(EventName.UpdateTimeState, timeData.StateIndex <= 1);
    }
    private void SetLight(float intensity, Quaternion rotation, Color color)
    {
        mainLight.intensity = intensity;
        mainLight.transform.rotation = rotation;
        mainLight.color = color;
        // 设置环境光的亮度
        RenderSettings.ambientIntensity = intensity;
    }

    private IEnumerator ChangeBGAudio(AudioClip audioClip)
    {
        float old = AudioManager.Instance.BGVolume;
        if (old<=0) yield break;
        float current = old;
        // 降低音量
        while (current>0)
        {
            yield return null;
            current -= Time.deltaTime / 2;
            AudioManager.Instance.BGVolume = current;
        }
        AudioManager.Instance.PlayBGAudio(audioClip);
        // 提高音量
        while (current<old)
        {
            yield return null;
            current += Time.deltaTime / 2;
            AudioManager.Instance.BGVolume = current;
        }
        AudioManager.Instance.BGVolume = old;
    }

    private void OnGameSave()
    {
        ArchiveManager.Instance.SaveTimeData();
    }
}
