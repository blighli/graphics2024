using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;

public class MoveJoints : MonoBehaviour
{
    #region definition

    public float speed = 5f;
    //线段渲染器*4  
    private LineRenderer lineRenderer1;
    private LineRenderer lineRenderer2;
    private LineRenderer lineRenderer3;
    private LineRenderer lineRenderer4;

    private int lineLength = 300;

    GameObject LEar16;
    GameObject LEye14;
    GameObject MNose0;
    GameObject REye15;
    GameObject REar17;
    GameObject MMid1;
    GameObject LShoulder2;
    GameObject RShoulder5;
    GameObject LElbow3;
    GameObject RElbow6;
    GameObject LWrist4;
    GameObject RWrist7;
    GameObject LAss8;
    GameObject RAss11;
    GameObject LKnee9;
    GameObject RKnee12;
    GameObject LFoot10;
    GameObject RFoot13;

    #endregion

    //将场景的物体关联到该脚本中
    void InitObject()
    {
        LEar16 = GameObject.Find("LEar16");
        LEye14 = GameObject.Find("LEye14");
        MNose0 = GameObject.Find("MNose0");
        REye15 = GameObject.Find("REye15");
        REar17 = GameObject.Find("REar17");
        MMid1 = GameObject.Find("MMid1");
        LShoulder2 = GameObject.Find("LShoulder2");
        RShoulder5 = GameObject.Find("RShoulder5");
        LElbow3 = GameObject.Find("LElbow3");
        RElbow6 = GameObject.Find("RElbow6");
        LWrist4 = GameObject.Find("LWrist4");
        RWrist7 = GameObject.Find("RWrist7");
        LAss8 = GameObject.Find("LAss8");
        RAss11 = GameObject.Find("RAss11");
        LKnee9 = GameObject.Find("LKnee9");
        RKnee12 = GameObject.Find("RKnee12");
        LFoot10 = GameObject.Find("LFoot10");
        RFoot13 = GameObject.Find("RFoot13");

        //   Line1 = GameObject.Find("Line1");

        lineRenderer1 = (LineRenderer)MNose0.GetComponent("LineRenderer");
        lineRenderer2 = (LineRenderer)MMid1.GetComponent("LineRenderer");
        lineRenderer3 = (LineRenderer)LShoulder2.GetComponent("LineRenderer");
        lineRenderer4 = (LineRenderer)LElbow3.GetComponent("LineRenderer");
    }

    // Use this for initialization
    void Start()
    {
        InitObject();
        lineRenderer1.positionCount = lineLength;
        lineRenderer2.positionCount = lineLength;
        lineRenderer3.positionCount = lineLength;
        lineRenderer4.positionCount = lineLength;

        lineRenderer1.positionCount = 7;
        lineRenderer2.positionCount = 7;
        lineRenderer3.positionCount = 4;
        lineRenderer4.positionCount = 4;
    }

    // Update is called once per frame

