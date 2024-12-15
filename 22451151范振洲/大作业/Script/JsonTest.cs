using UnityEngine;
using System.IO;
using System.Text;
using System;
using System.Collections;
using System.Linq;
using System.Collections.Generic;


public class JsonTest : MonoBehaviour
{
    #region 数据区域
    static public float[] pose_joint_x = new float[20];
    static public float[] pose_joint_y = new float[20];
    int num = 0;
    #endregion

    //一个对json文件进行数据解读的函数
    [InspectorButton]
    public void TryFigure_test()
    {
        TryFigure(1);
        
    }
    public ModelTest TryFigure(int q)
    {
        string a = @"E:\openpose-1.7.0\output\0000000" + q.ToString("00000")+"_keypoints.json";
        try
        {
            //解析过程
            string jsonTest = File.ReadAllText(a, Encoding.UTF8);
            ModelTest obj = JsonUtility.FromJson<ModelTest>(jsonTest);
            for (int i = 0; i < 18; i++)
            {
                print("第" + i + "个点的x坐标= " + obj.people[0].pose_keypoints_2d[3 * i]);
                print("第" + i + "个点的y坐标= " + obj.people[0].pose_keypoints_2d[3 * i + 1]);
            }
            return obj;
        }
        catch (ArgumentOutOfRangeException)
        {
            return null;
        }
    }

    void Start()
    {
        
    }


    // Update is called once per frame
    void Update()
    {
        try
        {
            num += 1;

            //get the last file name
            string numString = Convert.ToString(num);
            string zero = "";
            string pathBefore = "/Resources/OpenPose/";
            for (int j = 0; j < (12 - numString.Length); j++)
            {
                zero = zero + "0";
            }
            Debug.Log(pathBefore + zero + numString + "_keypoints.json");

            //analysis data from json
            string jsonTest = File.ReadAllText(Application.dataPath + pathBefore + zero + numString + "_keypoints.json", Encoding.UTF8);
            ModelTest obj = JsonUtility.FromJson<ModelTest>(jsonTest);

            //pervent joints return to (0,0,0)
            for (int i = 0; i < 18; i++)
            {
                if (obj.people[0].pose_keypoints_2d[3 * i] == 0.0f)
                {

                }
                else
                {
                    pose_joint_x[i] = obj.people[0].pose_keypoints_2d[3 * i];
                }

                if (obj.people[0].pose_keypoints_2d[3 * i + 1] == 0.0f)
                {

                }
                else
                {
                    pose_joint_y[i] = obj.people[0].pose_keypoints_2d[3 * i + 1];
                }
            }
        }
        catch (ArgumentOutOfRangeException)
        {

        }

        //stay if file doesn't reach the num
        catch (FileNotFoundException)
        {
            num = num - 1;
        }
    }
    
    //private void FixedUpdate()
    //{
    //    num += 1;

    //    ModelTest obj = TryFigure(num);

    //    //将关节点坐标还原
    //    for (int i = 0; i < 18; i++)
    //    {
    //        if (obj.people[0].pose_keypoints_2d[3 * i] == 0.0f)
    //        {

    //        }
    //        else
    //        {
    //            pose_joint_x[i] = obj.people[0].pose_keypoints_2d[3 * i];
    //        }

    //        if (obj.people[0].pose_keypoints_2d[3 * i + 1] == 0.0f)
    //        {

    //        }
    //        else
    //        {
    //            pose_joint_y[i] = obj.people[0].pose_keypoints_2d[3 * i + 1];
    //        }
    //    }
    //}

}
    
