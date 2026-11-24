/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 15 "src/engine/psycon.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psycon.yacc.h"
#define YYPRINT(file, type, value) print_token_value (file, type, value)
/*#define DEBUG*/

char *ErrorMsg = NULL;
int yylex (void);
void yyerror (AstNode **pproot, char **errmsg, char const *s);


#line 85 "src/engine/psycon.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "psycon.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of text"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_UNKNOWN = 3,                  /* T_UNKNOWN  */
  YYSYMBOL_T_NEWLINE = 4,                  /* "end of line"  */
  YYSYMBOL_T_IF = 5,                       /* "if"  */
  YYSYMBOL_T_ELSE = 6,                     /* "else"  */
  YYSYMBOL_T_ELSEIF = 7,                   /* "elseif"  */
  YYSYMBOL_T_END = 8,                      /* "end"  */
  YYSYMBOL_T_WHILE = 9,                    /* "while"  */
  YYSYMBOL_T_FOR = 10,                     /* "for"  */
  YYSYMBOL_T_BREAK = 11,                   /* "break"  */
  YYSYMBOL_T_CONTINUE = 12,                /* "continue"  */
  YYSYMBOL_T_SWITCH = 13,                  /* "switch"  */
  YYSYMBOL_T_CASE = 14,                    /* "case"  */
  YYSYMBOL_T_OTHERWISE = 15,               /* "otherwise"  */
  YYSYMBOL_T_FUNCTION = 16,                /* "function"  */
  YYSYMBOL_T_STATIC = 17,                  /* "static"  */
  YYSYMBOL_T_RETURN = 18,                  /* "return"  */
  YYSYMBOL_T_SIGMA = 19,                   /* "sigma"  */
  YYSYMBOL_T_TRY = 20,                     /* "try"  */
  YYSYMBOL_T_CATCH = 21,                   /* "catch"  */
  YYSYMBOL_T_CATCHBACK = 22,               /* "catchback"  */
  YYSYMBOL_T_OP_SHIFT = 23,                /* ">>"  */
  YYSYMBOL_T_OP_CONCAT = 24,               /* "++"  */
  YYSYMBOL_T_LOGIC_EQ = 25,                /* "=="  */
  YYSYMBOL_T_LOGIC_NE = 26,                /* "!="  */
  YYSYMBOL_T_LOGIC_LE = 27,                /* "<="  */
  YYSYMBOL_T_LOGIC_GE = 28,                /* ">="  */
  YYSYMBOL_T_LOGIC_AND = 29,               /* "&&"  */
  YYSYMBOL_T_LOGIC_OR = 30,                /* "||"  */
  YYSYMBOL_T_REPLICA = 31,                 /* ".."  */
  YYSYMBOL_T_MATRIXMULT = 32,              /* "**"  */
  YYSYMBOL_T_NUMBER = 33,                  /* "number"  */
  YYSYMBOL_T_STRING = 34,                  /* "string"  */
  YYSYMBOL_T_ID = 35,                      /* "identifier"  */
  YYSYMBOL_T_ENDPOINT = 36,                /* T_ENDPOINT  */
  YYSYMBOL_T_FULLRANGE = 37,               /* T_FULLRANGE  */
  YYSYMBOL_38_ = 38,                       /* '='  */
  YYSYMBOL_39_ = 39,                       /* '\''  */
  YYSYMBOL_40_ = 40,                       /* '<'  */
  YYSYMBOL_41_ = 41,                       /* '>'  */
  YYSYMBOL_42_ = 42,                       /* ':'  */
  YYSYMBOL_43_ = 43,                       /* '~'  */
  YYSYMBOL_44_ = 44,                       /* '-'  */
  YYSYMBOL_45_ = 45,                       /* '+'  */
  YYSYMBOL_46_ = 46,                       /* "->"  */
  YYSYMBOL_47_ = 47,                       /* '%'  */
  YYSYMBOL_48_ = 48,                       /* '@'  */
  YYSYMBOL_49_ = 49,                       /* '#'  */
  YYSYMBOL_50_ = 50,                       /* '*'  */
  YYSYMBOL_51_ = 51,                       /* '/'  */
  YYSYMBOL_52_ = 52,                       /* '^'  */
  YYSYMBOL_T_LOGIC_NOT = 53,               /* T_LOGIC_NOT  */
  YYSYMBOL_T_POSITIVE = 54,                /* T_POSITIVE  */
  YYSYMBOL_T_NEGATIVE = 55,                /* T_NEGATIVE  */
  YYSYMBOL_T_TRANSPOSE = 56,               /* T_TRANSPOSE  */
  YYSYMBOL_T_ASYNC_ASSIGN = 57,            /* "<-"  */
  YYSYMBOL_58_ = 58,                       /* '|'  */
  YYSYMBOL_59_ = 59,                       /* ','  */
  YYSYMBOL_60_ = 60,                       /* ';'  */
  YYSYMBOL_61_ = 61,                       /* '('  */
  YYSYMBOL_62_ = 62,                       /* ')'  */
  YYSYMBOL_63_ = 63,                       /* '{'  */
  YYSYMBOL_64_ = 64,                       /* '}'  */
  YYSYMBOL_65_ = 65,                       /* '!'  */
  YYSYMBOL_66_ = 66,                       /* "+="  */
  YYSYMBOL_67_ = 67,                       /* "-="  */
  YYSYMBOL_68_ = 68,                       /* "*="  */
  YYSYMBOL_69_ = 69,                       /* "/="  */
  YYSYMBOL_70_ = 70,                       /* "@="  */
  YYSYMBOL_71_ = 71,                       /* "@@="  */
  YYSYMBOL_72_ = 72,                       /* ">>="  */
  YYSYMBOL_73_ = 73,                       /* "%="  */
  YYSYMBOL_74_ = 74,                       /* "->="  */
  YYSYMBOL_75_ = 75,                       /* "~="  */
  YYSYMBOL_76_ = 76,                       /* "<>="  */
  YYSYMBOL_77_ = 77,                       /* "#="  */
  YYSYMBOL_78_ = 78,                       /* "++="  */
  YYSYMBOL_79_ = 79,                       /* '.'  */
  YYSYMBOL_80_ = 80,                       /* '['  */
  YYSYMBOL_81_ = 81,                       /* ']'  */
  YYSYMBOL_82_ = 82,                       /* '$'  */
  YYSYMBOL_83_ = 83,                       /* "<>"  */
  YYSYMBOL_YYACCEPT = 84,                  /* $accept  */
  YYSYMBOL_input = 85,                     /* input  */
  YYSYMBOL_block_func = 86,                /* block_func  */
  YYSYMBOL_block = 87,                     /* block  */
  YYSYMBOL_line = 88,                      /* line  */
  YYSYMBOL_shellarg = 89,                  /* shellarg  */
  YYSYMBOL_shell = 90,                     /* shell  */
  YYSYMBOL_debug = 91,                     /* debug  */
  YYSYMBOL_auxsys = 92,                    /* auxsys  */
  YYSYMBOL_line_func = 93,                 /* line_func  */
  YYSYMBOL_eol = 94,                       /* eol  */
  YYSYMBOL_eol2 = 95,                      /* eol2  */
  YYSYMBOL_func_end = 96,                  /* func_end  */
  YYSYMBOL_func_decl = 97,                 /* func_decl  */
  YYSYMBOL_funcdef = 98,                   /* funcdef  */
  YYSYMBOL_case_list = 99,                 /* case_list  */
  YYSYMBOL_stmt = 100,                     /* stmt  */
  YYSYMBOL_conditional = 101,              /* conditional  */
  YYSYMBOL_elseif_list = 102,              /* elseif_list  */
  YYSYMBOL_expcondition = 103,             /* expcondition  */
  YYSYMBOL_csig = 104,                     /* csig  */
  YYSYMBOL_initcell = 105,                 /* initcell  */
  YYSYMBOL_condition = 106,                /* condition  */
  YYSYMBOL_id_list = 107,                  /* id_list  */
  YYSYMBOL_arg = 108,                      /* arg  */
  YYSYMBOL_arg_list = 109,                 /* arg_list  */
  YYSYMBOL_matrix = 110,                   /* matrix  */
  YYSYMBOL_vector = 111,                   /* vector  */
  YYSYMBOL_range = 112,                    /* range  */
  YYSYMBOL_exp_range = 113,                /* exp_range  */
  YYSYMBOL_compop = 114,                   /* compop  */
  YYSYMBOL_assign2this = 115,              /* assign2this  */
  YYSYMBOL_varblock = 116,                 /* varblock  */
  YYSYMBOL_tid = 117,                      /* tid  */
  YYSYMBOL_assign = 118,                   /* assign  */
  YYSYMBOL_exp = 119                       /* exp  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 120 "src/engine/psycon.y"

AstNode *newAstNode(int type, YYLTYPE loc);
AstNode *makeFunctionCall(const char *name, AstNode *first, AstNode *second, YYLTYPE loc);
AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc);
void print_token_value(FILE *file, int type, YYSTYPE value);
char *getT_ID_str(AstNode *p);
void handle_tilde(AstNode *proot, AstNode *pp, YYLTYPE loc);
int consumeNumberUnitMask(int line, int col);

#line 248 "src/engine/psycon.tab.c"


#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  93
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2589

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  162
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  300

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   312


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    65,     2,    49,    82,    47,     2,    39,
      61,    62,    50,    45,    59,    44,    79,    51,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    60,
      40,    38,    41,     2,    48,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    80,     2,    81,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,    58,    64,    43,     2,     2,     2,
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
      35,    36,    37,    46,    53,    54,    55,    56,    57,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    83
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   147,   147,   148,   152,   153,   174,   181,   203,   205,
     210,   214,   218,   222,   223,   228,   237,   242,   244,   249,
     257,   266,   271,   281,   288,   293,   304,   305,   308,   308,
     308,   311,   314,   315,   318,   323,   330,   337,   355,   362,
     383,   384,   386,   401,   418,   419,   420,   421,   447,   455,
     466,   475,   484,   496,   510,   524,   528,   530,   535,   535,
     535,   535,   539,   542,   555,   576,   579,   582,   591,   597,
     599,   601,   603,   605,   607,   609,   614,   616,   621,   624,
     630,   638,   640,   641,   644,   649,   660,   670,   677,   693,
     696,   708,   714,   722,   726,   733,   733,   733,   736,   740,
     742,   744,   746,   748,   753,   755,   757,   764,   771,   778,
     787,   791,   800,   809,   829,   834,   853,   863,   867,   876,
     877,   883,   888,   900,   904,   909,   915,   923,   928,   933,
     939,   945,   953,   961,   965,   974,   979,   984,   991,  1006,
    1021,  1030,  1031,  1032,  1038,  1043,  1047,  1052,  1058,  1066,
    1068,  1070,  1072,  1074,  1076,  1078,  1080,  1082,  1084,  1086,
    1088,  1090,  1092
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of text\"", "error", "\"invalid token\"", "T_UNKNOWN",
  "\"end of line\"", "\"if\"", "\"else\"", "\"elseif\"", "\"end\"",
  "\"while\"", "\"for\"", "\"break\"", "\"continue\"", "\"switch\"",
  "\"case\"", "\"otherwise\"", "\"function\"", "\"static\"", "\"return\"",
  "\"sigma\"", "\"try\"", "\"catch\"", "\"catchback\"", "\">>\"", "\"++\"",
  "\"==\"", "\"!=\"", "\"<=\"", "\">=\"", "\"&&\"", "\"||\"", "\"..\"",
  "\"**\"", "\"number\"", "\"string\"", "\"identifier\"", "T_ENDPOINT",
  "T_FULLRANGE", "'='", "'\\''", "'<'", "'>'", "':'", "'~'", "'-'", "'+'",
  "\"->\"", "'%'", "'@'", "'#'", "'*'", "'/'", "'^'", "T_LOGIC_NOT",
  "T_POSITIVE", "T_NEGATIVE", "T_TRANSPOSE", "\"<-\"", "'|'", "','", "';'",
  "'('", "')'", "'{'", "'}'", "'!'", "\"+=\"", "\"-=\"", "\"*=\"",
  "\"/=\"", "\"@=\"", "\"@@=\"", "\">>=\"", "\"%=\"", "\"->=\"", "\"~=\"",
  "\"<>=\"", "\"#=\"", "\"++=\"", "'.'", "'['", "']'", "'$'", "\"<>\"",
  "$accept", "input", "block_func", "block", "line", "shellarg", "shell",
  "debug", "auxsys", "line_func", "eol", "eol2", "func_end", "func_decl",
  "funcdef", "case_list", "stmt", "conditional", "elseif_list",
  "expcondition", "csig", "initcell", "condition", "id_list", "arg",
  "arg_list", "matrix", "vector", "range", "exp_range", "compop",
  "assign2this", "varblock", "tid", "assign", "exp", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-211)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-141)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     871,    33,  -211,  2213,  2213,   -15,  -211,  -211,   748,  -211,
      30,  -211,   -12,  1959,    78,  -211,  -211,    46,  -211,    77,
     748,   748,    77,    77,  2213,  2103,  2213,  2213,   -29,    59,
     945,  -211,  -211,   158,  -211,     7,   203,  -211,    10,  -211,
    -211,  -211,  2511,  2469,  -211,  2372,  -211,    22,  1959,   203,
    -211,     5,   120,   -27,   818,  1959,    24,  2213,   154,  -211,
     -29,  1439,  -211,  2103,   748,  2213,  2103,  -211,  -211,    15,
      58,  -211,    85,   -27,    45,    45,  -211,    85,  -211,    27,
      73,    98,  -211,    92,  -211,   -19,   215,  -211,   -52,   549,
     215,   120,   -27,  -211,  -211,  1084,    52,  -211,  -211,  -211,
    -211,  -211,  -211,  2213,  2213,  2246,   668,   748,  -211,  -211,
    -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,
    2213,  2213,  -211,  2246,  -211,   112,   137,  -211,   748,   748,
     748,   748,   748,   748,   748,   748,   748,   748,   748,   748,
     748,   748,   748,   748,   748,   748,   748,   748,   748,  1149,
    -211,  -211,  1569,  2213,  -211,   122,   -21,   152,   153,  -211,
      55,   626,   215,   107,  -211,  -211,  -211,  -211,  -211,  -211,
    -211,  -211,  2103,  -211,  2213,   127,  2213,   134,   215,  2136,
    1019,   181,  -211,   189,    21,   215,  2491,  -211,  -211,   109,
    1237,   231,  2328,   215,  -211,   187,  -211,    -8,    50,  2443,
    2443,  2443,  2443,   -26,  2443,  2443,  1318,  2473,    50,    50,
      -8,    -8,    -8,    -8,   -26,   -26,   -26,  2443,  2213,   147,
    -211,  1214,  -211,  2301,  1959,  2213,  1959,  1959,  -211,   121,
    -211,  -211,  2158,  2213,   215,  2191,   102,   111,   117,  -211,
    -211,  2024,  -211,  -211,  -211,   748,  1959,  1959,  -211,  1959,
    1634,  2103,   441,  1699,   163,  1764,  1829,  2103,   -13,  2048,
    -211,   -10,  2081,   190,  1959,  2268,  1019,  2443,  1149,  1504,
    1894,  -211,    67,  1959,  -211,   748,  -211,  -211,   149,  -211,
    -211,  -211,  -211,  -211,  1019,   150,  -211,   228,  -211,   232,
    1294,  2402,  -211,  -211,  1959,  1959,  -211,  1019,  1374,  -211
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     8,     0,     0,     0,    56,    57,     0,    34,
       0,    55,     0,     0,   123,   143,   144,   114,   145,     0,
       0,     0,     0,     0,     0,     0,     0,    86,     0,     0,
       0,    26,     4,     0,    27,     0,    44,    65,   141,    97,
      96,    66,   119,   142,    45,    95,     9,   114,     0,     0,
     141,    58,   119,   142,    60,     0,     0,     0,    40,    35,
       0,     0,     6,     0,     0,     0,     0,    18,    20,    16,
       0,    24,     0,    17,   146,   147,    21,     0,    23,     0,
      97,    66,    81,   141,    84,     0,    82,    75,     0,    87,
      90,   118,     0,     1,     5,     0,   119,    30,    29,    28,
      31,    13,    14,     0,     0,     0,     0,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
       0,     0,   136,     0,   127,     0,     0,   135,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      59,    61,     0,     0,    41,     0,     0,     0,     0,     7,
       0,     0,   137,     0,    19,    25,    12,    22,    10,    11,
      68,   134,     0,    67,    89,   133,     0,   117,    91,    78,
       0,     0,    76,    77,   141,   110,   142,   138,   122,     0,
       0,   111,   112,   113,   139,     0,   115,   161,   162,    71,
      72,    74,    73,   153,    69,    70,    93,   156,   150,   149,
     159,   155,   160,   157,   151,   152,   154,   158,     0,     0,
      52,     0,    48,     0,     0,     0,     0,     0,   124,   125,
     120,    85,    88,    86,    92,    86,   114,     0,    66,    33,
      36,     0,   121,   116,    15,     0,     0,     0,    47,     0,
       0,     0,     0,     0,    66,     0,     0,     0,     0,    87,
     128,     0,    87,     0,     0,    78,     0,    94,     0,     0,
       0,    53,     0,     0,    49,     0,    50,    51,     0,   132,
     131,   130,   129,    80,     0,     0,    37,    63,    54,    67,
       0,     0,   126,    38,     0,     0,   148,     0,     0,    39
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -211,  -211,  -211,   349,   213,    16,  -211,  -211,  -211,   209,
      71,  -211,  -210,  -211,  -211,  -211,  -211,    -3,   -20,   562,
    -211,    79,    12,   -23,    80,   -22,   -39,  -170,  -211,   575,
    -211,   212,   162,     0,     1,   416
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    29,    30,    61,    62,    71,    77,    79,    72,    32,
     101,   102,   240,    33,    34,   155,    35,    48,   219,    36,
      37,    50,    39,   237,    84,   163,    88,    89,    40,    41,
     121,   127,    52,    53,    44,    45
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      43,    55,    14,    85,   232,    97,    47,    97,   174,    98,
     -46,    98,   124,    43,   -46,    51,    51,   225,   124,    73,
      56,  -140,    73,    73,   134,  -140,   147,    97,    92,   175,
      43,    98,    57,    92,   -97,   -97,    80,    46,    76,    78,
     172,   160,   145,   146,   147,   173,    59,   174,    43,    60,
     174,    27,   126,    28,  -114,    43,   286,   148,   126,    93,
     156,    43,   153,   259,    99,   262,    99,   100,   279,   -46,
     -46,   281,    73,   128,   293,   148,    66,    73,  -114,    38,
    -140,  -140,   134,    66,   189,    97,    99,   299,   165,    98,
     181,   164,    38,   167,  -114,    43,   141,   142,   143,   144,
     145,   146,   147,    65,    83,   186,   187,    66,    14,    38,
      67,    68,    69,   106,   172,   107,    14,   228,    67,    68,
      69,    70,   150,   186,   194,   151,   172,    38,   148,    70,
     222,   289,   191,   148,    38,   170,   223,   224,    57,    63,
      38,    64,    83,   166,    99,    83,    57,   195,   168,    43,
     169,   -83,    43,   247,   -83,   248,   -83,    27,   154,    28,
     171,   -79,    42,    66,   -79,    27,   172,    28,   172,   230,
     263,   242,   196,   264,    38,    42,   -82,   128,   129,   171,
      43,   106,   257,   107,   184,    83,   134,   226,   227,    14,
      91,    80,    42,    95,   258,    96,   261,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   233,   172,   263,
      42,   292,   294,    31,   235,   246,   241,    42,   103,    57,
     244,    43,   275,    42,    43,   283,    43,    43,    38,   272,
      51,    38,   103,   104,   247,   278,   295,   148,    27,    94,
      28,    43,   285,    31,   -66,   -66,    43,    43,   287,    43,
      43,    83,   231,    43,   122,    43,    43,    42,    83,    38,
     -97,   -97,     0,     0,    43,     0,    43,    42,    43,    43,
      43,     0,     0,    43,   159,     0,     0,    80,     0,     0,
       0,     0,     0,     0,    43,    42,     0,     0,     0,     0,
      43,     0,     0,     0,    43,    43,     0,    43,    43,     0,
      38,     0,     0,    38,     0,    38,    38,     0,     0,     0,
       0,    42,     0,     0,    42,     0,     0,     0,     0,     0,
      38,     0,     0,     0,     0,    38,    38,     0,    38,    38,
      83,     0,    38,     0,    38,    38,    83,     0,     0,     0,
       0,     0,    42,    38,     0,    38,     0,    38,    38,    38,
       0,     0,    38,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   159,    38,     0,   159,     0,     0,     0,    38,
       0,     0,     0,    38,    38,     0,    38,    38,     0,     0,
       0,     0,     0,    42,     0,     0,    42,     0,    42,    42,
       0,     0,     0,   159,     0,     0,     0,   149,     0,     0,
       0,     0,     0,    42,   152,     0,     0,     0,    42,    42,
       0,    42,    42,     0,     0,    42,     0,    42,    42,    54,
      54,     0,     0,     0,    58,     0,    42,     0,    42,     0,
      42,    42,    42,     0,     0,    42,    74,    75,     0,     0,
       0,     0,     0,     0,   180,   273,    42,     0,     0,     0,
       0,     0,    42,     0,     0,     0,    42,    42,     0,    42,
      42,     0,     0,   159,   128,   129,   159,     0,   159,   159,
       0,     0,     0,   134,     0,     0,     0,     0,     0,   159,
     161,   159,   159,   159,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,     0,     0,     0,   159,     0,     0,
       0,     0,     0,   159,     0,     0,     0,     0,     0,     0,
     159,   159,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   190,   148,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   192,     0,     0,     0,
       0,     0,     0,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,    49,    49,     0,    12,     0,
     250,     0,     0,   253,     0,   255,   256,     0,     0,     0,
      14,     0,    15,    16,    47,    18,    49,    49,    87,    49,
     266,     0,     0,    20,    21,   268,   269,     0,   270,    81,
      86,     0,    90,     0,     0,     0,     0,     0,   176,     0,
      24,     0,    25,   284,    26,     0,     0,     0,     0,    49,
       0,     0,   290,     0,     0,    49,     0,    49,    49,    27,
     177,    28,    81,     0,    54,     0,     0,     0,    86,   252,
     162,    86,     0,   297,   298,     0,     0,     0,     0,   128,
     129,    49,     0,     0,     0,     0,     0,     0,   134,     0,
       0,   267,     0,     0,   178,   182,   183,    49,    49,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,     0,
     185,    86,    49,    49,     0,    49,     0,    12,     0,     0,
     229,   291,     0,     0,     0,     0,   193,     0,   185,    14,
       0,    15,    16,    47,    18,     0,     0,     0,     0,   148,
      82,     0,    20,    21,     0,    49,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   221,    24,
     188,    25,     0,    26,    49,     0,    49,     0,    49,     0,
       0,    49,     0,     0,     0,     0,     0,    86,    27,    90,
      28,   234,     0,     0,   238,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    12,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
      49,    15,    16,    47,    18,     0,     0,    49,     0,     0,
       0,     0,    20,    21,    49,    49,     0,    49,     0,     0,
     254,     0,     0,     0,     0,     0,     0,   178,    90,    57,
      90,    25,     0,    49,     0,     0,     0,     0,    97,    49,
       0,    49,    98,     0,    49,     0,    86,    49,    27,     0,
      28,     0,    86,     0,   178,     0,     0,   178,     0,     0,
      81,   128,   129,   130,   131,   132,   133,   -95,   -95,     0,
     134,     0,     0,     0,     0,     0,     0,     0,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,    -2,     1,     0,     0,     2,     3,    99,     0,     0,
       4,     5,     6,     7,     8,     0,     0,     9,    10,    11,
      12,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   148,    14,     0,    15,    16,    17,    18,     0,     0,
       0,     0,    19,     0,     0,    20,    21,     0,     0,     0,
      22,     0,    23,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,     0,    26,     0,     0,     0,
       0,     0,     0,     0,     0,    -3,     1,     0,     0,     2,
       3,    27,     0,    28,     4,     5,     6,     7,     8,     0,
       0,     9,    10,    11,    12,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,    15,    16,
      17,    18,     0,     0,     0,     0,    19,     0,     0,    20,
      21,     0,     0,     0,    22,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    25,     0,
      26,     0,     0,     0,     0,     0,     0,     0,     0,   239,
       1,     0,     0,     2,     3,    27,     0,    28,     4,     5,
       6,     7,     8,     0,     0,   -32,   -32,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,     0,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,  -114,  -114,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   179,     0,    25,     0,    26,
       1,     0,     0,     2,     3,   -62,   218,   -62,     4,     5,
       6,     7,     8,  -114,    27,     0,    28,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,     0,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,   -66,   -66,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
     128,   129,     0,    22,     0,    23,     0,     0,     0,   134,
       0,     0,     0,   249,     0,    24,     0,    25,     0,    26,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
       0,     0,     0,     0,    27,     1,    28,     0,     2,     3,
       0,   243,   -42,     4,     5,     6,     7,     8,   -42,   -42,
       0,     0,    11,    12,    13,     0,     0,     0,     0,     0,
     148,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,   128,   129,    22,     0,    23,     0,     0,     0,     0,
     134,     0,     0,     0,     0,    24,     0,    25,     0,    26,
     245,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,     0,     0,     0,    27,     1,    28,     0,     2,     3,
       0,     0,   -43,     4,     5,     6,     7,     8,   -43,   -43,
       0,     0,    11,    12,    13,     0,     0,     0,     0,     0,
       0,   148,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    25,     0,    26,
       1,     0,     0,     2,     3,     0,     0,     0,     4,     5,
       6,     7,     8,     0,    27,     0,    28,    11,    12,    13,
     157,   158,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
     -64,     0,   -64,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    25,     0,    26,
       1,     0,     0,     2,     3,     0,     0,   220,     4,     5,
       6,     7,     8,     0,    27,     0,    28,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,   271,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    25,     0,    26,
       1,     0,     0,     2,     3,     0,     0,   274,     4,     5,
       6,     7,     8,     0,    27,     0,    28,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,   276,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    25,     0,    26,
       1,     0,     0,     2,     3,     0,     0,   277,     4,     5,
       6,     7,     8,     0,    27,     0,    28,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,   288,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,     0,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    25,     0,    26,
       1,     0,     0,     2,     3,     0,     0,     0,     4,     5,
       6,     7,     8,     0,    27,     0,    28,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,     0,     0,     0,     0,
      19,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,     0,    26,     1,     0,     0,     2,     3,
       0,     0,     0,     4,     5,     6,     7,     8,     0,    27,
       0,    28,    11,    12,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,    15,    16,    17,
      18,     0,     0,     0,     0,    19,     0,    12,    20,    21,
       0,     0,     0,    22,     0,    23,     0,     0,     0,    14,
       0,    15,    16,    47,    18,   265,     0,    25,     0,    26,
       0,     0,    20,    21,     0,     0,     0,     0,     0,     0,
      12,     0,     0,     0,    27,     0,    28,   176,     0,    24,
       0,    25,    14,    26,    15,    16,    47,    18,     0,     0,
       0,     0,    12,     0,     0,    20,    21,     0,    27,   280,
      28,     0,     0,     0,    14,     0,    15,    16,    47,    18,
     176,     0,    24,     0,    25,    82,    26,    20,    21,     0,
       0,     0,     0,     0,     0,    12,     0,     0,     0,     0,
       0,    27,   282,    28,    24,     0,    25,    14,    26,    15,
      16,   236,    18,     0,     0,     0,     0,    12,    82,     0,
      20,    21,     0,    27,     0,    28,     0,     0,     0,    14,
       0,    15,    16,    47,    18,     0,     0,    24,     0,    25,
       0,    26,    20,    21,     0,     0,     0,     0,     0,     0,
      12,     0,     0,     0,     0,     0,    27,   176,    28,    24,
       0,    25,    14,    26,    15,    16,    47,    18,     0,     0,
       0,     0,    12,     0,     0,    20,    21,     0,    27,     0,
      28,     0,     0,     0,    14,     0,    15,    16,    47,    18,
       0,     0,    24,     0,    25,     0,    26,    20,    21,     0,
       0,     0,     0,     0,     0,    12,     0,     0,     0,     0,
       0,    27,   260,    28,    24,     0,    25,    14,    26,    15,
      16,    17,    18,     0,     0,     0,     0,    12,     0,     0,
      20,    21,     0,    27,     0,    28,     0,     0,     0,    14,
       0,    15,    16,   236,    18,     0,     0,    24,     0,    25,
       0,    26,    20,    21,     0,     0,     0,     0,     0,     0,
      12,     0,     0,     0,     0,     0,    27,     0,    28,    24,
       0,    25,    14,    26,    15,    16,    47,    18,     0,     0,
       0,     0,     0,     0,     0,    20,    21,     0,    27,     0,
      28,   128,   129,   130,   131,   132,   133,   -95,   -95,     0,
     134,     0,    57,     0,   251,     0,     0,     0,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,    27,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,   129,   130,   131,   132,
     133,     0,     0,     0,   134,     0,     0,     0,     0,     0,
       0,   148,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   128,   129,     0,     0,     0,
       0,     0,     0,     0,   134,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,     0,     0,     0,     0,
       0,     0,     0,     0,   296,     0,   128,   129,     0,     0,
       0,     0,     0,     0,     0,   134,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   148,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   128,   129,     0,     0,
       0,     0,     0,     0,     0,   134,     0,   123,   124,     0,
       0,     0,     0,     0,     0,     0,     0,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   125,     0,   123,
     124,     0,     0,     0,     0,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   126,   105,
       0,     0,     0,     0,     0,     0,   148,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     126,     0,   106,     0,   107,     0,     0,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120
};

static const yytype_int16 yycheck[] =
{
       0,     4,    31,    25,   174,     0,    35,     0,    60,     4,
       0,     4,    39,    13,     4,     3,     4,    38,    39,    19,
      35,     0,    22,    23,    32,     4,    52,     0,    28,    81,
      30,     4,    61,    33,    29,    30,    24,     4,    22,    23,
      59,    63,    50,    51,    52,    64,    16,    60,    48,    61,
      60,    80,    79,    82,    39,    55,   266,    83,    79,     0,
      60,    61,    38,   233,    59,   235,    59,    60,    81,    59,
      60,    81,    72,    23,   284,    83,    61,    77,    63,     0,
      59,    60,    32,    61,   106,     0,    59,   297,    72,     4,
      38,    33,    13,    77,    79,    95,    46,    47,    48,    49,
      50,    51,    52,    57,    25,   105,   105,    61,    31,    30,
      33,    34,    35,    61,    59,    63,    31,    62,    33,    34,
      35,    44,    51,   123,   123,    54,    59,    48,    83,    44,
       8,    64,   120,    83,    55,    62,    14,    15,    61,    61,
      61,    63,    63,    72,    59,    66,    61,    35,    77,   149,
      79,    59,   152,     6,    62,     8,    64,    80,     4,    82,
      62,    59,     0,    61,    62,    80,    59,    82,    59,    62,
      59,    62,    35,    62,    95,    13,    59,    23,    24,    62,
     180,    61,    61,    63,   105,   106,    32,    35,    35,    31,
      28,   179,    30,    35,   233,    33,   235,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    80,    59,    59,
      48,    62,    62,     0,    80,   218,    35,    55,    29,    61,
      33,   221,    59,    61,   224,    35,   226,   227,   149,   251,
     218,   152,    29,    30,     6,   257,     4,    83,    80,    30,
      82,   241,   265,    30,    29,    30,   246,   247,   268,   249,
     250,   172,   172,   253,    42,   255,   256,    95,   179,   180,
      29,    30,    -1,    -1,   264,    -1,   266,   105,   268,   269,
     270,    -1,    -1,   273,    61,    -1,    -1,   265,    -1,    -1,
      -1,    -1,    -1,    -1,   284,   123,    -1,    -1,    -1,    -1,
     290,    -1,    -1,    -1,   294,   295,    -1,   297,   298,    -1,
     221,    -1,    -1,   224,    -1,   226,   227,    -1,    -1,    -1,
      -1,   149,    -1,    -1,   152,    -1,    -1,    -1,    -1,    -1,
     241,    -1,    -1,    -1,    -1,   246,   247,    -1,   249,   250,
     251,    -1,   253,    -1,   255,   256,   257,    -1,    -1,    -1,
      -1,    -1,   180,   264,    -1,   266,    -1,   268,   269,   270,
      -1,    -1,   273,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   149,   284,    -1,   152,    -1,    -1,    -1,   290,
      -1,    -1,    -1,   294,   295,    -1,   297,   298,    -1,    -1,
      -1,    -1,    -1,   221,    -1,    -1,   224,    -1,   226,   227,
      -1,    -1,    -1,   180,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    -1,   241,    55,    -1,    -1,    -1,   246,   247,
      -1,   249,   250,    -1,    -1,   253,    -1,   255,   256,     3,
       4,    -1,    -1,    -1,     8,    -1,   264,    -1,   266,    -1,
     268,   269,   270,    -1,    -1,   273,    20,    21,    -1,    -1,
      -1,    -1,    -1,    -1,    95,     4,   284,    -1,    -1,    -1,
      -1,    -1,   290,    -1,    -1,    -1,   294,   295,    -1,   297,
     298,    -1,    -1,   250,    23,    24,   253,    -1,   255,   256,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,   266,
      64,   268,   269,   270,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    -1,    -1,   284,    -1,    -1,
      -1,    -1,    -1,   290,    -1,    -1,    -1,    -1,    -1,    -1,
     297,   298,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,     3,     4,    -1,    19,    -1,
     221,    -1,    -1,   224,    -1,   226,   227,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    24,    25,    26,    27,
     241,    -1,    -1,    44,    45,   246,   247,    -1,   249,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    -1,    59,    -1,
      61,    -1,    63,   264,    65,    -1,    -1,    -1,    -1,    57,
      -1,    -1,   273,    -1,    -1,    63,    -1,    65,    66,    80,
      81,    82,    57,    -1,   218,    -1,    -1,    -1,    63,   223,
      65,    66,    -1,   294,   295,    -1,    -1,    -1,    -1,    23,
      24,    89,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      -1,   245,    -1,    -1,    89,   103,   104,   105,   106,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
     105,   106,   120,   121,    -1,   123,    -1,    19,    -1,    -1,
      64,   275,    -1,    -1,    -1,    -1,   121,    -1,   123,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,    83,
      42,    -1,    44,    45,    -1,   153,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   153,    61,
      62,    63,    -1,    65,   172,    -1,   174,    -1,   176,    -1,
      -1,   179,    -1,    -1,    -1,    -1,    -1,   172,    80,   174,
      82,   176,    -1,    -1,   179,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
     218,    33,    34,    35,    36,    -1,    -1,   225,    -1,    -1,
      -1,    -1,    44,    45,   232,   233,    -1,   235,    -1,    -1,
     225,    -1,    -1,    -1,    -1,    -1,    -1,   232,   233,    61,
     235,    63,    -1,   251,    -1,    -1,    -1,    -1,     0,   257,
      -1,   259,     4,    -1,   262,    -1,   251,   265,    80,    -1,
      82,    -1,   257,    -1,   259,    -1,    -1,   262,    -1,    -1,
     265,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,     0,     1,    -1,    -1,     4,     5,    59,    -1,    -1,
       9,    10,    11,    12,    13,    -1,    -1,    16,    17,    18,
      19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    31,    -1,    33,    34,    35,    36,    -1,    -1,
      -1,    -1,    41,    -1,    -1,    44,    45,    -1,    -1,    -1,
      49,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    63,    -1,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     0,     1,    -1,    -1,     4,
       5,    80,    -1,    82,     9,    10,    11,    12,    13,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,
      45,    -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,
       1,    -1,    -1,     4,     5,    80,    -1,    82,     9,    10,
      11,    12,    13,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
      -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    38,    39,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
       1,    -1,    -1,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    79,    80,    -1,    82,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
      -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      23,    24,    -1,    49,    -1,    51,    -1,    -1,    -1,    32,
      -1,    -1,    -1,    59,    -1,    61,    -1,    63,    -1,    65,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    -1,    -1,    -1,    80,     1,    82,    -1,     4,     5,
      -1,    64,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    -1,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      83,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    23,    24,    49,    -1,    51,    -1,    -1,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    -1,    -1,    80,     1,    82,    -1,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    -1,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
       1,    -1,    -1,     4,     5,    -1,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    80,    -1,    82,    18,    19,    20,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
       6,    -1,     8,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
       1,    -1,    -1,     4,     5,    -1,    -1,     8,     9,    10,
      11,    12,    13,    -1,    80,    -1,    82,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
       1,    -1,    -1,     4,     5,    -1,    -1,     8,     9,    10,
      11,    12,    13,    -1,    80,    -1,    82,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
       1,    -1,    -1,     4,     5,    -1,    -1,     8,     9,    10,
      11,    12,    13,    -1,    80,    -1,    82,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    -1,    65,
       1,    -1,    -1,     4,     5,    -1,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    80,    -1,    82,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    44,    45,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    -1,    65,     1,    -1,    -1,     4,     5,
      -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,    80,
      -1,    82,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    -1,    -1,    -1,    -1,    41,    -1,    19,    44,    45,
      -1,    -1,    -1,    49,    -1,    51,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    61,    -1,    63,    -1,    65,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    80,    -1,    82,    59,    -1,    61,
      -1,    63,    31,    65,    33,    34,    35,    36,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    44,    45,    -1,    80,    81,
      82,    -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,
      59,    -1,    61,    -1,    63,    42,    65,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    80,    81,    82,    61,    -1,    63,    31,    65,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    19,    42,    -1,
      44,    45,    -1,    80,    -1,    82,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    61,    -1,    63,
      -1,    65,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    -1,    80,    59,    82,    61,
      -1,    63,    31,    65,    33,    34,    35,    36,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    44,    45,    -1,    80,    -1,
      82,    -1,    -1,    -1,    31,    -1,    33,    34,    35,    36,
      -1,    -1,    61,    -1,    63,    -1,    65,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    80,    81,    82,    61,    -1,    63,    31,    65,    33,
      34,    35,    36,    -1,    -1,    -1,    -1,    19,    -1,    -1,
      44,    45,    -1,    80,    -1,    82,    -1,    -1,    -1,    31,
      -1,    33,    34,    35,    36,    -1,    -1,    61,    -1,    63,
      -1,    65,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    -1,    80,    -1,    82,    61,
      -1,    63,    31,    65,    33,    34,    35,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    -1,    80,    -1,
      82,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      32,    -1,    61,    -1,    63,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    80,    -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    23,    24,    25,    26,    27,
      28,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    23,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    62,    -1,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    38,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    83,    58,    -1,    38,
      39,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,    61,    -1,    63,    -1,    -1,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     4,     5,     9,    10,    11,    12,    13,    16,
      17,    18,    19,    20,    31,    33,    34,    35,    36,    41,
      44,    45,    49,    51,    61,    63,    65,    80,    82,    85,
      86,    88,    93,    97,    98,   100,   103,   104,   105,   106,
     112,   113,   116,   117,   118,   119,     4,    35,   101,   103,
     105,   106,   116,   117,   119,   101,    35,    61,   119,    16,
      61,    87,    88,    61,    63,    57,    61,    33,    34,    35,
      44,    89,    92,   117,   119,   119,    89,    90,    89,    91,
     106,   113,    42,   105,   108,   109,   113,   103,   110,   111,
     113,   116,   117,     0,    93,    35,   116,     0,     4,    59,
      60,    94,    95,    29,    30,    38,    61,    63,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,   114,   115,    38,    39,    58,    79,   115,    23,    24,
      25,    26,    27,    28,    32,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    83,    87,
      94,    94,    87,    38,     4,    99,   117,    21,    22,    88,
     109,   119,   113,   109,    33,    89,    94,    89,    94,    94,
      62,    62,    59,    64,    60,    81,    59,    81,   113,    61,
      87,    38,   103,   103,   105,   113,   117,   118,    62,   109,
     119,   106,   119,   113,   118,    35,    35,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,     7,   102,
       8,   113,     8,    14,    15,    38,    35,    35,    62,    64,
      62,   108,   111,    80,   113,    80,    35,   107,   113,     0,
      96,    35,    62,    64,    33,    42,   101,     6,     8,    59,
      87,    63,   119,    87,   113,    87,    87,    61,   110,   111,
      81,   110,   111,    59,    62,    61,    87,   119,    87,    87,
      87,     8,   109,     4,     8,    59,     8,     8,   109,    81,
      81,    81,    81,    35,    87,   107,    96,   102,     8,    64,
      87,   119,    62,    96,    62,     4,    62,    87,    87,    96
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    84,    85,    85,    86,    86,    87,    87,    88,    88,
      88,    88,    88,    88,    88,    88,    89,    89,    89,    89,
      89,    90,    90,    91,    92,    92,    93,    93,    94,    94,
      94,    95,    96,    96,    97,    97,    98,    98,    98,    98,
      99,    99,    99,    99,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   101,   101,
     101,   101,   102,   102,   102,   103,   104,   105,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   107,   107,
     107,   108,   108,   108,   109,   109,   110,   110,   110,   110,
     111,   111,   111,   112,   112,   113,   113,   113,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     115,   115,   115,   115,   116,   116,   116,   116,   116,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   118,   118,   118,   118,   118,
     118,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
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
       4,     4,     3,     1,     4,     4,     7,     2,     5,     6,
       6,     6,     6,     3,     3,     2,     2,     3,     3,     3,
       3,     1,     1,     1,     1,     1,     2,     2,     8,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (pproot, errmsg, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, pproot, errmsg); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (pproot);
  YY_USE (errmsg);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, pproot, errmsg);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, AstNode **pproot, char **errmsg)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), pproot, errmsg);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, pproot, errmsg); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, AstNode **pproot, char **errmsg)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (pproot);
  YY_USE (errmsg);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_T_STRING: /* "string"  */
