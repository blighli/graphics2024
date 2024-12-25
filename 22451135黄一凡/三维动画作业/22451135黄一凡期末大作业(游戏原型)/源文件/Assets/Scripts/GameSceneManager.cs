using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

public class GameSceneManager : LogicManagerBase<GameSceneManager>
{
    #region 测试逻辑
    public bool IsTest = false;
    public bool IsCreateNewArchive;
    #endregion

    private bool isGameOver = false;
    public bool IsGameOver => isGameOver;

    protected override void RegisterEventListener() { }
    protected override void CancelEventListener() { }

    private void Start()
    {
        #region 测试逻辑
        if (IsTest)
        {
            if (IsCreateNewArchive)
            {
                ArchiveManager.Instance.CreateNewArchive(10, 1, 1, 0.6f);
            }
            else
            {
                ArchiveManager.Instance.LoadCurrentArchive();
            }
        }
        #endregion
        
        UIManager.Instance.CloseAll();
        StartGame();
    }

    private bool isPause = false;
    private void Update()
    {
        if (IsInitialized == false) return;
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            isPause = !isPause;
            if (isPause) PauseGame();
            else UnPauseGame();
        }
    }

    public void PauseGame()
    {
        isPause = true;
        UIManager.Instance.Show<UI_PauseWindow>();
        Time.timeScale = 0;
    }

    public void UnPauseGame()
    {
        isPause = false;
        UIManager.Instance.Close<UI_PauseWindow>();
        Time.timeScale = 1;
    }
    
    private void StartGame()
    {
        // 如果运行到这里，那么一定所有的存档都准备好了
        IsInitialized = false;
        //加载进度条
        loadingWindow = UIManager.Instance.Show<UI_GameLoadingWindow>();
        
        loadingWindow.UpdateProgress(0);
        // 确定地图初始化配置数据
        MapConfig mapConfig = ConfigManager.Instance.GetConfig<MapConfig>(ConfigName.Map);
        float mapSizeOnWorld =
            ArchiveManager.Instance.MapInitData.mapSize * mapConfig.mapChunkSize * mapConfig.cellSize;
        
        // 显示主信息面板
        // 依赖于TimeManager的信息发送
        // 依赖于Player_Controller的信息发送
        UIManager.Instance.Show<UI_MainInfoWindow>();
        
        // 初始化角色、相机
        Player_Controller.Instance.Init(mapSizeOnWorld);
        Camera_Controller.Instance.Init(mapSizeOnWorld);
        
        // 初始化时间
        TimeManager.Instance.Init();
        
        // 初始化地图
        MapManager.Instance.UpdateViewer(Player_Controller.Instance.transform);
        MapManager.Instance.Init();
        
        // 初始化物品栏
        InventoryManager.Instance.Init();
        
        // 初始化输入管理器
        InputManager.Instance.Init();
        
        // 初始化建造管理器
        BuildManager.Instance.Init();
        
        // 初始化科技管理器
        ScienceManager.Instance.Init();
    }

    public void GameOver()
    {
        isGameOver = true;
        // 删除存档
        ArchiveManager.Instance.CleanArchive();
        // 延迟进入新场景
        Invoke(nameof(EnterMenuScene), 2);
    }

    public void CloseAndSave()
    {
        // 存档
        EventManager.EventTrigger(EventName.SaveGame);
        // 进入菜单场景
        EnterMenuScene();
    }

    private void EnterMenuScene()
    {
        Time.timeScale = 1;
        // 回收场景资源
        MapManager.Instance.OnCloseGameScene();
        EventManager.Clear();
        MonoManager.Instance.StopAllCoroutines();
        UIManager.Instance.CloseAll();
        // 进入新场景
        GameManager.Instance.EnterMenu();
    }

    private void OnApplicationQuit()
    {
        if (isGameOver)
        {
            // 紧急存档
            EventManager.EventTrigger(EventName.SaveGame);
        }
    }

    #region 加载进度

    public bool IsInitialized { get; private set; }
    private UI_GameLoadingWindow loadingWindow;
    
    // 初始化需要的最大值和目前
    private int currentMapChunkCount = 0;
    private int maxMapChunkCount = 0;
    /// <summary>
    /// 更新进度
    /// </summary>
    public void UpdateMapProgress(int current, int max)
    {
        float currentProgress = (100 / max) * current;
        if (current == max)
        {
            loadingWindow.UpdateProgress(100);
            IsInitialized = true;
            loadingWindow.Close();
            loadingWindow = null;
        }
        else
        {
            loadingWindow.UpdateProgress(currentProgress);
        }
    }

    public void SetProgressMapChunkCount(int max)
    {
        maxMapChunkCount = max;
    }

    public void OnGenerateMapChunkSucceed()
    {
        currentMapChunkCount++;
        UpdateMapProgress(currentMapChunkCount, maxMapChunkCount);
    }
    #endregion
}
