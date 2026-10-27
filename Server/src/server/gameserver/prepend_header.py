
try:
    header = '#include "ScriptManager.h"\n'
    with open('DynamicZoneScript.cpp', 'rb') as f:
        content = f.read()
    
    # Check if already included to avoid duplication
    if header.encode('utf-8') not in content:
        with open('DynamicZoneScript.cpp', 'wb') as f:
            f.write(header.encode('utf-8'))
            f.write(content)
        print("Header added.")
    else:
        print("Header already present.")
except Exception as e:
    print(e)
