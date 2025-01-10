using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.Serialization;

/// <summary>
/// 可序列化的字典
/// </summary>
[Serializable]
public class Serialization_Dic <K,V>
{
    private List<K> keyList;
    private List<V> valueList;
    // 不序列化，避免报错
    [NonSerialized]
    private Dictionary<K, V> dictionary;

    public Dictionary<K, V> Dictionary { get => dictionary; }

    public Serialization_Dic()
    {
        dictionary = new Dictionary<K, V>();
    }
    
    public Serialization_Dic(Dictionary<K, V> dictionary)
    {
        this.dictionary = dictionary;
    }
    
    // 序列化的时候把字典里面的内容放进List
    [OnSerializing]
    private void OnSerializing(StreamingContext context)
    {
        keyList = new(dictionary.Count);
        valueList = new(dictionary.Count);
        foreach (var item in dictionary)
        {
            keyList.Add(item.Key);
            valueList.Add(item.Value);
        }
    }
    
    // 反序列化时候自动完成字典的初始化
    [OnDeserialized]
    private void OnDeserialized(StreamingContext context)
    {
        dictionary = new Dictionary<K, V>(keyList.Count);
        for (int i = 0; i < keyList.Count; i++)
        {
            dictionary.Add(keyList[i], valueList[i]);
        }
    }
}
