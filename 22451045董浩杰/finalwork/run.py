import subprocess

# output保存路径
model_path = 'output/train-result'
# 一个示例，换成你自己的输出路径，反正肯定在output文件路径下面，找一下就好

# 脚本执行
command = f'SIBR_gaussianViewer_app.exe -m {model_path}'
run_path = './viewers/bin/'

print (command)

subprocess.run(command, shell=True, cwd=run_path)
