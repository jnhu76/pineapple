import sys
from backend import Interpreter


# https://github.com/KevinXuxuxu/pineapple-py/blob/main/src/backend.py
def main():
    source_file = sys.argv[1]
    with open(source_file) as f:
        source_code = f.read()
    interpreter = Interpreter(source_code)
    interpreter.execute()
    

if __name__ == "__main__":
    main()
