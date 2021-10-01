/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         knparse
#define yylex           knlex
#define yyerror         knerror
#define yydebug         kndebug
#define yynerrs         knnerrs

#define yylval          knlval
#define yychar          knchar

/* Copy the first part of user declarations.  */

#line 75 "k.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "k.tab.h".  */
#ifndef YY_KN_K_TAB_H_INCLUDED
# define YY_KN_K_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int kndebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TRUE = 258,
    FALSE = 259,
    NUM = 260,
    FLOAT = 261,
    STRING = 262,
    VARIABLE = 263,
    OPENPAREN = 264,
    CLOSEPAREN = 265,
    EQQ = 266,
    COMMA = 267,
    ACTSTR = 268,
    LOCINI = 269,
    KOF = 270,
    KEYPRE = 271,
    KNVERSION = 272,
    DOTT = 273,
    SIGNERKEY = 274,
    HINT = 275,
    OPENBLOCK = 276,
    CLOSEBLOCK = 277,
    SIGNATUREENTRY = 278,
    PRIVATEKEY = 279,
    SEMICOLON = 280,
    EQ = 281,
    NE = 282,
    LT = 283,
    GT = 284,
    LE = 285,
    GE = 286,
    REGEXP = 287,
    OR = 288,
    AND = 289,
    NOT = 290,
    PLUS = 291,
    MINUS = 292,
    MULT = 293,
    DIV = 294,
    MOD = 295,
    EXP = 296,
    UNARYMINUS = 297,
    DEREF = 298,
    OPENNUM = 299,
    OPENFLT = 300
  };
#endif
/* Tokens.  */
#define TRUE 258
#define FALSE 259
#define NUM 260
#define FLOAT 261
#define STRING 262
#define VARIABLE 263
#define OPENPAREN 264
#define CLOSEPAREN 265
#define EQQ 266
#define COMMA 267
#define ACTSTR 268
#define LOCINI 269
#define KOF 270
#define KEYPRE 271
#define KNVERSION 272
#define DOTT 273
#define SIGNERKEY 274
#define HINT 275
#define OPENBLOCK 276
#define CLOSEBLOCK 277
#define SIGNATUREENTRY 278
#define PRIVATEKEY 279
#define SEMICOLON 280
#define EQ 281
#define NE 282
#define LT 283
#define GT 284
#define LE 285
#define GE 286
#define REGEXP 287
#define OR 288
#define AND 289
#define NOT 290
#define PLUS 291
#define MINUS 292
#define MULT 293
#define DIV 294
#define MOD 295
#define EXP 296
#define UNARYMINUS 297
#define DEREF 298
#define OPENNUM 299
#define OPENFLT 300

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 20 "keynote.y" /* yacc.c:355  */

    char   *string;
    double  doubval;
    int     intval;
    int     bool;

#line 212 "k.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE knlval;

int knparse (void);

#endif /* !YY_KN_K_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 44 "keynote.y" /* yacc.c:358  */

#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */

#include "header.h"
#include "keynote.h"
#include "assertion.h"

static int *keynote_kth_array = (int *) NULL;
static int keylistcount = 0;

static int   resolve_assertion(char *);
static int   keynote_init_kth(void);
static int   isfloatstring(char *);
static int   checkexception(int);
static char *my_lookup(char *);
static int   intpow(int, int);
static int   get_kth(int);

#line 259 "k.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   260

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  96
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  172

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    76,    76,    76,    78,    78,    79,    79,    81,    81,
      82,    82,    84,    84,    90,    90,    95,    98,   102,   103,
     105,   105,   114,   114,   123,   124,   124,   124,   142,   150,
     159,   197,   198,   200,   235,   234,   269,   274,   275,   275,
     289,   296,   304,   317,   320,   321,   321,   326,   326,   331,
     332,   333,   334,   335,   336,   338,   339,   340,   341,   342,
     343,   345,   346,   347,   348,   350,   351,   352,   353,   361,
     369,   370,   371,   372,   373,   388,   389,   390,   391,   399,
     403,   404,   405,   406,   422,   434,   446,   458,   470,   482,
     494,   583,   607,   609,   610,   611,   637
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TRUE", "FALSE", "NUM", "FLOAT",
  "STRING", "VARIABLE", "OPENPAREN", "CLOSEPAREN", "EQQ", "COMMA",
  "ACTSTR", "LOCINI", "KOF", "KEYPRE", "KNVERSION", "DOTT", "SIGNERKEY",
  "HINT", "OPENBLOCK", "CLOSEBLOCK", "SIGNATUREENTRY", "PRIVATEKEY",
  "SEMICOLON", "EQ", "NE", "LT", "GT", "LE", "GE", "REGEXP", "OR", "AND",
  "NOT", "PLUS", "MINUS", "MULT", "DIV", "MOD", "EXP", "UNARYMINUS",
  "DEREF", "OPENNUM", "OPENFLT", "$accept", "grammarswitch", "$@1", "$@2",
  "$@3", "$@4", "$@5", "$@6", "$@7", "keypredicate",
  "notemptykeypredicate", "keyexp", "$@8", "$@9", "$@10", "$@11",
  "keylist", "key", "localinit", "localconstants", "$@12", "program",
  "prog", "$@13", "notemptyprog", "afterhint", "expr", "$@14", "$@15",
  "numexp", "floatexp", "numex", "floatex", "stringexp", "str",
  "strnotconcat", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300
};
# endif

