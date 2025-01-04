import os


def rename_images_in_directory(directory):
    # 获取目录下所有 jpg 文件
    files = [f for f in os.listdir(directory)]

    # 遍历文件并重命名
    for index, filename in enumerate(files):
        # 构建新的文件名
        new_filename = f"{index}.jpg"  # 使用自然数格式，例如 0, 1, 2, ...

        # 构建完整路径
        old_path = os.path.join(directory, filename)
        new_path = os.path.join(directory, new_filename)

        # 重命名文件
        os.rename(old_path, new_path)
        print(f"Renamed '{old_path}' to '{new_path}'")


# 指定目录
directory = 'D:\\mywork\\3d gaussian\\gaussian-splatting-main\\data\\images\\'

# 调用函数
rename_images_in_directory(directory)