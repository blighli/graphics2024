using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

public enum CursorState
{
    Normal = 0,
    Handle = 1
}

public class GameManager : SingletonMono<GameManager>
{
    private void Start()
    {
        Init();
    }

    // 整个游戏执行时，最先执行的
    private void Init()
    {
        SetCursorState(CursorState.Normal);
    }

    #region 鼠标指针

    private CursorState currentCursorState = CursorState.Normal;
    [SerializeField] private Texture2D[] cursorTextures;
    public void SetCursorState(CursorState cursorState)
    {
        if (currentCursorState == cursorState) return;
        currentCursorState = cursorState;
        Texture2D tex = cursorTextures[(int)cursorState];
        Cursor.SetCursor(tex, Vector2.zero, CursorMode.Auto);
    }

    #endregion

    #region 跨场景

    /// <summary>
    /// 创建新存档进入游戏
    /// </summary>
    public void CreateNewArchive_EnterGame(int mapSize, int mapSeed, int spawnSeed, float marshLimit)
    {
        // 初始化存档
        ArchiveManager.Instance.CreateNewArchive(mapSize, mapSeed, spawnSeed, marshLimit);
        // 加载场景
        SceneManager.LoadScene("GameScene");
    }

    /// <summary>
    /// 使用当前存档进入游戏
    /// </summary>
    public void UseCurrentArchive_EnterGame()
    {
        ArchiveManager.Instance.LoadCurrentArchive();
        // 加载场景
        SceneManager.LoadScene("GameScene");
    }

    public void EnterMenu()
    {
        // 加载场景
        SceneManager.LoadScene("MenuScene");
    }
    #endregion
}
