using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEngine;

public class ChanController : MonoBehaviour
{
    public GameObject l_foot, r_foot, l_knee, r_knee, l_leg, r_leg, l_hand, r_hand, l_elbow, r_elbow, l_arm, r_arm, head;
    //记录关节点数据
    public Vector2[] data;
    //关节点个数
    int num = 13;
    //记录是否开始识别
    public bool is_begin = false;
    //记录每个关节点是否开始被记录
    public bool[] data_bool = { false };
    //记录当前读取的json文件序号
    int num_json = 1;
    //读取数据的速度
    public int speed = 1;
    //判断现在是否开始数据识别
    [InspectorButton]
    public bool if_begin()
    {
        print(data_bool[0]);
        string a = @"E:\openpose-1.7.0\output\000000000000_keypoints.json";
        // @"E:\openpose-1.7.0\output\000000000000_keypoints.json";
        try
        {
            //解析过程
            string jsonTest = File.ReadAllText(a, Encoding.UTF8);
            ModelTest obj = JsonUtility.FromJson<ModelTest>(jsonTest);
            for (int i = 0; i < 18; i++)
            {
                print("第" + i + "个点的x坐标= " + obj.people[0].pose_keypoints_2d[3 * i]);
                print("第" + i + "个点的y坐标= " + obj.people[0].pose_keypoints_2d[3 * i + 1]);
                data[i] = new Vector2(obj.people[0].pose_keypoints_2d[3 * i], obj.people[0].pose_keypoints_2d[3 * i + 1]);


            }
            return true;
        }
        catch (ArgumentOutOfRangeException)
        {
            return false;
        }
    }

    public ModelTest TryFigure(int q)
    {
        string a = @"E:\openpose-1.7.0\output\0000" + q.ToString("00000000") + "_keypoints.json";
        try
        {
            //解析过程
            string jsonTest = File.ReadAllText(a, Encoding.UTF8);
            ModelTest obj = JsonUtility.FromJson<ModelTest>(jsonTest);
            for (int i = 0; i < 18; i++)
            {
                print("第" + i + "个点的x坐标= " + obj.people[0].pose_keypoints_2d[3 * i]);
                print("第" + i + "个点的y坐标= " + obj.people[0].pose_keypoints_2d[3 * i + 1]);
                data[i] = new Vector2(obj.people[0].pose_keypoints_2d[3 * i], obj.people[0].pose_keypoints_2d[3 * i + 1]);
                num_json += speed;
            }
            return obj;
        }
        catch (ArgumentOutOfRangeException)
        {
            num_json = -1;
            return null;
        }
    }

    private void Awake()
    {
        data = new Vector2[18];
    }

    #region 对肢体不同位置的旋转判断
    //对脖子左右旋转的判断
    public void Neck_LR_R()
    {
        //当进行判断的点有丢失的时候返回
        if (data[0] == new Vector2(0, 0) || data[1] == new Vector2(0, 0)) return;
        double tan = (data[0].x - data[1].x) / (data[0].y - data[1].y);
        double tanRadianValue2 = Math.Atan(tan);//求弧度值
        double tanAngleValue2 = tanRadianValue2 / Math.PI * 180;//求角度
        head.transform.localEulerAngles = new Vector3(head.transform.localEulerAngles.x, (float)tanAngleValue2,head.transform.localEulerAngles.z);
    }

    //右胳膊的控制
    public void L_arm_R()
    {
        //当进行判断的点有丢失的时候返回
        if (data[2] == new Vector2(0, 0) || data[3] == new Vector2(0, 0)) return;
        //手臂向下时
        if (data[2].y < data[3].y)
        {
            double tan = (data[3].y - data[2].y) / (data[2].x - data[3].x);
            double tanRadianValue2 = Math.Atan(tan);//求弧度值
            double tanAngleValue2 = tanRadianValue2 / Math.PI * 180;//求角度
            l_arm.transform.localEulerAngles = new Vector3(l_arm.transform.localEulerAngles.x, (float)tanAngleValue2, l_arm.transform.localEulerAngles.z);

        }
        //手臂向上
        else
        {
            double tan = (data[2].y - data[3].y) / (data[2].x - data[3].x);
            double tanRadianValue2 = Math.Atan(tan);//求弧度值
            double tanAngleValue2 = tanRadianValue2 / Math.PI * 180;//求角度
            l_arm.transform.localEulerAngles = new Vector3(l_arm.transform.localEulerAngles.x, -(float)tanAngleValue2, l_arm.transform.localEulerAngles.z);
        }
    }