#define YYPACT_NINF -56

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-56)))

#define YYTABLE_NINF -35

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     224,   -56,   -56,   -56,   -56,   -56,   -56,   -56,     4,    19,
       1,    32,    14,    70,    70,    31,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,    19,    19,   114,    70,    70,    70,   -56,
     -56,   -56,   107,   -56,   -56,   181,   195,   -56,   103,   -56,
       8,   -56,   -56,    32,   -56,   -56,    -4,   -56,   -56,    37,
     -56,    70,   -56,   -56,   -56,    47,   135,   157,   174,   -56,
     114,   -56,   -56,   -56,   -56,   -56,    35,   117,   -56,   -56,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     5,     7,     7,     7,     7,     7,     7,     7,     7,
       7,    70,    70,    70,    70,    70,    70,    70,    70,    67,
      -4,    66,    -3,    73,   -56,   -56,    -3,   -56,   -56,   -56,
     -56,    -5,   142,    19,    19,   -56,    37,    19,    19,     5,
       5,   213,   213,   213,   213,   213,   213,   115,   115,    43,
      43,    43,   -56,     7,     7,   219,   219,   219,   219,    12,
      12,    77,    77,   -56,   -56,    37,    37,    37,    37,    37,
      37,    37,   120,   -56,   -56,    32,    32,   -56,   100,   102,
     -56,     1,    70,   105,   -56,   -56,   -56,   127,   132,   -56,
      70,   -56
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     4,     2,     6,    12,     8,    10,    14,     0,    37,
      31,    16,     0,     0,     0,     0,     1,    53,    54,    73,
      82,    93,    95,     0,     0,     0,     0,     0,     0,     5,
      36,    38,    41,    50,    51,     0,     0,    52,     0,    92,
       0,     3,    32,     0,    25,     7,    17,    19,    18,    30,
      13,     0,     9,    11,    15,     0,     0,     0,     0,    49,
       0,    71,    80,    96,    74,    83,     0,     0,    47,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    19,    30,     0,    22,    20,     0,    44,    72,    81,
      94,     0,     0,    37,    37,    40,    42,     0,     0,     0,
       0,    57,    60,    55,    56,    58,    59,    65,    66,    67,
      68,    69,    70,     0,     0,    61,    62,    63,    64,    75,
      76,    77,    78,    79,    91,    84,    85,    86,    87,    88,
      89,    90,    33,    24,    26,     0,     0,    39,     0,    48,
      46,     0,     0,    23,    21,    43,    35,     0,    28,    27,
       0,    29
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -41,   104,   -56,   -56,   -56,   -56,   -24,   -13,   -56,   -11,
     -56,   -56,   -55,   -56,   -56,   -56,   -12,   -56,   -56,   -56,
     -56,    23,   -17,   -56,    -6,    82
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     8,    10,     9,    11,    13,    14,    12,    15,    45,
      46,    47,   156,   155,   103,   162,   167,    48,    41,    42,
     161,    29,    30,    66,    31,   115,    32,   118,   117,    33,
      34,    35,    36,    37,    49,    39
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      52,    53,   100,    38,    16,   108,    57,   110,    62,    40,
      19,    55,    59,    20,   119,    91,   133,    58,    38,    99,
      63,    50,    17,    18,    19,    20,    21,    22,    23,   104,
     105,    76,    77,    78,    79,    80,    81,   102,    54,    21,
      22,    43,   120,   112,   134,   106,    56,    44,    61,    27,
      88,    89,    28,    90,    24,    91,    25,   107,   157,   158,
     113,   116,    26,    27,    28,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   152,    26,   153,    21,    22,    51,
      68,    69,   154,   111,    81,   144,   145,   146,   147,   148,
     149,   150,   151,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   159,   160,    38,    38,    64,
      65,    38,    38,    26,   163,   164,   112,    62,    90,    19,
      20,    91,   165,    60,    21,    22,    51,    67,   -34,    92,
      93,    94,    95,    96,    97,    98,    69,   169,   114,   105,
      68,    69,   111,    61,   170,   108,   171,   101,     0,   168,
     166,    25,   109,    78,    79,    80,    81,   168,    27,    28,
      26,    70,    71,    72,    73,    74,    75,   109,     0,     0,
       0,    76,    77,    78,    79,    80,    81,     0,    86,    87,
      88,    89,     0,    90,   110,    82,    83,    84,    85,     0,
       0,     0,    91,    86,    87,    88,    89,     0,    90,     0,
      92,    93,    94,    95,    96,    97,    98,    70,    71,    72,
      73,    74,    75,     0,     0,     0,     0,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     0,     0,     0,
       0,    86,    87,    88,    89,     0,    90,     1,     2,     0,
       3,     4,     0,     5,     0,     0,     0,     6,     7,    76,
      77,    78,    79,    80,    81,    86,    87,    88,    89,     0,
      90
};

