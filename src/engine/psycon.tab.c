
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 15 "psycon.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psycon.yacc.h"
#define YYPRINT(file, type, value) print_token_value (file, type, value)
/*#define DEBUG*/

char *ErrorMsg = NULL;
int yylex (void);
void yyerror (AstNode **pproot, char **errmsg, char const *s);


/* Line 189 of yacc.c  */
#line 87 "psycon.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 1
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_EOF = 0,
     T_UNKNOWN = 258,
     T_NEWLINE = 259,
     T_IF = 260,
     T_ELSE = 261,
     T_ELSEIF = 262,
     T_END = 263,
     T_WHILE = 264,
     T_FOR = 265,
     T_BREAK = 266,
     T_CONTINUE = 267,
     T_SWITCH = 268,
     T_CASE = 269,
     T_OTHERWISE = 270,
     T_FUNCTION = 271,
     T_STATIC = 272,
     T_RETURN = 273,
     T_SIGMA = 274,
     T_TRY = 275,
     T_CATCH = 276,
     T_CATCHBACK = 277,
     T_OP_SHIFT = 278,
     T_OP_CONCAT = 279,
     T_LOGIC_EQ = 280,
     T_LOGIC_NE = 281,
     T_LOGIC_LE = 282,
     T_LOGIC_GE = 283,
     T_LOGIC_AND = 284,
     T_LOGIC_OR = 285,
     T_REPLICA = 286,
     T_MATRIXMULT = 287,
     T_NUMBER = 288,
     T_STRING = 289,
     T_ID = 290,
     T_ENDPOINT = 291,
     T_FULLRANGE = 292,
     T_NEGATIVE = 294,
     T_POSITIVE = 295,
     T_LOGIC_NOT = 296,
     T_TRANSPOSE = 297
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 34 "psycon.y"

	double dval;
	char *str;
	AstNode *pnode;



/* Line 214 of yacc.c  */
#line 173 "psycon.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

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


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 118 "psycon.y"

AstNode *newAstNode(int type, YYLTYPE loc);
AstNode *makeFunctionCall(const char *name, AstNode *first, AstNode *second, YYLTYPE loc);
AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc);
void print_token_value(FILE *file, int type, YYSTYPE value);
char *getT_ID_str(AstNode *p);
void handle_tilde(AstNode *proot, AstNode *pp, YYLTYPE loc);


