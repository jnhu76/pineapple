import string

# print(string.digits)

digist_list = list()
digist_list.extend(string.digits)

letters_list = list()
letters_list.extend(string.ascii_letters)
print(digist_list)
print(digist_list[0])
print(type(digist_list[0]))
letters_list.append('_')

def is_alpha(i):
    if i in letters_list:
        return str()

def is_digit(i):
    if i in digist_list:
        return int()

def test(t):
    match t:
        case '$':
            print("t is $\n")
        case *[letters_list]:
            print("t is alpha\n")
        case *[digist_list]:
            print("t is digit.\n")
        case _:
            print("error!\n")

test('1')
test('a')
test('_')
test('$')
test('!')
