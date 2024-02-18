%{
#include <stdio.h>
#include "ce.tab.h"
%}

primitives ("byte"|"short"|"int"|"long"|"float"|"double"|"boolean"|"char")
non_primitives ("String")


%%
{primitives}     printf("ident=%s ", yytext);
{non_primitives} printf("NON_PRIMITIVE ");
[0-9]+           printf("NUMBER ");
[a-zA-Z]*        printf("WORD ");
\{               printf("OPEN_BRACE ");
\}               printf("CLOSE_BRACE ");
;                printf("SEMICOLON ");
\=               printf("EQUALS ");
\(               printf("OPEN_PAREN ");
\)               printf("CLOSE_PAREN ");
\*               printf("MULTIPLY ");
>               printf("ARROW ");
.        ; /* игнорируем все остальное */
%%