/* Line 264 of yacc.c  */
#line 208 "psycon.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  92
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2614

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  163
/* YYNRULES -- Number of states.  */
#define YYNSTATES  305

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    64,     2,    49,    81,    47,     2,    39,
      60,    61,    50,    45,    58,    44,    78,    51,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    59,
      40,    38,    41,     2,    48,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    79,     2,    80,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,    57,    63,    43,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    46,    53,    54,    55,    56,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    82
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    16,    18,
      21,    25,    29,    33,    36,    39,    44,    46,    48,    50,
      53,    55,    57,    60,    62,    64,    67,    69,    71,    73,
      75,    77,    79,    80,    82,    84,    87,    92,    99,   107,
     117,   118,   120,   126,   134,   136,   138,   140,   146,   151,
     158,   165,   172,   177,   184,   192,   194,   196,   198,   200,
     203,   205,   208,   209,   214,   218,   220,   222,   226,   230,
     234,   238,   242,   246,   250,   254,   257,   261,   265,   266,
     268,   272,   274,   276,   278,   280,   284,   285,   287,   291,
     294,   296,   299,   303,   307,   313,   315,   317,   319,   321,
     323,   325,   327,   329,   331,   333,   335,   337,   339,   341,
     343,   346,   349,   352,   355,   357,   361,   366,   370,   373,
     375,   380,   385,   393,   398,   402,   404,   409,   414,   422,
     425,   431,   438,   445,   452,   459,   463,   467,   470,   473,
     477,   481,   485,   487,   489,   491,   493,   495,   498,   501,
     510,   514,   518,   522,   526,   530,   534,   538,   542,   546,
     550,   554,   558,   562
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      84,     0,    -1,    -1,    85,    -1,    92,    -1,    85,    92,
      -1,    87,    -1,    86,    87,    -1,     4,    -1,     1,     4,
      -1,    49,    89,    93,    -1,    51,    90,    93,    -1,    41,
      91,    93,    -1,    99,    93,    -1,    99,    94,    -1,   116,
      57,    35,    33,    -1,    35,    -1,   116,    -1,    33,    -1,
      44,    33,    -1,    34,    -1,    88,    -1,    89,    88,    -1,
      88,    -1,    88,    -1,    91,    88,    -1,    87,    -1,    97,
      -1,    58,    -1,     4,    -1,     0,    -1,    59,    -1,    -1,
       0,    -1,    16,    -1,    17,    16,    -1,    96,    35,    86,
      95,    -1,    96,   115,    38,    35,    86,    95,    -1,    96,
      35,    60,   106,    61,    86,    95,    -1,    96,   115,    38,
      35,    60,   106,    61,    86,    95,    -1,    -1,     4,    -1,
      98,    14,   118,     4,    86,    -1,    98,    14,    62,   108,
      63,     4,    86,    -1,   102,    -1,   117,    -1,   104,    -1,
       5,   100,    86,   101,     8,    -1,    13,   118,    98,     8,
      -1,    13,   118,    98,    15,    86,     8,    -1,    20,    86,
      21,    35,    86,     8,    -1,    20,    86,    22,    35,    86,
       8,    -1,     9,   100,    86,     8,    -1,    10,    35,    38,
     112,    86,     8,    -1,    10,    35,    38,   112,    58,    86,
       8,    -1,    18,    -1,    11,    -1,    12,    -1,   105,    -1,
     105,    93,    -1,   118,    -1,   118,    93,    -1,    -1,     7,
     100,    86,   101,    -1,   101,     6,    86,    -1,   103,    -1,
     112,    -1,    62,   108,    63,    -1,    60,   105,    61,    -1,
     118,    40,   118,    -1,   118,    41,   118,    -1,   118,    25,
     118,    -1,   118,    26,   118,    -1,   118,    28,   118,    -1,
     118,    27,   118,    -1,    64,   102,    -1,   102,    29,   102,
      -1,   102,    30,   102,    -1,    -1,    35,    -1,   106,    58,
      35,    -1,    42,    -1,   112,    -1,   104,    -1,   107,    -1,
     108,    58,   107,    -1,    -1,   110,    -1,   109,    59,   110,
      -1,   109,    59,    -1,   112,    -1,   110,   112,    -1,   110,
      58,   112,    -1,   118,    42,   118,    -1,   118,    42,   118,
      42,   118,    -1,   118,    -1,   111,    -1,   105,    -1,    65,
      -1,    66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    38,   112,    -1,    77,   105,    -1,    77,
     118,    -1,   113,   112,    -1,    35,    -1,   116,    78,    35,
      -1,   115,    62,   118,    63,    -1,    79,   110,    80,    -1,
      81,   115,    -1,   115,    -1,    35,    60,   108,    61,    -1,
      35,    62,   118,    63,    -1,    35,    62,   118,    63,    60,
     108,    61,    -1,   115,    60,   108,    61,    -1,   115,    60,
      61,    -1,    31,    -1,    31,    60,   108,    61,    -1,    31,
      62,   118,    63,    -1,    31,    62,   118,    63,    60,   108,
      61,    -1,   116,    39,    -1,    79,   110,    80,    79,    80,
      -1,    79,   110,    80,    79,   110,    80,    -1,    79,   110,
      80,    79,   109,    80,    -1,    79,   109,    80,    79,   110,
      80,    -1,    79,   109,    80,    79,   109,    80,    -1,    79,
     109,    80,    -1,    60,   112,    61,    -1,   116,   114,    -1,
     115,   114,    -1,   115,    38,   117,    -1,   116,    38,   117,
      -1,   115,    38,   104,    -1,   104,    -1,   116,    -1,    33,
      -1,    34,    -1,    36,    -1,    44,   118,    -1,    45,   118,
      -1,    19,    60,   116,    38,   112,    58,   118,    61,    -1,
     118,    45,   118,    -1,   118,    44,   118,    -1,   118,    50,
     118,    -1,   118,    51,   118,    -1,   118,    32,   118,    -1,
     118,    52,   118,    -1,   118,    47,   118,    -1,   118,    43,
     118,    -1,   118,    49,   118,    -1,   118,    82,   118,    -1,
     118,    46,   118,    -1,   118,    48,   118,    -1,   118,    23,
     118,    -1,   118,    24,   118,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   144,   144,   145,   149,   150,   171,   178,   200,   202,
     207,   211,   215,   219,   220,   225,   234,   239,   241,   246,
     254,   263,   268,   278,   285,   290,   301,   302,   305,   305,
     305,   308,   311,   312,   315,   320,   327,   334,   352,   359,
     380,   381,   383,   398,   415,   416,   417,   418,   444,   452,
     463,   472,   481,   493,   507,   521,   525,   527,   532,   532,
     532,   532,   536,   539,   552,   573,   576,   579,   588,   594,
     596,   598,   600,   602,   604,   606,   611,   613,   618,   621,
     627,   635,   637,   638,   641,   646,   657,   667,   674,   680,
     683,   695,   701,   709,   713,   720,   720,   720,   723,   727,
     729,   731,   733,   735,   740,   742,   744,   751,   758,   765,
     774,   778,   787,   796,   816,   821,   840,   846,   850,   859,
     860,   866,   873,   882,   887,   899,   903,   908,   914,   922,
     927,   932,   938,   944,   952,   960,   964,   973,   978,   983,
     998,  1013,  1022,  1023,  1024,  1029,  1034,  1038,  1043,  1049,
    1057,  1059,  1061,  1063,  1065,  1067,  1069,  1071,  1073,  1075,
    1077,  1079,  1081,  1083
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of text\"", "error", "$undefined", "T_UNKNOWN",
  "\"end of line\"", "\"if\"", "\"else\"", "\"elseif\"", "\"end\"",
  "\"while\"", "\"for\"", "\"break\"", "\"continue\"", "\"switch\"",
  "\"case\"", "\"otherwise\"", "\"function\"", "\"static\"", "\"return\"",
  "\"sigma\"", "\"try\"", "\"catch\"", "\"catchback\"", "\">>\"", "\"++\"",
  "\"==\"", "\"!=\"", "\"<=\"", "\">=\"", "\"&&\"", "\"||\"", "\"..\"",
  "\"**\"", "\"number\"", "\"string\"", "\"identifier\"", "T_ENDPOINT",
  "T_FULLRANGE", "'='", "'\\''", "'<'", "'>'", "':'", "'~'", "'-'", "'+'",
  "\"->\"", "'%'", "'@'", "'#'", "'*'", "'/'", "'^'", "T_NEGATIVE",
  "T_POSITIVE", "T_LOGIC_NOT", "T_TRANSPOSE", "'|'", "','", "';'", "'('",
  "')'", "'{'", "'}'", "'!'", "\"+=\"", "\"-=\"", "\"*=\"", "\"/=\"",
  "\"@=\"", "\"@@=\"", "\">>=\"", "\"%=\"", "\"->=\"", "\"~=\"", "\"<>=\"",
  "\"#=\"", "\"++=\"", "'.'", "'['", "']'", "'$'", "\"<>\"", "$accept",
  "input", "block_func", "block", "line", "shellarg", "shell", "debug",
  "auxsys", "line_func", "eol", "eol2", "func_end", "func_decl", "funcdef",
  "case_list", "stmt", "conditional", "elseif_list", "expcondition",
  "csig", "initcell", "condition", "id_list", "arg", "arg_list", "matrix",
  "vector", "range", "exp_range", "compop", "assign2this", "varblock",
  "tid", "assign", "exp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,    61,    39,
      60,    62,    58,   126,    45,    43,   293,    37,    64,    35,
      42,    47,    94,   294,   295,   296,   297,   124,    44,    59,
      40,    41,   123,   125,    33,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,    46,    91,
      93,    36,   311
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    84,    85,    85,    86,    86,    87,    87,
      87,    87,    87,    87,    87,    87,    88,    88,    88,    88,
      88,    89,    89,    90,    91,    91,    92,    92,    93,    93,
      93,    94,    95,    95,    96,    96,    97,    97,    97,    97,
      98,    98,    98,    98,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,   100,   100,
     100,   100,   101,   101,   101,   102,   103,   104,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   106,   106,
     106,   107,   107,   107,   108,   108,   109,   109,   109,   109,
     110,   110,   110,   111,   111,   112,   112,   112,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     114,   114,   114,   114,   115,   115,   115,   115,   115,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   117,   117,   117,
     117,   117,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     2,     1,     2,
       3,     3,     3,     2,     2,     4,     1,     1,     1,     2,
       1,     1,     2,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     2,     4,     6,     7,     9,
       0,     1,     5,     7,     1,     1,     1,     5,     4,     6,
       6,     6,     4,     6,     7,     1,     1,     1,     1,     2,
       1,     2,     0,     4,     3,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     0,     1,
       3,     1,     1,     1,     1,     3,     0,     1,     3,     2,
       1,     2,     3,     3,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     1,     3,     4,     3,     2,     1,
       4,     4,     7,     4,     3,     1,     4,     4,     7,     2,
       5,     6,     6,     6,     6,     3,     3,     2,     2,     3,
       3,     3,     1,     1,     1,     1,     1,     2,     2,     8,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     8,     0,     0,     0,    56,    57,     0,    34,
       0,    55,     0,     0,   125,   144,   145,   114,   146,     0,
       0,     0,     0,     0,     0,     0,     0,    86,     0,     0,
       0,    26,     4,     0,    27,     0,    44,    65,   142,    97,
      96,    66,   119,   143,    45,    95,     9,     0,     0,   142,
      58,   119,   143,    60,     0,     0,     0,    40,    35,     0,
       0,     6,     0,     0,     0,     0,    18,    20,    16,     0,
      24,     0,    17,   147,   148,    21,     0,    23,     0,    97,
      66,    81,   142,    84,     0,    82,    75,     0,    87,    90,
     118,     0,     1,     5,     0,   119,    30,    29,    28,    31,
      13,    14,     0,     0,     0,     0,     0,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,     0,
       0,   138,     0,   129,     0,     0,   137,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
      61,     0,     0,    41,     0,     0,     0,     0,     7,     0,
       0,     0,     0,    19,    25,    12,    22,    10,    11,    68,
     136,     0,    67,    89,   135,     0,   117,    91,    78,     0,
       0,     0,    76,    77,   142,   110,   143,   139,   124,     0,
       0,   111,   112,   113,   140,     0,   115,   162,   163,    71,
      72,    74,    73,   154,    69,    70,    93,   157,   151,   150,
     160,   156,   161,   158,   152,   153,   155,   159,     0,     0,
      52,     0,    48,     0,     0,     0,     0,     0,   126,   127,
     120,   121,    85,    88,    86,    92,    86,   114,     0,    66,
      95,    33,    36,     0,   123,   116,    15,     0,     0,     0,
      47,     0,     0,     0,     0,     0,    66,     0,     0,     0,
       0,     0,    87,   130,     0,    87,     0,     0,    78,     0,
      94,     0,     0,     0,    53,     0,     0,    49,     0,    50,
      51,     0,     0,   134,   133,   132,   131,    80,     0,     0,
      37,    63,    54,    67,     0,     0,   128,   122,    38,     0,
       0,   149,     0,     0,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    29,    30,    60,    61,    70,    76,    78,    71,    32,
     100,   101,   242,    33,    34,   154,    35,    47,   219,    36,
      37,    49,    39,   238,    83,    84,    87,    88,    40,    41,
     120,   126,    51,    52,    44,    45
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -168
static const yytype_int16 yypact[] =
{
     852,     4,  -168,  2198,  2198,   -14,  -168,  -168,  2252,  -168,
      42,  -168,    54,  1924,   -48,  -168,  -168,    48,  -168,   295,
    2252,  2252,   295,   295,  2198,  2090,  2198,  2198,    82,   100,
     925,  -168,  -168,   159,  -168,    79,   217,  -168,    81,  -168,
    -168,  -168,  2537,  2496,  -168,    -8,  -168,  1924,   217,  -168,
      92,   112,    31,   797,  1924,    85,  2198,   578,  -168,    82,
    1412,  -168,  2090,  2252,  2090,  2252,  -168,  -168,    11,   114,
    -168,    22,    31,    76,    76,  -168,    22,  -168,     3,   101,
     105,  -168,    14,  -168,     5,   232,  -168,    40,   728,   232,
     112,    31,  -168,  -168,  1062,   115,  -168,  -168,  -168,  -168,
    -168,  -168,  2198,  2198,  2198,   327,  2252,  -168,  -168,  -168,
    -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  -168,  2198,
    2198,  -168,  2198,  -168,   157,   162,  -168,  2252,  2252,  2252,
    2252,  2252,  2252,  2252,  2252,  2252,  2252,  2252,  2252,  2252,
    2252,  2252,  2252,  2252,  2252,  2252,  2252,  2252,  1126,  -168,
    -168,  1540,  2198,  -168,   187,    13,   172,   177,  -168,    99,
    1213,   123,  1292,  -168,  -168,  -168,  -168,  -168,  -168,  -168,
    -168,  2090,  -168,  2198,   109,  2198,   138,   232,  2122,  2090,
     998,   199,  -168,   193,    86,   232,  2518,  -168,  -168,   125,
    2379,   235,  2295,   232,  -168,   208,  -168,   -23,   448,  2500,
    2500,  2500,  2500,    -6,  2500,  2500,  2420,    83,   448,   448,
     -23,   -23,   -23,   -23,    -6,    -6,    -6,  2500,  2198,   212,
    -168,  1190,  -168,  2272,  1924,  2198,  1924,  1924,  -168,   184,
    -168,   194,  -168,  2144,  2198,   232,  2176,    94,   147,   153,
    2338,  -168,  -168,  1988,  -168,  -168,  -168,  2252,  1924,  1924,
    -168,  1924,  1604,  2090,   668,  1668,   198,  1732,  1796,  2090,
    2090,    84,  2026,  -168,    87,  2058,   231,  1924,  2230,   998,
    2500,  1126,  1476,  1860,  -168,    53,  1924,  -168,  2252,  -168,
    -168,   170,   171,  -168,  -168,  -168,  -168,  -168,   998,   181,
    -168,   262,  -168,   270,  1269,  2460,  -168,  -168,  -168,  1924,
    1924,  -168,   998,  1348,  -168
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -168,  -168,  -168,   156,   233,    26,  -168,  -168,  -168,   247,
     120,  -168,  -120,  -168,  -168,  -168,  -168,    -3,     8,   515,
    -168,     0,     7,    10,   110,   -60,     1,  -167,  -168,   551,
    -168,   240,   176,    65,   -99,   418
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -142
static const yytype_int16 yytable[] =
{
      38,    54,   159,    96,   161,   187,   233,    97,    46,   133,
      50,    50,    62,    38,    63,   127,   128,   129,   130,   131,
     132,    55,    96,   194,   133,    82,    97,   144,   145,   146,
      38,    79,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   189,   146,    38,    75,    77,
    -114,   225,   123,    14,    38,    66,    67,    68,    58,   147,
      38,    98,    82,   171,    82,    43,    69,   262,   172,   265,
     123,    64,   -83,    65,   147,   -83,   147,   -83,    43,    96,
      98,   -46,    56,    97,    72,   -46,  -141,    72,    72,  -114,
    -141,   125,    96,    91,    38,    43,    97,   164,    91,   173,
      92,    27,   166,    28,   184,    82,   127,   128,    64,   125,
      65,   171,    43,    14,    59,   133,   293,    17,   161,    43,
     174,   -97,   -97,   152,   155,    43,   191,   138,   139,   140,
     141,   142,   143,   144,   145,   146,    72,    98,    99,   -46,
     -46,    72,    56,   173,  -141,  -141,   173,   163,    38,   290,
      98,    38,   -79,   181,    64,   -79,    65,   171,   147,    43,
     228,    27,   169,    28,   283,   147,   170,   285,   298,   186,
     149,    82,   105,   150,   106,   105,    42,   106,    82,    82,
      38,   171,   304,   171,   230,    79,   244,   186,   234,    42,
      14,   165,   195,   275,    94,   222,   167,   196,   168,   281,
     282,   223,   224,   148,    90,   266,    42,   226,   267,    95,
     151,   -82,   227,    43,   170,   248,    43,   236,   249,    56,
     250,    38,   102,    42,    38,    50,    38,    38,   171,   171,
      42,   296,   297,    31,   243,   261,    42,   264,    27,   266,
      28,   246,   299,    38,   259,    43,   102,   103,    38,    38,
     180,    38,    38,    82,   260,    38,   278,    38,    38,    82,
      82,   -66,   -66,    31,   -97,   -97,   287,    38,   249,    38,
      42,    38,    38,    38,   300,    79,    38,    93,   289,   291,
      42,   232,   121,     0,     0,     0,    43,     0,    38,    43,
       0,    43,    43,   158,    38,     0,     0,     0,    42,    38,
      38,     0,    38,    38,     0,     0,     0,     0,    43,     0,
       0,     0,     0,    43,    43,     0,    43,    43,     0,     0,
      43,     0,    43,    43,    42,     0,    14,    42,    66,    67,
      68,     0,    43,     0,    43,     0,    43,    43,    43,    69,
       0,    43,     0,     0,     0,     0,    12,     0,     0,     0,
       0,     0,     0,    43,     0,    56,    42,     0,    14,    43,
      15,    16,    17,    18,    43,    43,     0,    43,    43,    81,
       0,    20,    21,     0,    27,     0,    28,   252,     0,     0,
     255,   158,   257,   258,   158,     0,     0,    24,   188,    25,
       0,    26,     0,     0,     0,     0,     0,    42,     0,   269,
      42,     0,    42,    42,   271,   272,    27,   273,    28,     0,
       0,     0,     0,   158,     0,     0,     0,     0,     0,    42,
       0,    53,    53,   288,    42,    42,    57,    42,    42,     0,
       0,    42,   294,    42,    42,     0,     0,     0,    73,    74,
       0,     0,     0,    42,     0,    42,     0,    42,    42,    42,
       0,     0,    42,     0,     0,   302,   303,     0,     0,     0,
       0,     0,     0,     0,    42,     0,     0,     0,     0,     0,
      42,   127,     0,     0,     0,    42,    42,     0,    42,    42,
     133,   160,     0,   162,     0,   158,     0,     0,   158,     0,
     158,   158,     0,     0,   140,   141,   142,   143,   144,   145,
     146,     0,   158,     0,   158,   158,   158,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,    48,
       0,   158,     0,     0,   190,     0,     0,   158,     0,     0,
     147,     0,     0,     0,     0,   158,   158,   192,     0,    48,
      48,    86,    48,     0,     0,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,     0,     0,     0,     0,
       0,    48,     0,     0,     0,    80,    85,    48,    89,    48,
       0,     0,   153,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   240,     0,     0,
       0,   127,   128,    48,     0,     0,     0,    80,     0,     0,
     133,     0,     0,    85,     0,    85,     0,   182,   183,    48,
      48,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,     0,     0,     0,    48,    48,    53,    48,     0,   177,
       0,   254,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   185,    85,     0,     0,     0,
     147,     0,     0,     0,     0,   270,     0,    48,     0,     0,
       0,   193,   276,   185,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,    48,     0,
      48,   127,   128,    48,    48,     0,   295,     0,     0,     0,
     133,     0,     0,   221,     0,     0,     0,     0,     0,     0,
       0,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,     0,    85,     0,    89,     0,   235,     0,     0,   239,
      85,     0,     0,    48,     0,     0,     0,     0,     0,     0,
      48,     0,     0,     0,     0,     0,     0,    12,    48,    48,
     147,    48,     0,     0,     0,     0,     0,     0,     0,    14,
       0,    15,    16,    17,    18,     0,     0,     0,    48,     0,
       0,     0,    20,    21,    48,    48,   256,    48,     0,     0,
      48,     0,     0,    48,   177,    89,   175,    89,    24,     0,
      25,     0,    26,     0,     0,     0,     0,    96,     0,     0,
       0,    97,     0,     0,    85,     0,     0,    27,   176,    28,
      85,    85,     0,   177,     0,     0,   177,     0,     0,    80,
     127,   128,   129,   130,   131,   132,   -95,   -95,     0,   133,
       0,     0,     0,     0,     0,     0,     0,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
       0,     0,    -2,     1,     0,    98,     2,     3,     0,     0,
       0,     4,     5,     6,     7,     8,     0,     0,     9,    10,
      11,    12,    13,     0,     0,     0,     0,     0,     0,   147,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       0,     0,     0,    19,     0,     0,    20,    21,     0,     0,
       0,    22,     0,    23,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,     0,    26,     0,     0,     0,
       0,     0,     0,     0,     0,    -3,     1,     0,     0,     2,
       3,    27,     0,    28,     4,     5,     6,     7,     8,     0,
       0,     9,    10,    11,    12,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,    15,    16,
      17,    18,     0,     0,     0,     0,    19,     0,     0,    20,
      21,     0,     0,     0,    22,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    25,     0,    26,
       0,     0,     0,     0,     0,     0,     0,     0,   241,     1,
       0,     0,     2,     3,    27,     0,    28,     4,     5,     6,
       7,     8,     0,     0,   -32,   -32,    11,    12,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,    15,    16,    17,    18,     0,     0,     0,     0,    19,
       0,     0,    20,    21,     0,     0,     0,    22,     0,    23,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
      25,     0,    26,     1,     0,     0,     2,     3,     0,     0,
       0,     4,     5,     6,     7,     8,     0,    27,     0,    28,
      11,    12,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
    -114,  -114,     0,    19,     0,     0,    20,    21,     0,     0,
       0,    22,     0,    23,     0,     0,     0,     0,     0,     0,
       0,     0,   178,     0,   179,     0,    26,     1,     0,     0,
       2,     3,   -62,   218,   -62,     4,     5,     6,     7,     8,
    -114,    27,     0,    28,    11,    12,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,    15,
      16,    17,    18,     0,     0,     0,     0,    19,     0,     0,
      20,    21,     0,     0,     0,    22,     0,    23,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    25,     0,
      26,     1,     0,     0,     2,     3,     0,     0,     0,     4,
       5,     6,     7,     8,     0,    27,     0,    28,    11,    12,
      13,     0,     0,     0,     0,     0,     0,     0,     0,   -66,
     -66,    14,     0,    15,    16,    17,    18,     0,     0,     0,
       0,    19,     0,     0,    20,    21,   127,   128,     0,    22,
       0,    23,     0,     0,     0,   133,     0,     0,   251,     0,
      24,     0,    25,     0,    26,     0,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,     0,     0,     0,    27,
       1,    28,     0,     2,     3,     0,   229,   -42,     4,     5,
       6,     7,     8,   -42,   -42,     0,     0,    11,    12,    13,
       0,     0,     0,     0,     0,   147,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,   127,   128,     0,    22,     0,
      23,     0,     0,     0,   133,     0,     0,     0,     0,    24,
       0,    25,     0,    26,     0,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,     0,     0,     0,    27,     1,
      28,     0,     2,     3,     0,   231,   -43,     4,     5,     6,
       7,     8,   -43,   -43,     0,     0,    11,    12,    13,     0,
       0,     0,     0,     0,   147,     0,     0,     0,     0,    14,
       0,    15,    16,    17,    18,     0,     0,     0,     0,    19,
       0,     0,    20,    21,     0,     0,     0,    22,     0,    23,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
      25,     0,    26,     1,     0,     0,     2,     3,     0,     0,
       0,     4,     5,     6,     7,     8,     0,    27,     0,    28,
      11,    12,    13,   156,   157,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       0,     0,     0,    19,     0,     0,    20,    21,     0,     0,
       0,    22,     0,    23,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,     0,    26,     1,     0,     0,
       2,     3,   -64,     0,   -64,     4,     5,     6,     7,     8,
       0,    27,     0,    28,    11,    12,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,    15,
      16,    17,    18,     0,     0,     0,     0,    19,     0,     0,
      20,    21,     0,     0,     0,    22,     0,    23,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    25,     0,
      26,     1,     0,     0,     2,     3,     0,     0,   220,     4,
       5,     6,     7,     8,     0,    27,     0,    28,    11,    12,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,    15,    16,    17,    18,     0,     0,     0,
       0,    19,     0,     0,    20,    21,     0,     0,     0,    22,
       0,    23,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,   274,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    25,     0,    26,     1,
       0,     0,     2,     3,     0,     0,   277,     4,     5,     6,
       7,     8,     0,    27,     0,    28,    11,    12,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,    15,    16,    17,    18,     0,     0,     0,     0,    19,
       0,     0,    20,    21,     0,     0,     0,    22,     0,    23,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
      25,     0,    26,     1,     0,     0,     2,     3,     0,     0,
     279,     4,     5,     6,     7,     8,     0,    27,     0,    28,
      11,    12,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
       0,     0,     0,    19,     0,     0,    20,    21,     0,     0,
       0,    22,     0,    23,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,     0,    26,     1,     0,     0,
       2,     3,     0,     0,   280,     4,     5,     6,     7,     8,
       0,    27,     0,    28,    11,    12,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,    15,
      16,    17,    18,     0,     0,     0,     0,    19,     0,     0,
      20,    21,     0,     0,     0,    22,     0,    23,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    25,     0,
      26,     1,     0,     0,     2,     3,     0,     0,   292,     4,
       5,     6,     7,     8,     0,    27,     0,    28,    11,    12,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,    15,    16,    17,    18,     0,     0,     0,
       0,    19,     0,     0,    20,    21,     0,     0,     0,    22,
       0,    23,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,     0,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    25,     0,    26,     1,
       0,     0,     2,     3,     0,     0,     0,     4,     5,     6,
       7,     8,     0,    27,     0,    28,    11,    12,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,    15,    16,    17,    18,     0,     0,     0,     0,    19,
       0,     0,    20,    21,     0,     0,     0,    22,     0,    23,
       0,     0,     0,     0,     0,    12,     0,     0,   268,     0,
      25,     0,    26,     0,     0,     0,     0,    14,     0,    15,
      16,    17,    18,     0,     0,     0,     0,    27,     0,    28,
      20,    21,     0,     0,     0,     0,     0,    12,     0,     0,
       0,     0,     0,     0,   175,     0,    24,     0,    25,    14,
      26,    15,    16,    17,    18,     0,     0,     0,     0,     0,
       0,     0,    20,    21,     0,    27,   284,    28,     0,    12,
       0,     0,     0,     0,     0,     0,   175,     0,    24,     0,
      25,    14,    26,    15,    16,    17,    18,     0,     0,     0,
       0,     0,    81,     0,    20,    21,     0,    27,   286,    28,
       0,    12,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,    14,    26,    15,    16,   237,    18,     0,
       0,     0,     0,    12,    81,     0,    20,    21,     0,    27,
       0,    28,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,    24,     0,    25,     0,    26,     0,    20,    21,
       0,     0,     0,     0,     0,    12,     0,     0,     0,     0,
       0,    27,   175,    28,    24,     0,    25,    14,    26,    15,
      16,    17,    18,     0,     0,     0,     0,    12,     0,     0,
      20,    21,     0,    27,     0,    28,     0,     0,     0,    14,
       0,    15,    16,    17,    18,     0,    24,     0,    25,     0,
      26,     0,    20,    21,     0,     0,     0,     0,     0,    12,
       0,     0,     0,     0,     0,    27,   263,    28,    24,     0,
      25,    14,    26,    15,    16,   237,    18,     0,     0,     0,
       0,    12,     0,     0,    20,    21,     0,    27,     0,    28,
       0,     0,     0,    14,     0,    15,    16,    17,    18,     0,
      24,    12,    25,     0,    26,     0,    20,    21,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,    27,
       0,    28,    56,     0,    25,     0,    20,    21,   127,   128,
     129,   130,   131,   132,   -95,   -95,     0,   133,     0,     0,
       0,    27,    56,    28,   253,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,     0,     0,
       0,    27,     0,    28,     0,     0,     0,     0,     0,     0,
       0,   127,   128,   129,   130,   131,   132,     0,     0,     0,
     133,     0,     0,     0,     0,     0,     0,   147,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   231,   127,   128,     0,     0,     0,     0,     0,     0,
       0,   133,     0,     0,     0,     0,     0,     0,     0,     0,
     147,     0,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   245,   127,   128,     0,     0,     0,     0,     0,
       0,     0,   133,     0,     0,     0,     0,     0,     0,     0,
       0,   147,   247,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   127,   128,     0,     0,     0,     0,     0,
       0,     0,   133,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   147,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,     0,     0,     0,     0,     0,     0,     0,
       0,   301,     0,   127,   128,     0,     0,     0,     0,     0,
       0,     0,   133,     0,   122,   123,     0,     0,     0,     0,
       0,     0,   147,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   124,     0,     0,   122,   123,     0,     0,
       0,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   125,   104,     0,     0,     0,     0,
       0,     0,   147,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   125,   105,     0,   106,
       0,     0,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119
};

static const yytype_int16 yycheck[] =
{
       0,     4,    62,     0,    64,   104,   173,     4,     4,    32,
       3,     4,    60,    13,    62,    23,    24,    25,    26,    27,
      28,    35,     0,   122,    32,    25,     4,    50,    51,    52,
      30,    24,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,   105,    52,    47,    22,    23,
      39,    38,    39,    31,    54,    33,    34,    35,    16,    82,
      60,    58,    62,    58,    64,     0,    44,   234,    63,   236,
      39,    60,    58,    62,    82,    61,    82,    63,    13,     0,
      58,     0,    60,     4,    19,     4,     0,    22,    23,    78,
       4,    78,     0,    28,    94,    30,     4,    71,    33,    59,
       0,    79,    76,    81,   104,   105,    23,    24,    60,    78,
      62,    58,    47,    31,    60,    32,    63,    35,   178,    54,
      80,    29,    30,    38,    59,    60,   119,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    71,    58,    59,    58,
      59,    76,    60,    59,    58,    59,    59,    33,   148,   269,
      58,   151,    58,    38,    60,    61,    62,    58,    82,    94,
      61,    79,    61,    81,    80,    82,    61,    80,   288,   104,
      50,   171,    60,    53,    62,    60,     0,    62,   178,   179,
     180,    58,   302,    58,    61,   178,    61,   122,    79,    13,
      31,    71,    35,   253,    35,     8,    76,    35,    78,   259,
     260,    14,    15,    47,    28,    58,    30,    35,    61,    33,
      54,    58,    35,   148,    61,   218,   151,    79,     6,    60,
       8,   221,    29,    47,   224,   218,   226,   227,    58,    58,
      54,    61,    61,     0,    35,   234,    60,   236,    79,    58,
      81,    33,    61,   243,    60,   180,    29,    30,   248,   249,
      94,   251,   252,   253,    60,   255,    58,   257,   258,   259,
     260,    29,    30,    30,    29,    30,    35,   267,     6,   269,
      94,   271,   272,   273,     4,   268,   276,    30,   268,   271,
     104,   171,    42,    -1,    -1,    -1,   221,    -1,   288,   224,
      -1,   226,   227,    60,   294,    -1,    -1,    -1,   122,   299,
     300,    -1,   302,   303,    -1,    -1,    -1,    -1,   243,    -1,
      -1,    -1,    -1,   248,   249,    -1,   251,   252,    -1,    -1,
     255,    -1,   257,   258,   148,    -1,    31,   151,    33,    34,
      35,    -1,   267,    -1,   269,    -1,   271,   272,   273,    44,
      -1,   276,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,   288,    -1,    60,   180,    -1,    31,   294,
      33,    34,    35,    36,   299,   300,    -1,   302,   303,    42,
      -1,    44,    45,    -1,    79,    -1,    81,   221,    -1,    -1,
     224,   148,   226,   227,   151,    -1,    -1,    60,    61,    62,
      -1,    64,    -1,    -1,    -1,    -1,    -1,   221,    -1,   243,
     224,    -1,   226,   227,   248,   249,    79,   251,    81,    -1,
      -1,    -1,    -1,   180,    -1,    -1,    -1,    -1,    -1,   243,
      -1,     3,     4,   267,   248,   249,     8,   251,   252,    -1,
      -1,   255,   276,   257,   258,    -1,    -1,    -1,    20,    21,
      -1,    -1,    -1,   267,    -1,   269,    -1,   271,   272,   273,
      -1,    -1,   276,    -1,    -1,   299,   300,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   288,    -1,    -1,    -1,    -1,    -1,
     294,    23,    -1,    -1,    -1,   299,   300,    -1,   302,   303,
      32,    63,    -1,    65,    -1,   252,    -1,    -1,   255,    -1,
     257,   258,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    -1,   269,    -1,   271,   272,   273,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,   288,    -1,    -1,   106,    -1,    -1,   294,    -1,    -1,
      82,    -1,    -1,    -1,    -1,   302,   303,   119,    -1,    24,
      25,    26,    27,    -1,    -1,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,    -1,    -1,    -1,    -1,
      -1,    56,    -1,    -1,    -1,    24,    25,    62,    27,    64,
      -1,    -1,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   179,    -1,    -1,
      -1,    23,    24,    88,    -1,    -1,    -1,    56,    -1,    -1,
      32,    -1,    -1,    62,    -1,    64,    -1,   102,   103,   104,
     105,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    -1,    -1,   119,   120,   218,   122,    -1,    88,
      -1,   223,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,   105,    -1,    -1,    -1,
      82,    -1,    -1,    -1,    -1,   247,    -1,   152,    -1,    -1,
      -1,   120,     4,   122,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   171,    -1,   173,    -1,
     175,    23,    24,   178,   179,    -1,   278,    -1,    -1,    -1,
      32,    -1,    -1,   152,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,   171,    -1,   173,    -1,   175,    -1,    -1,   178,
     179,    -1,    -1,   218,    -1,    -1,    -1,    -1,    -1,    -1,
     225,    -1,    -1,    -1,    -1,    -1,    -1,    19,   233,   234,
      82,   236,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    -1,   253,    -1,
      -1,    -1,    44,    45,   259,   260,   225,   262,    -1,    -1,
     265,    -1,    -1,   268,   233,   234,    58,   236,    60,    -1,
      62,    -1,    64,    -1,    -1,    -1,    -1,     0,    -1,    -1,
      -1,     4,    -1,    -1,   253,    -1,    -1,    79,    80,    81,
     259,   260,    -1,   262,    -1,    -1,   265,    -1,    -1,   268,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    -1,     0,     1,    -1,    58,     4,     5,    -1,    -1,
      -1,     9,    10,    11,    12,    13,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    44,    45,    -1,    -1,
      -1,    49,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    -1,    62,    -1,    64,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     0,     1,    -1,    -1,     4,
       5,    79,    -1,    81,     9,    10,    11,    12,    13,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,
      45,    -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    -1,    62,    -1,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,
      -1,    -1,     4,     5,    79,    -1,    81,     9,    10,    11,
      12,    13,    -1,    -1,    16,    17,    18,    19,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,
      62,    -1,    64,     1,    -1,    -1,     4,     5,    -1,    -1,
      -1,     9,    10,    11,    12,    13,    -1,    79,    -1,    81,
      18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      38,    39,    -1,    41,    -1,    -1,    44,    45,    -1,    -1,
      -1,    49,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    -1,    62,    -1,    64,     1,    -1,    -1,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      78,    79,    -1,    81,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      44,    45,    -1,    -1,    -1,    49,    -1,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    62,    -1,
      64,     1,    -1,    -1,     4,     5,    -1,    -1,    -1,     9,
      10,    11,    12,    13,    -1,    79,    -1,    81,    18,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    44,    45,    23,    24,    -1,    49,
      -1,    51,    -1,    -1,    -1,    32,    -1,    -1,    58,    -1,
      60,    -1,    62,    -1,    64,    -1,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    -1,    -1,    79,
       1,    81,    -1,     4,     5,    -1,    63,     8,     9,    10,
      11,    12,    13,    14,    15,    -1,    -1,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    23,    24,    -1,    49,    -1,
      51,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    60,
      -1,    62,    -1,    64,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    -1,    -1,    79,     1,
      81,    -1,     4,     5,    -1,    63,     8,     9,    10,    11,
      12,    13,    14,    15,    -1,    -1,    18,    19,    20,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,
      62,    -1,    64,     1,    -1,    -1,     4,     5,    -1,    -1,
      -1,     9,    10,    11,    12,    13,    -1,    79,    -1,    81,
      18,    19,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    44,    45,    -1,    -1,
      -1,    49,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    -1,    62,    -1,    64,     1,    -1,    -1,
       4,     5,     6,    -1,     8,     9,    10,    11,    12,    13,
      -1,    79,    -1,    81,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      44,    45,    -1,    -1,    -1,    49,    -1,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    62,    -1,
      64,     1,    -1,    -1,     4,     5,    -1,    -1,     8,     9,
      10,    11,    12,    13,    -1,    79,    -1,    81,    18,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    -1,    62,    -1,    64,     1,    -1,    -1,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    -1,    79,
      -1,    81,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    62,    -1,    64,     1,
      -1,    -1,     4,     5,    -1,    -1,     8,     9,    10,    11,
      12,    13,    -1,    79,    -1,    81,    18,    19,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,
      62,    -1,    64,     1,    -1,    -1,     4,     5,    -1,    -1,
       8,     9,    10,    11,    12,    13,    -1,    79,    -1,    81,
      18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    44,    45,    -1,    -1,
      -1,    49,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    -1,    62,    -1,    64,     1,    -1,    -1,
       4,     5,    -1,    -1,     8,     9,    10,    11,    12,    13,
      -1,    79,    -1,    81,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      44,    45,    -1,    -1,    -1,    49,    -1,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    62,    -1,
      64,     1,    -1,    -1,     4,     5,    -1,    -1,     8,     9,
      10,    11,    12,    13,    -1,    79,    -1,    81,    18,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    -1,    62,    -1,    64,     1,    -1,    -1,     4,     5,
      -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,    79,
      -1,    81,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    -1,    62,    -1,    64,     1,
      -1,    -1,     4,     5,    -1,    -1,    -1,     9,    10,    11,
      12,    13,    -1,    79,    -1,    81,    18,    19,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    60,    -1,
      62,    -1,    64,    -1,    -1,    -1,    -1,    31,    -1,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    79,    -1,    81,
      44,    45,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    -1,    60,    -1,    62,    31,
      64,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    79,    80,    81,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,    60,    -1,
      62,    31,    64,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    45,    -1,    79,    80,    81,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    -1,    62,    31,    64,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    19,    42,    -1,    44,    45,    -1,    79,
      -1,    81,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    60,    -1,    62,    -1,    64,    -1,    44,    45,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    79,    58,    81,    60,    -1,    62,    31,    64,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    19,    -1,    -1,
      44,    45,    -1,    79,    -1,    81,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    60,    -1,    62,    -1,
      64,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    79,    80,    81,    60,    -1,
      62,    31,    64,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    19,    -1,    -1,    44,    45,    -1,    79,    -1,    81,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      60,    19,    62,    -1,    64,    -1,    44,    45,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,    79,
      -1,    81,    60,    -1,    62,    -1,    44,    45,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    32,    -1,    -1,
      -1,    79,    60,    81,    62,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    -1,
      -1,    79,    -1,    81,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    82,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    63,    23,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    23,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    57,    -1,    -1,    38,    39,    -1,    -1,
      -1,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    38,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    60,    -1,    62,
      -1,    -1,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     4,     5,     9,    10,    11,    12,    13,    16,
      17,    18,    19,    20,    31,    33,    34,    35,    36,    41,
      44,    45,    49,    51,    60,    62,    64,    79,    81,    84,
      85,    87,    92,    96,    97,    99,   102,   103,   104,   105,
     111,   112,   115,   116,   117,   118,     4,   100,   102,   104,
     105,   115,   116,   118,   100,    35,    60,   118,    16,    60,
      86,    87,    60,    62,    60,    62,    33,    34,    35,    44,
      88,    91,   116,   118,   118,    88,    89,    88,    90,   105,
     112,    42,   104,   107,   108,   112,   102,   109,   110,   112,
     115,   116,     0,    92,    35,   115,     0,     4,    58,    59,
      93,    94,    29,    30,    38,    60,    62,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
     113,   114,    38,    39,    57,    78,   114,    23,    24,    25,
      26,    27,    28,    32,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    82,    86,    93,
      93,    86,    38,     4,    98,   116,    21,    22,    87,   108,
     118,   108,   118,    33,    88,    93,    88,    93,    93,    61,
      61,    58,    63,    59,    80,    58,    80,   112,    60,    62,
      86,    38,   102,   102,   104,   112,   116,   117,    61,   108,
     118,   105,   118,   112,   117,    35,    35,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,     7,   101,
       8,   112,     8,    14,    15,    38,    35,    35,    61,    63,
      61,    63,   107,   110,    79,   112,    79,    35,   106,   112,
     118,     0,    95,    35,    61,    63,    33,    42,   100,     6,
       8,    58,    86,    62,   118,    86,   112,    86,    86,    60,
      60,   109,   110,    80,   109,   110,    58,    61,    60,    86,
     118,    86,    86,    86,     8,   108,     4,     8,    58,     8,
       8,   108,   108,    80,    80,    80,    80,    35,    86,   106,
      95,   101,     8,    63,    86,   118,    61,    61,    95,    61,
       4,    61,    86,    86,    95
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (pproot, errmsg, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, pproot, errmsg); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (pproot);
  YYUSE (errmsg);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, AstNode **pproot, char **errmsg)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, pproot, errmsg)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    AstNode **pproot;
    char **errmsg;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , pproot, errmsg);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, pproot, errmsg); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, pproot, errmsg)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pproot);
  YYUSE (errmsg);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 34: /* "\"string\"" */