static const yytype_int16 yycheck[] =
{
      13,    14,    43,     9,     0,    10,    23,    10,    25,     8,
       5,    23,    24,     6,     9,    18,     9,    23,    24,    11,
      26,     7,     3,     4,     5,     6,     7,     8,     9,    33,
      34,    36,    37,    38,    39,    40,    41,    43,     7,     7,
       8,     9,    37,    60,    37,    51,    23,    15,    25,    44,
      38,    39,    45,    41,    35,    18,    37,    10,   113,   114,
      25,    67,    43,    44,    45,    82,    83,    84,    85,    86,
      87,    88,    89,    90,     7,    43,    10,     7,     8,     9,
      33,    34,     9,    60,    41,    91,    92,    93,    94,    95,
      96,    97,    98,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,   117,   118,   113,   114,    27,
      28,   117,   118,    43,   155,   156,   133,   134,    41,     5,
       6,    18,    22,     9,     7,     8,     9,    20,     8,    26,
      27,    28,    29,    30,    31,    32,    34,    10,    21,    34,
      33,    34,   119,   120,    12,    10,   170,    43,    -1,   162,
     161,    37,    10,    38,    39,    40,    41,   170,    44,    45,
      43,    26,    27,    28,    29,    30,    31,    10,    -1,    -1,
      -1,    36,    37,    38,    39,    40,    41,    -1,    36,    37,
      38,    39,    -1,    41,    10,    28,    29,    30,    31,    -1,
      -1,    -1,    18,    36,    37,    38,    39,    -1,    41,    -1,
      26,    27,    28,    29,    30,    31,    32,    26,    27,    28,
      29,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    40,    41,    28,    29,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    41,    13,    14,    -1,
      16,    17,    -1,    19,    -1,    -1,    -1,    23,    24,    36,
      37,    38,    39,    40,    41,    36,    37,    38,    39,    -1,
      41
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    13,    14,    16,    17,    19,    23,    24,    47,    49,
      48,    50,    53,    51,    52,    54,     0,     3,     4,     5,
       6,     7,     8,     9,    35,    37,    43,    44,    45,    67,
      68,    70,    72,    75,    76,    77,    78,    79,    80,    81,
       8,    64,    65,     9,    15,    55,    56,    57,    63,    80,
       7,     9,    63,    63,     7,    72,    77,    78,    80,    72,
       9,    77,    78,    80,    81,    81,    69,    20,    33,    34,
      26,    27,    28,    29,    30,    31,    36,    37,    38,    39,
      40,    41,    28,    29,    30,    31,    36,    37,    38,    39,
      41,    18,    26,    27,    28,    29,    30,    31,    32,    11,
      56,    57,    80,    60,    33,    34,    80,    10,    10,    10,
      10,    77,    78,    25,    21,    71,    80,    74,    73,     9,
      37,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,     9,    37,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    80,    80,    80,    80,    80,    80,
      80,    80,     7,    10,     9,    59,    58,    68,    68,    72,
      72,    66,    61,    56,    56,    22,    65,    62,    63,    10,
      12,    62
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    46,    48,    47,    49,    47,    50,    47,    51,    47,
      52,    47,    53,    47,    54,    47,    55,    55,    56,    56,
      58,    57,    59,    57,    57,    60,    61,    57,    62,    62,
      63,    64,    64,    65,    66,    65,    67,    68,    69,    68,
      70,    70,    71,    71,    72,    73,    72,    74,    72,    72,
      72,    72,    72,    72,    72,    75,    75,    75,    75,    75,
      75,    76,    76,    76,    76,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    79,    79,    79,    79,    79,    79,
      79,    80,    80,    81,    81,    81,    81
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     0,     3,     0,     3,     0,     3,
       0,     3,     0,     3,     0,     3,     0,     1,     1,     1,
       0,     4,     0,     4,     3,     0,     0,     6,     1,     3,
       1,     0,     1,     3,     0,     5,     1,     0,     0,     4,
       3,     1,     1,     3,     3,     0,     4,     0,     4,     2,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     1,     2,     3,     3,     3,     3,     3,
       2,     3,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     1,     3,     1,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
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
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
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
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
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
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 76 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1480 "k.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 78 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1486 "k.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 79 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1492 "k.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 81 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1498 "k.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 82 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1504 "k.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 84 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1510 "k.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 85 "keynote.y" /* yacc.c:1646  */
    { keynote_lex_remove((yyvsp[0].string));
				 if (strcmp((yyvsp[0].string), KEYNOTE_VERSION_STRING))
				   keynote_errno = ERROR_SYNTAX;
				 free((yyvsp[0].string));
			       }
#line 1520 "k.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 90 "keynote.y" /* yacc.c:1646  */
    { keynote_exceptionflag = keynote_donteval = 0; }
#line 1526 "k.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 91 "keynote.y" /* yacc.c:1646  */
    { keynote_lex_remove((yyvsp[0].string));
			         keynote_privkey = (yyvsp[0].string);
			       }
#line 1534 "k.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 95 "keynote.y" /* yacc.c:1646  */
    { keynote_returnvalue = 0;
                                return 0; 
                              }
#line 1542 "k.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 98 "keynote.y" /* yacc.c:1646  */
    { keynote_returnvalue = (yyvsp[0].intval);
				return 0;
                              }
#line 1550 "k.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 102 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[0].intval); }
#line 1556 "k.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 103 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[0].intval); }
#line 1562 "k.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 105 "keynote.y" /* yacc.c:1646  */
    { if (((yyvsp[-1].intval) == 0) && !keynote_justrecord)
                                     keynote_donteval = 1;
                                 }
#line 1570 "k.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 108 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[-3].intval) > (yyvsp[0].intval))
		     (yyval.intval) = (yyvsp[0].intval);
		   else
	       	     (yyval.intval) = (yyvsp[-3].intval);
		   keynote_donteval = 0;
                 }
