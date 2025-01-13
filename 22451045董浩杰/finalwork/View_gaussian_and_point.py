import time
from scene import RenderScene, Scene, GaussianModelSH
from gaussian_renderer import render,render_sh
from arguments import ModelParams, PipelineParams,OptimizationParams
from argparse import ArgumentParser
from scene.cameras import Camera
import  numpy as np
import copy
from utils.general_utils import safe_state
import  torch
import  cv2
import sys
import os

def getWorld2View(R, t):
    T = np.eye(4)
    Rt = np.eye(4)
    Rt[:3, :3] = R
    T[:3, 3] = t
    R_T = Rt @ T
    C2W = np.linalg.inv(R_T)
    cam_center = C2W[:3, 3]
    C2W[:3, 3] = cam_center
    Rt = np.linalg.inv(C2W)
    return np.float32(Rt)


def euler_angles_to_rotation_matrix(theta):
    R_x = np.array([[1, 0, 0],
                    [0, np.cos(theta[0]), -np.sin(theta[0])],
                    [0, np.sin(theta[0]), np.cos(theta[0])]
                    ])

    R_y = np.array([[np.cos(theta[1]), 0, np.sin(theta[1])],
                    [0, 1, 0],
                    [-np.sin(theta[1]), 0, np.cos(theta[1])]
                    ])

    R_z = np.array([[np.cos(theta[2]), -np.sin(theta[2]), 0],
                    [np.sin(theta[2]), np.cos(theta[2]), 0],
                    [0, 0, 1]
                    ])

    R = np.dot(R_z, np.dot(R_y, R_x))

    return R

def update_camera_position(Cam,delta_x, delta_y, delta_z):
    """ 更新相机位置 """
    Cam.T[0] += delta_x
    Cam.T[1] += delta_y
    Cam.T[2] += delta_z
    Cam.world_view_transform = torch.tensor(getWorld2View(Cam.R, Cam.T)).transpose(0,1).cuda()
    Cam.full_proj_transform = (
        Cam.world_view_transform.unsqueeze(0).bmm(Cam.projection_matrix.unsqueeze(0))
    ).squeeze(0)
    Cam.camera_center = Cam.world_view_transform.inverse()[3, :3]

def update_camera_rotation(Cam,delta_theta_x, delta_theta_y,delta_theta_z):
    """ 更新相机旋转 """
    theta[0] -= delta_theta_x
    theta[1] -= delta_theta_y
    theta[2] -= delta_theta_z
    Cam.R =euler_angles_to_rotation_matrix(theta)
    Cam.world_view_transform = torch.tensor(getWorld2View(Cam.R, Cam.T)).transpose(0, 1).cuda()
    Cam.full_proj_transform = (
        Cam.world_view_transform.unsqueeze(0).bmm(Cam.projection_matrix.unsqueeze(0))
    ).squeeze(0)
    Cam.camera_center = Cam.world_view_transform.inverse()[3, :3]


