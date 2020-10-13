/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTEGER = 258,
    ID = 259,
    TYPE = 260,
    FLOAT = 261,
    CHAR = 262,
    COMPARE = 263,
    NEW = 264,
    ANNOUNCE = 265,
    ASSIGN = 266,
    LBR = 267,
    RBR = 268,
    LCBR = 269,
    RCBR = 270,
    RETURN = 271,
    IF = 272,
    ELSE = 273,
    WHILE = 274,
    BREAK = 275,
    CONTINUE = 276,
    FUNC = 277,
    FUNC_RETURN_TYPE = 278,
    COMMA = 279,
    EOL = 280,
    AND = 281,
    OR = 282,
    NOT = 283,
    PLUS = 284,
    MINUS = 285,
    MULTI = 286,
    DIVID = 287,
    INCREASE = 288,
    DECREASE = 289,
    FUNC_CALL = 290,
    FUNC_ANNOUNCE = 291,
    IF_THEN = 292,
    IF_THEN_ELSE = 293,
    Exp_STATMENT = 294,
    WHOLE_STATEMENT = 295,
    STATEMENT_LIST = 296,
    ARGS = 297,
    FUNC_PARAMETERS = 298,
    FUNC_PARAMETER = 299,
    VAR_LIST = 300,
    VAR_DEFINE = 301,
    DEFINE_ASSIGN = 302,
    DEFINE_LIST = 303,
    EXT_DEF_LIST = 304,
    EXT_DEF = 305,
    LOWER_THEN_ELSE = 306
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "./src/parser.y"

	int intValue;
	float floatValue;
	char charValue;
	char idValue[32];
        char cmpCommand[2];
        class ASTNode *ptr;

#line 118 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