#line 1581 "k.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 114 "keynote.y" /* yacc.c:1646  */
    { if (((yyvsp[-1].intval) == (keynote_current_session->ks_values_num - 1)) && !keynote_justrecord)
	                             keynote_donteval = 1;
       	                         }
#line 1589 "k.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 117 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[-3].intval) >= (yyvsp[0].intval))
		     (yyval.intval) = (yyvsp[-3].intval);
		   else
		     (yyval.intval) = (yyvsp[0].intval);
		   keynote_donteval = 0;
                 }
#line 1600 "k.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 123 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-1].intval); }
#line 1606 "k.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 124 "keynote.y" /* yacc.c:1646  */
    { keylistcount = 0; }
#line 1612 "k.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 124 "keynote.y" /* yacc.c:1646  */
    {
			 if (!keynote_justrecord && !keynote_donteval)
 	                   if (keynote_init_kth() == -1)
			     return -1;
                       }
#line 1622 "k.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 129 "keynote.y" /* yacc.c:1646  */
    {
			      if (keylistcount < (yyvsp[-5].intval))
			      {
				  keynote_errno = ERROR_SYNTAX;
				  return -1;
			      }

			    if (!keynote_justrecord && !keynote_donteval)
			      (yyval.intval) = get_kth((yyvsp[-5].intval));
			    else
			      (yyval.intval) = 0;
			  }
#line 1639 "k.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 143 "keynote.y" /* yacc.c:1646  */
    { /* Don't do anything if we're just recording */ 
              if (!keynote_justrecord && !keynote_donteval)
		if (((yyvsp[0].intval) < keynote_current_session->ks_values_num) && ((yyvsp[0].intval) >= 0))
		  keynote_kth_array[(yyvsp[0].intval)]++;

	      keylistcount++;
            }
#line 1651 "k.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 151 "keynote.y" /* yacc.c:1646  */
    { /* Don't do anything if we're just recording */ 
	      if (!keynote_justrecord && !keynote_donteval)
		if (((yyvsp[-2].intval) < keynote_current_session->ks_values_num) && ((yyvsp[-2].intval) >= 0))
		  keynote_kth_array[(yyvsp[-2].intval)]++;

	      keylistcount++;
            }
#line 1663 "k.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 159 "keynote.y" /* yacc.c:1646  */
    {
		   if (keynote_donteval)
		     (yyval.intval) = 0;
		   else
		   {
		       keynote_lex_remove((yyvsp[0].string));
		       if (keynote_justrecord)
		       {
			   if (keynote_keylist_add(&keynote_keypred_keylist,
						   (yyvsp[0].string)) == -1)
			   {
			       free((yyvsp[0].string));
			       return -1;
			   }

			   (yyval.intval) = 0;
		       }
		       else
			 switch (keynote_in_action_authorizers((yyvsp[0].string), KEYNOTE_ALGORITHM_UNSPEC))
			 {
			     case -1:
				 free((yyvsp[0].string));
				 return -1;
				 
			     case RESULT_TRUE:
				 free((yyvsp[0].string));
				 (yyval.intval) = keynote_current_session->ks_values_num -
				      1;
				 break;
				 
			     default:
				 (yyval.intval) = resolve_assertion((yyvsp[0].string));
				 free((yyvsp[0].string));
				 break;
			 }
		   }
                 }
#line 1705 "k.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 201 "keynote.y" /* yacc.c:1646  */
    {
            int i;

            keynote_lex_remove((yyvsp[-2].string));
	    keynote_lex_remove((yyvsp[0].string));
 
	    /*
	     * Variable names starting with underscores are illegal here.
	     */
	    if ((yyvsp[-2].string)[0] == '_')
	    {
		free((yyvsp[-2].string));
		free((yyvsp[0].string));
		keynote_errno = ERROR_SYNTAX;
		return -1;
	    }
	    
	    /* If the identifier already exists, report error. */
	    if (keynote_env_lookup((yyvsp[-2].string), &keynote_init_list, 1) != (char *) NULL)
	    {
		free((yyvsp[-2].string));
		free((yyvsp[0].string));
		keynote_errno = ERROR_SYNTAX;
		return -1;
	    }

	    i = keynote_env_add((yyvsp[-2].string), (yyvsp[0].string), &keynote_init_list, 1, 0);
	    free((yyvsp[-2].string));
	    free((yyvsp[0].string));

	    if (i != RESULT_TRUE)
	      return -1;
	  }
#line 1743 "k.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 235 "keynote.y" /* yacc.c:1646  */
    {
            int i;

	    keynote_lex_remove((yyvsp[-2].string));
	    keynote_lex_remove((yyvsp[0].string));

	    /*
	     * Variable names starting with underscores are illegal here.
	     */
	    if ((yyvsp[-2].string)[0] == '_')
	    {
		free((yyvsp[-2].string));
		free((yyvsp[0].string));
		keynote_errno = ERROR_SYNTAX;
		return -1;
	    }
	 
	    /* If the identifier already exists, report error. */
	    if (keynote_env_lookup((yyvsp[-2].string), &keynote_init_list, 1) != (char *) NULL)
	    {
		free((yyvsp[-2].string));
		free((yyvsp[0].string));
		keynote_errno = ERROR_SYNTAX;
		return -1;
	    }

	    i = keynote_env_add((yyvsp[-2].string), (yyvsp[0].string), &keynote_init_list, 1, 0);
	    free((yyvsp[-2].string));
	    free((yyvsp[0].string));

	    if (i != RESULT_TRUE)
	      return -1;
	  }
