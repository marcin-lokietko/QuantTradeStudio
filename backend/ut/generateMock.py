import re
import sys
from pathlib import Path

def generate_mock(input_file_path, output_file_path):
    src = Path(input_file_path)
    if not src.exists() or not src.is_file():
        print(f"File not found: {input_file_path}")
        return

    with src.open("r") as f:
        content = f.read()

    content = replace_methods_with_mocks(content)
    content = remove_virtual_destructor(content)
    
    
    # remove all includes, then include interface and <gmock/gmock.h>
    # change class name and inherit from interface

    dest = Path(output_file_path)
    with dest.open("w") as f:
        f.write(content)

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

def remove_virtual_destructor(content):
    line_starting_with_virtual_tilde = re.compile(r'^\s*virtual\s~.*$', re.MULTILINE)
    return line_starting_with_virtual_tilde.sub('', content)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python generate_mock.py path_to_interface_file path_to_output_mock_file")
    else:
        generate_mock(sys.argv[1], sys.argv[2])