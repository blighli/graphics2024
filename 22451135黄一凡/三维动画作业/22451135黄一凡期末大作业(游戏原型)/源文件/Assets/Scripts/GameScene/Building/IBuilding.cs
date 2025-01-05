using System.Collections.Generic;
using UnityEngine;

public interface IBuilding
{
    public GameObject GameObject { get; }
    public Collider Collider { get; }
    public static Color Red = new Color(1, 0, 0, 0.5f);
    public static Color Green = new Color(0, 1, 0, 0.5f);
    public List<Material> MaterialList { get; set; }
    
    public virtual void InitOnPreview()
    {
        Collider.enabled = false;
        MeshRenderer[] meshRenderers = GameObject.GetComponentsInChildren<MeshRenderer>();
        MaterialList = new List<Material>(10);
        for (int i = 0; i < meshRenderers.Length; i++)
        {
            MaterialList.AddRange(meshRenderers[i].materials);
        }
        for (int i = 0; i < MaterialList.Count; i++)
        {
            MaterialList[i].color = Red;
            ProjectTool.SetMaterialRenderingMode(MaterialList[i], ProjectTool.RenderingMode.Fade);
        }
        OnPreview();
    }

    public void OnPreview();
    
    public void SetColorOnPreview(bool isRed)
    {
        for (int i = 0; i < MaterialList.Count; i++)
        {
            MaterialList[i].color = isRed ? Red : Green;
        }
    }
}