/* Line 1000 of yacc.c  */
#line 111 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", (yyvaluep->str));
#endif
  free((yyvaluep->str));
};

/* Line 1000 of yacc.c  */
#line 1885 "psycon.tab.c"
	break;
      case 35: /* "\"identifier\"" */

/* Line 1000 of yacc.c  */
#line 111 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", (yyvaluep->str));
#endif
  free((yyvaluep->str));
};

/* Line 1000 of yacc.c  */
#line 1899 "psycon.tab.c"
	break;
      case 85: /* "block_func" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1913 "psycon.tab.c"
	break;
      case 86: /* "block" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1927 "psycon.tab.c"
	break;
      case 87: /* "line" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1941 "psycon.tab.c"
	break;
      case 88: /* "shellarg" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1955 "psycon.tab.c"
	break;
      case 89: /* "shell" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1969 "psycon.tab.c"
	break;
      case 90: /* "debug" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1983 "psycon.tab.c"
	break;
      case 91: /* "auxsys" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1997 "psycon.tab.c"
	break;
      case 92: /* "line_func" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2011 "psycon.tab.c"
	break;
      case 96: /* "func_decl" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2025 "psycon.tab.c"
	break;
      case 97: /* "funcdef" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2039 "psycon.tab.c"
	break;
      case 98: /* "case_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2053 "psycon.tab.c"
	break;
      case 99: /* "stmt" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2067 "psycon.tab.c"
	break;
      case 100: /* "conditional" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2081 "psycon.tab.c"
	break;
      case 101: /* "elseif_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2095 "psycon.tab.c"
	break;
      case 102: /* "expcondition" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2109 "psycon.tab.c"
	break;
      case 103: /* "csig" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2123 "psycon.tab.c"
	break;
      case 104: /* "initcell" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2137 "psycon.tab.c"
	break;
      case 105: /* "condition" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2151 "psycon.tab.c"
	break;
      case 106: /* "id_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2165 "psycon.tab.c"
	break;
      case 107: /* "arg" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2179 "psycon.tab.c"
	break;
      case 108: /* "arg_list" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2193 "psycon.tab.c"
	break;
      case 109: /* "matrix" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2207 "psycon.tab.c"
	break;
      case 110: /* "vector" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2221 "psycon.tab.c"
	break;
      case 111: /* "range" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2235 "psycon.tab.c"
	break;
      case 112: /* "exp_range" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2249 "psycon.tab.c"
	break;
      case 113: /* "compop" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2263 "psycon.tab.c"
	break;
      case 114: /* "assign2this" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2277 "psycon.tab.c"
	break;
      case 115: /* "varblock" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2291 "psycon.tab.c"
	break;
      case 116: /* "tid" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2305 "psycon.tab.c"
	break;
      case 117: /* "assign" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2319 "psycon.tab.c"
	break;
      case 118: /* "exp" */

