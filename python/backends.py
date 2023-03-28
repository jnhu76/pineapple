import sys

from definition import Variable, Statement, Assignment, Print, SourceCode
from lexer import Lexer
from parsers import parse
from typing import Dict, Callable
from exceptions import ResolveException


class Interpreter:
    variables: Dict
    ast: Callable
    lexer: Lexer
    
    def __init__(self, source_code: str) -> None:
        self.lexer = Lexer(source_code=source_code)
        self.ast = parse(self.lexer)
        self.variables = {}
        
    def resolveAST(self, ast: SourceCode) -> None:
        if len(ast.statements) == 0:
            raise ResolveException('resolveAST(): no code to execute, please check your input.')
        for statement in ast.statements:
            self.resolveStatement(statement)
        

    def resolveStatement(self, statement: Statement) -> Callable:
        if isinstance(statement, Print):
            self.resolvePrint(statement)
        elif isinstance(statement, Assignment):
            self.resolveAssignment(statement)
        else:
            raise ResolveException('resolve_statement(): unexpected statement type: {}'.format(statement))

    def resolveAssignment(self, assignment: Assignment):
        self.variables[assignment.variable.name] = assignment.string
        
    def resolvePrint(self, print_statement: Print):
        print(self.variables[print_statement.variable.name])
        
    def execute(self):
        self.resolveAST(self.ast)

    
def main():
    # source_file = sys.argv[1]
    with open("1.pineapole") as f:
        source_code = f.read()
    interpreter = Interpreter(source_code)
    interpreter.execute()


if __name__ == "__main__":
    main()
