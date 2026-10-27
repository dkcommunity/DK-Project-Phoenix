
import os

filepath = 'quest/ActionTeachSkill.cpp'

try:
    with open(filepath, 'r', encoding='latin1') as f:
        lines = f.readlines()

    new_lines = []
    for line in lines:
        if 'else :' in line:
            new_lines.append('        else { cerr << "Unknown error" << endl; }\n')
        else:
            new_lines.append(line)

    with open(filepath, 'w', encoding='latin1') as f:
        f.writelines(new_lines)
        
    print(f"Fixed {filepath}")

except Exception as e:
    print(f"Error: {e}")
