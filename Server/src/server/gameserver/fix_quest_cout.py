
import os
import glob

quest_dir = 'quest'
cpp_files = glob.glob(os.path.join(quest_dir, '*.cpp'))

for filepath in cpp_files:
    try:
        with open(filepath, 'r', encoding='latin1') as f:
            lines = f.readlines()
        
        new_lines = []
        modified = False
        for line in lines:
            if '<< endl; :' in line:
                # Keep everything up to " << endl;" and add newline
                parts = line.split('<< endl;')
                new_line = parts[0] + '<< endl;\n'
                new_lines.append(new_line)
                modified = True
                print(f"Fixed line in {filepath}: {line.strip()} -> {new_line.strip()}")
            elif ' : ' in line and '");' in line and 'cout' in line:
                 # Fallback for other patterns if any
                 # cout << "ConditionRaceComp::read() : unknown race" << endl; : unknown race");
                 if '<< endl;' in line:
                    parts = line.split('<< endl;')
                    new_line = parts[0] + '<< endl;\n'
                    new_lines.append(new_line)
                    modified = True
                    print(f"Fixed line in {filepath}: {line.strip()} -> {new_line.strip()}")
                 else:
                    new_lines.append(line)
            else:
                new_lines.append(line)
        
        if modified:
            with open(filepath, 'w', encoding='latin1') as f:
                f.writelines(new_lines)
            print(f"Saved {filepath}")

    except Exception as e:
        print(f"Error processing {filepath}: {e}")
