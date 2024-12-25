/*****
 * 用来存放可以传递的方法
 * 只要有需要传递的方法，只需要令该类继承该方法即可找到
 * ***/
using DataProtocol;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;

public class BaseSendOrder : MonoBehaviour
{
    protected virtual void Awake()
    {
        StartCoroutine(Init());
    }

    IEnumerator Init()
    {
        yield return new WaitUntil(() => OrderManager.Instance != null);
        OrderManager.Instance.BaseSendOrderList.Add(this);

    }

    protected virtual void OnDestroy()
    {
        OrderManager.Instance.BaseSendOrderList.Remove(this);
    }
}
