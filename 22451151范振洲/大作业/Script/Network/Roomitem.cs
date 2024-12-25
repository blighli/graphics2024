using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Roomitem : MonoBehaviour
{
    public Text RoomNameText;
    public string RoomName;


    public void JoinRoom()
    {
        MessageSendManager.Instance.JoinRoom(RoomName);
    }
}
