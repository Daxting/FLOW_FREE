import sys
import matplotlib.pyplot as plt
import os
import matplotlib.colors as mcolors

# 擴充顏色調色盤，包含 'tab20', 'tab20b', 'tab20c'
color_palette = (
    list(mcolors.TABLEAU_COLORS.values()) +
    list(mcolors.CSS4_COLORS.values()) +
    list(plt.cm.tab20.colors) +
    list(plt.cm.tab20b.colors) +
    list(plt.cm.tab20c.colors)
)

# 自動生成從 'A' 到 'z' 的固定顏色對應表，並將 '.' 設為白色
ascii_chars = [chr(i) for i in range(65, 123)]  # 'A' 到 'z'
color_map = {char: color_palette[i % len(color_palette)] for i, char in enumerate(ascii_chars)}
color_map['.'] = '#FFFFFF'  # 設定 '.' 為純白色



# 檢查命令列參數，確保輸入了檔案路徑和輸出資料夾
if len(sys.argv) < 3:
    print("請提供檔案路徑和輸出資料夾名稱作為參數")
    sys.exit(1)

file_path = sys.argv[1]
output_folder = sys.argv[2]

# 讀取提供的檔案
with open(file_path, 'r', encoding='utf-8') as file:
    lines = file.readlines()

# 如果file是以_info.txt結尾，略過不處理
if file_path.endswith('_info.txt'):
    print(f"跳過檔案: {file_path}")
    sys.exit(0)

# 創建一個與輸入文本對應的矩陣
matrix = [line.strip().split() for line in lines]
rows, cols = len(matrix), len(matrix[0])

# 創建一個用於顯示的圖像
fig, ax = plt.subplots(figsize=(cols, rows))

# 繪製每個格子，並使用對應的顏色填充
for r in range(rows):
    for c in range(cols):
        char = matrix[r][c]
        color = color_map.get(char, 'black')  # 若字符不在查表中，預設為黑色
        rect = plt.Rectangle((c, rows - r - 1), 1, 1, facecolor=color)
        ax.add_patch(rect)
        # 在每個格子中間顯示字符
        ax.text(c + 0.5, rows - r - 0.5, char, ha='center', va='center', fontsize=12)

# 設定軸的範圍和隱藏軸
ax.set_xlim(0, cols)
ax.set_ylim(0, rows)
ax.axis('off')

# 設定輸出圖片的路徑，將圖片放入指定的資料夾
output_filename = os.path.join(output_folder, os.path.splitext(os.path.basename(file_path))[0] + '.jpg')

# 儲存圖片
plt.tight_layout()
plt.savefig(output_filename, dpi=300, format='jpg')
print(f"圖片已經儲存為: {output_filename}")
