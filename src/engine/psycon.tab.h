/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_ENGINE_PSYCON_TAB_H_INCLUDED
# define YY_YY_SRC_ENGINE_PSYCON_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    T_EOF = 0,                     /* "end of text"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_UNKNOWN = 258,               /* T_UNKNOWN  */
    T_NEWLINE = 259,               /* "end of line"  */
    T_IF = 260,                    /* "if"  */
    T_ELSE = 261,                  /* "else"  */
    T_ELSEIF = 262,                /* "elseif"  */
    T_END = 263,                   /* "end"  */
    T_WHILE = 264,                 /* "while"  */
    T_FOR = 265,                   /* "for"  */
    T_BREAK = 266,                 /* "break"  */
    T_CONTINUE = 267,              /* "continue"  */
    T_SWITCH = 268,                /* "switch"  */
    T_CASE = 269,                  /* "case"  */
    T_OTHERWISE = 270,             /* "otherwise"  */
    T_FUNCTION = 271,              /* "function"  */
    T_STATIC = 272,                /* "static"  */
    T_RETURN = 273,                /* "return"  */
    T_SIGMA = 274,                 /* "sigma"  */
    T_TRY = 275,                   /* "try"  */
    T_CATCH = 276,                 /* "catch"  */
    T_CATCHBACK = 277,             /* "catchback"  */
    T_OP_SHIFT = 278,              /* ">>"  */
    T_OP_CONCAT = 279,             /* "++"  */
    T_LOGIC_EQ = 280,              /* "=="  */
    T_LOGIC_NE = 281,              /* "!="  */
    T_LOGIC_LE = 282,              /* "<="  */
    T_LOGIC_GE = 283,              /* ">="  */
    T_LOGIC_AND = 284,             /* "&&"  */
    T_LOGIC_OR = 285,              /* "||"  */
    T_REPLICA = 286,               /* ".."  */
    T_MATRIXMULT = 287,            /* "**"  */
    T_NUMBER = 288,                /* "number"  */
    T_STRING = 289,                /* "string"  */
    T_ID = 290,                    /* "identifier"  */
    T_ENDPOINT = 291,              /* T_ENDPOINT  */
    T_FULLRANGE = 292,             /* T_FULLRANGE  */
    T_LOGIC_NOT = 294,             /* T_LOGIC_NOT  */
    T_POSITIVE = 295,              /* T_POSITIVE  */
    T_NEGATIVE = 296,              /* T_NEGATIVE  */
    T_TRANSPOSE = 297              /* T_TRANSPOSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 35 "src/engine/psycon.y"

	double dval;
	char *str;
	AstNode *pnode;

#line 111 "src/engine/psycon.tab.h"

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

int yyparse (AstNode **pproot, char **errmsg);


#endif /* !YY_YY_SRC_ENGINE_PSYCON_TAB_H_INCLUDED  */
