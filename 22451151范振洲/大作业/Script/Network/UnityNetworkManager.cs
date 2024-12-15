using Netcore;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using UnityEngine;

public class UnityNetworkManager : MonoBehaviour
{
    public string Ip;
    public int Port;
    public static UnityNetworkManager Instance { get; private set;  }
    public Queue<byte[]> ReceiveDataQueue = new Queue<byte[]>();

    private void Awake()
    {
        Instance = this;
        //注册连接，断开，接受事件
        SocketClient.Instance.ConnectResultEvent += OnConnectResult;
        SocketClient.Instance.DataReceiveEvent += OnDataReceive;
        SocketClient.Instance.DisconnectEvent += OnDisConnect;
    }

    private void OnDisConnect()
    {
        Debug.Log("OnDisconnect");
    }

    private void OnDataReceive(byte[] data)
    {
        ReceiveDataQueue.Enqueue(data);
    }

    private void OnConnectResult(bool result)
    {
        Debug.Log("connect result:" + result);
        if(result)
        {
            UIManager.Instance.OnConnect();
            //开始发送心跳包
            StartCoroutine(SendHeart());
        }
    }

    /// <summary>
    /// 发送心跳包，用户连接后如不发送数据则服务器不知道用户是否在连接状态所以通过每5秒发送一个空包的方式确认连接
    /// </summary>
    /// <returns></returns>
    IEnumerator SendHeart()
    {
        while(SocketClient.Instance.Connected)
        {
            yield return new WaitForSeconds(5);//制作时间间隔
            //发送心跳数据
            Send(new DataModel());//发送一个空包
        }
    }

    public void Send(DataModel model)
    {
        if(!SocketClient.Instance.Connected)
        {
            Debug.Log("off line");
            return;
        }
        //byte[] message = model.Message;
        //int messageLength = message == null ? 0 : message.Length;
        //byte[] len = BitConverter.GetBytes(messageLength+2);
        ////消息总长度
        //byte[] buffer = new byte[messageLength + 2+4]; //加上两字节的消息类型和请求类型   4字节的消息长度
        ////按照消息封装顺序消息长度+消息类型+消息包的顺序封装
        //Buffer.BlockCopy(len, 0, buffer, 0, 4);
        //byte[] code = new byte[2] { model.Type, model.Reequest };
        //Buffer.BlockCopy(code, 0, buffer, 4, 2);
        ////消息包不为空才赋值
        //if(message!=null)
        //{
        //    Buffer.BlockCopy(message, 0, buffer, 6, messageLength);
        //}
        //发送消息
        SocketClient.Instance.Send(MessageCodeC.Encode(model));//通过使用已写类的方法减少重复脚本的编写
    }

    public void Connect()
    {
        IPAddress iPAddress;
        if (!IPAddress.TryParse(Ip, out iPAddress) || Port < 0 || Port > 65535)
        {
            Debug.LogError("ip or port is wrong");
            return;
        }
        SocketClient.Instance.ConnectServer(Ip, Port);

    }

    public void DisConnect()
    {
        SocketClient.Instance.DisConnectServer();
    }
}
