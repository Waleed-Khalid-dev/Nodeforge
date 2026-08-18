import os
import re

base_dir = r"D:\[Project]\Touch Designer\build\vcpkg_installed\x64-windows\share"

count = 0
for root, dirs, files in os.walk(base_dir):
    for f in files:
        if f.endswith('.cmake'):
            path = os.path.join(root, f)
            try:
                with open(path, 'r', encoding='utf-8') as file:
                    content = file.read()
            except:
                continue
            
            original_content = content
            
            if 'IMPORTED_IMPLIB_RELEASE' in content:
                content = re.sub(
                    r'IMPORTED_IMPLIB_RELEASE\s+("[^"]+")', 
                    r'IMPORTED_IMPLIB \1\n  IMPORTED_IMPLIB_RELEASE \1', 
                    content
                )
                content = re.sub(
                    r'IMPORTED_IMPLIB_RELEASE\s+(\[\[[^\]]+\]\])', 
                    r'IMPORTED_IMPLIB \1\n  IMPORTED_IMPLIB_RELEASE \1', 
                    content
                )
            
            if 'IMPORTED_LOCATION_RELEASE' in content:
                content = re.sub(
                    r'IMPORTED_LOCATION_RELEASE\s+("[^"]+")', 
                    r'IMPORTED_LOCATION \1\n  IMPORTED_LOCATION_RELEASE \1', 
                    content
                )
                content = re.sub(
                    r'IMPORTED_LOCATION_RELEASE\s+(\[\[[^\]]+\]\])', 
                    r'IMPORTED_LOCATION \1\n  IMPORTED_LOCATION_RELEASE \1', 
                    content
                )
                
            if content != original_content:
                with open(path, 'w', encoding='utf-8') as file:
                    file.write(content)
                print(f"Patched {f}")
                count += 1

print(f"Total files patched: {count}")
