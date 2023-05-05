import os

def count_lines(filepath):
    with open(filepath, 'rb') as f:
        return sum(1 for _ in f)

def count_lines_in_dir(dir_path):
    total_lines = 0
    for root, _, files in os.walk(dir_path):
        for file in files:
            ext = os.path.splitext(file)[1]
            if ext in ['.h', '.hpp', '.c', '.cpp']:
                filepath = os.path.join(root, file)
                total_lines += count_lines(filepath)
    return total_lines

dir_path = os.getcwd()
total_lines = count_lines_in_dir(dir_path)
print(f'Total lines of code: {total_lines}')
