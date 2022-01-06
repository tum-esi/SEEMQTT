/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 20 "keynote.y" /* yacc.c:1909  */

    char   *string;
    double  doubval;
    int     intval;
    int     bool;

#line 151 "k.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE knlval;

int knparse (void);

#endif /* !YY_KN_K_TAB_H_INCLUDED  */
