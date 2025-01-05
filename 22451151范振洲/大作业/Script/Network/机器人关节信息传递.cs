using System.Collections;
using System.Collections.Generic;
using DataProtocol;
using UnityEngine;

public class 机器人关节信息传递 : MonoBehaviour
{
    public  Vector3 position,rotation;
    float aa=0.1f;

    void Awake()
    {
        position=this.transform.localPosition;
        rotation=transform.localEulerAngles;
    }
    void  Update()
    {
        if(aa>0f)
        {
            aa-=Time.deltaTime;
            return;
        }
        aa=1.0f;
        if (Vector3.Distance(this.transform.localEulerAngles, rotation) > 0.5f)
        {
            print("更新");
            DataModel model = new DataModel();
            model.Type = DataType.TYPE_SPORDER;
            model.Reequest = DataRequest.ROBOT_TRANS_INFO;
            RobotTransInfo robotTransInfo=new RobotTransInfo();
            robotTransInfo.name=name;
            robotTransInfo.posx=transform.localPosition.x;
            robotTransInfo.posy=transform.localPosition.y;
            robotTransInfo.posz=transform.localPosition.z;
            robotTransInfo.rotx=transform.localEulerAngles.x;
            robotTransInfo.roty=transform.localEulerAngles.y;
            robotTransInfo.rotz=transform.localEulerAngles.z;
            model.Message = DataCodeC.tobyteArray(robotTransInfo);
            UnityNetworkManager.Instance.Send(model);
            position=this.transform.localPosition;
            rotation=this.transform.localEulerAngles;
        }
        
    }
}
