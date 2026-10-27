
try:
    with open('ViewAbility.cpp', 'rb') as f:
        content = f.read().decode('utf-8', errors='replace')
        if 'addCanSeeEffect' in content:
            start_index = content.find('addCanSeeEffect')
            print(content[start_index:start_index+500])
except Exception as e:
    print(e)
