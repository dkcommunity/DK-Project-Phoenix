
import sys

try:
    with open('SkillParentInfo.cpp', 'rb') as f:
        lines = f.readlines()
        for i, line in enumerate(lines):
            try:
                decoded = line.decode('utf-8', errors='replace')
                if 'insert' in decoded:
                    print(f"Line {i+1}: {decoded.strip()}")
            except Exception as e:
                pass
except Exception as e:
    print(e)
