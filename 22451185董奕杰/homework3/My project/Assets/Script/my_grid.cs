using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class my_grid
{
    private int width;
    private int height;
    private int length;
    private float cellSize;
    private int[,,] grid;
    private GameObject[,,] cellGrid;
    public int firstThread = 7;
    public int lastThread = 4;
    public int maxThread = 10;

    public my_grid(int width, int length, int height, float cellSize)
    {
        this.width = width;
        this.height = height;
        this.length = length;
        this.cellSize = cellSize;

        this.grid = new int[width, length, height];
        this.cellGrid = new GameObject[width, length, height];
        addCell(0, 0, 0);
        Debug.Log(width + " " + length + " " + height);
    }

    public void update()
    {
        int[,,] next_state = new int[this.width, this.length, this.height];
        for (int i = 0; i < this.width; i++)
        {
            for (int j = 0; j < this.length; j++)
            {
                for (int k = 0; k < this.height; k++)
                {
                    int counter = countNeighbor(i, j, k);
                    if (counter > firstThread && counter <= maxThread)
                    {
                        next_state[i, j, k] = 1;
                    }
                    else if (counter > lastThread && counter <= firstThread)
                    {
                        next_state[i, j, k] = this.grid[i, j, k];
                    }
                    else
                    {
                        next_state[i, j, k] = 0;
                    }
                    if (next_state[i, j, k] == 0 && this.cellGrid[i, j, k] != null)
                    {
                        GameObject.Destroy(cellGrid[i, j, k], 0.0f);
                        this.cellGrid[i, j, k] = null;
                    }
                    else if (next_state[i, j, k] == 1 && this.cellGrid[i, j, k] == null)
                    {
                        Debug.Log("Add Cell:");
                        var (x_index, y_index, z_index) = array2index(i, j, k);
                        var (x_i, y_i, z_i) = index2array(x_index, y_index, z_index);
                        var (x, y, z) = index2pos(x_i, y_i, z_i);
                        Vector3 position = new Vector3(x, y, z);
                        Debug.Log(x + " " + y + " " + z);
                        GameObject cell = GameObject.Instantiate(Resources.Load("Cell"), position, Quaternion.identity) as GameObject;
                        this.cellGrid[x_i, y_i, z_i] = cell;
                    }

                }
            }
        }
        this.grid = next_state;
    }

    public void addCell(int x_index, int y_index, int z_index)
    {
        var (x_i, y_i, z_i) = index2array(x_index, y_index, z_index);
        if (this.grid[x_i, y_i, z_i] == 1)
        {
            return;
        }
        this.grid[x_i, y_i, z_i] = 1;
        var(x, y, z) = index2pos(x_i, y_i, z_i);
        Vector3 position = new Vector3(x, y, z);
        Debug.Log(x + " " + y + " " + z);
        GameObject cell = GameObject.Instantiate(Resources.Load("Cell"), position, Quaternion.identity) as GameObject;
        this.cellGrid[x_i, y_i, z_i] = cell;
    }

    public void delCell(int x_index, int y_index, int z_index)
    {
        Debug.Log("delCell");
        var (x_i, y_i, z_i) = index2array(x_index, y_index, z_index);
        
        Debug.Log(x_i + " " + y_i + " " + z_i);
        this.grid[x_i, y_i, z_i] = 0;
        GameObject cell = this.cellGrid[x_i, y_i, z_i];
        GameObject.Destroy(cell, 0.0f);
        this.cellGrid[x_i, y_i, z_i] = null;
    }

    private int countNeighbor(int x, int y, int z) {
        int res = 0;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++){
                for (int k = -1; k <= 1; k++){
                    if (x+i >= this.width || x+i<0 || y+j>=this.length || y+j<0 || z+k>=this.height || z+k<0)
                        continue;
                    if (i == 0 && j == 0 && k == 0)
                        continue;
                    res += this.grid[x + i, y + j, z + k];
                }
            }
        }
        //res -= this.grid[x,y,z];
        return res;
    }

    private (int x_inex, int y_index, int z_index) index2array(int x, int y, int z)
    {
        int x_index = x + this.width/2;
        int y_index = y + this.height/2;
        int z_index = z + this.height/2;
        return (x_index, y_index, z_index);
    }

    private (int x_inex, int y_index, int z_index) array2index(int x, int y, int z)
    {
        int x_index = x - this.width / 2;
        int y_index = y - this.height / 2;
        int z_index = z - this.height / 2;
        return (x_index, y_index, z_index);
    }

    public (int x_inex, int y_index, int z_index) pos2index(float x, float y, float z)
    {
        int x_index = (int) (x / cellSize);
        int y_index = (int) (y / cellSize);
        int z_index = (int) (z / cellSize);
        var (x_res, y_res, z_res) = index2array(x_index, y_index, z_index);
        return (x_res, y_res, z_res);
    }

    private (float x, float y, float z) index2pos(int x_index, int y_index, int z_index)
    {
        x_index -= this.width / 2;
        y_index -= this.length / 2;
        z_index -= this.height / 2;
        float x = (float) x_index * this.cellSize;
        float y = (float) y_index * this.cellSize;
        float z = (float) z_index * this.cellSize;
        return (x, y, z);
    }

}
