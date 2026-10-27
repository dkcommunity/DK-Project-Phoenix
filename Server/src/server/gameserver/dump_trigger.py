
import os

def read_lines(filepath, start, end, encoding='cp949'):
    try:
        with open(filepath, 'r', encoding=encoding) as f:
            lines = f.readlines()
            for i in range(max(0, start-1), min(len(lines), end)):
                print(f"{i+1}: {lines[i].rstrip()}")
    except Exception as e:
        print(f"Error reading {filepath}: {e}")

print("--- Trigger.h ---")
read_lines('quest/Trigger.h', 80, 100)
print("\n--- Trigger.cpp ---")
read_lines('quest/Trigger.cpp', 330, 350)