#line 113 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", ((*yyvaluep).str));
#endif
  free(((*yyvaluep).str));
}
#line 1977 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_T_ID: /* "identifier"  */
#line 113 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", ((*yyvaluep).str));
#endif
  free(((*yyvaluep).str));
}
#line 1988 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_block_func: /* block_func  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 1999 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_block: /* block  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2010 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_line: /* line  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2021 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_shellarg: /* shellarg  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2032 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_shell: /* shell  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2043 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_debug: /* debug  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2054 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_auxsys: /* auxsys  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2065 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_line_func: /* line_func  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2076 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_func_decl: /* func_decl  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2087 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_funcdef: /* funcdef  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2098 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_case_list: /* case_list  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2109 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_stmt: /* stmt  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2120 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_conditional: /* conditional  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2131 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_elseif_list: /* elseif_list  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2142 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_expcondition: /* expcondition  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2153 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_csig: /* csig  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2164 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_initcell: /* initcell  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2175 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_condition: /* condition  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2186 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_id_list: /* id_list  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2197 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_arg: /* arg  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2208 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_arg_list: /* arg_list  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2219 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_matrix: /* matrix  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2230 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_vector: /* vector  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2241 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_range: /* range  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2252 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_exp_range: /* exp_range  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2263 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_compop: /* compop  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2274 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_assign2this: /* assign2this  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2285 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_varblock: /* varblock  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2296 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_tid: /* tid  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2307 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_assign: /* assign  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2318 "src/engine/psycon.tab.c"
        break;

    case YYSYMBOL_exp: /* exp  */
