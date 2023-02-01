#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

/**
 * Sets start and current to beginning of source and line to 1.
 * @param source the pointer to source text
 */
void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/**
 * @param c the char to test
 * @return true if c is letter or _ false otherwise
 */
static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
}

/**
 * @param c the char to test
 * @return true if c is between '0' and '9' false otherwise.
 */
static bool isDigit(char c) {
    return c <= '9' && c >= '0';
}

/**
 * @return true if at '\0' false otherwise
 */
static bool isAtEnd() {
    return *scanner.current == '\0';
}

/**
 * Returns the value at the current pointer and then increments the current pointer.
 * @return the char at the current pointer.
 */
static char advance() {
    return *scanner.current++;
}

/**
 * @return the char at the current source text pointer
 */
static char peek() {
    return *scanner.current;
}

/**
 * @return the value of the char at next location or '\0' if at end of text
 */
static char peekNext() {
    if(isAtEnd()) return '\0';
    return *(scanner.current + 1);
}

/**
 * Checks if the current location in the source text is the same as a char. If matched then
 * increments the scanners current pointer.
 * @param expected the char to compare to the current source char
 * @return true or false depending on if it matched.
 */
static bool match(char expected) {
    if(isAtEnd()) return false;
    if(*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

/**
 * Creates a token.
 * @param type the type of the token.
 * @return the created token.
 */
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

/**
 * Creates an error token.
 * @param message the message that the error token holds.
 * @return the error token
 */
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

/**
 * Skips all whitespaces increments line information and skips comments.
 */
static void skipWhitespace() {
    for(;;) {
        char c = peek();
        switch(c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if(peekNext() == '/') {
                    while(peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                } break;
            default:
                return;
        }
    }
}

/**
 * Determines if the current part of the source string is of a certain token type.
 * @param start the length of the part of the keyword we know matches
 * @param length the length of the part of the keyword that remains to be matched
 * @param rest the text of the remaining part of keyword that remains to be matched
 * @param type the TokenType of the keyword we are matching for
 * @return the correct TokenType of the token.
 */
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if(scanner.current - scanner.start == start + length &&
       memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    } else {
        return TOKEN_IDENTIFIER;
    }
}

/**
 * Determines if the token that starts at *scanner.start is an identifier or a keyword.
 * @return the TokenType of the token.
 */
static TokenType identifierType() {
    switch (*scanner.start) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if(scanner.current - scanner.start > 1) {
                switch(*scanner.start) {
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Builds a TOKEN_IDENTIFIER or keyword token from the data that starts at the current source pointer.
 * @return the identifier or keyword token that is built.
 */
static Token identifier() {
    while(isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

/**
 * Builds a TOKEN_NUMBER from the data that starts at the current source pointer.
 * @return the number token that is built.
 */
static Token number() {
    while(isDigit(peek())) advance();

    if(peek() == '.' && isDigit(peekNext())) {
        advance();

        while(isDigit(peek())) advance();
    }
    return makeToken(TOKEN_NUMBER);
}

/**
 * Builds a TOKEN_STRING from the data that starts at the current source pointer.
 * @return the string token that is created or an error token if the string is unterminated.
 */
static Token string() {
    while(peek() != '"' && !isAtEnd()) {
        if(peek() == '\n') scanner.line++;
    }
    if(isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TOKEN_STRING);
}

/**
 * Scans the source code for the very next token.
 * @return the next token in the source code.
 */
Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if(isAtEnd()) {
        return makeToken(TOKEN_EOF);
    }

    char c = advance();

    if(isAlpha(c)) return identifier();
    if(isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(
                    match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(
                    match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(
                    match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(
                    match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return string();
    }

    return errorToken("unexpected character.");
}