using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using JKFrame;

/// <summary>
/// 可击打的地图对象基类
/// </summary>
public abstract class HitMapObjectBase : MapObjectBase
{
    [SerializeField] private Animator animator;
    [SerializeField] private AudioClip[] hurtAudioClips;
    [SerializeField] private float maxHp;
    [SerializeField] private int lootConfigID = -1; // 死亡时掉落的配置ID
    private float hp;

    public override void Init(MapChunkController mapChunk, ulong id, bool isFromBuild)
    {
        base.Init(mapChunk, id, isFromBuild);
        hp = maxHp;
    }
    
    public void Hurt(float damage)
    {
        hp -= damage;
        if (hp <= 0)
        {
            // 死亡
            Dead();
        }
        else
        {
            animator.SetTrigger("Hurt");
        }
        AudioManager.Instance.PlayOnShot(hurtAudioClips[Random.Range(0,hurtAudioClips.Length)],transform.position);
    }

    private void Dead()
    {
        RemoveOnMap();
        // 物品掉落逻辑
        if (lootConfigID == -1) return;
        LootConfig lootConfig = ConfigManager.Instance.GetConfig<LootConfig>(ConfigName.Loot, lootConfigID);
        if (lootConfig != null) lootConfig.GenerateMapObject(mapChunk, transform.position + new Vector3(0, 1, 0));
    }
}