#line 106 "src/engine/psycon.y"
{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName(((*yyvaluep).pnode)));
#endif
  yydeleteAstNode(((*yyvaluep).pnode), 0);
}
#line 2329 "src/engine/psycon.tab.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (AstNode **pproot, char **errmsg)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */


/* User initialization code.  */
#line 97 "src/engine/psycon.y"
{
  if (ErrorMsg) {
	free(ErrorMsg);
	ErrorMsg = NULL;
  }
  *errmsg = NULL;
}

#line 2427 "src/engine/psycon.tab.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= T_EOF)
    {
      yychar = T_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* input: %empty  */
#line 147 "src/engine/psycon.y"
        { *pproot = NULL;}
#line 2640 "src/engine/psycon.tab.c"
    break;

  case 3: /* input: block_func  */
#line 149 "src/engine/psycon.y"
        { *pproot = (yyvsp[0].pnode);}
#line 2646 "src/engine/psycon.tab.c"
    break;

  case 5: /* block_func: block_func line_func  */
#line 154 "src/engine/psycon.y"
        {
		if ((yyvsp[0].pnode)) {
			if ((yyvsp[-1].pnode) == NULL)
				(yyval.pnode) = (yyvsp[0].pnode);
			else if ((yyvsp[-1].pnode)->type == N_BLOCK)
			{
				(yyvsp[-1].pnode)->tail->next = (yyvsp[0].pnode);
				(yyvsp[-1].pnode)->tail = (yyvsp[0].pnode);
			}
			else
			{ // a=1; b=2; ==> $1->type is '='. So first, a N_BLOCK tree should be made.
				(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
				(yyval.pnode)->next = (yyvsp[-1].pnode);
				(yyvsp[-1].pnode)->next = (yyval.pnode)->tail = (yyvsp[0].pnode);
			}
		} else
			(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 2669 "src/engine/psycon.tab.c"
    break;

  case 6: /* block: line  */
#line 175 "src/engine/psycon.y"
        {
		if ((yyvsp[0].pnode)) // if cond1, x=1, end ==> x=1 comes here.
			(yyval.pnode) = (yyvsp[0].pnode);
		else
			(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
	}
#line 2680 "src/engine/psycon.tab.c"
    break;

  case 7: /* block: block line  */
#line 182 "src/engine/psycon.y"
        {
		if ((yyvsp[0].pnode)) {
			if ((yyvsp[-1].pnode)->type == N_BLOCK) {
				if ((yyval.pnode)->next) {
					(yyvsp[-1].pnode)->tail->next = (yyvsp[0].pnode);
					(yyvsp[-1].pnode)->tail = (yyvsp[0].pnode);
				} else {
					(yyval.pnode) = (yyvsp[0].pnode);
					free((yyvsp[-1].pnode));
				}
			} else { //if the second argument doesn't have N_BLOCK, make one
				(yyval.pnode) = newAstNode(N_BLOCK, (yyloc));
				(yyval.pnode)->next = (yyvsp[-1].pnode);
				(yyvsp[-1].pnode)->next = (yyval.pnode)->tail = (yyvsp[0].pnode);
			}
		}
		else // only "block" is given
			(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 2704 "src/engine/psycon.tab.c"
    break;

  case 8: /* line: "end of line"  */
#line 204 "src/engine/psycon.y"
        { (yyval.pnode) = NULL;}
#line 2710 "src/engine/psycon.tab.c"
    break;

  case 9: /* line: error "end of line"  */
#line 206 "src/engine/psycon.y"
        {
		(yyval.pnode) = NULL;
		yyerrok;
	}
#line 2719 "src/engine/psycon.tab.c"
    break;

  case 10: /* line: '#' shell eol  */
#line 211 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 2727 "src/engine/psycon.tab.c"
    break;

  case 11: /* line: '/' debug eol  */
#line 215 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 2735 "src/engine/psycon.tab.c"
    break;

  case 12: /* line: '>' auxsys eol  */
#line 219 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 2743 "src/engine/psycon.tab.c"
    break;

  case 14: /* line: stmt eol2  */
#line 224 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->suppress = 1 | ((yyval.pnode)->suppress & AST_SUPPRESS_ASYNC_ASSIGN);
	}
#line 2752 "src/engine/psycon.tab.c"
    break;

  case 15: /* line: tid '|' "identifier" "number"  */
#line 229 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-3].pnode);
		(yyval.pnode)->tail = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->tail->str = (yyvsp[-1].str);
		(yyval.pnode)->tail->dval = (yyvsp[0].dval);
	}
#line 2763 "src/engine/psycon.tab.c"
    break;

  case 16: /* shellarg: "identifier"  */
#line 238 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[0].str);
	}
#line 2772 "src/engine/psycon.tab.c"
    break;

  case 18: /* shellarg: "number"  */
#line 245 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_NUMBER, (yyloc));
		(yyval.pnode)->dval = (yyvsp[0].dval);
	}
#line 2781 "src/engine/psycon.tab.c"
    break;

  case 19: /* shellarg: '-' "number"  */
#line 250 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_NEGATIVE, (yyloc));
		AstNode *p = newAstNode(T_NUMBER, (yyloc));
		p->dval = (yyvsp[0].dval);
		(yyval.pnode)->child = p;
	}
#line 2792 "src/engine/psycon.tab.c"
    break;

  case 20: /* shellarg: "string"  */
#line 258 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_STRING, (yyloc));
		(yyval.pnode)->str = (yyvsp[0].str);
	}
#line 2801 "src/engine/psycon.tab.c"
    break;

  case 21: /* shell: shellarg  */
#line 267 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_SHELL, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 2810 "src/engine/psycon.tab.c"
    break;

  case 22: /* shell: shell shellarg  */
#line 272 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[0].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[0].pnode);
	}
