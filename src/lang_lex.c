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
//   lex.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "lang_lex.h"
#include <ctype.h>

static char save_token [LEXER_TOKEN_SIZE];
static int  save_pos;
static int  save_tok;
static int  save_line;


//-------------------------------------------------------------------
//
//   return tok number or 0.
//
//-------------------------------------------------------------------
//
int lex (LEXER *l) {
    register char *p;
    register int c;
    int next; // next char

    p = l->token;
    *p = 0;

top:
    c = l->text[ l->pos ];

    //##############  REMOVE SPACE  #############
    //
    if (c <= 32) {
        if (c == 0) {
            l->tok = 0;
            return 0;
        }
        if (c == '\n') l->line++;
        l->pos++; //<<<<<<<<<<  increment position  >>>>>>>>>>
        goto top;
    }

    //################  STRING  #################
    //
    if (c == '"') {
        l->pos++; // '"'
        while ((c=l->text[l->pos]) && c != '"' && c != '\r' && c != '\n') {
            l->pos++;
            *p++ = c;
        }
        *p = 0;

        if (c=='"') l->pos++; else Erro("String erro");

        l->tok = TOK_STRING;
        return TOK_STRING;
    }

    //##########  WORD, IDENTIFIER ...  #########
    //
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        for (;;) {
            c = l->text[l->pos];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
                l->pos++;
                *p++ = c;
            } else break;
        }
        *p = 0;

        if (!strcmp(l->token, "int"))       { l->tok = TOK_INT;       return TOK_INT; }
        if (!strcmp(l->token, "OBJECT"))    { l->tok = TOK_OBJECT;    return TOK_OBJECT; }
        if (!strcmp(l->token, "float"))     { l->tok = TOK_FLOAT;     return TOK_FLOAT; }
        if (!strcmp(l->token, "var"))       { l->tok = TOK_VAR;       return TOK_VAR; }
        if (!strcmp(l->token, "if"))        { l->tok = TOK_IF;        return TOK_IF; }
        if (!strcmp(l->token, "for"))       { l->tok = TOK_FOR;       return TOK_FOR; }
        if (!strcmp(l->token, "break"))     { l->tok = TOK_BREAK;     return TOK_BREAK; }
        if (!strcmp(l->token, "return"))    { l->tok = TOK_RETURN;    return TOK_RETURN; }
        if (!strcmp(l->token, "module"))    { l->tok = TOK_MODULE;    return TOK_MODULE; }
        if (!strcmp(l->token, "import"))    { l->tok = TOK_IMPORT;    return TOK_IMPORT; }
        if (!strcmp(l->token, "function"))  { l->tok = TOK_FUNCTION;  return TOK_FUNCTION; }
        //
        if (!strcmp(l->token, "include"))   { l->tok = TOK_INCLUDE;   return TOK_INCLUDE; }
        if (!strcmp(l->token, "define"))    { l->tok = TOK_DEFINE;    return TOK_DEFINE; }
        if (!strcmp(l->token, "ifdef"))     { l->tok = TOK_IFDEF;     return TOK_IFDEF; }
        if (!strcmp(l->token, "endif"))     { l->tok = TOK_ENDIF;     return TOK_ENDIF; }

        l->tok = TOK_ID;
        return TOK_ID;
    }

    //#################  NUMBER  ################
    //
    if (c >= '0' && c <= '9') {
        for (;;) {
            c = l->text[l->pos];
            if ((c >= '0' && c <= '9') || c == '.') {
                l->pos++;
                *p++ = c;
            } else break;
        }
        *p = 0;
        l->tok = TOK_NUMBER;
        return TOK_NUMBER;
    }

    //##########  REMOVE COMMENTS  ##########
    //
    if (c == '/') {
        if (l->text[l->pos+1] == '*') { // comment block
            l->pos += 2;
            do {
                while ((c=l->text[l->pos]) && c != '*') {
                    if (c == '\n') l->line++; //<<<<<<<<<<  line++  >>>>>>>>>>
                    l->pos++;
                }
                if (c) {
                    l->pos++;
                    c = l->text[l->pos];
                }
            } while (c && c != '/');
            if (c == '/') l->pos++;
            else          Erro ("BLOCK COMMENT ERRO: '/'");
            goto top;

        } else if (l->text[l->pos+1] == '/') { // comment line
            l->pos += 2;
            while ((c=l->text[l->pos]) && (c != '\n') && (c != '\r'))
                l->pos++;
            goto top;
        }
    }//: if (c == '/')

    //---------------------------------------------------------------
    //################# ! C suported character ...  #################
    //---------------------------------------------------------------
    //
    next = l->text[ l->pos+1 ];

    if (c=='+') {
        if (next == '+') { // ++
            *p++ = '+'; *p++ = '+'; *p = 0;
            l->pos += 2;
            l->tok = TOK_PLUS_PLUS;
            return TOK_PLUS_PLUS;
        }
        if (next == '=') { // +=
            *p++ = '+'; *p++ = '='; *p = 0;
            l->pos += 2;
            l->tok = TOK_PLUS_EQUAL;
            return TOK_PLUS_EQUAL;
        }
    }
    if (c=='=' && next == '=') { // ==
        *p++ = '='; *p++ = '='; *p = 0;
        l->pos += 2;
        l->tok = TOK_EQUAL_EQUAL;
        return TOK_EQUAL_EQUAL;
    }
    if (c=='!' && next == '=') { // !=
        *p++ = '!'; *p++ = '='; *p = 0;
        l->pos += 2;
        l->tok = TOK_NOT_EQUAL;
        return TOK_NOT_EQUAL;
    }
    if (c=='&' && next == '&') { // &&
        *p++ = '&'; *p++ = '&'; *p = 0;
        l->pos += 2;
        l->tok = TOK_AND_AND;
        return TOK_AND_AND;
    }
    if (c=='-') {
        if (next=='-') {
            *p++ = '-'; *p++ = '-'; *p=0;
            l->pos += 2;
            l->tok = TOK_MINUS_MINUS;
            return TOK_MINUS_MINUS;
        }
        if (isdigit(next)) { // number: -100
            l->pos++;
            *p++ = c;
            for (;;) {
                c = l->text[l->pos];
                if ((c >= '0' && c <= '9') || c == '.') {
                    l->pos++;
                    *p++ = c;
                } else break;
            }
            *p = 0;
            l->tok = TOK_NUMBER;
            return TOK_NUMBER;
        }
    }

    *p++ = c;
    *p = 0;
    l->pos++;
    l->tok = c;

    return c;

}//: lex()

void lex_set (LEXER *l, char *text, char *name) {
    if (l && text) {
        l->pos = 0;
        l->line = 1;
        l->level = 0;
        l->text = text;
        if (name)
            strcpy (l->name, name);
    }
}//: lex_set()

void lex_save (LEXER *l) {
    sprintf (save_token, "%s", l->token);
    save_pos  = l->pos;
    save_tok  = l->tok;
    save_line = l->line;
}

void lex_restore (LEXER *l) {
    sprintf (l->token, "%s", save_token);
    l->pos  = save_pos;
    l->tok  = save_tok;
    l->line = save_line;
}