if __name__ == "__main__":
    # Set up command line argument parser
    parser = ArgumentParser(description="Training script parameters")
    lp = ModelParams(parser)
    op = OptimizationParams(parser)
    pp = PipelineParams(parser)
    # parser.add_argument('--ip', type=str, default="127.0.0.1")
    # parser.add_argument('--port', type=int, default=6009)
    parser.add_argument('--debug_from', type=int, default=-1)
    parser.add_argument('--detect_anomaly', action='store_true', default=False)
    parser.add_argument("--test_iterations", nargs="+", type=int,
                        default=[1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000])
    parser.add_argument("--save_iterations", nargs="+", type=int, default=[6000])
    parser.add_argument("--quiet", action="store_true")
    parser.add_argument("--checkpoint_iterations", nargs="+", type=int, default=[])
    parser.add_argument("--start_checkpoint", type=str, default=None)
    parser.add_argument("--near", type=int, default=0)
    args = parser.parse_args(sys.argv[1:])
    args.save_iterations.append(args.iterations)
    # args.checkpoint_iterations.append(args.iterations)

    print("Optimizing " + args.model_path)

    # Initialize system state (RNG)
    safe_state(args.quiet)

    frame_count = 0
    start_time = time.time()
    speed_move = 5

    name = "horns"
    checkpoint = r".\nerf_example_data\nerf_llff_data\fern\point_cloud.ply"

    args.source_path = r"./nerf_example_data\nerf_llff_data/"+name

    gaussians = GaussianModelSH(3)
    scene = Scene(lp.extract(args), gaussians)
    gaussians.load_ply_2(checkpoint)
    gaussians.training_setup(op)
    gaussians_point = copy.deepcopy(gaussians)

    gaussians_point._features_dc.requires_grad_(False)
    gaussians_point._features_dc[:, :, 0] = 255
    gaussians_point._features_dc[:, :, 1] = 0
    gaussians_point._features_dc[:, :,2] = 0

    print(gaussians._features_dc)

    Cam_list = scene.getTrainCameras()
    cam_list_lengths = len(Cam_list)
    cam_list_index = 0

    Cam = Cam_list[cam_list_index]


    background = torch.tensor([0,0,0], dtype=torch.float32, device="cuda")
    s_mod = 0.1
    pr = False

    for Cam in Cam_list:
        folder_path = os.path.join(".", name + "_render_and_point_notbase")
        os.makedirs(folder_path, exist_ok=True)
        Cam.set_picturesize(torch.tensor(torch.zeros((3, int(3024/4), int(4032/4))), dtype=torch.float32))

        image = render_sh(Cam, gaussians_point, pp, background, scaling_modifier=s_mod, point_render=True)['render']
        image2 = render_sh(Cam, gaussians, pp, background, 1)['render']
        image /= 100

        image2 /= 100
        image = image2

        image = image.permute(1, 2, 0).clamp(0, 1).detach().cpu().numpy()
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        image_path = os.path.join(folder_path, Cam.image_name+".jpg")
        cv2.imwrite(image_path, image * 255)


    """
    while True:
        image = render_sh(Cam, gaussians_point, pp, background,s_mod,point_render=True)['render']
        image2 = render_sh(Cam, gaussians, pp, background, 1)['render']
        image /= 100

        image2/=100
        image = image + image2

        image = image.permute(1, 2, 0).clamp(0, 1).detach().cpu().numpy()
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

        cv2.imshow('Rendered Video Stream', image)

        # 计算并显示FPS
        frame_count += 1
        if frame_count % 30 == 0:
            end_time = time.time()
            fps = frame_count / (end_time - start_time)
            print(f"FPS: {fps}")
            frame_count = 0
            start_time = time.time()

        # 响应键盘事件
        
        key = cv2.waitKey(1) & 0xFF
        if key == ord('c'):
            break
        elif key == ord('a'):
            update_camera_position(Cam,1, 0, 0)
        elif key == ord('d'):
            update_camera_position(Cam,-1, 0, 0)
        elif key == ord('w'):
            update_camera_position(Cam,0, 0, -1)
        elif key == ord('s'):
            update_camera_position(Cam,0, 0, 1)
        elif key == ord('q'):
            update_camera_position(Cam,0, 1, 0)
        elif key == ord('e'):
            update_camera_position(Cam,0, -1, 0)
        elif key == ord('i'):
            update_camera_rotation(Cam,0.05, 0,0)
        elif key == ord('k'):
            update_camera_rotation(Cam,-0.05, 0,0)
        elif key == ord('j'):
            update_camera_rotation(Cam,0, -0.05,0)
        elif key == ord('l'):
            update_camera_rotation(Cam,0, 0.05,0)
        elif key == ord('u'):
            update_camera_rotation(Cam,0,0, -0.05)
        elif key == ord('o'):
            update_camera_rotation(Cam,0, 0,0.05)
        elif key == ord('='):
            s_mod += 0.01
        elif key == ord('-'):
            s_mod -= 0.01
        elif key == ord('9'):
            cv2.imwrite("image1.jpg", image*255)
        elif key == ord('8'):
            print(Cam.camera_center)
            print(theta)
        elif key == ord('0'):
            pr = not pr"""

    cv2.destroyAllWindows()
