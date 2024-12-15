using Netcore;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DataProtocol;

public class MessagehandlerCenter : MonoBehaviour
{

    // Update is called once per frame
    void Update()
    {
        if(UnityNetworkManager.Instance.ReceiveDataQueue.Count>0)
        {
            byte[] data = UnityNetworkManager.Instance.ReceiveDataQueue.Dequeue();
            DataModel model = MessageCodeC.Decode(data);
            HandlerModel(model);
        }
    }

    private void HandlerModel(DataModel model)
    {
        switch (model.Type)
        {
            case DataType.TYPE_NONE:
                break;
            case DataType.TYPE_SPORDER:
                SpOrderHandler(model);
                break;
            default:
                break;
        }

    }

    private void SpOrderHandler(DataModel model)
    {
        switch (model.Reequest)
        {
            case DataRequest.UPDATA_NAME_S:
                OnUpdateName(model);
                break;
            case DataRequest.GET_ROOMLIST_S:
                OnGetRoomList(model);
                break;
            case DataRequest.ROOM_CREATE_S:
                OnRoomCreate(model);
                break;
            case DataRequest.ROOM_UPLOAD_ANCHOR_S:
                OnUploadAnchor(model);
                break;
            case DataRequest.ROOM_JOIN_S:
                OnRoomJoin(model);
                break;
            case DataRequest.ROOM_DOWNLOAD_ANCHOR_S:
                OnDownloadAnchor(model);
                break;
            case DataRequest.ROOM_LEAVE_S:
                OnRoomLeave(model);
                break;
            case DataRequest.ROOM_IS_STOP_OR_CONTINUE:
                OnGameConStop(model);
                break;
            default:
                break;
        }

    }

    private void OnGameConStop(DataModel model)
    {
        var a=DataCodeC.ToObject<GameStopContinue>(model.Message);
        print(a.is_stop);
    }

    private void OnSendOrder(DataModel model)
    {
        OrderInfo order = DataCodeC.ToObject<OrderInfo>(model.Message);
        OrderManager.Instance.HandlerOrder(order);
    }

    private void OnDownloadAnchor(DataModel model)
    {
        if(model.Message.Length==4)
        {
            //失败
            ResultCode result = (ResultCode)BitConverter.ToInt32(model.Message, 0);
            print("OnDownloadAnchor:" + result);
        }
        else if(model.Message.Length>4)
        {
            print("OnDownLoadAnchor:" + model.Message.Length);
            UIManager.Instance.OnDownloadAnchor(model.Message);
        }
    }

    private void OnUploadAnchor(DataModel model)
    {
        ResultCode result = (ResultCode)BitConverter.ToInt32(model.Message, 0);
        print("OnUpdateAnchor:" + result);
    }

    private void OnRoomLeave(DataModel model)
    {
        ResultCode result = (ResultCode)BitConverter.ToInt32(model.Message, 0);
        print("OnLeaveRoom:" + result);
    }

    private void OnRoomJoin(DataModel model)
    {
        ResultCode result = (ResultCode)BitConverter.ToInt32(model.Message, 0);
        print("OnRoomJoin:" + result);
    }

    private void OnRoomCreate(DataModel model)
    {
        ResultCode result = (ResultCode)BitConverter.ToInt32(model.Message, 0);
        Debug.Log("OnCreateRoom:" + result);
    }

    private void OnGetRoomList(DataModel model)
    {
        RoomList list = DataCodeC.ToObject<RoomList>(model.Message);
        print("OnUpdatename:" + list.List.Count);
        //刷新房间列表
        UIManager.Instance.RefreashRoomList(list);
    }

    private void OnUpdateName(DataModel model)
    {
        
       ResultCode result= (ResultCode)BitConverter.ToInt32(model.Message, 0);
        print("OnUpdatename:" + result);
    }
}
