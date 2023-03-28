import re
import string
from enum import Enum
from typing import Tuple

from exceptions import LexerException


class TokenType(Enum):
    
    TOKEN_EOF = 0           # end of file
    TOKEN_VAR_PREFIX = 1    # $
    TOEKN_LEFT_PAREN = 2    # (
    TOKEN_RIGHT_PAREN = 3   # )
    TOKEN_EQUAL = 4         # =
    TOKEN_QUOTE = 5         # "
    TOKEN_DUOQUOTE = 6      # ""
    TOKEN_NAME = 7          # Name ::= [_A-Za-z][_0-9A-Za-z]
    TOKEN_PRINT = 8         # print
    TOKEN_IGNORED = 9       # ignored

KEYWORDS = {
    'print': TokenType.TOKEN_PRINT
}

class TokenInfo:
    line_num: int
    token_type: TokenType
    token: str
    
    def __init__(self, line_num: int, token_type: TokenType, token: str) -> None:
        self.line_num = line_num
        self.token_type = token_type
        self.token = token
        
    def __repr__(self) -> Tuple[int, TokenType, str]:
        return str((self.line_num, self.token_type, self.token))



class Lexer:
    source_code: str
    cur: int
    line_num: int
    token_info: TokenInfo
    next_token_info: TokenInfo
    
    def __init__(self, source_code: str, toke_info: TokenInfo = None, cur: int = 0, line_num: int = 1) -> None:
        self.source_code = source_code
        self.cur = cur
        self.line_num = line_num
        self.token_info = toke_info
        self.next_token_info = None
        
    def finished(self) -> bool:
        return self.cur >= len(self.source_code)

    def next_source_code_is(self, prefix: str) -> bool:
        return self.source_code[self.cur:].startswith(prefix)

    def scan_pattern(self, pattern: str) -> str:
        result = re.findall(pattern, self.source_code[self.cur:])
        if len(result) != 1:
            raise LexerException('scan_pattern(): returned unexpected result: {} for pattern {}'.format(result, pattern))
        return result[0]
    
    def scan_name(self) -> str:
        return self.scan_pattern(r'^[_a-zA-Z][_a-zA-Z0-9]*')
    
    def scan_ignored(self) -> str:  # whitespace | lineterminator.
        return self.scan_pattern(r'^[\t\n\v\f\r ]+')
    
    def scan_before_token(self, token: str) -> str:
        result = self.source_code[self.cur:].split(token)
        if len(result) <= 1:
            raise LexerException("scan_before_token(): missing token {}".format(token))
        self.cur += len(result[0])
        self.process_new_line(result[0])
        return result[0]

    def process_new_line(self, ignored: str) -> None:
        i = 0
        while i < len(ignored):
            if ignored[i:][:2] in ['\r\n', '\n\r']:
                i += 2
                self.line_num += 1
            else:
                if ignored[i] in ['\r', '\n']:
                    self.line_num += 1
                i += 1

    def getNextToken(self) -> TokenInfo:
        # next token info is loaded.
        if self.next_token_info is not None:
            next_token_info = self.next_token_info
            self.next_token_info = None
            return next_token_info
    
        # load next token.
        if self.finished():
            return TokenInfo(self.line_num, TokenType.TOKEN_EOF, 'EOF')

        next_chr = self.source_code[self.cur]
        
        if next_chr == '$':
            self.cur += 1
            return TokenInfo(self.line_num, TokenType.TOKEN_VAR_PREFIX, '$')
        if next_chr == '(':
            self.cur += 1
            return TokenInfo(self.line_num, TokenType.TOEKN_LEFT_PAREN, '(')
        if next_chr == ')':
            self.cur += 1
            return TokenInfo(self.line_num, TokenType.TOKEN_RIGHT_PAREN, ')')
        if next_chr == '=':
            self.cur += 1
            return TokenInfo(self.line_num, TokenType.TOKEN_EQUAL, '=')
        if next_chr == '"':
            if self.next_source_code_is('"'):
                self.cur += 2
                return TokenInfo(self.line_num, TokenType.TOKEN_DUOQUOTE, '""')
            self.cur += 1
            return TokenInfo(self.line_num, TokenType.TOKEN_QUOTE, '"')
        if next_chr == '_' or next_chr.isalpha():
            name = self.scan_name()
            if name in KEYWORDS:
                self.cur += len(name)
                return TokenInfo(self.line_num, KEYWORDS[name], name)
            self.cur += len(name)
            return TokenInfo(self.line_num, TokenType.TOKEN_NAME, name)
        if next_chr in ['\t', '\n', '\v', '\f', '\r', ' ']:
            ignored = self.scan_ignored()
            line_num = self.line_num
            self.cur += len(ignored)
            self.process_new_line(ignored)
            return TokenInfo(line_num, TokenType.TOKEN_IGNORED, ignored)
        # todo: update if else to match case.
        # match next_chr:
        #     case '$':
        #         ...
        #     case '(':
        #         ...
        #     case ')':
        #         ...
        #     case '=':
        #         ...
        #     case '"':
        #         ...
        #     case '\t' | '\n' | '\v' | '\f' | '\r' | ' ':
        #         ...
        #     case _:
        #         if next_chr == '_' or next_chr.isalpha():
        #             ...
        #         raise LexerException('get_next_token(): unexpected symbol {}'.format(next_chr))
        raise LexerException('get_next_token(): unexpected symbol {}'.format(next_chr))

    def next_token_is(self, t: TokenType) -> TokenInfo:
        next_token_info = self.getNextToken()
        if next_token_info.token_type != t:
            raise LexerException('next_token_is(): syntax error near {}, expecting {} but got {}'.format(
                    next_token_info.token, t, next_token_info))
        return next_token_info
        
    def look_ahead(self) -> TokenType:
        if self.next_token_info is None:
            self.next_token_info = self.getNextToken()
        return self.next_token_info.token_type
