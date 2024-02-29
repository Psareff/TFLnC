#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum token_type
{
	IDENT, KEYWORD, OPERATION, PAREN, INVALID, WHITESPACE, NUMBER, EQUALS, ASSIGNMENT, COMMA, SEMICOLON
} token_type_e;

typedef enum errors
{
	NO_ERR, INFO, LOG, ALARM, CRITICAL
}errors_t;

#define KEYWORDS_COUNT 8
char* keywords[KEYWORDS_COUNT] = {
	"byte", "short", "int", "long", "float", "double", "boolean", "char"
};

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
	unsigned lexeme_start;
	unsigned lexeme_end;
	token_type_e type;
} token_t;

char *type_to_str(token_type_e type)
{
	TYPE_TO_STR(type);
}

token_t create_token(const char *expr, int start, int end, token_type_e type)
{
	token_t *token = malloc(sizeof(token_t));
	token->lexeme = malloc(end - start + 1);
	token->lexeme_start = start;
	token->lexeme_end = end;
	token->type = type;
	memcpy(token->lexeme, expr + start, end - start + 1);
	IS_KEYWORD(token);
	printf("%10s | [%2d - %2d] | %s\n", token->lexeme, token->lexeme_start, token->lexeme_end, type_to_str(token->type));

}

token_t *tokenize(const char *expr, int *count)
{

	char *buffer  = malloc(strlen(expr) + 1);
	memcpy(buffer, expr, strlen(expr) + 1);

	int lexeme_start, lexeme_end = 0;
	int current_token = 0;
	token_type_e type = INVALID;
	count = 0;

	for(; *expr != '\0'; expr++)
	{
		lexeme_start = lexeme_end;
		switch(*expr)
		{
			case 65 ... 90:
			case 97 ... 122:
				while (*expr >= 65 && *expr <= 90 ||
				       *expr >= 97 && *expr <= 122)
				{
					expr++;
					lexeme_end++;
				}
				expr--;
				lexeme_end--;
				type = IDENT;
				break;
			case ' ':
			case '\t':
			case '\n':
				type = WHITESPACE;
				break;
			case 48 ... 57:
				lexeme_end--;
number_recognition:
				while (*expr >= 48 && *expr <= 57 || *expr == '.')
				{
					expr++;
					lexeme_end++;
				}
				expr--;
				type = NUMBER;
				break;
			case '=':
				type = EQUALS;
				break;
			case '+':
			case '*':
			case '/':
			case '^':
				type = OPERATION;
				break;
			case '-':
				switch (*++expr)
				{
					case '>':
						type = ASSIGNMENT;
						break;
					case 48 ... 57:
						goto number_recognition;
					default:
						type = OPERATION;
				}
				lexeme_end++;
				break;
			case '(':
			case ')':
				type = PAREN;
				break;
			case ',':
				type = COMMA;
				break;
			case ';':
				type = SEMICOLON;
				break;
			default:
				goto emergency_exit;
		}
		token_t t = create_token(buffer, lexeme_start, lexeme_end, type);

		lexeme_end++;
		count++;
	}
	printf("Lexeme end=%d\n", lexeme_end);
	return NULL;
emergency_exit:
	fprintf(stderr, "ERROR=%d, Invalid symbol at [%d]!\n", CRITICAL, lexeme_start);
	return NULL;
}

int main()
{
	char *expr = "operation = (int x, int y) -> x + y + -1235.213;_";
	int count;
	token_t *tokens = tokenize(expr, &count);
	printf("Count: %d\n", count);
	/*for (int i = 0; i < count; i++)
		printf("%s [%d - %d], %d\n", tokens[i].lexeme,
		                             tokens[i].lexeme_start,
		                             tokens[i].lexeme_end,
		                             tokens[i].type);*/
}
