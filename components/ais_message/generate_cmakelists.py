#!/usr/bin/env python3
"""
generate_cmakelists.py

Script tự động tạo CMakeLists.txt cho ESP-IDF component.
Quét toàn bộ src/ để lấy file .cpp và include/ để liệt kê các thư mục con.
"""

import os

# Thư mục chứa source và include gốc
SRC_DIR = "src"
INCLUDE_ROOT = "include"
# Tên file output
OUTPUT_FILE = "CMakeLists.txt"

def collect_sources(src_dir):
    """Duyệt lấy tất cả file .cpp trong src_dir."""
    cpp_files = []
    for root, _, files in os.walk(src_dir):
        for f in files:
            if f.lower().endswith(".cpp"):
                path = os.path.join(root, f).replace("\\", "/")
                cpp_files.append(path)
    return sorted(cpp_files)

def collect_include_dirs(include_root):
    """
    Duyệt đệ quy include_root và trả về danh sách tất cả thư mục con (bao gồm chính nó),
    chuẩn hóa đường dẫn về dạng forward-slash.
    """
    dirs = set()
    for root, subdirs, files in os.walk(include_root):
        # Nếu thư mục hiện tại chứa file header hoặc có thư mục con, ta thêm vào
        if files or subdirs:
            path = os.path.normpath(root).replace("\\", "/")
            dirs.add(path)
    return sorted(dirs)

def write_cmakelists(sources, include_dirs, output_path):
    """Ghi file CMakeLists.txt theo định dạng idf_component_register."""
    with open(output_path, "w", encoding="utf-8") as fp:
        fp.write("idf_component_register(\n")
        fp.write("    SRCS\n")
        for src in sources:
            fp.write(f'        "{src}"\n')
        fp.write("    INCLUDE_DIRS\n")
        for inc in include_dirs:
            fp.write(f'        "{inc}"\n')
        fp.write(")\n")

def main():
    sources = collect_sources(SRC_DIR)
    if not sources:
        print(f"[Error] Không tìm thấy file .cpp nào trong '{SRC_DIR}/'")
        return

    include_dirs = collect_include_dirs(INCLUDE_ROOT)
    if not include_dirs:
        print(f"[Warning] Không tìm thấy thư mục con nào trong '{INCLUDE_ROOT}/'")

    write_cmakelists(sources, include_dirs, OUTPUT_FILE)
    print(f"[OK] Đã tạo '{OUTPUT_FILE}' với {len(sources)} source và {len(include_dirs)} include dirs.")

if __name__ == "__main__":
    main()
