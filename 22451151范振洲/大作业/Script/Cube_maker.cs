using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cube_maker : MonoBehaviour
{
    public GameObject cube;
    public int dis = 2;

    [InspectorButton("生成定位点")]
    public void Make_cube()
    {
        var x = cube.transform.position.x;
        var y = cube.transform.position.z;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                //对第一个已经在场景中的模型的特殊处理
                if (i == 0 && j == 0)
                {
                    x-=dis;
                    continue;
                }
                var cube_new = GameObject.Instantiate(cube);
                cube_new.transform.position = new Vector3(x, cube.transform.position.y, y);

                cube_new.name = j.ToString() + i.ToString();
                cube_new.transform.parent = cube.transform.parent;
                x-=dis;
            }
            x = cube.transform.position.x;
            y-=dis;
        }
    }
}
