import re
import sys
import subprocess
from pathlib import Path

def generate_mock(input_file_path, output_file_path):
    src = Path(input_file_path)
    if not src.exists() or not src.is_file():
        print(f"File not found: {input_file_path}")
        return

    with src.open("r") as f:
        content = f.read()

    content = remove_non_interface_types(content)
    content = replace_methods_with_mocks(content)
    content = remove_virtual_destructor(content)
    content = remove_includes(content)
    content = add_interface_and_gmock_includes(content, input_file_path)

    dest = Path(output_file_path)

    interface_name = src.stem
    mock_name = dest.stem
    content = replace_class_name_with_mock(content, interface_name, mock_name)

    with dest.open("w") as f:
        f.write(content)

    format_cpp_file(output_file_path)

    print(f"Mock file generated: {dest}")

def replace_methods_with_mocks(content):
    method_pattern = re.compile(
        r"""virtual\s+                 # virtual keyword
            ([^\s(]+(?:\s[^\s(]+)*)\s+ # return type (can contain spaces like "const std::string&")
            ([^\s(]+)\s*               # method name
            \(([^)]*)\)\s*             # arguments
            (const\s*)?=\s*0\s*;       # const and "=0"
        """,
        re.VERBOSE
    )

    def replacement(match):
        return_type = match.group(1).strip()
        method_name = match.group(2).strip()
        args = match.group(3).strip()
        constness = match.group(4) or ""
        constness = constness.strip()
        keywords = "override"
        if constness:
            keywords = f"{constness}, override"
        return f"MOCK_METHOD({return_type}, {method_name}, ({args}), ({keywords}));"

    return method_pattern.sub(replacement, content)

def remove_non_interface_types(content):
    output_lines = []
    lines = content.splitlines()

    is_skipping_until_structure_end = False
    for line in lines:
        if is_skipping_until_structure_end:
            if line.startswith("};"):
                is_skipping_until_structure_end = False
            continue
        if line.strip().startswith('DEFINE_STRONG_TYPE'):
            continue
        if line.strip().startswith('struct') or line.strip().startswith('enum'):
            is_skipping_until_structure_end = True
            continue

        output_lines.append(line)

    return "\n".join(output_lines)

def remove_virtual_destructor(content):
    lines = content.splitlines()
    filtered_lines = [line for line in lines if not line.strip().startswith("virtual ~")]
    return "\n".join(filtered_lines)

def remove_includes(content):
    lines = content.splitlines()
    filtered_lines = [line for line in lines if not line.strip().startswith("#include")]
    return "\n".join(filtered_lines)

def add_interface_and_gmock_includes(content, input_file_path):
    lines = content.splitlines()

    input_include_path = ""
    index = input_file_path.find("backend/src/")
    if index != -1:
         input_include_path = input_file_path[index + len("backend/src/"):]
    lines.insert(2, "#include \"" + input_include_path + "\"")
    lines.insert(2, "#include <gmock/gmock.h>")
    return "\n".join(lines)

def replace_class_name_with_mock(content, interface_name, mock_name):
    lines = content.splitlines()
    for i, line in enumerate(lines):
        if line.strip().startswith("class"):
            lines[i] = "class " + mock_name + ": public " + interface_name + "{"
            break
    return "\n".join(lines)

def format_cpp_file(file_path):
    try:
        subprocess.run(['clang-format', '--style=file', '-i', file_path], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error formatting file: {e}")

if __name__ == "__main__":
    print(sys.argv)
    if len(sys.argv) < 3:
        print("Usage: python3 generate_mock.py path_to_interface_file path_to_output_mock_file")
    else:
        generate_mock(sys.argv[1], sys.argv[2])