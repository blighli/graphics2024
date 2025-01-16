using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class camera_controller : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    public float horizontalinput;//水平参数
    public float Verticalinput;//垂直参数
    float speed = 10.0f;//声明一个参数，没有规定
    public float Zv = 0;

    void Update()   
    {
        horizontalinput = Input.GetAxis("Horizontal");
        Verticalinput = Input.GetAxis("Vertical");
        
        
        if (Input.GetKey(KeyCode.Space))
        {
            Zv += 0.01533f;
        }
        else if (Input.GetKey(KeyCode.X))
        {
            Zv -= 0.01533f;
        }
        else if (Zv < 0)
        {
            Zv += 0.01533f;
        }
        else if (Zv > 0)
        {
            Zv -= 0.01533f;
        }
        Zv = Mathf.Clamp(Zv, -1, 1);
        if ((Zv > 0 && Zv < 0.01f) || (Zv < 0 && Zv > -0.01f))
        {
            Zv = 0f;
        }
        if (horizontalinput != 0 && Verticalinput != 0)
        {
            horizontalinput = horizontalinput * 0.6f;
            Verticalinput = Verticalinput * 0.6f;
        }

        this.transform.Translate(Vector3.right * horizontalinput * Time.deltaTime * speed);
        this.transform.Translate(Vector3.forward * Verticalinput * Time.deltaTime * speed);
        this.transform.Translate(Vector3.up * Zv * Time.deltaTime * speed);
    }
}
