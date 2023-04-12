from dataclasses import dataclass
from enum import Enum
from typing import Tuple, Optional, List


class LexerException(Exception):
    pass



class TokenType(Enum):
    
    TOKEN_EOF = 0
    TOKEN_VAR_PREFIX = 1
    TOKEN_LEFT_PAREN = 2
    TOKEN_RIGHT_PAREN = 3
    TOKEN_EQUAL = 4
    TOKEN_QUOTE = 5
    TOKEN_DUOQUOTE = 6
    TOKEN_NAME = 7
    TOKEN_PRINT = 8
    TOKEN_IGNORED = 9


KEYWORDS = {
    'print': TokenType.TOKEN_PRINT
}


@dataclass
class TypeInfo:
    
    line_num: int
    token_type: TokenType
    token: str
    
    def __init__(self, line_num: int, token_type: TokenType, token: str) -> None:
        self.line_num = line_num
        self.token_type = token_type
        self.token = token
    
    def __repr__(self) ->  Tuple[int, TokenType, str]:
        return str((self.line_num, self.token_type, self.token))



class Lexer:
    source_code: str
    cur: int
    line_num: int
    next_token_info: Optional[TokenType]
    
    def __init__(self, source_code: str) -> None:
        self.source_code = source_code
        self.cur = 0
        self.line_num = 1
        self.next_token_info = None
    
    
    def finished(self) -> bool:
        return self.cur >= len(self.source_code)
    
    def next_source_code_is(self, prefix: str) -> bool:
        return self.source_code[self.cur:].startswith(prefix)
