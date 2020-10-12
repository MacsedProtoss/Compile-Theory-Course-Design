/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     ID = 259,
     FLOAT = 260,
     CHAR = 261,
     TYPE = 262,
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
     LOWER_THEN_ELSE = 304
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define ID 259
#define FLOAT 260
#define CHAR 261
#define TYPE 262
#define COMPARE 263
#define NEW 264
#define ANNOUNCE 265
#define ASSIGN 266
#define LBR 267
#define RBR 268
#define LCBR 269
#define RCBR 270
#define RETURN 271
#define IF 272
#define ELSE 273
#define WHILE 274
#define BREAK 275
#define CONTINUE 276
#define FUNC 277
#define FUNC_RETURN_TYPE 278
#define COMMA 279
#define EOL 280
#define AND 281
#define OR 282
#define NOT 283
#define PLUS 284
#define MINUS 285
#define MULTI 286
#define DIVID 287
#define INCREASE 288
#define DECREASE 289
#define FUNC_CALL 290
#define FUNC_ANNOUNCE 291
#define IF_THEN 292
#define IF_THEN_ELSE 293
#define Exp_STATMENT 294
#define WHOLE_STATEMENT 295
#define STATEMENT_LIST 296
#define ARGS 297
#define FUNC_PARAMETERS 298
#define FUNC_PARAMETER 299
#define VAR_LIST 300
#define VAR_DEFINE 301
#define DEFINE_ASSIGN 302
#define DEFINE_LIST 303
#define LOWER_THEN_ELSE 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 17 "parser.y"
{
	int intValue;
	float floatValue;
	char charValue;
	char idValue[32];
        char cmpCommand[2];
	TYPE type;
        class ASTNode *ptr;
}
/* Line 1529 of yacc.c.  */
#line 157 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
