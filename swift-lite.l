%{
#include "swift-lite.tab.h"
#include "stdio.h"
#include "string.h"
typedef enum{
	Char = 1,
	Int = 2,
	Float = 3
} TYPE;

typedef enum yytokentype {
        NEW = 1,
        ANNOUNCE = 2,
        TYPE = 3,
        INTEGER = 4,
        FLOAT = 5,
		CHAR = 6,
        ID = 7,
        OPERATOR = 8,
		MOV = 9,
		BRACKET = 10,
		EOL = 11
} YYTOKEN;

typedef union {
	int type_int;
	float type_float;
	char type_char;
	char type_id[32];
    char type_operator[2];
	TYPE type_type;
} YYLVAL;
#define YYSTYPE YYLVAL
%}

%%
(let)|(var)									{strcpy(yylval.type_id,yytext);  return NEW;}
[:]											{return ANNOUNCE;}
(Char)										{yylval.type_type=Char;  return TYPE;}
(Int)										{yylval.type_type=Int;  return TYPE;}
(Float)										{yylval.type_type=Float;  return TYPE;}
(')[A-Za-z0-9_](')							{yylval.type_char=yytext[1];  return CHAR;}
[+-]?0.[0-9]*|[+-]?[1-9][0-9]*.[0-9]*       {yylval.type_float=atof(yytext); return FLOAT;}
0|[+-]?[1-9][0-9]*                          {yylval.type_int=atoi(yytext);	return INTEGER;}
[_a-zA-Z][_a-zA-Z0-9]*                      {strcpy(yylval.type_id,yytext);	return ID;}
[><=!][=]|[+][+]|[-][-]|[!+\-*/><]		    {strcpy(yylval.type_operator,yytext); return OPERATOR;}
[=]											{strcpy(yylval.type_operator,yytext); return MOV;}
[()]                                        {strcpy(yylval.type_operator,yytext); return BRACKET;}
[\n]|[;]                                    {return EOL;}
.                                           {/*do nothing*/return yytext[0];}
%%

int yywrap()
{
return 1;
}