#line 2822 "src/engine/psycon.tab.c"
    break;

  case 23: /* debug: shellarg  */
#line 282 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_DEBUG, (yyloc));
		(yyval.pnode)->str = (yyvsp[0].pnode);
	}
#line 2831 "src/engine/psycon.tab.c"
    break;

  case 24: /* auxsys: shellarg  */
#line 289 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_AUXSYS, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 2840 "src/engine/psycon.tab.c"
    break;

  case 25: /* auxsys: auxsys shellarg  */
#line 294 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[0].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[0].pnode);
	}
#line 2852 "src/engine/psycon.tab.c"
    break;

  case 34: /* func_decl: "function"  */
#line 319 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->suppress = 2;
	}
#line 2861 "src/engine/psycon.tab.c"
    break;

  case 35: /* func_decl: "static" "function"  */
#line 324 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->suppress = 3;
	}
#line 2870 "src/engine/psycon.tab.c"
    break;

  case 36: /* funcdef: func_decl "identifier" block func_end  */
#line 331 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-3].pnode);
		(yyval.pnode)->str = (yyvsp[-2].str);
		(yyval.pnode)->child = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
	}
#line 2881 "src/engine/psycon.tab.c"
    break;

  case 37: /* funcdef: func_decl varblock '=' "identifier" block func_end  */
