using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class camera_perspect : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    private float X = 0;
    private float Y = 0;
    public float Xmax = 90;
    public float Xmin = -90;

    // Update is called once per frame
    void Update()
    {
        X -= Input.GetAxis("Mouse Y") * 2;
        Y += Input.GetAxis("Mouse X") * 2;
        X = Mathf.Clamp(X, Xmin, Xmax);
        this.transform.localEulerAngles = new Vector3(X, Y, 0);
    }
}
