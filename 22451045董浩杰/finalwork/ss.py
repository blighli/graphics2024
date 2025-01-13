import torch

# 示例数据
opacity = torch.tensor([0.3, 0.7, 0.1, 0.5, 0.9, 0.2])
valid_mask = torch.tensor([False, True, False, True, True, False], dtype=torch.bool)

# 筛选有效的索引
valid_indices = torch.nonzero(valid_mask).squeeze(1)
print("Valid Indices:", valid_indices)

# 筛选出有效的不透明度值
filtered_opacity = opacity[valid_indices]
print("Filtered Opacity after filtering:", filtered_opacity.size())

# 对筛选后的不透明度进行排序
sorted_opacity, sorted_idx = torch.sort(filtered_opacity)
print("Sorted Opacity:", sorted_opacity)
print("Sorted Indices:", sorted_idx)

# 检查排序结果是否正确
print("Filtered Opacity before sorting:", filtered_opacity)
print("Sorted Opacity:", sorted_opacity)