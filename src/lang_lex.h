//-------------------------------------------------------------------
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
//   02 - Fabrice Bellard: www.bellard.org
//
//   03 - Herbert Schildt: for expression based in the book ( C Completo e Total )
//
//   04 - Sam. L. - member of site ( www.vivaolinux.com.br )
//
// ----------------------------------------------
//
// THIS FILE IS PART OF SUMMER LANGUAGE:
//
// The Lexical Analyzer:
//
// FILE:
//   lex.h
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _LEX_H
#define _LEX_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc()

#ifdef __cplusplus
extern "C" {
#endif

#define LIBIMPORT extern

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define LEXER_NAME_SIZE   255
#define LEXER_TOKEN_SIZE  1024 * 4

LIBIMPORT void Erro (char *format, ...);

enum {
    TOK_INT = 255,
    TOK_OBJECT,
    TOK_FLOAT,
    TOK_VAR,
    TOK_IF,
    TOK_FOR,
    TOK_BREAK,
    TOK_RETURN,
    TOK_MODULE,
    TOK_IMPORT,
    TOK_FUNCTION,
    //-----------------------
    TOK_ID,
    TOK_STRING,
    TOK_NUMBER,
    //-----------------------
    TOK_INCLUDE,
    TOK_DEFINE,
    TOK_IFDEF,
    TOK_ENDIF,
    //-----------------------
    TOK_PLUS_PLUS,    // ++
    TOK_MINUS_MINUS,  // --
    TOK_PLUS_EQUAL,   // +=
    TOK_MINUS_EQUAL,  // -=
    TOK_EQUAL_EQUAL,  // ==
    TOK_NOT_EQUAL,    // !=
    TOK_AND_AND,      // &&
    TOK_PTR,          // ->
		//
		TOK_MENOR_EQUAL,	// <= esse
		TOK_MAIOR_EQUAL	// >=
};

typedef struct LEXER LEXER;

struct LEXER {
    char  *text;
    char  name  [LEXER_NAME_SIZE];
    char  token [LEXER_TOKEN_SIZE];
    //
    int   pos; // text [ pos ]
    int   tok;
    int   line;
    int   level; // in: '{' level++; | in '}' level--;
};

LIBIMPORT int   lex         (LEXER *lexer);

LIBIMPORT void  lex_set     (LEXER *lexer, char *text, char *name);

LIBIMPORT void  lex_save    (LEXER *l);

LIBIMPORT void  lex_restore (LEXER *l);

#ifdef __cplusplus
}
#endif

#endif // ! _LEX_H