#line 338 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-5].pnode);
		(yyval.pnode)->str = (yyvsp[-2].str);
		(yyval.pnode)->child = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
		if ((yyvsp[-4].pnode)->type!=N_VECTOR)
		{
			(yyval.pnode)->alt = newAstNode(N_VECTOR, (yylsp[-4]));
			AstNode *p = newAstNode(N_VECTOR, (yylsp[-4]));
			p->alt = p->tail = (yyvsp[-4].pnode);
			(yyval.pnode)->alt->str = (char*)p;
		}
		else
		{
			(yyval.pnode)->alt = (yyvsp[-4].pnode);
		}
	}
#line 2903 "src/engine/psycon.tab.c"
    break;

  case 38: /* funcdef: func_decl "identifier" '(' id_list ')' block func_end  */
#line 356 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-6].pnode);
		(yyval.pnode)->str = (yyvsp[-5].str);
		(yyval.pnode)->child = (yyvsp[-3].pnode);
		(yyvsp[-3].pnode)->next = (yyvsp[-1].pnode);
	}
#line 2914 "src/engine/psycon.tab.c"
    break;

  case 39: /* funcdef: func_decl varblock '=' "identifier" '(' id_list ')' block func_end  */
#line 363 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-8].pnode);
		(yyval.pnode)->str = (yyvsp[-5].str);
		(yyval.pnode)->child = (yyvsp[-3].pnode);
		(yyvsp[-3].pnode)->next = (yyvsp[-1].pnode);
		if ((yyvsp[-7].pnode)->type!=N_VECTOR)
		{
			(yyval.pnode)->alt = newAstNode(N_VECTOR, (yylsp[-7]));
			AstNode *p = newAstNode(N_VECTOR, (yylsp[-7]));
			p->alt = p->tail = (yyvsp[-7].pnode);
			(yyval.pnode)->alt->str = (char*)p;
		}
		else
		{
			(yyval.pnode)->alt = (yyvsp[-7].pnode);
		}
	}
#line 2936 "src/engine/psycon.tab.c"
    break;

  case 40: /* case_list: %empty  */
#line 383 "src/engine/psycon.y"
        { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));}
#line 2942 "src/engine/psycon.tab.c"
    break;

  case 41: /* case_list: "end of line"  */
#line 385 "src/engine/psycon.y"
        { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));}
#line 2948 "src/engine/psycon.tab.c"
    break;

  case 42: /* case_list: case_list "case" exp "end of line" block  */
#line 387 "src/engine/psycon.y"
        {
		if ((yyvsp[-4].pnode)->alt)
			(yyvsp[-4].pnode)->tail->alt = (yyvsp[-2].pnode);
		else
			(yyvsp[-4].pnode)->alt = (yyvsp[-2].pnode);
		AstNode *p = (yyvsp[0].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[0]));
			p->next = (yyvsp[0].pnode);
		}
		(yyvsp[-4].pnode)->tail = (yyvsp[-2].pnode)->next = p;
		(yyval.pnode) = (yyvsp[-4].pnode);
	}
