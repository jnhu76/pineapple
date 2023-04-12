from definition import Variable, Statement, Assignment, Print, SourceCode
from lexer import TokenType, Lexer


class ParserException(Exception):
    pass


def parse_ignored(lexer: Lexer) -> None:
    ...
    
    
    
def parse_variable(lexer: Lexer) -> Variable:
    ...
    
    
def parse_string(lexer: Lexer) -> str:
    ...
    
    
def parse_assignment(lexer: Lexer) -> Assignment:
    ...
    
    
def parse_print(lexer: Lexer) -> Print:
    ...
    
    
def parse_statement(lexer: Lexer) -> Statement:
    ...
    

def parse(lexer: Lexer) -> SourceCode:
    statements = []
    line_num = lexer.line_num
    while lexer.look_ahead() != TokenType.TOKEN_EOF:
        statements.append(parse_statement(lexer))
        
    return SourceCode(line_num, statements)
