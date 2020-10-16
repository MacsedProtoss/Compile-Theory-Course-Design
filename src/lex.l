%{
#include "parser.tab.h"
#include "stdio.h"
#include "string.h"
#include "swift-lite.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;

typedef union {
	int intValue;
	float floatValue;
	char charValue;
	char idValue[32];
    char cmpCommand[2];
} YYLVAL;
#define YYSTYPE YYLVAL
%}

%%
"var"										{strcpy(yylval.idValue,yytext);  return NEW;}
":"											{return ANNOUNCE;}
"Char"										{strcpy(yylval.idValue,yytext);  return TYPE;}
"Int"										{strcpy(yylval.idValue,yytext);  return TYPE;}
"Float"										{strcpy(yylval.idValue,yytext);  return TYPE;}
"return"     								{return RETURN;}
"if"         								{return IF;}
"else"       								{return ELSE;}
"while"      								{return WHILE;}
"break"      								{return BREAK;}
"continue"      							{return CONTINUE;}

"func"										{return FUNC;}
"->"										{return FUNC_RETURN_TYPE;}

">"|"<"|">="|"<="|"=="|"!="					{strcpy(yylval.cmpCommand,yytext);	return COMPARE;}

"&&"										{return AND;}
"||"										{return OR;}
"!"											{return NOT;}

[+]											{return PLUS;}
[-]											{return MINUS;}
[*]											{return MULTI;}
[/]											{return DIVID;}

"++"										{return INCREASE;}
"--"										{return DECREASE;}

[(]											{return LBR;}
[)]											{return RBR;}
[{]											{return LCBR;}
[}]											{return RCBR;}

[=]											{return ASSIGN;}

[,]											{return COMMA;}

['][A-Za-z0-9_][']							{yylval.charValue=yytext[1];  return CHAR;}
[+-]?0[.][0-9]*|[+-]?[1-9][0-9]*[.][0-9]*       {yylval.floatValue=atof(yytext); return FLOAT;}
0|[+-]?[1-9][0-9]*                          {yylval.intValue=atoi(yytext);	return INTEGER;}
[_a-zA-Z][_a-zA-Z0-9]*                      {strcpy(yylval.idValue,yytext);	return ID;}

[\n;]	                                	{if(yytext[0]=='\n'){yylineno++;yycolumn=1;};return EOL;}
[ \r\t]     								{}

.                                           {printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n", yytext, yylineno);}
%%

int yywrap()
{
return 1;
}