#line 1781 "k.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 269 "keynote.y" /* yacc.c:1646  */
    { 
	        keynote_returnvalue = (yyvsp[0].intval);
		return 0;
	      }
#line 1790 "k.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 274 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 1796 "k.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 275 "keynote.y" /* yacc.c:1646  */
    {
			  /* 
			   * Cleanup envlist of additions such as 
			   * regexp results
			   */
			  keynote_env_cleanup(&keynote_temp_list, 1);
                    }
#line 1808 "k.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 282 "keynote.y" /* yacc.c:1646  */
    {
		      if ((yyvsp[-3].intval) > (yyvsp[0].intval))
			(yyval.intval) = (yyvsp[-3].intval);
		      else
			(yyval.intval) = (yyvsp[0].intval);
                    }
#line 1819 "k.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 290 "keynote.y" /* yacc.c:1646  */
    {
		if (checkexception((yyvsp[-2].bool)))
		  (yyval.intval) = (yyvsp[0].intval);
		else
		  (yyval.intval) = 0;
	      }
#line 1830 "k.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 297 "keynote.y" /* yacc.c:1646  */
    {
		if (checkexception((yyvsp[0].bool)))
		  (yyval.intval) = keynote_current_session->ks_values_num - 1;
		else
		  (yyval.intval) = 0;
	      }
#line 1841 "k.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 304 "keynote.y" /* yacc.c:1646  */
    {  if (keynote_exceptionflag || keynote_donteval)
		    (yyval.intval) = 0;
		  else
		  {
		      keynote_lex_remove((yyvsp[0].string));

		      (yyval.intval) = keynote_retindex((yyvsp[0].string));
		      if ((yyval.intval) == -1)   /* Invalid return value */
			(yyval.intval) = 0;

		      free((yyvsp[0].string));
		  }
                }
#line 1859 "k.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 317 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-1].intval); }
#line 1865 "k.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 320 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-1].bool); }
#line 1871 "k.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 321 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].bool) == 0)
	               keynote_donteval = 1;
	           }
#line 1879 "k.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 323 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = ((yyvsp[-3].bool) && (yyvsp[0].bool));
		                          keynote_donteval = 0;
		                        }
#line 1887 "k.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 326 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].bool))
	              keynote_donteval = 1; 
	          }
#line 1895 "k.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 328 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = ((yyvsp[-3].bool) || (yyvsp[0].bool));
		                          keynote_donteval = 0;
                                        }
#line 1903 "k.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 331 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = !((yyvsp[0].bool)); }
#line 1909 "k.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 332 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[0].bool); }
#line 1915 "k.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 333 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[0].bool); }
#line 1921 "k.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 334 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[0].bool); }
#line 1927 "k.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 335 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = 1; }
#line 1933 "k.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 336 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = 0; }
#line 1939 "k.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 338 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].intval) < (yyvsp[0].intval); }
#line 1945 "k.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 339 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].intval) > (yyvsp[0].intval); }
#line 1951 "k.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 340 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].intval) == (yyvsp[0].intval); }
#line 1957 "k.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 341 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].intval) <= (yyvsp[0].intval); }
#line 1963 "k.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 342 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].intval) >= (yyvsp[0].intval); }
#line 1969 "k.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 343 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].intval) != (yyvsp[0].intval); }
#line 1975 "k.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 345 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].doubval) < (yyvsp[0].doubval); }
#line 1981 "k.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 346 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].doubval) > (yyvsp[0].doubval); }
#line 1987 "k.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 347 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].doubval) <= (yyvsp[0].doubval); }
#line 1993 "k.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 348 "keynote.y" /* yacc.c:1646  */
    { (yyval.bool) = (yyvsp[-2].doubval) >= (yyvsp[0].doubval); }
#line 1999 "k.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 350 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-2].intval) + (yyvsp[0].intval); }
#line 2005 "k.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 351 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-2].intval) - (yyvsp[0].intval); }
#line 2011 "k.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 352 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-2].intval) * (yyvsp[0].intval); }
#line 2017 "k.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 353 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[0].intval) == 0)
	                      {
				  if (!keynote_donteval)
				    keynote_exceptionflag = 1;
			      }
	                      else
			        (yyval.intval) = ((yyvsp[-2].intval) / (yyvsp[0].intval));
			    }
#line 2030 "k.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 361 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[0].intval) == 0)
	                      {
				  if (!keynote_donteval)
				    keynote_exceptionflag = 1;
			      }
	                      else
			        (yyval.intval) = (yyvsp[-2].intval) % (yyvsp[0].intval);
			    }
#line 2043 "k.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 369 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = intpow((yyvsp[-2].intval), (yyvsp[0].intval)); }
#line 2049 "k.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 370 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = -((yyvsp[0].intval)); }
#line 2055 "k.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 371 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-1].intval); }
#line 2061 "k.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 372 "keynote.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[0].intval); }
#line 2067 "k.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 373 "keynote.y" /* yacc.c:1646  */
    { if (keynote_exceptionflag ||
					      keynote_donteval)
	                                    (yyval.intval) = 0;
 	                                  else
					  {
					      keynote_lex_remove((yyvsp[0].string));

					      if (!isfloatstring((yyvsp[0].string)))
						(yyval.intval) = 0;
					      else
						(yyval.intval) = (int) floor(atof((yyvsp[0].string)));
					      free((yyvsp[0].string));
					  }
					}
