using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class DisplayName : MonoBehaviour
{
    [SerializeField] private GameObject _text;
    private void OnMouseEnter()
    {
        _text.SetActive(true);
    }

    private void OnMouseExit()
    {
        _text.SetActive(false);
    }
}
