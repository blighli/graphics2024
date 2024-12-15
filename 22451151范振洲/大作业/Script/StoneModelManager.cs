using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.AI;
using static StoneManager;
using static UnityEngine.GraphicsBuffer;

public class StoneModelManager : MonoBehaviour
{
    public struct Stone
    {
        public enum TYPE { JIANG, CHE, PAO, MA, BING, SHI, XIANG };

        //棋子的ID
        public int _id;

        //棋子是红子还是黑子,ID小于16的是红子，大于16是黑子
        public bool _red;

        //棋子是否死亡
        public bool _dead;

        //棋子在棋盘中的位置,
        public Vector2 _pos;

        //棋子的类型
        public TYPE _type;
       

        //棋子初始化，赋予32个棋子对应的属性参数
    }
    public Stone stone;
    public Vector2 pos;
    public bool _red;
    //记录自身在管理器的编号
    public int id;
    //测试参数
    public int a, b;
    bool _move, attack, _dead;
    GameObject move_target;
    //平滑运动参数
    float t1 = 0;
    private  bool is_move_no_target = false;

    //用来进行对自己类型的自定义
    void Init()
    {
        //对棋子颜色的判断
        stone._red = (name[0] == '红');
        stone._dead = false;
        string a = "";
       for(int i=1;i<name.Length;i++)
       {
                a += name[i];
        }
        

        //对黑棋的处理和对红棋的预处理实现对其的位置初始化
        switch(a)
        {
            case "车1":
                stone._pos = new Vector2(0f, 0f);
                break;
            case "马1":
                stone._pos = new Vector2(1f, 0f);
                break;
            case "象1":
                stone._pos = new Vector2(2f, 0f);
                break;
            case "士1":
                stone._pos = new Vector2(3f, 0f);
                break;
            case "将1":
                stone._pos = new Vector2(4f, 0f);
                break;
            case "士2":
                stone._pos = new Vector2(5f, 0f);
                break;
            case "象2":
                stone._pos = new Vector2(6f, 0f);
                break;
            case "马2":
                stone._pos = new Vector2(7f, 0f);
                break;
            case "车2":
                stone._pos = new Vector2(8f, 0f);
                break;
            case "炮1":
                stone._pos = new Vector2(1f, 2f);
                break;
            case "炮2":
                stone._pos = new Vector2(7f, 2f);
                break;
            case "兵1":
                stone._pos = new Vector2(0f, 3f);
                break;
            case "兵2":
                stone._pos = new Vector2(2f, 3f);
                break;
            case "兵3":
                stone._pos = new Vector2(4f, 3f);
                break;
            case "兵4":
                stone._pos = new Vector2(6f, 3f);
                break;
            case "兵5":
                stone._pos = new Vector2(8f, 3f);
                break;
        }
        //对红棋的对称处理
        if(stone._red)
        {
            stone._pos = new Vector2(stone._pos.x, 9 - stone._pos.y);
        }
        _red = stone._red;
    }

