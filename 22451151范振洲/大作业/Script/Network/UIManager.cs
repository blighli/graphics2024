using DataProtocol;
using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UIManager : MonoBehaviour
{
    public static UIManager Instance;
    public GameObject RoomItemPrefab;
    public Transform RoomListContent;

    public TMP_InputField roomName;//记录inputField上输入的房间名字

    //要导出锚点的物体
    public List<GameObject> AnchorObj = new List<GameObject>();
    private void Awake()
    {
        Instance = this;
    }
    void Start()
    {
        
    }

    internal void OnConnect()
    {
        MessageSendManager.Instance.GetRoomList();
    }

    public void CreateRoom()
    {
        MessageSendManager.Instance.CreateRoom("a");
    }

    //通过Button创建房间
    public void CreateRoomButtonUI()
    {
        if(roomName.text==null)
        return;
        MessageSendManager.Instance.CreateRoom(roomName.text);
    }

    public void CreateRoom(string a)
    {
        MessageSendManager.Instance.CreateRoom(a);
    }

    public void LeaveaRoom()
    {
        MessageSendManager.Instance.LeaveRoom();
    }

    public void RefreashRoomList(RoomList list)
    {
        for(int i=0;i<RoomListContent.childCount;i++)
        {
            DestroyImmediate(RoomListContent.GetChild(i).gameObject);
        }
        for(int i=0;i<list.List.Count;i++)
        {
            GameObject go = Instantiate(RoomItemPrefab, RoomListContent);
            go.SetActive(true);
            
            go.GetComponent<RectTransform>().anchoredPosition = new Vector2(0, i );

            Roomitem roomitem = go.GetComponent<Roomitem>();
            roomitem.RoomNameText.text = list.List[i];
            //提取房间名字
            roomitem.RoomName = list.List[i].Split(',')[0];
        }
    }

    public void UpLoadAnchor()
    {
        //导出锚点

    }

    public void DownloadAnchor()
    {
        //下载锚点
    }

    internal void OnDownloadAnchor(byte[] message)
    {
        //调用锚点类将数据传输过去
    }

    //关闭房间菜单
    public void CloseRoomList()
    {
        RoomListContent.gameObject.active=!RoomListContent.gameObject.active;
    }
}
