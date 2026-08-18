import os
import sys
import re

def fix_glob_and_properties(base_dir):
    if not os.path.exists(base_dir):
        return

    # Walk all files without relying on glob modules that might misinterpret brackets
    for root, dirs, files in os.walk(base_dir):
        for fname in files:
            if not fname.endswith('.cmake'):
                continue
            filepath = os.path.join(root, fname)
            try:
                with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()

                modified = False

                # 1. Fix file(GLOB _cmake_config_files "${CMAKE_CURRENT_LIST_DIR}/...-*.cmake")
                # When paths have brackets like [Project], CMake's file(GLOB) treats [Project] as a regex character class.
                # We replace the GLOB block with explicit if(EXISTS ...) include(...) for all sibling cmake files.
                if 'file(GLOB' in content and 'CMAKE_CURRENT_LIST_DIR' in content:
                    # Find what pattern is being globbed
                    glob_match = re.search(r'file\s*\(\s*GLOB\s+([A-Za-z0-9_]+)\s+"?\$\{CMAKE_CURRENT_LIST_DIR\}/([^"\)\s]+)"?\s*\)', content)
                    if glob_match:
                        var_name = glob_match.group(1)
                        pattern = glob_match.group(2) # e.g. ZLIB-shared-*.cmake or *-targets-*.cmake
                        
                        # Convert simple glob pattern to regex
                        regex_pat = '^' + re.escape(pattern).replace(r'\*', '.*') + '$'
                        matching_files = [f for f in files if re.match(regex_pat, f, re.IGNORECASE) and f != fname]
                        
                        if matching_files:
                            # Build explicit include block
                            includes_block = f"# Replaced file(GLOB) for bracketed path compatibility\n"
                            for mf in sorted(matching_files):
                                includes_block += f'if(EXISTS "${{CMAKE_CURRENT_LIST_DIR}}/{mf}")\n  include("${{CMAKE_CURRENT_LIST_DIR}}/{mf}")\nendif()\n'
                            
                            # Replace the entire file(GLOB ...) and subsequent foreach loop
                            loop_pattern = re.compile(
                                r'file\s*\(\s*GLOB\s+' + re.escape(var_name) + r'\s+"?\$\{CMAKE_CURRENT_LIST_DIR\}/[^"\)\s]+"?[^\)]*\)\s*'
                                r'foreach\s*\(\s*[A-Za-z0-9_]+\s+IN\s+LISTS\s+' + re.escape(var_name) + r'\s*\)\s*'
                                r'include\s*\(\s*"?[^"\)\s]+"?\s*\)\s*'
                                r'endforeach\s*\(\s*\)\s*'
                                r'(?:unset\s*\([^\)]+\)\s*)*',
                                re.MULTILINE
                            )
                            if loop_pattern.search(content):
                                content = loop_pattern.sub(includes_block, content)
                                modified = True
                            else:
                                # Fallback: replace just the file(GLOB) line with set of explicit files
                                file_list_str = ' '.join([f'"${{CMAKE_CURRENT_LIST_DIR}}/{mf}"' for mf in matching_files])
                                content = content.replace(glob_match.group(0), f'set({var_name} {file_list_str})')
                                modified = True

                # 2. Fix IMPORTED_IMPLIB / IMPORTED_LOCATION properties on Debug/Release files
                if '-debug' in fname or '-release' in fname or 'Debug' in fname or 'Release' in fname:
                    cfg = 'DEBUG' if ('debug' in fname or 'Debug' in fname) else 'RELEASE'
                    if f"IMPORTED_LOCATION_{cfg}" in content:
                        if 'IMPORTED_IMPLIB "' in content:
                            content = re.sub(r'\s*IMPORTED_IMPLIB\s+"[^"]+"', '', content)
                            modified = True
                        if f"IMPORTED_IMPLIB_{cfg}" not in content:
                            match = re.search(fr'IMPORTED_LOCATION_{cfg}\s+"([^"]+)\.(dll|exe)"', content)
                            if match:
                                loc_val = match.group(1)
                                implib_str = f'"{loc_val}.lib"'.replace('/bin/', '/lib/')
                                replacement = f"IMPORTED_IMPLIB_{cfg} {implib_str}\n  " + match.group(0)
                                content = content.replace(match.group(0), replacement)
                                modified = True

                    if f"IMPORTED_IMPLIB_{cfg}" in content and f"IMPORTED_LOCATION_{cfg}" not in content:
                        match = re.search(fr'IMPORTED_IMPLIB_{cfg}\s+"([^"]+)\.lib"', content)
                        if match:
                            loc_val = match.group(1)
                            loc_str = f'"{loc_val}.dll"'.replace('/lib/', '/bin/')
                            replacement = match.group(0) + f"\n  IMPORTED_LOCATION_{cfg} {loc_str}"
                            content = content.replace(match.group(0), replacement)
                            modified = True

                if modified:
                    with open(filepath, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"[PATCHED] {filepath}")
            except Exception as e:
                print(f"[ERROR] {filepath}: {e}")

if __name__ == '__main__':
    dirs_to_patch = [
        r"D:\[Project]\Touch Designer\build\vcpkg_installed\x64-windows\share",
        r"C:\dev\vcpkg\installed\x64-windows\share"
    ]
    if len(sys.argv) > 1:
        dirs_to_patch = [sys.argv[1]]
    
    for d in dirs_to_patch:
        print(f"Scanning and patching {d}...")
        fix_glob_and_properties(d)
    print("Patching complete.")