#line 2086 "k.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 388 "keynote.y" /* yacc.c:1646  */
    { (yyval.doubval) = ((yyvsp[-2].doubval) + (yyvsp[0].doubval)); }
#line 2092 "k.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 389 "keynote.y" /* yacc.c:1646  */
    { (yyval.doubval) = ((yyvsp[-2].doubval) - (yyvsp[0].doubval)); }
#line 2098 "k.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 390 "keynote.y" /* yacc.c:1646  */
    { (yyval.doubval) = ((yyvsp[-2].doubval) * (yyvsp[0].doubval)); }
#line 2104 "k.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 391 "keynote.y" /* yacc.c:1646  */
    { if ((yyvsp[0].doubval) == 0)
	                                  {
					      if (!keynote_donteval)
						keynote_exceptionflag = 1;
					  }
	                                  else
			        	   (yyval.doubval) = ((yyvsp[-2].doubval) / (yyvsp[0].doubval));
					}
#line 2117 "k.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 399 "keynote.y" /* yacc.c:1646  */
    { if (!keynote_exceptionflag &&
						      !keynote_donteval)
	                                            (yyval.doubval) = pow((yyvsp[-2].doubval), (yyvsp[0].doubval));
	                                        }
#line 2126 "k.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 403 "keynote.y" /* yacc.c:1646  */
    { (yyval.doubval) = -((yyvsp[0].doubval)); }
#line 2132 "k.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 404 "keynote.y" /* yacc.c:1646  */
    { (yyval.doubval) = (yyvsp[-1].doubval); }
#line 2138 "k.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 405 "keynote.y" /* yacc.c:1646  */
    { (yyval.doubval) = (yyvsp[0].doubval); }
#line 2144 "k.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 406 "keynote.y" /* yacc.c:1646  */
    {
	                                  if (keynote_exceptionflag ||
					      keynote_donteval)
					    (yyval.doubval) = 0.0;
					  else
					  {
					      keynote_lex_remove((yyvsp[0].string));
					  
					      if (!isfloatstring((yyvsp[0].string)))
						(yyval.doubval) = 0.0;
					      else
						(yyval.doubval) = atof((yyvsp[0].string));
					      free((yyvsp[0].string));
					  }
	                                }
#line 2164 "k.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 422 "keynote.y" /* yacc.c:1646  */
    {
                        if (keynote_exceptionflag || keynote_donteval)
			  (yyval.bool) = 0;
			else
			{
			    (yyval.bool) = strcmp((yyvsp[-2].string), (yyvsp[0].string)) == 0 ? 1 : 0;
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			}
		      }
#line 2181 "k.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 434 "keynote.y" /* yacc.c:1646  */
    {
	                if (keynote_exceptionflag || keynote_donteval)
			  (yyval.bool) = 0;
			else
			{
			    (yyval.bool) = strcmp((yyvsp[-2].string), (yyvsp[0].string)) != 0 ? 1 : 0;
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			}
		      }
#line 2198 "k.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 446 "keynote.y" /* yacc.c:1646  */
    {
	                if (keynote_exceptionflag || keynote_donteval)
			  (yyval.bool) = 0;
			else
			{
			    (yyval.bool) = strcmp((yyvsp[-2].string), (yyvsp[0].string)) < 0 ? 1 : 0;
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			}
		      }
#line 2215 "k.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 458 "keynote.y" /* yacc.c:1646  */
    {
	                if (keynote_exceptionflag || keynote_donteval)
			  (yyval.bool) = 0;
			else
			{
			    (yyval.bool) = strcmp((yyvsp[-2].string), (yyvsp[0].string)) > 0 ? 1 : 0;
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			}
		      }
#line 2232 "k.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 470 "keynote.y" /* yacc.c:1646  */
    {
	                if (keynote_exceptionflag || keynote_donteval)
			  (yyval.bool) = 0;
			else
			{
			    (yyval.bool) = strcmp((yyvsp[-2].string), (yyvsp[0].string)) <= 0 ? 1 : 0;
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			}
		      }
#line 2249 "k.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 482 "keynote.y" /* yacc.c:1646  */
    {
	                if (keynote_exceptionflag || keynote_donteval)
			  (yyval.bool) = 0;
			else
			{
			    (yyval.bool) = strcmp((yyvsp[-2].string), (yyvsp[0].string)) >= 0 ? 1 : 0;
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			}
		      }
