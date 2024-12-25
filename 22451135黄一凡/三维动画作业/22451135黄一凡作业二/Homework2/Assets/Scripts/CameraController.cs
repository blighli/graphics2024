using UnityEngine;

public class CameraController : MonoBehaviour
{
    private Camera _camera;
    private const  float _cameraPanSpeed = 5f;
    private const float _cameraZoomSpeed = 0.3f;
    
    private Vector3 lastMousePosition;

    private void Awake()
    {
        _camera = Camera.main;
    }

    private void Update()
    {
        // 使用鼠标滚轮控制相机缩放
        if (Input.mouseScrollDelta.y > 0f)
        {
            _camera.orthographicSize -= Input.mouseScrollDelta.y * _cameraZoomSpeed;
        }
        else if (Input.mouseScrollDelta.y < 0f)
        {
            _camera.orthographicSize -= Input.mouseScrollDelta.y * _cameraZoomSpeed;
        }
        
        // 使用鼠标左键控制相机移动
        if (Input.GetMouseButton(0)) 
        {
            Vector3 mouseDelta = Input.mousePosition - lastMousePosition;
            Vector3 panMovement = new Vector3(-mouseDelta.x * _cameraPanSpeed * Time.deltaTime, -mouseDelta.y * _cameraPanSpeed * Time.deltaTime, 0);
            transform.Translate(panMovement, Space.Self);
        }
        
        // 更新上一帧鼠标位置
        lastMousePosition = Input.mousePosition;
    }
}
