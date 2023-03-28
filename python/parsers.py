from exceptions import ParserException
from lexer import Lexer, TokenInfo, TokenType
from definition import Variable, Statement, Print, Assignment, SourceCode


def parseIgnred(lexer: Lexer) -> None:
    if lexer.look_ahead() == TokenType.TOKEN_IGNORED:
        lexer.next_token_is(TokenType.TOKEN_IGNORED)


def parseName(lexer: Lexer) -> str:
    token_info = lexer.next_token_is()
    return token_info.token


def parseString(lexer: Lexer) -> str:
    match lexer.look_ahead():
        case TokenType.TOKEN_DUOQUOTE:
            lexer.next_token_is(TokenType.TOKEN_DUOQUOTE)
            return ''
        case TokenType.TOKEN_QUOTE:
            lexer.next_token_is(TokenType.TOKEN_QUOTE)
            string = lexer.scan_before_token('"')  # 利用 " 分割成不同的str
            lexer.next_token_is(TokenType.TOKEN_QUOTE)
            return string


def parseVariable(lexer: Lexer) -> Variable:
    token_info = lexer.next_token_is(TokenType.TOKEN_VAR_PREFIX)
    v = Variable(token_info.line_num, token_info.token)
    parseIgnred(lexer)
    return v


def parsePrint(lexer: Lexer):
    token_info = lexer.next_token_is(TokenType.TOKEN_PRINT)
    lexer.next_token_is(TokenType.TOEKN_LEFT_PAREN)
    parseIgnred(lexer)
    variable = parseVariable(lexer)
    parseIgnred(lexer)
    lexer.next_token_is(TokenType.TOKEN_RIGHT_PAREN)
    parseIgnred(lexer)
    return Print(token_info.line_num, variable)
    
    
    
def parseAssignment(lexer: Lexer):
    var = parseVariable(lexer)
    parseIgnred(lexer)
    lexer.next_token_is(TokenType.TOKEN_EQUAL)
    parseIgnred(lexer)
    string = parseString(lexer)
    parseIgnred(lexer)
    return Assignment(var.line_num, var, string)
    
    
def parseStatement(lexer: Lexer):
    match lexer.look_ahead():
        case TokenType.TOKEN_PRINT:
            return parsePrint(lexer)
        case TokenType.TOKEN_VAR_PREFIX:
            return parseAssignment(lexer)
        case _:
            raise ParserException('parse_statement(): unexpected token {}'.format(lexer.look_ahead()))


def parse(lexer: Lexer) -> SourceCode:
    statements = []
    line_num = lexer.line_num
    while lexer.look_ahead() != TokenType.TOKEN_EOF:
        statements.append(parseStatement(lexer))
    return SourceCode(line_num=line_num, statements=statements)
