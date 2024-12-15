using UnityEngine;
using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Collections.Generic;

[Serializable]
public class ModelTest
{
    public float version;
    public List<People_sets> people;
}

[Serializable]
public class People_sets
{
    public float[] pose_keypoints_2d;
}