    //左胳膊的控制
    public void R_arm_R()
    {
        //当进行判断的点有丢失的时候返回
        if (data[5] == new Vector2(0, 0) || data[6] == new Vector2(0, 0)) return;
        //手臂向下时
        if (data[5].y < data[6].y)
        {
            double tan = (data[6].y - data[5].y) / (data[6].x - data[5].x);
            double tanRadianValue2 = Math.Atan(tan);//求弧度值
            double tanAngleValue2 = tanRadianValue2 / Math.PI * 180;//求角度
            r_arm.transform.localEulerAngles = new Vector3(r_arm.transform.localEulerAngles.x, -(float)tanAngleValue2, r_arm.transform.localEulerAngles.z);

        }
        //手臂向上
        else
        {
            double tan = (data[5].y - data[6].y) / (data[6].x - data[5].x);
            double tanRadianValue2 = Math.Atan(tan);//求弧度值
            double tanAngleValue2 = tanRadianValue2 / Math.PI * 180;//求角度
            r_arm.transform.localEulerAngles = new Vector3(r_arm.transform.localEulerAngles.x, (float)tanAngleValue2, r_arm.transform.localEulerAngles.z);
        }
    }

    //右手胳膊肘控制
    public void Elbow_R_R()
    {
        //胳膊肘向里且4的x小于2 不同情况需要对手臂进行翻转
        if (data[2] == new Vector2(0, 0) || data[3] == new Vector2(0, 0) || data[4] == new Vector2(0, 0)) return;
        if (data[4].y > data[3].y)
        {
            double a = Vector2.Distance(data[4], data[2]);//对边
            double b = Vector2.Distance(data[3], data[4]);
            double c = Vector2.Distance(data[3], data[2]);
            //角度的余弦值
            double cos_ang1 = (b * b + c * c - a * a) / (2 * b * c);
            //弧度
            double cosRadian = Math.Acos(cos_ang1);
            //转角度
            double tanAngleValue2 = 180 - (cosRadian / Math.PI * 180);//求角度
            l_elbow.transform.localEulerAngles = new Vector3(0f, (float)tanAngleValue2,l_elbow.transform.localEulerAngles.z);
        }
        //胳膊肘向里且4的x大于2
        else
        {
            //260
            double a = Vector2.Distance(data[4], data[2]);//对边
            double b = Vector2.Distance(data[3], data[4]);
            double c = Vector2.Distance(data[3], data[2]);
            //角度的余弦值
            double cos_ang1 = (b * b + c * c - a * a) / (2 * b * c);
            //弧度
            double cosRadian = Math.Acos(cos_ang1);
            //转角度
            double tanAngleValue2 = 180 - (cosRadian / Math.PI * 180);//求角度
            l_elbow.transform.localEulerAngles = new Vector3(-119f, -(float)tanAngleValue2, l_elbow.transform.localEulerAngles.z);
        }
    }

    //左手胳膊肘的控制
    public void Elbow_L_R()
    {
        //胳膊肘向里且4的x小于2 不同情况需要对手臂进行翻转
        if (data[5] == new Vector2(0, 0) || data[6] == new Vector2(0, 0) || data[7] == new Vector2(0, 0)) return;
        if (data[7].y > data[6].y)
        {
            double a = Vector2.Distance(data[5], data[7]);//对边
            double b = Vector2.Distance(data[5], data[6]);
            double c = Vector2.Distance(data[6], data[7]);
            //角度的余弦值
            double cos_ang1 = (b * b + c * c - a * a) / (2 * b * c);
            //弧度
            double cosRadian = Math.Acos(cos_ang1);
            //转角度
            double tanAngleValue2 = 180 - (cosRadian / Math.PI * 180);//求角度
            r_elbow.transform.localEulerAngles = new Vector3(r_elbow.transform.localEulerAngles.x, 0f, -(float)tanAngleValue2);
        }
        //胳膊肘向里且4的x大于2
        else
        {
            //260
            double a = Vector2.Distance(data[5], data[7]);//对边
            double b = Vector2.Distance(data[5], data[6]);
            double c = Vector2.Distance(data[6], data[7]);
            //角度的余弦值
            double cos_ang1 = (b * b + c * c - a * a) / (2 * b * c);
            //弧度
            double cosRadian = Math.Acos(cos_ang1);
            //转角度
            double tanAngleValue2 = 180 - (cosRadian / Math.PI * 180);//求角度
            r_elbow.transform.localEulerAngles = new Vector3(r_elbow.transform.localEulerAngles.x, -145f, -(float)tanAngleValue2);
        }
    }

    #endregion

    //对数据更新的集合函数
    public void Update_body_data()
    {
        TryFigure(num_json);
        Neck_LR_R();
        L_arm_R();
        R_arm_R();
        Elbow_R_R();
        Elbow_L_R();
    }

    private void Update()
    {
        if (if_begin())
        {
            //判断读取的文件没有超出范围
            if (num_json != -1)
            {
                Update_body_data();
            }
        }
    }
}
