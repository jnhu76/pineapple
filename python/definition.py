from abc import ABC
from collections import namedtuple
from dataclasses import dataclass
from typing import List


class Variable:
    
    line_num: int
    name: str    

    def __init__(self, line_num: int, name: str) -> None:
        self.line_num = line_num
        self.name = name
        
    def __repr__(self) -> str:
        return 'Variable({}, {})'.format(self.line_num, self.name)
    
    
class Statement(ABC):
    pass


class Assignment(Statement):
    line_num: int
    variable: Variable
    string: str
    
    
    def __init__(self, line_num: int, variable: Variable, string: str) -> None:
        super().__init__()
        self.line_num = line_num
        self.variable = variable
        self.string = string

        
    def __repr__(self) -> str:
        return 'Assignment({}, {}, {})'.format(self.line_num, self.variable, self.string)
    

class Print(Statement):
    line_num: int
    variable: Variable
    
    def __init__(self, line_num: int, variable: Variable) -> None:
        super().__init__()
        self.line_num = line_num
        self.variable = variable
        
    def __repr__(self) -> str:
        return 'Print({}, {})'.format(self.line_num, self.variable)
    
    
class SourceCode:
    
    line_num: int
    statements: List[Statement]
    
    def __init__(self, line_num: int, statements: List[Statement]) -> None:
        self.line_num = line_num
        self.statements = statements
        
    def __repr__(self) -> str:
        return 'SourceCode({}, {})'.format(self.line_num, self.statements)
  