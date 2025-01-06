using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MessageTest : BaseSendOrder
{
    public InputField Roomname;
    public InputField AnchorInfo;

    public void UpdateName()
    {
        MessageSendManager.Instance.UpdateName("1");
    }
    public void CreatRoom()
    {
        MessageSendManager.Instance.CreateRoom("aa");
    }    

    public void JoinRoom()
    {
        MessageSendManager.Instance.JoinRoom("a");
    }

    public void LeaveRoom()
    {
        MessageSendManager.Instance.LeaveRoom();
    }

    public void UploadAnchor()
    {
        int size = int.Parse("100000    ");
        byte[] data = new byte[size];
        MessageSendManager.Instance.UploadAchor(data);
    }

    public void DownLoadAnchor()
    {
        MessageSendManager.Instance.DownloadAchor();
    }

    public void getRoomList()
    {
        MessageSendManager.Instance.GetRoomList();
    }

    public void SenRoomAll()
    {
        SendOrderTest101("qq", 1);
    }

    public void SendOrderTest101(string s,int i)
    {
        //将方法传给房间内所有成员
        MessageSendManager.Instance.SendMessageToRoomAll("OnSendOrederTest01", new object[2]{s,i });
    }

    public void OnSendOrederTest01(string s, int i)
    {
        print("OnSendOrederTest01:" + s +","+ i);
    }
}