/* Line 1000 of yacc.c  */
#line 104 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 2333 "psycon.tab.c"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (AstNode **pproot, char **errmsg);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (AstNode **pproot, char **errmsg)
#else
int
yyparse (pproot, errmsg)
    AstNode **pproot;
    char **errmsg;
#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 95 "psycon.y"
{
  if (ErrorMsg) {
	free(ErrorMsg);
	ErrorMsg = NULL;
  }
  *errmsg = NULL;
}

/* Line 1242 of yacc.c  */
#line 2494 "psycon.tab.c"

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 144 "psycon.y"
    { *pproot = NULL;;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 146 "psycon.y"
    { *pproot = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 151 "psycon.y"
    {
		if ((yyvsp[(2) - (2)].pnode)) {
			if ((yyvsp[(1) - (2)].pnode) == NULL)
				(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
			else if ((yyvsp[(1) - (2)].pnode)->type == N_BLOCK)
			{
				(yyvsp[(1) - (2)].pnode)->tail->next = (yyvsp[(2) - (2)].pnode);
				(yyvsp[(1) - (2)].pnode)->tail = (yyvsp[(2) - (2)].pnode);
			}
			else
			{ // a=1; b=2; ==> $1->type is '='. So first, a N_BLOCK tree should be made.
				(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
				(yyval.pnode)->next = (yyvsp[(1) - (2)].pnode);
				(yyvsp[(1) - (2)].pnode)->next = (yyval.pnode)->tail = (yyvsp[(2) - (2)].pnode);
			}
		} else
			(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 172 "psycon.y"
    {
		if ((yyvsp[(1) - (1)].pnode)) // if cond1, x=1, end ==> x=1 comes here.
			(yyval.pnode) = (yyvsp[(1) - (1)].pnode);
		else
			(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
	;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 179 "psycon.y"
    {
		if ((yyvsp[(2) - (2)].pnode)) {
			if ((yyvsp[(1) - (2)].pnode)->type == N_BLOCK) {
				if ((yyval.pnode)->next) {
					(yyvsp[(1) - (2)].pnode)->tail->next = (yyvsp[(2) - (2)].pnode);
					(yyvsp[(1) - (2)].pnode)->tail = (yyvsp[(2) - (2)].pnode);
				} else {
					(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
					free((yyvsp[(1) - (2)].pnode));
				}
			} else { //if the second argument doesn't have N_BLOCK, make one
				(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
				(yyval.pnode)->next = (yyvsp[(1) - (2)].pnode);
				(yyvsp[(1) - (2)].pnode)->next = (yyval.pnode)->tail = (yyvsp[(2) - (2)].pnode);
			}
		}
		else // only "block" is given
			(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 201 "psycon.y"
    { (yyval.pnode) = NULL;;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 203 "psycon.y"
    {
		(yyval.pnode) = NULL;
		yyerrok;
	;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 208 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 212 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 216 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 221 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		(yyval.pnode)->suppress=1;
	;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 226 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		(yyval.pnode)->tail = newAstNode(T_ID, (yyloc)); 
		(yyval.pnode)->tail->str = (yyvsp[(3) - (4)].str);
		(yyval.pnode)->tail->dval = (yyvsp[(4) - (4)].dval);
	;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 235 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 242 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NUMBER, (yyloc));
		(yyval.pnode)->dval = (yyvsp[(1) - (1)].dval);
	;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 247 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NEGATIVE, (yyloc));
		AstNode *p = newAstNode(T_NUMBER, (yyloc));
		p->dval = (yyvsp[(2) - (2)].dval);
		(yyval.pnode)->child = p;
	;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 255 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_STRING, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 264 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_SHELL, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 269 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[(2) - (2)].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 279 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_DEBUG, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 286 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_AUXSYS, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 291 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[(2) - (2)].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 316 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->suppress = 2;
	;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 321 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->suppress = 3;
	;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 328 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		(yyval.pnode)->str = (yyvsp[(2) - (4)].str);
		(yyval.pnode)->child = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child->next = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 335 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (6)].pnode);
		(yyval.pnode)->str = (yyvsp[(4) - (6)].str);
		(yyval.pnode)->child = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
		if ((yyvsp[(2) - (6)].pnode)->type!=N_VECTOR)
		{
			(yyval.pnode)->alt = newAstNode(N_VECTOR, (yylsp[(2) - (6)]));
			AstNode *p = newAstNode(N_VECTOR, (yylsp[(2) - (6)]));
			p->alt = p->tail = (yyvsp[(2) - (6)].pnode);
			(yyval.pnode)->alt->str = (char*)p;
		}
		else
		{
			(yyval.pnode)->alt = (yyvsp[(2) - (6)].pnode);
		}
	;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 353 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (7)].pnode);
		(yyval.pnode)->str = (yyvsp[(2) - (7)].str);
		(yyval.pnode)->child = (yyvsp[(4) - (7)].pnode);
		(yyvsp[(4) - (7)].pnode)->next = (yyvsp[(6) - (7)].pnode);
	;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 360 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (9)].pnode);
		(yyval.pnode)->str = (yyvsp[(4) - (9)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (9)].pnode);
		(yyvsp[(6) - (9)].pnode)->next = (yyvsp[(8) - (9)].pnode);
		if ((yyvsp[(2) - (9)].pnode)->type!=N_VECTOR)
		{
			(yyval.pnode)->alt = newAstNode(N_VECTOR, (yylsp[(2) - (9)]));
			AstNode *p = newAstNode(N_VECTOR, (yylsp[(2) - (9)]));
			p->alt = p->tail = (yyvsp[(2) - (9)].pnode);
			(yyval.pnode)->alt->str = (char*)p;
		}
		else
		{
			(yyval.pnode)->alt = (yyvsp[(2) - (9)].pnode);
		}
	;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 380 "psycon.y"
    { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 382 "psycon.y"
    { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 384 "psycon.y"
    {
		if ((yyvsp[(1) - (5)].pnode)->alt)
			(yyvsp[(1) - (5)].pnode)->tail->alt = (yyvsp[(3) - (5)].pnode);
		else
			(yyvsp[(1) - (5)].pnode)->alt = (yyvsp[(3) - (5)].pnode);
		AstNode *p = (yyvsp[(5) - (5)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(5) - (5)]));
			p->next = (yyvsp[(5) - (5)].pnode);
		}
		(yyvsp[(1) - (5)].pnode)->tail = (yyvsp[(3) - (5)].pnode)->next = p;
		(yyval.pnode) = (yyvsp[(1) - (5)].pnode);
	;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 399 "psycon.y"
    {
		if ((yyvsp[(1) - (7)].pnode)->alt)
			(yyvsp[(1) - (7)].pnode)->tail->alt = (yyvsp[(4) - (7)].pnode);
		else
			(yyvsp[(1) - (7)].pnode)->alt = (yyvsp[(4) - (7)].pnode);
		AstNode *p = (yyvsp[(7) - (7)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(7) - (7)]));
			p->next = (yyvsp[(7) - (7)].pnode);
		}
		(yyvsp[(1) - (7)].pnode)->tail = (yyvsp[(4) - (7)].pnode)->next = p;
		(yyval.pnode) = (yyvsp[(1) - (7)].pnode);
	;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 419 "psycon.y"
    { // This works, too, for "if cond, act; end" without else, because elseif_list can be empty
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
		AstNode *p = (yyvsp[(3) - (5)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (5)]));
			p->next = (yyvsp[(3) - (5)].pnode);
		}
		(yyval.pnode)->child = (yyvsp[(2) - (5)].pnode);
		(yyvsp[(2) - (5)].pnode)->next = p;
		AstNode *pElse = (yyvsp[(4) - (5)].pnode);
		if (pElse->type!=N_BLOCK)
		{
			pElse = newAstNode(N_BLOCK, (yylsp[(4) - (5)]));
			pElse->next = (yyvsp[(4) - (5)].pnode);
		}
		(yyval.pnode)->alt = pElse;
		if ((yyvsp[(4) - (5)].pnode)->child==NULL && (yyvsp[(4) - (5)].pnode)->next==NULL) // When elseif_list is empty, T_IF is made, but no child and next
		{
			yydeleteAstNode((yyval.pnode)->alt, 1);
			(yyval.pnode)->alt=NULL;
		}
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 445 "psycon.y"
    { // case is cascaded through alt
		(yyval.pnode) = (yyvsp[(3) - (4)].pnode);
		(yyval.pnode)->alt = (yyvsp[(3) - (4)].pnode)->alt;
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 453 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(3) - (6)].pnode);
		(yyval.pnode)->alt = (yyvsp[(3) - (6)].pnode)->alt;
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		AstNode *p = newAstNode(T_OTHERWISE, (yylsp[(5) - (6)]));
		p->next = (yyvsp[(5) - (6)].pnode);
		(yyval.pnode)->tail = (yyvsp[(3) - (6)].pnode)->tail->alt = p;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 464 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_TRY, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->alt = newAstNode(T_CATCH, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child = newAstNode(T_ID, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child->str = (yyvsp[(4) - (6)].str);
		(yyval.pnode)->alt->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 473 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_TRY, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->alt = newAstNode(T_CATCHBACK, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child = newAstNode(T_ID, (yylsp[(4) - (6)]));
		(yyval.pnode)->alt->child->str = (yyvsp[(4) - (6)].str);
		(yyval.pnode)->alt->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 482 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_WHILE, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		AstNode *p = (yyvsp[(3) - (4)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (4)]));
			p->next = (yyvsp[(3) - (4)].pnode);
		}
		(yyval.pnode)->alt = p;
	;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 494 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(2) - (6)]));
		(yyval.pnode)->child->str = (yyvsp[(2) - (6)].str);
		(yyval.pnode)->child->child = (yyvsp[(4) - (6)].pnode);
		AstNode *p = (yyvsp[(5) - (6)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(5) - (6)]));
			p->next = (yyvsp[(5) - (6)].pnode);
		}
		(yyval.pnode)->alt = p;
	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 508 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(2) - (7)]));
		(yyval.pnode)->child->str = (yyvsp[(2) - (7)].str);
		(yyval.pnode)->child->child = (yyvsp[(4) - (7)].pnode);
		AstNode *p = (yyvsp[(6) - (7)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(6) - (7)]));
			p->next = (yyvsp[(6) - (7)].pnode);
		}
		(yyval.pnode)->alt = p;
	;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 522 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_RETURN, (yyloc));
	;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 526 "psycon.y"
    { (yyval.pnode) = newAstNode(T_BREAK, (yyloc));;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 528 "psycon.y"
    { (yyval.pnode) = newAstNode(T_CONTINUE, (yyloc));;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 536 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
	;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 540 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		AstNode *p = (yyvsp[(3) - (4)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (4)]));
			p->next = (yyvsp[(3) - (4)].pnode);
		}
		(yyvsp[(2) - (4)].pnode)->next = p;
		(yyval.pnode)->alt = (yyvsp[(4) - (4)].pnode);
	;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 553 "psycon.y"
    {
		AstNode *p = (yyvsp[(3) - (3)].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[(3) - (3)]));
			p->next = (yyvsp[(3) - (3)].pnode);
		}
		if ((yyvsp[(1) - (3)].pnode)->child==NULL) // if there's no elseif; i.e., elseif_list is empty
		{
			yydeleteAstNode((yyvsp[(1) - (3)].pnode), 1);
			(yyval.pnode) = p;
		}
		else
		{
			(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
			(yyvsp[(1) - (3)].pnode)->alt = p;
		}
	;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 580 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->type = N_INITCELL;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 589 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 595 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('<', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 597 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('>', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 599 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_EQ, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 601 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_NE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 603 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_GE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 605 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_LE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 607 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_LOGIC_NOT, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 612 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_AND, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 614 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_OR, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 618 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_IDLIST, (yyloc));
	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 622 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->tail = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->tail->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 628 "psycon.y"
    {
		(yyvsp[(1) - (3)].pnode)->tail = (yyvsp[(1) - (3)].pnode)->tail->next = newAstNode(T_ID, (yylsp[(3) - (3)]));
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		(yyval.pnode)->tail->str = (yyvsp[(3) - (3)].str);
	;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 636 "psycon.y"
    {	(yyval.pnode) = newAstNode(T_FULLRANGE, (yyloc)); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 642 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_ARGS, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 647 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[(3) - (3)].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[(3) - (3)].pnode);
	;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 657 "psycon.y"
    {
	// N_MATRIX consists of "outer" N_MATRIX--alt for dot notation
	// and "inner" N_VECTOR--alt for all successive items thru next
	// the str field of the outer N_MATRIX node is cast to the inner N_VECTOR.
	// this "fake" str pointer is freed during normal clean-up
	// 11/4/2019
		(yyval.pnode) = newAstNode(N_MATRIX, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		(yyval.pnode)->str = (char*)p;
	;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 668 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_MATRIX, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		p->alt = p->tail = (yyvsp[(1) - (1)].pnode);
		(yyval.pnode)->str = (char*)p;
	;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 675 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		AstNode * p = (AstNode *)(yyvsp[(1) - (3)].pnode)->str;
		p->tail = p->tail->next = (AstNode *)(yyvsp[(3) - (3)].pnode);
	;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 684 "psycon.y"
    {
	// N_VECTOR consists of "outer" N_VECTOR--alt for dot notation
	// and "inner" N_VECTOR--alt for all successive items thru next
	// Because N_VECTOR doesn't use str, the inner N_VECTOR is created there and cast for further uses.
	// this "fake" str pointer is freed during normal clean-up
	// 11/4/2019
		(yyval.pnode) = newAstNode(N_VECTOR, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		p->alt = p->tail = (yyvsp[(1) - (1)].pnode);
		(yyval.pnode)->str = (char*)p;
	;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 696 "psycon.y"
    {
		AstNode * p = (AstNode *)(yyvsp[(1) - (2)].pnode)->str;
		p->tail = p->tail->next = (yyvsp[(2) - (2)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 702 "psycon.y"
    {
		AstNode * p = (AstNode *)(yyvsp[(1) - (3)].pnode)->str;
		p->tail = p->tail->next = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 710 "psycon.y"
    {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));
	;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 714 "psycon.y"
    {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[(1) - (5)].pnode), (yyvsp[(5) - (5)].pnode), (yyloc));
		(yyvsp[(5) - (5)].pnode)->next = (yyvsp[(3) - (5)].pnode);
	;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 724 "psycon.y"
    {
		(yyval.pnode) = newAstNode('+', (yyloc));
	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 728 "psycon.y"
    { 		(yyval.pnode) = newAstNode('-', (yyloc));	;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 730 "psycon.y"
    { 		(yyval.pnode) = newAstNode('*', (yyloc));	;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 732 "psycon.y"
    { 		(yyval.pnode) = newAstNode('/', (yyloc));	;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 734 "psycon.y"
    { 		(yyval.pnode) = newAstNode('@', (yyloc));	;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 736 "psycon.y"
    {
		(yyval.pnode) = newAstNode('@', (yyloc));
		(yyval.pnode)->child = newAstNode('@', (yyloc));
	;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 741 "psycon.y"
    { 		(yyval.pnode) = newAstNode(T_OP_SHIFT, (yyloc));	;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 743 "psycon.y"
    { 		(yyval.pnode) = newAstNode('%', (yyloc));	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 745 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "movespec");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 752 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "respeed");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 759 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "timestretch");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 766 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "pitchscale");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 775 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 779 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_OP_CONCAT, (yyloc));
		AstNode *p = (yyval.pnode);
		if (p->alt)
			p = p->alt;
		p->child = 	newAstNode(T_REPLICA, (yylsp[(2) - (2)]));
		p->tail = p->child->next = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 788 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_OP_CONCAT, (yyloc));
		AstNode *p = (yyval.pnode);
		if (p->alt)
			p = p->alt;
		p->child = 	newAstNode(T_REPLICA, (yylsp[(2) - (2)]));
		p->tail = p->child->next = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 797 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		if ((yyval.pnode)->child) // compop should be "@@=" and $$->child->type should be '@'  (64)
		{
			(yyval.pnode)->child->child = newAstNode(T_REPLICA, (yyloc));
			(yyval.pnode)->child->tail = (yyval.pnode)->child->child->next = newAstNode(T_REPLICA, (yyloc));
			(yyval.pnode)->tail = (yyval.pnode)->child->next = (yyvsp[(2) - (2)].pnode);
		}
		else
		{
			AstNode *p = (yyval.pnode);
			if (p->alt)
				p = p->alt;
			p->child = 	newAstNode(T_REPLICA, (yylsp[(2) - (2)]));
			p->tail = p->child->next = (yyvsp[(2) - (2)].pnode);
		}
	;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 817 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 822 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		AstNode *p = newAstNode(N_STRUCT, (yyloc));
		p->str = (yyvsp[(3) - (3)].str);
		if ((yyval.pnode)->type==N_CELL)
		{
			(yyval.pnode)->alt->alt = p; //always initiating
			(yyval.pnode)->tail = p; // so that next concatenation can point to p, even thought p is "hidden" underneath $$->alt
		}
		if ((yyval.pnode)->tail)
		{
			(yyval.pnode)->tail = (yyval.pnode)->tail->alt = p;
		}
		else
		{
			(yyval.pnode)->tail = (yyval.pnode)->alt = p;
		}
	;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 841 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->tail->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 847 "psycon.y"
    {//tid-vector --> what's this comment? 12/30/2020
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 851 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_HOOK, (yyloc));
		(yyval.pnode)->str = (char*)calloc(1, strlen((yyvsp[(2) - (2)].pnode)->str)+1);
		strcpy((yyval.pnode)->str, (yyvsp[(2) - (2)].pnode)->str);
		(yyval.pnode)->alt = (yyvsp[(2) - (2)].pnode)->alt;
	;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 861 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (4)].str);
		handle_tilde((yyval.pnode), (yyvsp[(3) - (4)].pnode), (yylsp[(3) - (4)]));
	;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 867 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (4)].str);
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 874 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (7)].str);
		(yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (7)].pnode);
		handle_tilde((yyval.pnode)->alt, (yyvsp[(6) - (7)].pnode), (yylsp[(6) - (7)]));
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt; // we need this; or tail is broken and can't put '.' tid at the end
	;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 883 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
		handle_tilde((yyval.pnode), (yyvsp[(3) - (4)].pnode), (yylsp[(3) - (4)]));
	;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 888 "psycon.y"
    {
		if ((yyval.pnode)->alt != NULL  && (yyval.pnode)->alt->type==N_STRUCT)
		{ // dot notation with a blank parentheses, e.g., a.sqrt() or (1:2:5).sqrt()
			(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		}
		else // no longer used.,,.. absorbed by tid:  T_ID
		{ // udf_func()
			(yyval.pnode) = newAstNode(N_CALL, (yyloc));
			(yyval.pnode)->str = getT_ID_str((yyvsp[(1) - (3)].pnode));
		}
	;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 900 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
	;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 904 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		handle_tilde((yyval.pnode), (yyvsp[(3) - (4)].pnode), (yylsp[(3) - (4)]));
	;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 909 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 915 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		(yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[(3) - (7)].pnode);
		handle_tilde((yyval.pnode)->alt, (yyvsp[(6) - (7)].pnode), (yylsp[(6) - (7)]));
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt; // we need this; or tail is broken and can't put '.' tid at the end
	;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 923 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_TRANSPOSE, (yyloc));
 		(yyval.pnode)->child = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 928 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (5)].pnode);
	;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 933 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 939 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 945 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->str = (char*)malloc(8);
		strcpy((yyval.pnode)->str, "R");
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 953 "psycon.y"
    {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->str = (char*)malloc(8);
		strcpy((yyval.pnode)->str, "R");
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 961 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
	;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 965 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 974 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 979 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 984 "psycon.y"
    { //c=a(2)=44
		if (!(yyvsp[(1) - (3)].pnode)->child)
			(yyvsp[(1) - (3)].pnode)->child = (yyvsp[(3) - (3)].pnode);
		else
			for (AstNode *p = (yyvsp[(1) - (3)].pnode)->child; p; p=p->child)
			{
				if (!p->child)
				{
					p->child = (yyvsp[(3) - (3)].pnode);
					break;
				}
			}
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 999 "psycon.y"
    { //a(2)=d=11
		if (!(yyvsp[(1) - (3)].pnode)->child)
			(yyvsp[(1) - (3)].pnode)->child = (yyvsp[(3) - (3)].pnode);
		else
			for (AstNode *p = (yyvsp[(1) - (3)].pnode)->child; p; p=p->child)
			{
				if (!p->child)
				{
					p->child = (yyvsp[(3) - (3)].pnode);
					break;
				}
			}
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1014 "psycon.y"
    { // x={"bjk",noise(300), 4.5555}
		(yyval.pnode)->str = getT_ID_str((yyvsp[(1) - (3)].pnode));
		(yyval.pnode)->child = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1025 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NUMBER, (yyloc));
		(yyval.pnode)->dval = (yyvsp[(1) - (1)].dval);
	;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1030 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_STRING, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 1035 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ENDPOINT, (yyloc));
	;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 1039 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NEGATIVE, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 1044 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1050 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_SIGMA, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(3) - (8)]));
		(yyval.pnode)->child->str = getT_ID_str((yyvsp[(3) - (8)].pnode));
		(yyval.pnode)->child->child = (yyvsp[(5) - (8)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(7) - (8)].pnode);
	;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1058 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('+', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1060 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('-', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1062 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('*', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1064 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('/', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1066 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_MATRIXMULT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1068 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("^", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1070 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("mod", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 1072 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("respeed", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1074 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("pitchscale", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1076 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("timestretch", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1078 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("movespec", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1080 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('@', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1082 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_SHIFT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1084 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_CONCAT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;



/* Line 1455 of yacc.c  */
#line 4191 "psycon.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (pproot, errmsg, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (pproot, errmsg, yymsg);
	  }
	else
	  {
	    yyerror (pproot, errmsg, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, pproot, errmsg);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (pproot, errmsg, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, pproot, errmsg);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1101 "psycon.y"


/* Called by yyparse on error. */
void yyerror (AstNode **pproot, char **errmsg, char const *s)
{
  static size_t errmsg_len = 0;
#define ERRMSG_MAX 999
  char msgbuf[ERRMSG_MAX], *p;
  size_t msglen;

  sprintf(msgbuf, "Invalid syntax: Line %d, Col %d: %s.\n", yylloc.first_line, yylloc.first_column, s + (strncmp(s, "syntax error, ", 14) ? 0 : 14));
  if ((p=strstr(msgbuf, "$undefined"))) {
	sprintf(p, "'%c'(%d)", yychar, yychar);
    strcpy(p+strlen(p), p+10);
  }
  if ((p=strstr(msgbuf, "end of text or ")))
    strcpy(p, p+15);
  if ((p=strstr(msgbuf, " or ','")))
    strcpy(p, p+7);
  msglen = strlen(msgbuf);
  if (ErrorMsg == NULL)
    errmsg_len = 0;
  ErrorMsg = (char *)realloc(ErrorMsg, errmsg_len+msglen+1);
  strcpy(ErrorMsg+errmsg_len, msgbuf);
  errmsg_len += msglen;
  *errmsg = ErrorMsg;
}


int getTokenID(const char *str)
{
	size_t len, i;
	len = strlen(str);
	for (i = 0; i < YYNTOKENS; i++) {
		if (yytname[i] != 0
			&& yytname[i][0] == '"'
			&& !strncmp (yytname[i] + 1, str, len)
			&& yytname[i][len + 1] == '"'
			&& yytname[i][len + 2] == 0)
				break;
	}
	if (i < YYNTOKENS)
		return yytoknum[i];
	else
		return T_UNKNOWN;
}


void print_token_value(FILE *file, int type, YYSTYPE value)
{
	if (type == T_ID)
		fprintf (file, "%s", value.str);
	else if (type == T_NUMBER)
		fprintf (file, "%f", value.dval);
}


char *getAstNodeName(AstNode *p)
{
#define N_NAME_MAX 99
  static char buf[N_NAME_MAX];

  if (!p)
	return NULL;
  switch (p->type) {
  case '=':
    sprintf(buf, "[%s=]", p->str);
    break;
  case T_ID:
    sprintf(buf, "[%s]", p->str);
    break;
  case T_STRING:
    sprintf(buf, "\"%s\"", p->str);
    break;
  case N_CALL:
    sprintf(buf, "%s()", p->str);
    break;
  case N_CELL:
    sprintf(buf, "%s()", p->str);
    break;
  case T_NUMBER:
    sprintf(buf, "%.1f", p->dval);
    break;
  case N_BLOCK:
    sprintf(buf, "BLOCK");
    break;
  case N_ARGS:
    sprintf(buf, "ARGS");
    break;
  case N_MATRIX:
    sprintf(buf, "MATRIX");
    break;
  case N_VECTOR:
    sprintf(buf, "VECTOR");
    break;
  case N_IDLIST:
    sprintf(buf, "ID_LIST");
    break;
  case N_TIME_EXTRACT:
    sprintf(buf, "TIME_EXTRACT");
    break;
  case N_CELLASSIGN:
    sprintf(buf, "INITCELL");
    break;
  case N_SHELL:
    sprintf(buf, "SHELL");
    break;
  default:
    if (YYTRANSLATE(p->type) == 2)
      sprintf(buf, "[%d]", p->type);
    else
      sprintf(buf, "%s", yytname[YYTRANSLATE(p->type)]);
  }
  return buf;
}

/* As of 4/17/2018
In makeFunctionCall and makeBinaryOpNode,
node->tail is removed, because it caused conflict with tail made in
tid: tid '.' T_ID or tid: tid '(' arg_list ')'
or possibly other things.
The only downside from this change is, during debugging, the last argument is not seen at the top node where several nodes are cascaded: e.g., a+b+c
*/

AstNode *makeFunctionCall(const char *name, AstNode *first, AstNode *second, YYLTYPE loc)
{
	AstNode *node;

	node = newAstNode(T_ID, loc);
	node->str = (char*)calloc(1, strlen(name)+1);
	strcpy(node->str, name);
	node->tail = node->alt = newAstNode(N_ARGS, loc);
	node->alt->child = first;
	first->next = second;
	return node;
}

AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc)
{
	AstNode *node;

	node = newAstNode(op, loc);
	node->child = first;
	first->next = second;
	return node;
}

AstNode *newAstNode(int type, YYLTYPE loc)
{
#ifdef DEBUG
    static int cnt=0;
#endif
  AstNode *node;

  node = (AstNode *)malloc(sizeof(AstNode));
  if (node==NULL)
    exit(2);
  memset(node, 0, sizeof(AstNode));
  node->type = type;
#ifdef DEBUG
    printf("created node %d: %s\n", ++cnt, getAstNodeName(node));
#endif
  node->line = loc.first_line;
  node->col = loc.first_column;
  return node;
}

char *getT_ID_str(AstNode *p)
{
	if (p->type==T_ID)
		return p->str;
	printf("Must be T_ID\n");
	return NULL;
}

void handle_tilde(AstNode *proot, AstNode *pp, YYLTYPE loc)
{
	AstNode *p = pp->child;
	if (p->type==T_ID && !strcmp(p->str,"respeed"))
	{ // x{2}(t1~t2) checks here because t1~t2 can be arg_list through
        AstNode *q = newAstNode(N_TIME_EXTRACT, loc);
		q->child = p->alt->child;
		q->child->next = p->alt->child->next;
        if (proot->tail)
            proot->tail = proot->tail->alt = q;
		else
			proot->tail = proot->alt = q;
		p->alt->child = NULL;
		yydeleteAstNode(p, 1);
	}
	else
	{
	    if (proot->tail)
			proot->tail = proot->tail->alt = pp;
		else
			proot->tail = proot->alt = pp;
	}
}

int yydeleteAstNode(AstNode *p, int fSkipNext)
{
#ifdef DEBUG
    static int cnt=0;
#endif
  AstNode *tmp, *next;

  if (!p)
	return 0;
#ifdef DEBUG
    printf("deleting node %d: %s\n", ++cnt, getAstNodeName(p));
#endif
  if (p->str)
    free(p->str);
  if (p->child)
    yydeleteAstNode(p->child, 0);
  if (!fSkipNext && p->next) {
	for (tmp=p->next; tmp; tmp=next) {
      next = tmp->next;
      yydeleteAstNode(tmp, 1);
    }
  }
  free(p);
  return 0;
}

int yyPrintf(const char *msg, AstNode *p)
{
	if (p)
		printf("[%16s]token type: %d, %s, \n", msg, p->type, p->str);
	return 1;
}
