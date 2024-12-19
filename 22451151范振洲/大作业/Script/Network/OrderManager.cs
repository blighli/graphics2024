using DataProtocol;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;

public class OrderManager : MonoBehaviour
{
    public static OrderManager Instance;
    public List<BaseSendOrder> BaseSendOrderList = new List<BaseSendOrder>();
    void Awake()
    {
        Instance = this;
    }

   public void HandlerOrder(OrderInfo order)
    {
        print("1");
        string method = order.MethodName;
        object[] parameters = order.Paramerer;

        MethodInfo mi=null;
        BaseSendOrder baseOrder = null;
        for(int i=0;i<BaseSendOrderList.Count;i++)
        {
            mi = BaseSendOrderList[i].GetType().GetMethod(method); 
            if(mi!=null)
            {
                
                baseOrder = BaseSendOrderList[i];
                break;
            }
        }
        if(mi!=null)
        {
            mi.Invoke(baseOrder, parameters);
        }
        else
        {
            print("-->方法没有找到!");
        }

    }
}
