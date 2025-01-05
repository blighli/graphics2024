using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class StoneManager : MonoBehaviour
{

    //初始化14种棋子的预制体
    public GameObject che_black;
    public GameObject ma_black;
    public GameObject xiang_black;
    public GameObject shi_black;
    public GameObject jiang_black;
    public GameObject pao_black;
    public GameObject bing_black;

    public GameObject che_red;
    public GameObject ma_red;
    public GameObject xiang_red;
    public GameObject shi_red;
    public GameObject jiang_red;
    public GameObject pao_red;
    public GameObject bing_red;

    //初始化32个棋子
    public static Stone[] s = new Stone[32];

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
        public float _x;
        public float _y;

        //棋子的类型
        public TYPE _type;

        //棋子初始化，赋予32个棋子对应的属性参数
        public void init(int id)
        {
            _id = id;
            _red = id < 16;
            _dead = false;

            //每个点上的棋子的类型
            StonePos[] pos = {
                                 new StonePos(-2.04f,2.25f,Stone.TYPE.CHE),
                                 new StonePos(-1.53f,2.25f,Stone.TYPE.MA),
                                 new StonePos(-1.02f,2.25f,Stone.TYPE.XIANG),
                                 new StonePos(-0.51f,2.25f,Stone.TYPE.SHI),
                                 new StonePos(0,2.25f,Stone.TYPE.JIANG),
                                 new StonePos(0.51f,2.25f,Stone.TYPE.SHI),
                                 new StonePos(1.02f,2.25f,Stone.TYPE.XIANG),
                                 new StonePos(1.53f,2.25f,Stone.TYPE.MA),
                                 new StonePos(2.04f,2.25f,Stone.TYPE.CHE),

                                 new StonePos(-1.53f,1.23f,Stone.TYPE.PAO),
                                 new StonePos(1.53f,1.23f,Stone.TYPE.PAO),

                                 new StonePos(-2.04f,0.72f,Stone.TYPE.BING),
                                 new StonePos(-1.02f,0.72f,Stone.TYPE.BING),
                                 new StonePos(0,0.72f,Stone.TYPE.BING),
                                 new StonePos(1.02f,0.72f,Stone.TYPE.BING),
                                 new StonePos(2.04f,0.72f,Stone.TYPE.BING),
                             };
            if (id < 16)
            {
                _x = pos[id].x;
                _y = pos[id].y;
                _type = pos[id].type;
            }
            else
            {
                _x = -pos[id - 16].x;
                _y = -pos[id - 16].y - 0.1f;
                _type = pos[id - 16].type;
            }
        }
    }


    /// <summary>
    /// 通过棋子的坐标，获得对应的类型，将三个值关联在一起
    /// </summary>
    public struct StonePos
    {
        public float x, y;
        public Stone.TYPE type;
        public StonePos(float _x, float _y, Stone.TYPE _type)
        {
            x = _x;
            y = _y;
            type = _type;
        }
    }


    /// <summary>
    /// 通过棋子的ID和类型赋予棋子对应的预制体
    /// </summary>
    /// <param name="id"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public GameObject GetPrefab(int id, StoneManager.Stone.TYPE type)
    {
        if (id < 16)
        {
            switch (type)
            {
                case StoneManager.Stone.TYPE.CHE:
                    return che_red;
                case StoneManager.Stone.TYPE.MA:
                    return ma_red;
                case StoneManager.Stone.TYPE.XIANG:
                    return xiang_red;
                case StoneManager.Stone.TYPE.SHI:
                    return shi_red;
                case StoneManager.Stone.TYPE.JIANG:
                    return jiang_red;
                case StoneManager.Stone.TYPE.PAO:
                    return pao_red;
                case StoneManager.Stone.TYPE.BING:
                    return bing_red;
            }
        }
        else
        {
            switch (type)
            {
                case StoneManager.Stone.TYPE.CHE:
                    return che_black;
                case StoneManager.Stone.TYPE.MA:
                    return ma_black;
                case StoneManager.Stone.TYPE.XIANG:
                    return xiang_black;
                case StoneManager.Stone.TYPE.SHI:
                    return shi_black;
                case StoneManager.Stone.TYPE.JIANG:
                    return jiang_black;
                case StoneManager.Stone.TYPE.PAO:
                    return pao_black;
                case StoneManager.Stone.TYPE.BING:
                    return bing_black;
            }
        }

        return bing_black;
    }


    /// <summary>
    /// 初始并实例化棋子
    /// </summary>
    public void StoneInit()
    {
        for (int i = 0; i < 32; ++i)
        {
            s[i].init(i);
        }

        //实例化32个棋子
        for (int i = 0; i < 32; ++i)
        {
            GameObject fabs = GetPrefab(i, s[i]._type);
            GameObject Stone = Instantiate(fabs, transform.localPosition, Quaternion.identity) as GameObject;
            Stone.name = i.ToString();
            Stone.transform.position = new Vector3(s[i]._x, s[i]._y, 0);
            Stone.AddComponent<BoxCollider>();
        }
    }

    

}