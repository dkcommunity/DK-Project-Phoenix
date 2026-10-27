
import os
import re

def remove_exception_specs(directory):
    # Regex to match throw(...) specifications
    pattern = re.compile(r'\s*throw\s*\([^)]*\)')
    
    encodings = ['utf-8', 'cp949', 'latin1']
    
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.h'):
                filepath = os.path.join(root, file)
                content = None
                used_encoding = None
                
                for enc in encodings:
                    try:
                        with open(filepath, 'r', encoding=enc) as f:
                            content = f.read()
                        used_encoding = enc
                        break
                    except UnicodeDecodeError:
                        continue
                
                if content is None:
                    print(f"Failed to read {filepath} with any encoding")
                    continue
                
                new_content = pattern.sub('', content)
                
                if new_content != content:
                    print(f"Modifying {filepath} ({used_encoding})")
                    try:
                        with open(filepath, 'w', encoding=used_encoding) as f:
                            f.write(new_content)
                    except Exception as e:
                        print(f"Error writing {filepath}: {e}")

if __name__ == '__main__':
    remove_exception_specs('quest')
