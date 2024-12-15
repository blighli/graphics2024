using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class StoneAllModelController : MonoBehaviour
{
    public static StoneAllModelController Instance;
    //-2代表没有棋子直接存储棋子编号(因为编号里面有0)
    public int[][] Qi_Pan=new int[9][];
    public List<GameObject> Model;
    //生成棋盘定位点的案例
    [Tooltip("生成棋盘定位点的案例")]
    public GameObject cube;
    public Transform[][] Qi_Pan_Pos = new Transform[9][];
    [Tooltip("控制棋子的移动速度 0.01为慢速1为快速")]
    public float speed = 0.01f;

    private void Awake()
    {
        Instance = this;
        //初始化总的二维数组
        for(int i=0;i<9;i++)
        {
            Qi_Pan[i] = new int[10];
            for(int j=0;j<10;j++)
            {
                Qi_Pan[i][j] = -2;
            }
        }
        Init_Qi_Pan_Pos();
  

    }

    //初始化棋盘定位点
    void Init_Qi_Pan_Pos()
    {
        
        int a = 0;
        var pan = GameObject.Find("3D棋盘");
        //开辟空间
        for(int i=0;i<9;i++)
        {
            Qi_Pan_Pos[i] = new Transform[10];
        }
        for(int i=0;i<10;i++)
        {
            for(int j=0;j<9;j++)
            {
                Qi_Pan_Pos[j][i] = pan.transform.GetChild(a);
                a++;
            }
        }
        print(Qi_Pan_Pos[0][0].position);
    }
    //初始化棋子的位置
    void 
    Init_Stone_pos()
    {
        for(int i=0;i<Model.Count;i++)
        {
            Model[i].GetComponent<NavMeshAgent>().enabled = false;
            //获得对应位置坐标
            int x = (int)Model[i].GetComponent<StoneModelManager>().pos.x;
            int y = (int)Model[i].GetComponent<StoneModelManager>().pos.y;
            Model[i].transform.position = new Vector3(Qi_Pan_Pos[x][y].position.x, Model[i].transform.position.y, Qi_Pan_Pos[x][y].position.z);
           
        }
        
    }

        //对棋子模型数据的初始化更新
        void Init()
        {
            //找到所有模型
            GameObject oo = GameObject.Find("棋盘模型");
            for (int i = 0; i < oo.transform.childCount; i++)
            {
                Model.Add(oo.transform.GetChild(i).gameObject);
                //添加编号
                oo.transform.GetChild(i).gameObject.GetComponent<StoneModelManager>().id = i;
            }
            //把模型的位置表更新
            for (int i = 0; i < Model.Count; i++)
            {
                if (Model[i].GetComponent<StoneModelManager>()._red)
                    Qi_Pan[(int)Model[i].GetComponent<StoneModelManager>().pos.x][(int)Model[i].GetComponent<StoneModelManager>().pos.y] = i;
                else
                    Qi_Pan[(int)Model[i].GetComponent<StoneModelManager>().pos.x][(int)Model[i].GetComponent<StoneModelManager>().pos.y] = i;
            }
        }

    private void Start()
    {
        Init();
        Init_Stone_pos();
    }




}