#line 2266 "k.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 495 "keynote.y" /* yacc.c:1646  */
    {
	      regmatch_t pmatch[32];
	      char grp[4], *gr;
	      regex_t preg;
	      int i;

	      if (keynote_exceptionflag || keynote_donteval)
		(yyval.bool) = 0;
	      else
	      {
		  keynote_lex_remove((yyvsp[-2].string));
		  keynote_lex_remove((yyvsp[0].string));

		  memset(pmatch, 0, sizeof(pmatch));
		  memset(grp, 0, sizeof(grp));

#if HAVE_REGCOMP
		  if (regcomp(&preg, (yyvsp[0].string), REG_EXTENDED))
		  {
#else /* HAVE_REGCOMP */
#error "This system does not have regcomp()."
#endif /* HAVE_REGCOMP */
		      free((yyvsp[-2].string));
		      free((yyvsp[0].string));
		      keynote_exceptionflag = 1;
		  }
		  else
		  {
		      /* Clean-up residuals from previous regexps */
		      keynote_env_cleanup(&keynote_temp_list, 1);

		      free((yyvsp[0].string));
		      i = regexec(&preg, (yyvsp[-2].string), 32, pmatch, 0);
		      (yyval.bool) = (i == 0 ? 1 : 0);
		      if (i == 0)
		      {
#if !defined(HAVE_SNPRINTF)
			  sprintf(grp, "%d", preg.re_nsub);
#else /* !HAVE_SNPRINTF */
			  snprintf(grp, 3, "%d", preg.re_nsub);
#endif /* !HAVE_SNPRINTF */
			  if (keynote_env_add("_0", grp, &keynote_temp_list,
					      1, 0) != RESULT_TRUE)
			  {
			      free((yyvsp[-2].string));
			      regfree(&preg);
			      return -1;
			  }

			  for (i = 1; i < 32 && pmatch[i].rm_so != -1; i++)
			  {
			      gr = calloc(pmatch[i].rm_eo - pmatch[i].rm_so +
					  1, sizeof(char));
			      if (gr == (char *) NULL)
			      {
				  free((yyvsp[-2].string));
				  regfree(&preg);
				  keynote_errno = ERROR_MEMORY;
				  return -1;
			      }

			      strncpy(gr, (yyvsp[-2].string) + pmatch[i].rm_so,
				      pmatch[i].rm_eo - pmatch[i].rm_so);
			      gr[pmatch[i].rm_eo - pmatch[i].rm_so] = '\0';
#if !defined(HAVE_SNPRINTF)
			      sprintf(grp, "_%d", i);
#else /* !HAVE_SNPRINTF */
			      snprintf(grp, 3, "_%d", i);
#endif /* !HAVE_SNPRINTF */
			      if (keynote_env_add(grp, gr, &keynote_temp_list,
						  1, 0) == -1)
			      {
				  free((yyvsp[-2].string));
				  regfree(&preg);
				  free(gr);
				  return -1;
			      }
			      else
				free(gr);
			  }
		      }

		      regfree(&preg);
		      free((yyvsp[-2].string));
		  }
	      }
	    }
#line 2358 "k.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 583 "keynote.y" /* yacc.c:1646  */
    {  if (keynote_exceptionflag || keynote_donteval)
			  (yyval.string) = (char *) NULL;
			else
			{
			    (yyval.string) = calloc(strlen((yyvsp[-2].string)) + strlen((yyvsp[0].string)) + 1,
					sizeof(char));
			    keynote_lex_remove((yyvsp[-2].string));
			    keynote_lex_remove((yyvsp[0].string));
			    if ((yyval.string) == (char *) NULL)
			    {
				free((yyvsp[-2].string));
				free((yyvsp[0].string));
				keynote_errno = ERROR_MEMORY;
				return -1;
			    }
 
			    strcpy((yyval.string), (yyvsp[-2].string));
			    strcpy((yyval.string) + strlen((yyvsp[-2].string)), (yyvsp[0].string));
			    free((yyvsp[-2].string));
			    free((yyvsp[0].string));
			    if (keynote_lex_add((yyval.string), LEXTYPE_CHAR) == -1)
			      return -1;
			}
		      }
#line 2387 "k.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 607 "keynote.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 2393 "k.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 609 "keynote.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 2399 "k.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 610 "keynote.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[-1].string); }
#line 2405 "k.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 611 "keynote.y" /* yacc.c:1646  */
    {  if (keynote_exceptionflag || keynote_donteval)
	                     (yyval.string) = (char *) NULL;
 	                   else
			   {
			       (yyval.string) = my_lookup((yyvsp[0].string));
			       keynote_lex_remove((yyvsp[0].string));
			       free((yyvsp[0].string));
			       if ((yyval.string) == (char *) NULL)
			       {
				   if (keynote_errno)
				     return -1;
				   (yyval.string) = strdup("");
			       }
			       else
				 (yyval.string) = strdup((yyval.string));

			       if ((yyval.string) == (char *) NULL)
			       {
				   keynote_errno = ERROR_MEMORY;
				   return -1;
			       }

			       if (keynote_lex_add((yyval.string), LEXTYPE_CHAR) == -1)
				 return -1;
			   }
	                 }
#line 2436 "k.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 637 "keynote.y" /* yacc.c:1646  */
    {  if (keynote_exceptionflag || keynote_donteval)
			      (yyval.string) = (char *) NULL;
			    else
			    {
				(yyval.string) = my_lookup((yyvsp[0].string));
				keynote_lex_remove((yyvsp[0].string));
				free((yyvsp[0].string));
				if ((yyval.string) == (char *) NULL)
				{
				    if (keynote_errno)
				      return -1;
				    (yyval.string) = strdup("");
				}
				else
				  (yyval.string) = strdup((yyval.string));

				if ((yyval.string) == (char *) NULL)
				{
				    keynote_errno = ERROR_MEMORY;
				    return -1;
				}

				if (keynote_lex_add((yyval.string), LEXTYPE_CHAR) == -1)
				  return -1;
			    }
			 }
