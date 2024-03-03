#ifndef LEXER_H_
#define LEXER_H_

#include <string.h>

typedef enum token_type
{
	IDENT, KEYWORD, OPERATION, PAREN, BRACE, INVALID, WHITESPACE, NUMBER, EQUALS, ASSIGNMENT, COMMA, SEMICOLON
} token_type_e;

typedef enum errors
{
	NO_ERR, INFO, LOG, ALARM, CRITICAL
}errors_t;

#define KEYWORDS_COUNT 8

#define IS_KEYWORD(token) \
	{ \
	for (int i = 0; i < KEYWORDS_COUNT; i++) \
		if (strcmp(token->lexeme, keywords[i]) == 0) \
		{ \
			token->type = KEYWORD;\
			break; \
		} \
	}

#define TYPE_TO_STR(type) \
switch(type) \
	{ \
		case IDENT: \
			return "IDENT"; \
		case KEYWORD: \
			return "KEYWORD"; \
		case OPERATION: \
			return "OPERATION"; \
		case PAREN: \
			return "PAREN"; \
		case BRACE: \
			return "BRACE"; \
		case INVALID: \
			return "INVALID"; \
		case WHITESPACE: \
			return "WHITESPACE"; \
		case NUMBER: \
			return "NUMBER"; \
		case EQUALS: \
			return "EQUALS"; \
		case ASSIGNMENT: \
			return "ASSIGNMENT"; \
		case COMMA: \
			return "COMMA"; \
		case SEMICOLON: \
			return "SEMICOLON"; \
	} \

typedef struct token
{
	char *lexeme;
	int lexeme_start;
	int lexeme_end;
	token_type_e type;
} token_t;

static char *type_to_str(token_type_e type);
static token_t *create_token(char *expr, int start, int end, token_type_e type);
token_t *tokenize(const char *expr, int *count);
static char *token_stringize(token_t token);
char *expr_to_tokens_str(char *expr);

#endif // LEXER_H_