    void Update()
    {
        float step = speed * Time.deltaTime;
        //move joints using interpolation method
        LEar16.transform.localPosition = new Vector3(Mathf.Lerp(LEar16.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[16], step), Mathf.Lerp(LEar16.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[16], step), 0);
        MNose0.transform.localPosition = new Vector3(Mathf.Lerp(MNose0.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[0], step), Mathf.Lerp(MNose0.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[0], step), 0);
        LEye14.transform.localPosition = new Vector3(Mathf.Lerp(LEye14.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[14], step), Mathf.Lerp(LEye14.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[14], step), 0);
        REye15.transform.localPosition = new Vector3(Mathf.Lerp(REye15.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[15], step), Mathf.Lerp(REye15.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[15], step), 0);
        MMid1.transform.localPosition = new Vector3(Mathf.Lerp(MMid1.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[1], step), Mathf.Lerp(MMid1.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[1], step), 0);
        LShoulder2.transform.localPosition = new Vector3(Mathf.Lerp(LShoulder2.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[2], step), Mathf.Lerp(LShoulder2.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[2], step), 0);
        RShoulder5.transform.localPosition = new Vector3(Mathf.Lerp(RShoulder5.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[5], step), Mathf.Lerp(RShoulder5.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[5], step), 0);
        LElbow3.transform.localPosition = new Vector3(Mathf.Lerp(LElbow3.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[3], step), Mathf.Lerp(LElbow3.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[3], step), 0);
        RElbow6.transform.localPosition = new Vector3(Mathf.Lerp(RElbow6.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[6], step), Mathf.Lerp(RElbow6.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[6], step), 0);
        LWrist4.transform.localPosition = new Vector3(Mathf.Lerp(LWrist4.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[4], step), Mathf.Lerp(LWrist4.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[4], step), 0);
        RWrist7.transform.localPosition = new Vector3(Mathf.Lerp(RWrist7.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[7], step), Mathf.Lerp(RWrist7.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[7], step), 0);
        LAss8.transform.localPosition = new Vector3(Mathf.Lerp(LAss8.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[8], step), Mathf.Lerp(LAss8.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[8], step), 0);
        RAss11.transform.localPosition = new Vector3(Mathf.Lerp(RAss11.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[11], step), Mathf.Lerp(RAss11.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[11], step), 0);
        LKnee9.transform.localPosition = new Vector3(Mathf.Lerp(LKnee9.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[9], step), Mathf.Lerp(LKnee9.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[9], step), 0);
        RKnee12.transform.localPosition = new Vector3(Mathf.Lerp(RKnee12.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[12], step), Mathf.Lerp(RKnee12.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[12], step), 0);
        LFoot10.transform.localPosition = new Vector3(Mathf.Lerp(LFoot10.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[10], step), Mathf.Lerp(LFoot10.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[10], step), 0);
        RFoot13.transform.localPosition = new Vector3(Mathf.Lerp(RFoot13.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[13], step), Mathf.Lerp(RFoot13.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[13], step), 0);
        REar17.transform.localPosition = new Vector3(Mathf.Lerp(REar17.transform.localPosition.x, 0.1f * JsonTest.pose_joint_x[17], step), Mathf.Lerp(REar17.transform.localPosition.y, -0.1f * JsonTest.pose_joint_y[17], step), 0);

        //it requires 4 lines to connect all the joints
        lineRenderer1.SetPosition(0, LEar16.transform.localPosition);
        lineRenderer1.SetPosition(1, LEye14.transform.localPosition);
        lineRenderer1.SetPosition(2, MNose0.transform.localPosition);
        lineRenderer1.SetPosition(3, MMid1.transform.localPosition);
        lineRenderer1.SetPosition(4, LShoulder2.transform.localPosition);
        lineRenderer1.SetPosition(5, LElbow3.transform.localPosition);
        lineRenderer1.SetPosition(6, LWrist4.transform.localPosition);

        lineRenderer2.SetPosition(0, REar17.transform.localPosition);
        lineRenderer2.SetPosition(1, REye15.transform.localPosition);
        lineRenderer2.SetPosition(2, MNose0.transform.localPosition);
        lineRenderer2.SetPosition(3, MMid1.transform.localPosition);
        lineRenderer2.SetPosition(4, RShoulder5.transform.localPosition);
        lineRenderer2.SetPosition(5, RElbow6.transform.localPosition);
        lineRenderer2.SetPosition(6, RWrist7.transform.localPosition);

        lineRenderer3.SetPosition(0, LFoot10.transform.localPosition);
        lineRenderer3.SetPosition(1, LKnee9.transform.localPosition);
        lineRenderer3.SetPosition(2, LAss8.transform.localPosition);
        lineRenderer3.SetPosition(3, MMid1.transform.localPosition);

        lineRenderer4.SetPosition(0, RFoot13.transform.localPosition);
        lineRenderer4.SetPosition(1, RKnee12.transform.localPosition);
        lineRenderer4.SetPosition(2, RAss11.transform.localPosition);
        lineRenderer4.SetPosition(3, MMid1.transform.localPosition);
    }
}
