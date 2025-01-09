using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class camera_click : MonoBehaviour
{
    public bool isLocked = true;
    public bool isRunning = false;
    private my_grid grid;
    // Start is called before the first frame update
    void Start()
    {
        this.grid = new my_grid(50, 50, 50, 1.0f);
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonDown(1))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit)) {
                int x = (int) (hit.transform.position[0] + hit.normal[0]);
                int y = (int) (hit.transform.position[1] + hit.normal[1]);
                int z = (int) (hit.transform.position[2] + hit.normal[2]);
                this.grid.addCell(x,y,z);
                
            }
        }

        if (Input.GetMouseButtonDown(0))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit))
            {
                int x = (int)hit.transform.position[0];
                int y = (int)hit.transform.position[1];
                int z = (int)hit.transform.position[2];
                Debug.Log(x + ", "+y +", "+z);
                this.grid.delCell(x, y, z);
            }
        }

        if (Input.GetKeyDown(KeyCode.R))
        {
            this.grid.addCell(0,0,0);
        }

        if (Input.GetKeyDown(KeyCode.Return))
        {
            isRunning = !isRunning;
        }


        if (Input.GetKey(KeyCode.Escape))
        {
            isLocked = !isLocked;
        }
        Cursor.visible = !isLocked;
        Cursor.lockState = isLocked ? CursorLockMode.Locked : CursorLockMode.None;

        if (Time.frameCount % 150 == 0 && isRunning)
        {
            this.grid.update();
        }
    }
}
