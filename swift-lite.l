%{
#include "swift-lite.tab.h"
#include "stdio.h"
#include "string.h"
typedef union {
	int type_int;
	float type_float;
	char type_id[32];
    char type_operator[32];

} YYLVAL;
#define YYSTYPE YYLVAL
%}

%%
(let)|(var)									{strcpy(yylval.type_id,yytext);return NEW;}
[:]											{return ANNOUNCE;}
(Char)										{return TYPE_CHAR;}
(Int)										{return TYPE_INT;}
(Float)										{return TYPE_FLOAT;}
[+-]?0.[0-9]*|[+-]?[1-9][0-9]*.[0-9]*       {yylval.type_float=atof(yytext); return FLOAT;}
0|[+-]?[1-9][0-9]*                          {yylval.type_int=atoi(yytext);	return INTEGER;}
[_a-zA-Z][_a-zA-Z0-9]*                      {strcpy(yylval.type_id,yytext);	return ID;}
[><=!][=]|[+][+]|[-][-]|[!+\-*/><]		    {strcpy(yylval.type_operator,yytext); return CAL_OPERATOR;}
[=]											{strcpy(yylval.type_operator,yytext); return MOV;}
[()]                                        {strcpy(yylval.type_operator,yytext); return BRACKET;}
[\n]|[;]                                    {return EOL;}
.                                           {/*do nothing*/return yytext[0];}
%%

int yywrap()
{
return 1;
}