#line 2967 "src/engine/psycon.tab.c"
    break;

  case 43: /* case_list: case_list "case" '{' arg_list '}' "end of line" block  */
#line 402 "src/engine/psycon.y"
        {
		if ((yyvsp[-6].pnode)->alt)
			(yyvsp[-6].pnode)->tail->alt = (yyvsp[-3].pnode);
		else
			(yyvsp[-6].pnode)->alt = (yyvsp[-3].pnode);
		AstNode *p = (yyvsp[0].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[0]));
			p->next = (yyvsp[0].pnode);
		}
		(yyvsp[-6].pnode)->tail = (yyvsp[-3].pnode)->next = p;
		(yyval.pnode) = (yyvsp[-6].pnode);
	}
#line 2986 "src/engine/psycon.tab.c"
    break;

  case 47: /* stmt: "if" conditional block elseif_list "end"  */
#line 422 "src/engine/psycon.y"
        { // This works, too, for "if cond, act; end" without else, because elseif_list can be empty
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
		AstNode *p = (yyvsp[-2].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[-2]));
			p->next = (yyvsp[-2].pnode);
		}
		(yyval.pnode)->child = (yyvsp[-3].pnode);
		(yyvsp[-3].pnode)->next = p;
		AstNode *pElse = (yyvsp[-1].pnode);
		if (pElse->type!=N_BLOCK)
		{
			pElse = newAstNode(N_BLOCK, (yylsp[-1]));
			pElse->next = (yyvsp[-1].pnode);
		}
		(yyval.pnode)->alt = pElse;
		if ((yyvsp[-1].pnode)->child==NULL && (yyvsp[-1].pnode)->next==NULL) // When elseif_list is empty, T_IF is made, but no child and next
		{
			yydeleteAstNode((yyval.pnode)->alt, 1);
			(yyval.pnode)->alt=NULL;
		}
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3016 "src/engine/psycon.tab.c"
    break;

  case 48: /* stmt: "switch" exp case_list "end"  */
#line 448 "src/engine/psycon.y"
        { // case is cascaded through alt
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->alt = (yyvsp[-1].pnode)->alt;
		(yyval.pnode)->child = (yyvsp[-2].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3028 "src/engine/psycon.tab.c"
    break;

  case 49: /* stmt: "switch" exp case_list "otherwise" block "end"  */
#line 456 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-3].pnode);
		(yyval.pnode)->alt = (yyvsp[-3].pnode)->alt;
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		AstNode *p = newAstNode(T_OTHERWISE, (yylsp[-1]));
		p->next = (yyvsp[-1].pnode);
		(yyval.pnode)->tail = (yyvsp[-3].pnode)->tail->alt = p;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3043 "src/engine/psycon.tab.c"
    break;

  case 50: /* stmt: "try" block "catch" "identifier" block "end"  */
#line 467 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_TRY, (yyloc));
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		(yyval.pnode)->alt = newAstNode(T_CATCH, (yylsp[-2]));
		(yyval.pnode)->alt->child = newAstNode(T_ID, (yylsp[-2]));
		(yyval.pnode)->alt->child->str = (yyvsp[-2].str);
		(yyval.pnode)->alt->next = (yyvsp[-1].pnode);
	}
#line 3056 "src/engine/psycon.tab.c"
    break;

  case 51: /* stmt: "try" block "catchback" "identifier" block "end"  */
#line 476 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_TRY, (yyloc));
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		(yyval.pnode)->alt = newAstNode(T_CATCHBACK, (yylsp[-2]));
		(yyval.pnode)->alt->child = newAstNode(T_ID, (yylsp[-2]));
		(yyval.pnode)->alt->child->str = (yyvsp[-2].str);
		(yyval.pnode)->alt->next = (yyvsp[-1].pnode);
	}
#line 3069 "src/engine/psycon.tab.c"
    break;

  case 52: /* stmt: "while" conditional block "end"  */
#line 485 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_WHILE, (yyloc));
		(yyval.pnode)->child = (yyvsp[-2].pnode);
		AstNode *p = (yyvsp[-1].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[-1]));
			p->next = (yyvsp[-1].pnode);
		}
		(yyval.pnode)->alt = p;
	}
#line 3085 "src/engine/psycon.tab.c"
    break;

  case 53: /* stmt: "for" "identifier" '=' exp_range block "end"  */
#line 497 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[-4]));
		(yyval.pnode)->child->str = (yyvsp[-4].str);
		(yyval.pnode)->child->child = (yyvsp[-2].pnode);
		AstNode *p = (yyvsp[-1].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[-1]));
			p->next = (yyvsp[-1].pnode);
		}
		(yyval.pnode)->alt = p;
	}
#line 3103 "src/engine/psycon.tab.c"
    break;

  case 54: /* stmt: "for" "identifier" '=' exp_range ',' block "end"  */
#line 511 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[-5]));
		(yyval.pnode)->child->str = (yyvsp[-5].str);
		(yyval.pnode)->child->child = (yyvsp[-3].pnode);
		AstNode *p = (yyvsp[-1].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[-1]));
			p->next = (yyvsp[-1].pnode);
		}
		(yyval.pnode)->alt = p;
	}
#line 3121 "src/engine/psycon.tab.c"
    break;

  case 55: /* stmt: "return"  */
#line 525 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_RETURN, (yyloc));
	}
#line 3129 "src/engine/psycon.tab.c"
    break;

  case 56: /* stmt: "break"  */
#line 529 "src/engine/psycon.y"
        { (yyval.pnode) = newAstNode(T_BREAK, (yyloc));}
#line 3135 "src/engine/psycon.tab.c"
    break;

  case 57: /* stmt: "continue"  */
#line 531 "src/engine/psycon.y"
        { (yyval.pnode) = newAstNode(T_CONTINUE, (yyloc));}
#line 3141 "src/engine/psycon.tab.c"
    break;

  case 62: /* elseif_list: %empty  */
#line 539 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
	}
#line 3149 "src/engine/psycon.tab.c"
    break;

  case 63: /* elseif_list: "elseif" conditional block elseif_list  */
#line 543 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_IF, (yyloc));
		(yyval.pnode)->child = (yyvsp[-2].pnode);
		AstNode *p = (yyvsp[-1].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[-1]));
			p->next = (yyvsp[-1].pnode);
		}
		(yyvsp[-2].pnode)->next = p;
		(yyval.pnode)->alt = (yyvsp[0].pnode);
	}
#line 3166 "src/engine/psycon.tab.c"
    break;

  case 64: /* elseif_list: elseif_list "else" block  */
#line 556 "src/engine/psycon.y"
        {
		AstNode *p = (yyvsp[0].pnode);
		if (p->type!=N_BLOCK)
		{
			p = newAstNode(N_BLOCK, (yylsp[0]));
			p->next = (yyvsp[0].pnode);
		}
		if ((yyvsp[-2].pnode)->child==NULL) // if there's no elseif; i.e., elseif_list is empty
		{
			yydeleteAstNode((yyvsp[-2].pnode), 1);
			(yyval.pnode) = p;
		}
		else
		{
			(yyval.pnode) = (yyvsp[-2].pnode);
			(yyvsp[-2].pnode)->alt = p;
		}
	}
#line 3189 "src/engine/psycon.tab.c"
    break;

  case 67: /* initcell: '{' arg_list '}'  */
#line 583 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->type = N_INITCELL;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3200 "src/engine/psycon.tab.c"
    break;

  case 68: /* condition: '(' condition ')'  */
#line 592 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3210 "src/engine/psycon.tab.c"
    break;

  case 69: /* condition: exp '<' exp  */
#line 598 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('<', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3216 "src/engine/psycon.tab.c"
    break;

  case 70: /* condition: exp '>' exp  */
#line 600 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('>', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3222 "src/engine/psycon.tab.c"
    break;

  case 71: /* condition: exp "==" exp  */
#line 602 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_EQ, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3228 "src/engine/psycon.tab.c"
    break;

  case 72: /* condition: exp "!=" exp  */
#line 604 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_NE, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3234 "src/engine/psycon.tab.c"
    break;

  case 73: /* condition: exp ">=" exp  */
#line 606 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_GE, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3240 "src/engine/psycon.tab.c"
    break;

  case 74: /* condition: exp "<=" exp  */
#line 608 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_LE, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3246 "src/engine/psycon.tab.c"
    break;

  case 75: /* condition: '!' expcondition  */
#line 610 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_LOGIC_NOT, (yyloc));
		(yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 3255 "src/engine/psycon.tab.c"
    break;

  case 76: /* condition: expcondition "&&" expcondition  */
#line 615 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_AND, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3261 "src/engine/psycon.tab.c"
    break;

  case 77: /* condition: expcondition "||" expcondition  */
#line 617 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_OR, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3267 "src/engine/psycon.tab.c"
    break;

  case 78: /* id_list: %empty  */
#line 621 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_IDLIST, (yyloc));
	}
#line 3275 "src/engine/psycon.tab.c"
    break;

  case 79: /* id_list: "identifier"  */
#line 625 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_IDLIST, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->tail = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->tail->str = (yyvsp[0].str);
	}
#line 3285 "src/engine/psycon.tab.c"
    break;

  case 80: /* id_list: id_list ',' "identifier"  */
#line 631 "src/engine/psycon.y"
        {
		(yyvsp[-2].pnode)->tail = (yyvsp[-2].pnode)->tail->next = newAstNode(T_ID, (yylsp[0]));
		(yyval.pnode) = (yyvsp[-2].pnode);
		(yyval.pnode)->tail->str = (yyvsp[0].str);
	}
#line 3295 "src/engine/psycon.tab.c"
    break;

  case 81: /* arg: ':'  */
#line 639 "src/engine/psycon.y"
        {	(yyval.pnode) = newAstNode(T_FULLRANGE, (yyloc)); }
#line 3301 "src/engine/psycon.tab.c"
    break;

  case 84: /* arg_list: arg  */
#line 645 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_ARGS, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 3310 "src/engine/psycon.tab.c"
    break;

  case 85: /* arg_list: arg_list ',' arg  */
#line 650 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-2].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->next = (yyvsp[0].pnode);
		else
			(yyval.pnode)->tail = (yyval.pnode)->next = (yyvsp[0].pnode);
	}
#line 3322 "src/engine/psycon.tab.c"
    break;

  case 86: /* matrix: %empty  */
#line 660 "src/engine/psycon.y"
        {
	// N_MATRIX consists of "outer" N_MATRIX--alt for dot notation
	// and "inner" N_VECTOR--alt for all successive items thru next
	// the str field of the outer N_MATRIX node is cast to the inner N_VECTOR.
	// this "fake" str pointer is freed during normal clean-up
	// 11/4/2019
		(yyval.pnode) = newAstNode(N_MATRIX, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		(yyval.pnode)->str = (char*)p;
	}
#line 3337 "src/engine/psycon.tab.c"
    break;

  case 87: /* matrix: vector  */
#line 671 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_MATRIX, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		p->alt = p->tail = (yyvsp[0].pnode);
		(yyval.pnode)->str = (char*)p;
	}