#line 2467 "k.tab.c" /* yacc.c:1646  */
    break;


#line 2471 "k.tab.c" /* yacc.c:1646  */
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



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
                      yytoken, &yylval);
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
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
  return yyresult;
}
#line 663 "keynote.y" /* yacc.c:1906  */


/*
 * Find all assertions signed by s and give us the one with the highest
 * return value.
 */
static int
resolve_assertion(char *s)
{
    int i, alg = KEYNOTE_ALGORITHM_NONE, p = 0;
    void *key = (void *) s;
    struct assertion *as;
    struct keylist *kl;

    kl = keynote_keylist_find(keynote_current_assertion->as_keylist, s);
    if (kl != (struct keylist *) NULL)
    {
	alg = kl->key_alg;
	key = kl->key_key;
    }

    for (i = 0;; i++)
    {
	as = keynote_find_assertion(key, i, alg);
	if (as == (struct assertion *) NULL)  /* Gone through all of them */
	  return p;

	if (as->as_kresult == KRESULT_DONE)
	  if (p < as->as_result)
	    p = as->as_result;

	/* Short circuit if we find an assertion with maximum return value */
	if (p == (keynote_current_session->ks_values_num - 1))
	  return p;
    }

    return 0;
}

/* 
 * Environment variable lookup. 
 */
static char *
my_lookup(char *s)
{
    struct keynote_session *ks = keynote_current_session;
    char *ret;

    if (!strcmp(s, "_MIN_TRUST"))
    {
	keynote_used_variable = 1;
	return ks->ks_values[0];
    }
    else
    {
	if (!strcmp(s, "_MAX_TRUST"))
	{
	    keynote_used_variable = 1;
	    return ks->ks_values[ks->ks_values_num - 1];
	}
	else
	{
	    if (!strcmp(s, "_VALUES"))
	    {
		keynote_used_variable = 1;
		return keynote_env_lookup("_VALUES", ks->ks_env_table,
					  HASHTABLESIZE);
	    }
	    else
	    {
		if (!strcmp(s, "_ACTION_AUTHORIZERS"))
		{
		    keynote_used_variable = 1;
		    return keynote_env_lookup("_ACTION_AUTHORIZERS",
					      ks->ks_env_table, HASHTABLESIZE);
		}
	    }
	}
    }

    /* Temporary list (regexp results) */
    ret = keynote_env_lookup(s, &keynote_temp_list, 1);
    if (ret != (char *) NULL)
      return ret;
    else
      if (keynote_errno != 0)
	return (char *) NULL;

    /* Local-Constants */
    ret = keynote_env_lookup(s, &keynote_init_list, 1);
    if (ret != (char *) NULL)
      return ret;
    else
      if (keynote_errno != 0)
	return (char *) NULL;

    keynote_used_variable = 1;

    /* Action environment */
    ret = keynote_env_lookup(s, ks->ks_env_table, HASHTABLESIZE);
    if (ret != (char *) NULL)
      return ret;
    else
      if (keynote_errno != 0)
	return (char *) NULL;

    /* Regex table */
    return keynote_env_lookup(s, &(ks->ks_env_regex), 1);
}

/*
 * If we had an exception, the boolean expression should return false.
 * Otherwise, return the result of the expression (the argument).
 */
static int
checkexception(int i)
{
    if (keynote_exceptionflag)
    {
	keynote_exceptionflag = 0;
	return 0;
    }
    else
      return i;
}


/* 
 * Integer exponentation -- copied from Schneier's AC2, page 244. 
 */
static int
intpow(int x, int y)
{
    int s = 1;
    
    /* 
     * x^y with y < 0 is equivalent to 1/(x^y), which for
     * integer arithmetic is 0.
     */
    if (y < 0)
      return 0;

    while (y)
    {
	if (y & 1)
	  s *= x;
	
	y >>= 1;
	x *= x;
    }

    return s;
}

/* 
 * Check whether the string is a floating point number. 
 */
static int
isfloatstring(char *s)
{
    int i, point = 0;
    
    for (i = strlen(s) - 1; i >= 0; i--)
      if (!isdigit((int) s[i]))
      {
	  if (s[i] == '.')
	  {
	      if (point == 1)
	        return 0;
	      else
	        point = 1;
	  }
	  else
	    return 0;
      }

    return 1;
}

/*
 * Initialize array for threshold search.
 */
static int
keynote_init_kth(void)
{
    int i = keynote_current_session->ks_values_num;
    
    if (i == -1)
      return -1;
    
    keynote_kth_array = (int *) calloc(i, sizeof(int));
    if (keynote_kth_array == (int *) NULL)
    {
	keynote_errno = ERROR_MEMORY;
	return -1;
    }

    return RESULT_TRUE;
}

/*
 * Get the k-th best return value.
 */
static int
get_kth(int k)
{
    int i;

    for (i = keynote_current_session->ks_values_num - 1; i >= 0; i--)
    {
	k -= keynote_kth_array[i];
	
	if (k <= 0)
	  return i;
    }

    return 0;
}

/*
 * Cleanup array.
 */
void
keynote_cleanup_kth(void)
{
    if (keynote_kth_array != (int *) NULL)
    {
	free(keynote_kth_array);
	keynote_kth_array = (int *) NULL;
    }
}

void
knerror(char *s)
{}