    //移动棋子的函数
    [InspectorButton("移动测试")]
    public void movetest()
    {
        Move(a, b);
    }
    public void Move(int a,int b)
    {
        int x = (int)(pos.x + a);
        int y = (int)(pos.y + b);
        print(x + "  " + y);
        //判断移动点有没有棋子
        if (StoneAllModelController.Instance.Qi_Pan[x][y]==-2)
        {
            StoneAllModelController.Instance.Qi_Pan[x][y] = id;
            StoneAllModelController.Instance.Qi_Pan[(int)pos.x][(int)pos.y] = -2;
            pos = new Vector3(x, y);
            //to do 真实移动物体坐标
            move_target = StoneAllModelController.Instance.Qi_Pan_Pos[x][y].gameObject;
            is_move_no_target = true;
            this.GetComponent<Animator>().SetTrigger("walk");
            this.GetComponent<Animator>().ResetTrigger("idle");
            Move_To_point();
        }
        else 
        {
            //棋子移动的位置上是己方的棋子
            if (StoneAllModelController.Instance.Model[StoneAllModelController.Instance.Qi_Pan[x][y]].GetComponent<StoneModelManager>()._red==_red)
            {
                print("无法移动该位置是己方棋子：" + StoneAllModelController.Instance.Model[StoneAllModelController.Instance.Qi_Pan[x][y]].name);
            }
            //是对方棋子
            else
            {
                var agent = GetComponent<NavMeshAgent>();
                var target = StoneAllModelController.Instance.Model[StoneAllModelController.Instance.Qi_Pan[x][y]];
                transform.LookAt(target.transform);
                //this.GetComponent<NavMeshAgent>().destination=target.transform.position;
                this.GetComponent<Animator>().SetTrigger("walk");
                this.GetComponent<Animator>().ResetTrigger("idle");
                _move = true;
                move_target = target;
                StoneAllModelController.Instance.Qi_Pan[(int)pos.x][(int)pos.y] = -2;
                pos = new Vector3(x, y);
                StoneAllModelController.Instance.Qi_Pan[(int)pos.x][(int)pos.y] = id;
            }
        }
    }

    //当点上没有棋子时进行移动
    private void Move_To_point()
    {
        if (!is_move_no_target) return;
        transform.LookAt(move_target.transform);
        t1 += Time.deltaTime * StoneAllModelController.Instance.speed;
        //通过速度控制动画
        GetComponent<Animator>().speed =Mathf.Lerp(GetComponent<Animator>().speed, 2.0f, t1);
        transform.position = Vector3.Lerp(transform.position, move_target.transform.position, t1);
        if (Vector3.Distance(move_target.transform.position, transform.position) < 0.5f)
        {
            is_move_no_target = false;
            GetComponent<Animator>().speed = 1f;
            this.GetComponent<Animator>().ResetTrigger("walk");
            this.GetComponent<Animator>().SetTrigger("idle");
            t1 = 0f;
        }
    }

    private void Awake()
    {
        Init();
        pos = stone._pos;
        _move= attack= _dead = false;
    }

    //对攻击对象与自身距离的判定函数
    void MovingToTarget()
    {
        if(_move)
        {
            t1 += Time.deltaTime*StoneAllModelController.Instance.speed;
            //通过速度控制动画
            GetComponent<Animator>().speed = Mathf.Lerp(GetComponent<Animator>().speed, 2.0f, t1);
            transform.position = Vector3.Lerp(transform.position, move_target.transform.position, t1);
            //平滑运动
            if (Vector3.Distance(move_target.transform.position,transform.position)<1f)
        {
                GetComponent<Animator>().speed = 1f;
                _move = false;
            attack = true;
                this.GetComponent<Animator>().ResetTrigger("attack");
            this.GetComponent<Animator>().SetTrigger("attack");
             t1 = 0f;
        }
        }
    }
    //关键帧使用的杀死棋子的函数
    public void kill_Stone()
    {
        move_target.GetComponent<StoneModelManager>()._dead = true;
        move_target.GetComponent<Animator>().SetTrigger("dead");
        move_target = null;
        GetComponent<Animator>().ResetTrigger("attack");
        GetComponent<Animator>().SetTrigger("idle");
    }
    //死亡关键帧函数
    public void dead()
    {
        //to do 
        //制作渐变消失动画和网络同步
        this.gameObject.SetActive(false);
    }
    //对自己位置的更新
    [InspectorButton("更新位置")]
    public void UpDdate_Pos()
    {
        var par = transform.parent;
        transform.parent = StoneAllModelController.Instance.Qi_Pan_Pos[(int)pos.x][(int)pos.y];
        transform.localPosition = new Vector3(0f, transform.localPosition.y,0f);
        
        
    }
    private void Update()
    {
        MovingToTarget();
        Move_To_point();
    }

}