#line 3348 "src/engine/psycon.tab.c"
    break;

  case 88: /* matrix: matrix ';' vector  */
#line 678 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-2].pnode);
		AstNode * p = (AstNode *)(yyvsp[-2].pnode)->str;
		if (p->tail)
			p->tail = p->tail->next = (AstNode *)(yyvsp[0].pnode);
		else
		{ /* leading ';' ([; x]): first row empty, second row is $3 */
			AstNode *row0 = newAstNode(N_VECTOR, (yyloc));
			AstNode *inner0 = newAstNode(N_VECTOR, (yyloc));
			row0->str = (char *)inner0;
			row0->next = (AstNode *)(yyvsp[0].pnode);
			p->alt = row0;
			p->tail = (AstNode *)(yyvsp[0].pnode);
		}
	}
#line 3368 "src/engine/psycon.tab.c"
    break;

  case 90: /* vector: exp_range  */
#line 697 "src/engine/psycon.y"
        {
	// N_VECTOR consists of "outer" N_VECTOR--alt for dot notation
	// and "inner" N_VECTOR--alt for all successive items thru next
	// Because N_VECTOR doesn't use str, the inner N_VECTOR is created there and cast for further uses.
	// this "fake" str pointer is freed during normal clean-up
	// 11/4/2019
		(yyval.pnode) = newAstNode(N_VECTOR, (yyloc));
		AstNode * p = newAstNode(N_VECTOR, (yyloc));
		p->alt = p->tail = (yyvsp[0].pnode);
		(yyval.pnode)->str = (char*)p;
	}
#line 3384 "src/engine/psycon.tab.c"
    break;

  case 91: /* vector: vector exp_range  */
#line 709 "src/engine/psycon.y"
        {
		AstNode * p = (AstNode *)(yyvsp[-1].pnode)->str;
		p->tail = p->tail->next = (yyvsp[0].pnode);
		(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 3394 "src/engine/psycon.tab.c"
    break;

  case 92: /* vector: vector ',' exp_range  */
#line 715 "src/engine/psycon.y"
        {
		AstNode * p = (AstNode *)(yyvsp[-2].pnode)->str;
		p->tail = p->tail->next = (yyvsp[0].pnode);
		(yyval.pnode) = (yyvsp[-2].pnode);
	}
#line 3404 "src/engine/psycon.tab.c"
    break;

  case 93: /* range: exp ':' exp  */
#line 723 "src/engine/psycon.y"
        {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));
	}
#line 3412 "src/engine/psycon.tab.c"
    break;

  case 94: /* range: exp ':' exp ':' exp  */
#line 727 "src/engine/psycon.y"
        {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[-4].pnode), (yyvsp[0].pnode), (yyloc));
		(yyvsp[0].pnode)->next = (yyvsp[-2].pnode);
	}
#line 3421 "src/engine/psycon.tab.c"
    break;

  case 98: /* compop: "+="  */
#line 737 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode('+', (yyloc));
	}
#line 3429 "src/engine/psycon.tab.c"
    break;

  case 99: /* compop: "-="  */
#line 741 "src/engine/psycon.y"
        { 		(yyval.pnode) = newAstNode('-', (yyloc));	}
#line 3435 "src/engine/psycon.tab.c"
    break;

  case 100: /* compop: "*="  */
#line 743 "src/engine/psycon.y"
        { 		(yyval.pnode) = newAstNode('*', (yyloc));	}
#line 3441 "src/engine/psycon.tab.c"
    break;

  case 101: /* compop: "/="  */
#line 745 "src/engine/psycon.y"
        { 		(yyval.pnode) = newAstNode('/', (yyloc));	}
#line 3447 "src/engine/psycon.tab.c"
    break;

  case 102: /* compop: "@="  */
#line 747 "src/engine/psycon.y"
        { 		(yyval.pnode) = newAstNode('@', (yyloc));	}
#line 3453 "src/engine/psycon.tab.c"
    break;

  case 103: /* compop: "@@="  */
#line 749 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode('@', (yyloc));
		(yyval.pnode)->child = newAstNode('@', (yyloc));
	}
#line 3462 "src/engine/psycon.tab.c"
    break;

  case 104: /* compop: ">>="  */
#line 754 "src/engine/psycon.y"
        { 		(yyval.pnode) = newAstNode(T_OP_SHIFT, (yyloc));	}
#line 3468 "src/engine/psycon.tab.c"
    break;

  case 105: /* compop: "%="  */
#line 756 "src/engine/psycon.y"
        { 		(yyval.pnode) = newAstNode('%', (yyloc));	}
#line 3474 "src/engine/psycon.tab.c"
    break;

  case 106: /* compop: "->="  */
#line 758 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "movespec");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	}
#line 3485 "src/engine/psycon.tab.c"
    break;

  case 107: /* compop: "~="  */
#line 765 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "respeed");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	}
#line 3496 "src/engine/psycon.tab.c"
    break;

  case 108: /* compop: "<>="  */
#line 772 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "timestretch");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	}
#line 3507 "src/engine/psycon.tab.c"
    break;

  case 109: /* compop: "#="  */
#line 779 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (char*)calloc(16, 1);
		strcpy((yyval.pnode)->str, "pitchscale");
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_ARGS, (yyloc));
	}
#line 3518 "src/engine/psycon.tab.c"
    break;

  case 110: /* assign2this: '=' exp_range  */
#line 788 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[0].pnode);
	}
#line 3526 "src/engine/psycon.tab.c"
    break;

  case 111: /* assign2this: "++=" condition  */
#line 792 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_OP_CONCAT, (yyloc));
		AstNode *p = (yyval.pnode);
		if (p->alt)
			p = p->alt;
		p->child = 	newAstNode(T_REPLICA, (yylsp[0]));
		p->tail = p->child->next = (yyvsp[0].pnode);
	}
#line 3539 "src/engine/psycon.tab.c"
    break;

  case 112: /* assign2this: "++=" exp  */
#line 801 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_OP_CONCAT, (yyloc));
		AstNode *p = (yyval.pnode);
		if (p->alt)
			p = p->alt;
		p->child = 	newAstNode(T_REPLICA, (yylsp[0]));
		p->tail = p->child->next = (yyvsp[0].pnode);
	}
#line 3552 "src/engine/psycon.tab.c"
    break;

  case 113: /* assign2this: compop exp_range  */
#line 810 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		if ((yyval.pnode)->child) // compop should be "@@=" and $$->child->type should be '@'  (64)
		{
			(yyval.pnode)->child->child = newAstNode(T_REPLICA, (yyloc));
			(yyval.pnode)->child->tail = (yyval.pnode)->child->child->next = newAstNode(T_REPLICA, (yyloc));
			(yyval.pnode)->tail = (yyval.pnode)->child->next = (yyvsp[0].pnode);
		}
		else
		{
			AstNode *p = (yyval.pnode);
			if (p->alt)
				p = p->alt;
			p->child = 	newAstNode(T_REPLICA, (yylsp[0]));
			p->tail = p->child->next = (yyvsp[0].pnode);
		}
	}
#line 3574 "src/engine/psycon.tab.c"
    break;

  case 114: /* varblock: "identifier"  */
#line 830 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[0].str);
	}
#line 3583 "src/engine/psycon.tab.c"
    break;

  case 115: /* varblock: tid '.' "identifier"  */
#line 835 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-2].pnode);
		AstNode *p = newAstNode(N_STRUCT, (yyloc));
		p->str = (yyvsp[0].str);
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
	}
#line 3606 "src/engine/psycon.tab.c"
    break;

  case 116: /* varblock: varblock '{' exp '}'  */
#line 854 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-3].pnode);
		AstNode *p = newAstNode(N_CELL, (yyloc));
		p->child = (yyvsp[-1].pnode);
		if ((yyval.pnode)->tail)
			(yyval.pnode)->tail = (yyval.pnode)->tail->alt = p;
		else
			(yyval.pnode)->tail = (yyval.pnode)->alt = p;
	}
#line 3620 "src/engine/psycon.tab.c"
    break;

  case 117: /* varblock: '[' vector ']'  */
#line 864 "src/engine/psycon.y"
        {//tid-vector --> what's this comment? 12/30/2020
		(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 3628 "src/engine/psycon.tab.c"
    break;

  case 118: /* varblock: '$' varblock  */
#line 868 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_HOOK, (yyloc));
		(yyval.pnode)->str = (char*)calloc(1, strlen((yyvsp[0].pnode)->str)+1);
		strcpy((yyval.pnode)->str, (yyvsp[0].pnode)->str);
		(yyval.pnode)->alt = (yyvsp[0].pnode)->alt;
	}
#line 3639 "src/engine/psycon.tab.c"
    break;

  case 120: /* tid: "identifier" '(' arg_list ')'  */
#line 878 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[-3].str);
		handle_tilde((yyval.pnode), (yyvsp[-1].pnode), (yylsp[-1]));
	}
#line 3649 "src/engine/psycon.tab.c"
    break;

  case 121: /* tid: varblock '(' arg_list ')'  */
#line 884 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-3].pnode);
		handle_tilde((yyval.pnode), (yyvsp[-1].pnode), (yylsp[-1]));
	}
#line 3658 "src/engine/psycon.tab.c"
    break;

  case 122: /* tid: varblock '(' ')'  */
#line 889 "src/engine/psycon.y"
        {
		if ((yyval.pnode)->alt != NULL  && (yyval.pnode)->alt->type==N_STRUCT)
		{ // dot notation with a blank parentheses, e.g., a.sqrt() or (1:2:5).sqrt()
			(yyval.pnode) = (yyvsp[-2].pnode);
		}
		else // no longer used.,,.. absorbed by tid:  T_ID
		{ // udf_func()
			(yyval.pnode) = newAstNode(N_CALL, (yyloc));
			(yyval.pnode)->str = getT_ID_str((yyvsp[-2].pnode));
		}
	}
#line 3674 "src/engine/psycon.tab.c"
    break;

  case 123: /* tid: ".."  */
#line 901 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
	}
#line 3682 "src/engine/psycon.tab.c"
    break;

  case 124: /* tid: ".." '(' arg_list ')'  */
#line 905 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		handle_tilde((yyval.pnode), (yyvsp[-1].pnode), (yylsp[-1]));
	}
#line 3691 "src/engine/psycon.tab.c"
    break;

  case 125: /* tid: ".." '{' exp '}'  */
#line 910 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		(yyval.pnode)->tail = (yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[-1].pnode);
	}
#line 3701 "src/engine/psycon.tab.c"
    break;

  case 126: /* tid: ".." '{' exp '}' '(' arg_list ')'  */
