
try:
    with open('ViewAbility.h', 'rb') as f:
        lines = f.readlines()
        for i, line in enumerate(lines):
            if 19 <= i <= 35:
                print(f"Line {i+1}: {line.decode('utf-8', errors='replace').strip()}")
except Exception as e:
    print(e)