#line 916 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_REPLICA, (yyloc));
		(yyval.pnode)->alt = newAstNode(N_CELL, (yyloc));
		(yyval.pnode)->alt->child = (yyvsp[-4].pnode);
		handle_tilde((yyval.pnode)->alt, (yyvsp[-1].pnode), (yylsp[-1]));
		(yyval.pnode)->tail = (yyval.pnode)->alt->alt; // we need this; or tail is broken and can't put '.' tid at the end
	}
#line 3713 "src/engine/psycon.tab.c"
    break;

  case 127: /* tid: tid '\''  */
#line 924 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_TRANSPOSE, (yyloc));
		(yyval.pnode)->child = (yyvsp[-1].pnode);
	}
#line 3722 "src/engine/psycon.tab.c"
    break;

  case 128: /* tid: '[' vector ']' '[' ']'  */
#line 929 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[-3].pnode);
	}
#line 3731 "src/engine/psycon.tab.c"
    break;

  case 129: /* tid: '[' vector ']' '[' vector ']'  */
#line 934 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
	}
#line 3741 "src/engine/psycon.tab.c"
    break;

  case 130: /* tid: '[' vector ']' '[' matrix ']'  */
#line 940 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
	}
#line 3751 "src/engine/psycon.tab.c"
    break;

  case 131: /* tid: '[' matrix ']' '[' vector ']'  */
#line 946 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->str = (char*)malloc(8);
		strcpy((yyval.pnode)->str, "R");
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
	}
#line 3763 "src/engine/psycon.tab.c"
    break;

  case 132: /* tid: '[' matrix ']' '[' matrix ']'  */
#line 954 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(N_TSEQ, (yyloc));
		(yyval.pnode)->str = (char*)malloc(8);
		strcpy((yyval.pnode)->str, "R");
		(yyval.pnode)->child = (yyvsp[-4].pnode);
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
	}
#line 3775 "src/engine/psycon.tab.c"
    break;

  case 133: /* tid: '[' matrix ']'  */
#line 962 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
	}
#line 3783 "src/engine/psycon.tab.c"
    break;

  case 134: /* tid: '(' exp_range ')'  */
#line 966 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3793 "src/engine/psycon.tab.c"
    break;

  case 135: /* assign: tid assign2this  */
#line 975 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 3802 "src/engine/psycon.tab.c"
    break;

  case 136: /* assign: varblock assign2this  */
#line 980 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[-1].pnode);
		(yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 3811 "src/engine/psycon.tab.c"
    break;

  case 137: /* assign: "identifier" "<-" exp_range  */
#line 985 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ID, (yylsp[-2]));
		(yyval.pnode)->str = (yyvsp[-2].str);
		(yyval.pnode)->child = (yyvsp[0].pnode);
		(yyval.pnode)->suppress = AST_SUPPRESS_ASYNC_ASSIGN;
	}
#line 3822 "src/engine/psycon.tab.c"
    break;

  case 138: /* assign: varblock '=' assign  */
#line 992 "src/engine/psycon.y"
        { //c=a(2)=44
		if (!(yyvsp[-2].pnode)->child)
			(yyvsp[-2].pnode)->child = (yyvsp[0].pnode);
		else
			for (AstNode *p = (yyvsp[-2].pnode)->child; p; p=p->child)
			{
				if (!p->child)
				{
					p->child = (yyvsp[0].pnode);
					break;
				}
			}
		(yyval.pnode) = (yyvsp[-2].pnode);
	}
#line 3841 "src/engine/psycon.tab.c"
    break;

  case 139: /* assign: tid '=' assign  */
#line 1007 "src/engine/psycon.y"
        { //a(2)=d=11
		if (!(yyvsp[-2].pnode)->child)
			(yyvsp[-2].pnode)->child = (yyvsp[0].pnode);
		else
			for (AstNode *p = (yyvsp[-2].pnode)->child; p; p=p->child)
			{
				if (!p->child)
				{
					p->child = (yyvsp[0].pnode);
					break;
				}
			}
		(yyval.pnode) = (yyvsp[-2].pnode);
	}
#line 3860 "src/engine/psycon.tab.c"
    break;

  case 140: /* assign: varblock '=' initcell  */
#line 1022 "src/engine/psycon.y"
        { // x={"bjk",noise(300), 4.5555}
		(yyval.pnode)->str = getT_ID_str((yyvsp[-2].pnode));
		(yyval.pnode)->child = (yyvsp[0].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3871 "src/engine/psycon.tab.c"
    break;

  case 143: /* exp: "number"  */
#line 1033 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_NUMBER, (yyloc));
		(yyval.pnode)->dval = (yyvsp[0].dval);
		(yyval.pnode)->suppress = consumeNumberUnitMask((yylsp[0]).first_line, (yylsp[0]).first_column);
	}
#line 3881 "src/engine/psycon.tab.c"
    break;

  case 144: /* exp: "string"  */
#line 1039 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_STRING, (yyloc));
		(yyval.pnode)->str = (yyvsp[0].str);
	}
#line 3890 "src/engine/psycon.tab.c"
    break;

  case 145: /* exp: T_ENDPOINT  */
#line 1044 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_ENDPOINT, (yyloc));
	}
#line 3898 "src/engine/psycon.tab.c"
    break;

  case 146: /* exp: '-' exp  */
#line 1048 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_NEGATIVE, (yyloc));
		(yyval.pnode)->child = (yyvsp[0].pnode);
	}
#line 3907 "src/engine/psycon.tab.c"
    break;

  case 147: /* exp: '+' exp  */
#line 1053 "src/engine/psycon.y"
        {
		(yyval.pnode) = (yyvsp[0].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->col = (yyloc).first_column;
	}
#line 3917 "src/engine/psycon.tab.c"
    break;

  case 148: /* exp: "sigma" '(' tid '=' exp_range ',' exp ')'  */
#line 1059 "src/engine/psycon.y"
        {
		(yyval.pnode) = newAstNode(T_SIGMA, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[-5]));
		(yyval.pnode)->child->str = getT_ID_str((yyvsp[-5].pnode));
		(yyval.pnode)->child->child = (yyvsp[-3].pnode);
		(yyval.pnode)->child->next = (yyvsp[-1].pnode);
	}
#line 3929 "src/engine/psycon.tab.c"
    break;

  case 149: /* exp: exp '+' exp  */
#line 1067 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('+', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3935 "src/engine/psycon.tab.c"
    break;

  case 150: /* exp: exp '-' exp  */
#line 1069 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('-', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3941 "src/engine/psycon.tab.c"
    break;

  case 151: /* exp: exp '*' exp  */
#line 1071 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('*', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3947 "src/engine/psycon.tab.c"
    break;

  case 152: /* exp: exp '/' exp  */
#line 1073 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('/', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3953 "src/engine/psycon.tab.c"
    break;

  case 153: /* exp: exp "**" exp  */
#line 1075 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_MATRIXMULT, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3959 "src/engine/psycon.tab.c"
    break;

  case 154: /* exp: exp '^' exp  */
#line 1077 "src/engine/psycon.y"
        { (yyval.pnode) = makeFunctionCall("^", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3965 "src/engine/psycon.tab.c"
    break;

  case 155: /* exp: exp '%' exp  */
#line 1079 "src/engine/psycon.y"
        { (yyval.pnode) = makeFunctionCall("mod", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3971 "src/engine/psycon.tab.c"
    break;

  case 156: /* exp: exp '~' exp  */
#line 1081 "src/engine/psycon.y"
        { (yyval.pnode) = makeFunctionCall("respeed", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3977 "src/engine/psycon.tab.c"
    break;

  case 157: /* exp: exp '#' exp  */
#line 1083 "src/engine/psycon.y"
        { (yyval.pnode) = makeFunctionCall("pitchscale", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3983 "src/engine/psycon.tab.c"
    break;

  case 158: /* exp: exp "<>" exp  */
#line 1085 "src/engine/psycon.y"
        { (yyval.pnode) = makeFunctionCall("timestretch", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3989 "src/engine/psycon.tab.c"
    break;

  case 159: /* exp: exp "->" exp  */
#line 1087 "src/engine/psycon.y"
        { (yyval.pnode) = makeFunctionCall("movespec", (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 3995 "src/engine/psycon.tab.c"
    break;

  case 160: /* exp: exp '@' exp  */
#line 1089 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode('@', (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 4001 "src/engine/psycon.tab.c"
    break;

  case 161: /* exp: exp ">>" exp  */
#line 1091 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_OP_SHIFT, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 4007 "src/engine/psycon.tab.c"
    break;

  case 162: /* exp: exp "++" exp  */
#line 1093 "src/engine/psycon.y"
        { (yyval.pnode) = makeBinaryOpNode(T_OP_CONCAT, (yyvsp[-2].pnode), (yyvsp[0].pnode), (yyloc));}
#line 4013 "src/engine/psycon.tab.c"
    break;


#line 4017 "src/engine/psycon.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (pproot, errmsg, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= T_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == T_EOF)
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (pproot, errmsg, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, pproot, errmsg);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1110 "src/engine/psycon.y"


/* Called by yyparse on error. */
void yyerror (AstNode **pproot, char **errmsg, char const *s)
{
  static size_t errmsg_len = 0;
#define ERRMSG_MAX 999
  char msgbuf[ERRMSG_MAX], *p;
  size_t msglen;

  snprintf(msgbuf, sizeof(msgbuf), "Invalid syntax: Line %d, Col %d: %s.\n", yylloc.first_line, yylloc.first_column, s + (strncmp(s, "syntax error, ", 14) ? 0 : 14));
  if ((p=strstr(msgbuf, "$undefined"))) {
	snprintf(p, ERRMSG_MAX - (size_t)(p - msgbuf), "'%c'(%d)", yychar, yychar);
	memmove(p+strlen(p), p+10, strlen(p+10)+1);
  }
  if ((p=strstr(msgbuf, "end of text or ")))
    memmove(p, p+15, strlen(p+15)+1);
  if ((p=strstr(msgbuf, " or ','")))
    memmove(p, p+7, strlen(p+7)+1);
  msglen = strlen(msgbuf);
  if (ErrorMsg == NULL)
    errmsg_len = 0;
  char* new_errmsg = (char *)realloc(ErrorMsg, errmsg_len+msglen+1);
  if (!new_errmsg) {
    free(ErrorMsg);
    ErrorMsg = NULL;
    errmsg_len = 0;
    *errmsg = (char*)"Invalid syntax: out of memory while formatting parser error.\n";
    return;
  }
  ErrorMsg = new_errmsg;
  memmove(ErrorMsg+errmsg_len, msgbuf, msglen+1);
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
	{
		int token;
		for (token = 0; token <= YYMAXUTOK; ++token)
			if ((int)YYTRANSLATE(token) == (int)i)
				return token;
	}
	else
		return T_UNKNOWN;
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
