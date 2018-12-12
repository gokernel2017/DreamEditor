//--------------------------------------------------------------------
//
// APPLICATION LANGUAGE.
//
// The Core:
//
//--------------------------------------------------------------------
//
#include "app.h"

#define STR_ERRO_SIZE   1024

static void   word_int      (LEXER *l, VM *vm);
static void   word_OBJECT   (LEXER *l, VM *vm);
static void   word_function (LEXER *l, VM *vm);
//
static int    stmt          (LEXER *l, VM *vm);
static int    see           (LEXER *l);
static void   execute_call  (LEXER *l, VM *vm, TFunc *func);
//
//----------------  Expression:  ----------------
static int    expr0         (LEXER *l, VM *vm);
static void   expr1         (LEXER *l, VM *vm);
static void   expr2         (LEXER *l, VM *vm);
static void   expr3         (LEXER *l, VM *vm);
static void   atom          (LEXER *l, VM *vm);
//-----------------------------------------------
static F_STRING *fs_new (char *s);
//
void lib_info (int arg);
//
// Set VM CallBack Function
//
void lib_SetCall (OBJECT *o, char *name);

static TFunc stdlib[]={
  //--------------------------------------------------------------------------
  // char*            char*       UCHAR*                  int   int   TFunc*
  // name             proto       code                    type  len   next
  //--------------------------------------------------------------------------
  { "SetCall",        "0ps",      (UCHAR*)lib_SetCall,  0,    0,    NULL },
  { "app_NewButton",  "ppiiis",   (UCHAR*)app_NewButton,  0,    0,    NULL },
  { "info",           "0i",       (UCHAR*)lib_info,       0,    0,    NULL },
  { NULL, NULL, NULL, 0, 0, NULL }
};

int erro; // global
TVar Gvar [GVAR_SIZE]; // global:

static TFunc  * Gfunc = NULL;
static VM     * vm_function = NULL;
static ARG      argument [20];
static F_STRING * fs = NULL;

static int
    is_function,
    is_recursive,
    local_count,
    argument_count,
    main_variable_type,
    var_type
    ;

static char
    strErro [STR_ERRO_SIZE],
    func_name [100]
    ;

static void expression (LEXER *l, VM *vm) {
    if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok=='(') {
        int i;
        TFunc *fi;
        int next;

        if (l->tok==TOK_NUMBER && strchr(l->token, '.'))
            main_variable_type = var_type = TYPE_FLOAT;
        else
            main_variable_type = var_type = TYPE_LONG; // 0

        next = see(l);

        //
        // call a function without return:
        //   function_name (...);
        //
        if ((fi = FuncFind(l->token)) != NULL) {
            execute_call(l, vm, fi);
      return;
        }

        if ((i = VarFind (l->token)) != -1) {

            main_variable_type = var_type = Gvar[i].type;

            if (next=='=') {
                lex_save(l); // save the lexer position
                lex(l); // =
                if (lex(l)==TOK_ID) {

                    //
                    // call a function with return:
                    //   i = function_name (...);
                    //
                    if ((fi = FuncFind(l->token)) != NULL) {
                        execute_call (l, vm, fi);

                        // The function return is stored in variable VALUE( eax ) ... see in file: vm.c
                        emit_mov_eax_var(vm,i);

                  return;
                    }//: if ((fi = FuncFind(l->token)) != NULL)

                }
                lex_restore (l); // restore the lexer position

            }// if (next=='=')

        }//: if ((i = VarFind (l->token)) != -1)

        //---------------------------------------
        // Expression types:
        //   a * b + c * d;
        //   10 * a + 3 * b;
        //   i;
        //---------------------------------------
        //
        if (expr0(l,vm) == -1) {
            emit_pop_eax (vm); // %eax | eax.i := expression result
            emit_print_eax (vm,main_variable_type);
        }// if (expr0(l,a) == -1)

    }
    else Erro("%s: %d | Expression ERRO - Ilegar Word (%s)\n", l->name, l->line, l->token);
}


static int expr0 (LEXER *l, VM *vm) {
    if (l->tok == TOK_ID) {
        int i;
        //---------------------------------------
        //
        // Expression type:
        //
        //   i = a * b + c;
        //
        //---------------------------------------
        if (see(l)=='=') {
            if ((i=VarFind(l->token)) != -1) {
                lex_save (l); // save the lexer position
                if (lex(l) == '=') {
                    lex(l);
                    expr1(l,vm);
                    // Copia o TOPO DA PILHA ( sp ) para a variavel ... e decrementa sp++.
                    emit_pop_var (vm,i);
              return i;
                } else {
                    lex_restore (l); // restore the lexer position
                }
            }//: if ((i=VarFind(l->token)) != -1)
        }//: if (see(l)=='=')
    }
    expr1(l,vm);
    return -1;
}

static void expr1 (LEXER *l, VM *vm) { // '+' '-' : ADDITION | SUBTRACTION
    int op;
    expr2(l,vm);
    while ((op=l->tok) == '+' || op == '-') {
        lex(l);
        expr2(l,vm);
        if (var_type==TYPE_FLOAT) {
//            if (op=='+') emit_add_float(a);
        } else { // LONG
            if (op=='+') emit_add_long(vm);
            if (op=='-') emit_sub_long(vm);
        }
    }
}
static void expr2 (LEXER *l, VM *vm) { // '*' '/' : MULTIPLICATION | DIVISION
    int op;
    expr3(l,vm);
    while ((op=l->tok) == '*' || op == '/') {
        lex(l);
        expr3(l,vm);
        if (var_type==TYPE_FLOAT) {
//            if (op=='*') emit_mul_float(a);
        } else { // LONG
            if (op=='*') emit_mul_long(vm);
            if (op=='/') emit_div_long(vm);
        }
    }
}
static void expr3 (LEXER *l, VM *vm) { // '('
    if (l->tok=='(') {
        lex(l); expr0(l,vm);
        if (l->tok != ')') {
            Erro("ERRO )\n");
        }
        lex(l);
    }
    else atom(l,vm); // atom:
}
static void atom (LEXER *l, VM *a) { // expres

    if (l->tok==TOK_STRING) {
        F_STRING *s = fs_new (l->token);
        if (s) {
            emit_push_string (a, s->s);
        }
        lex(l);
  return;
    }

    if (l->tok==TOK_ID) {
        int i;
/*
        TFunc *fi;
        //
        // push the pointer of function:
        //
        // NO CALL THE FUNCTION
        //
        if ((fi = FuncFind (l->token)) != NULL) {
            if (see(l) == '(') {
                // "execute the function"
//                execute_call (l,a,fi);
                // and ... push the result
//                emit_push_eax(a);
//                lex (l);
            } else if (see(l) != '(') {
                //
                // push the real function pointer
                //
                emit_mov_var_reg (a, &fi->code, EAX);
                emit_push_eax(a);
                lex (l);
            } 
        }
        else
*/
        if ((i = VarFind(l->token)) !=-1) {
            var_type = Gvar[i].type;
            emit_push_var (a, i);
            lex(l);
        }
        else Erro("%s: %d: - Expression atom, Ilegar Word: '%s'", l->name, l->line, l->token);
    }
    else if (l->tok==TOK_NUMBER) {
        if (strchr(l->token, '.'))
            var_type = TYPE_FLOAT;

        if (var_type==TYPE_FLOAT) {
//            emit_push_float(a, atof(l->token));
        } else {
            emit_push_long(a, atoi(l->token));
        }
        lex(l);
    }
    else Erro("%s: %d Expression atom - Ilegal Word (%s)\n", l->line, l->token);

}// atom ()


static void do_block (LEXER *l, VM *vm) {
    while (!erro && l->tok && l->tok != '}') {
        stmt(l,vm);
    }
    l->tok = ';';
}

static int stmt (LEXER *l, VM *vm) {

    lex(l);

    switch (l->tok) {
    case '{':
        l->level++;
        //----------------------------------------------------
        do_block(l,vm); //<<<<<<<<<<  no recursive  >>>>>>>>>>
        //----------------------------------------------------
        return 1;
    case TOK_INT:      word_int      (l,vm); return 1;
    case TOK_OBJECT:   word_OBJECT   (l,vm); return 1;
//    case TOK_FLOAT:    word_float    (l,a); return 1;
//    case TOK_VAR:      word_var      (l,a); return 1;
//    case TOK_IF:       word_if       (l,a); return 1;
//    case TOK_FOR:      word_for      (l,a); return 1;
//    case TOK_BREAK:    word_break    (l,a); return 1;
//    case TOK_RETURN:   word_return   (l,a); return 1;
    case TOK_FUNCTION: word_function (l,vm); return 1;
//    case TOK_MODULE:   word_module   (l,a); return 1;
//    case TOK_IMPORT:   word_import   (l,a); return 1;
//    case TOK_INCLUDE:  word_INCLUDE  (l,a); return 1;
//    case TOK_DEFINE:   word_DEFINE   (l,a); return 1;
//    case TOK_IFDEF:    word_IFDEF    (l,a); return 1;
    default:           expression    (l,vm); return 1;
    case '}': l->level--; return 1;
    case ';':
    case '#':
    case TOK_ENDIF:
        return 1;
    case 0: return 0;
    }
    return 1;
}

int app_LangParse (LEXER *lexer, VM *vm, char *text, char *name) {
    lex_set(lexer, text, name);
    ErroReset();
    vm_Reset(vm);
    emit_begin(vm);
    while (!erro && stmt(lexer,vm)) {
        // ... compiling ...
    }
    emit_end(vm);
    if (lexer->level) { // { ... }
        Erro ("\nERRO: LEXER->level { ... }: %d\n", lexer->level);
    }
    return erro;
}

//
// function_name (a, b, c + d);
//
static void execute_call (LEXER *l, VM *vm, TFunc *func) {
    int count = 0;
    int return_type = TYPE_LONG;

    // no argument
    if (func->proto && func->proto[1] == '0') {
        while (lex(l))
            if (l->tok == ')' || l->tok == ';') break;
    } else {
        // get next: '('
        if (lex(l)!='(') { Erro ("Function need char: '('\n"); return; }

        while (lex(l)) {

            if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok==TOK_STRING || l->tok=='(') {
            
                main_variable_type = var_type = TYPE_LONG;

                //
                // The result of expression is store in the "stack".
                //
                expr0 (l,vm);

                if (count++ > 15) break;
            }
            if (l->tok == ')' || l->tok == ';') break;
        }
    }

    if (count > 6) {
        Erro ("%s:%d: - Call Function(%s) the max arguments is: 5\n", l->name, l->line, func->name);
  return;
    }


    if (func->proto) {
        if (func->proto[0] == '0') return_type = TYPE_NO_RETURN;
        if (func->proto[0] == 'f') return_type = TYPE_FLOAT;
    }
    if (func->type == FUNC_TYPE_VM) {
        //
        // THE_SUMMER_BASE_CODE: <<<<<<<<<<  Not implemented  >>>>>>>>>>
        //
        // here: fi->code ==  ASM*
        //
        emit_call_vm (vm, (VM*)(func->code), (UCHAR)count, return_type);

    } else {

        emit_call (vm, func->code, (UCHAR)count, return_type);
    }
}


static void word_int (LEXER *l, VM *vm) {
    while (lex(l)) {
        if (l->tok==TOK_ID) {
            char name[255];
            int value = 0;

            strcpy (name, l->token); // save

            if (lex(l) == '=') {
                if (lex(l) == TOK_NUMBER)
                    value = atoi (l->token);
            }
            CreateVarLong (name, value);
        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(float) need the char(;) on the end\n");

}// word_int()

static void word_OBJECT (LEXER *l, VM *vm) {
    while (lex(l)) {
        if (l->tok==TOK_ID) {
            CreateVarOBJECT (l->token);
        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(OBJECT) need the char(;) on the end\n");

}// word_OBJECT ()


static void word_function (LEXER *l, VM *a) {
    TFunc *func;
    char name[255], proto[255] = { '0', 0, 0, 0, 0, 0, 0, 0 };
    int i;

    lex(l);

    strcpy (name, l->token);

    // if exist ... return
    //
    if (FuncFind(name)!=NULL) {
        int brace = 0;

        printf ("Function exist: ... REBOBINANDO '%s'\n", name);

        while (lex(l) && l->tok != ')');

        if (see(l)=='{') { } else Erro ("word(if) need start block: '{'\n");

        while (lex(l)){
            if (l->tok == '{') brace++;
            if (l->tok == '}') brace--;
            if (brace <= 0) break;
        }

  return;
    }

    // PASSA PARAMETROS ... IMPLEMENTADA APENAS ( int ) ... AGUARDE
    //
    // O analizador de expressao vai usar esses depois...
    //
    // VEJA EM ( expr3() ):
    // ---------------------
    // Funcoes usadas:
    //     ArgumentFind();
    //     asm_push_argument();
    // ---------------------
    //
    argument_count = 0;
    while (lex(l)) {

        if (l->tok==TOK_INT) {
            argument[argument_count].type[0] = TYPE_LONG; // 0
            //strcpy (argument[argument_count].type, "int");
            if (lex(l)==TOK_ID) {
                strcpy (argument[argument_count].name, l->token);
                strcat (proto, "i");
                argument_count++;
            }
        }
//        else if (l->tok==TOK_FLOAT) {
//            argument[argument_count].type[0] = TYPE_FLOAT; // 1
            //strcpy (argument[argument_count].type, "int");
//            if (lex(l)==TOK_ID) {
//                strcpy (argument[argument_count].name, l->token);
//                strcat (proto, "f");
//                argument_count++;
//            }
//        }
        else if (l->tok==TOK_ID) {
            argument[argument_count].type[0] = TYPE_UNKNOW;
            strcpy (argument[argument_count].name, l->token);
            strcat (proto, "i");
            argument_count++;
        }

        if (l->tok=='{') break;
    }
    if (argument_count==0) {
        proto[1] = '0';
        proto[2] = 0;
    }
    if (l->tok=='{') l->pos--; else { Erro("Word Function need char: '{'"); return; }

    is_function = 1;
    local_count = 0;
    strcpy (func_name, name);

    // compiling to buffer ( f ):
    //
    vm_Reset (vm_function);
    emit_begin (vm_function);
    //stmt (l,a); // here start from char: '{'
    stmt (l,vm_function); // here start from char: '{'
    emit_end (vm_function);

    if (erro) return;

    int len = vm_GetLen (vm_function);

    VM *vm;
    if ((vm = vm_New (len + 5)) != NULL) {
        // new function:
        //
        func = (TFunc*) malloc (sizeof(TFunc));
        func->name = strdup (func_name);
        func->proto = strdup (proto);
        func->type = FUNC_TYPE_VM;
        func->len = len;
        // NOW: copy the buffer ( f ):
        for (i=0;i<func->len;i++) {
            vm->code[i] = vm_function->code[i];
        }
        vm->code[func->len  ] = 0;
        vm->code[func->len+1] = 0;
        vm->code[func->len+2] = 0;
/*
        //-------------------------------------------
        // HACKING ... ;)
        // Resolve Recursive:
        // change 4 bytes ( func_null ) to this
        //-------------------------------------------
        if (is_recursive)
        for (i=0;i<func->len;i++) {
            if (vm->code[i]==OP_CALL && *(void**)(vm->code+i+1) == func_null) {
                vm->code[i] = OP_CALL_VM;     //<<<<<<<  change here  >>>>>>>
                *(void**)(vm->code+i+1) = vm; //<<<<<<<  change here  >>>>>>>
                i += 5;
            }
        }
*/
printf ("Criando Function(%s)\n", func_name);

        func->code = (UCHAR*)vm;

    } else {
        is_function = is_recursive = argument_count = *func_name = 0;
        return;
    }

    // add on top:
    func->next = Gfunc;
    Gfunc = func;

    is_function = is_recursive = argument_count = *func_name = 0;

}//:word_function ()


VM * app_LangInit (unsigned int size) {
    VM *vm = NULL;
    static int init = 0;
    if (init) return NULL;
    init = 1;
    if ((vm          = vm_New(size)) == NULL) return NULL;
    if ((vm_function = vm_New(size)) == NULL) return NULL;
    return vm;
}

void CreateVarLong (char *name, int value) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < GVAR_SIZE) {
        v->name = strdup(name);
        v->type = TYPE_LONG;
        v->value.l = value;
        v->info = NULL;
    }
}
void CreateVarOBJECT (char *name) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < GVAR_SIZE) {
        v->name = strdup(name);
        v->type = TYPE_POINTER;
        v->value.p = NULL;
        v->info = NULL;
    }
}


TFunc *FuncFind (char *name) {
    // array:
    TFunc *lib = stdlib;
    while (lib->name) {
        if ((lib->name[0]==name[0]) && !strcmp(lib->name, name))
      return lib;
        lib++;
    }
    // linked list:
    TFunc *func = Gfunc;
    while (func) {
        if ((func->name[0]==name[0]) && !strcmp(func->name, name))
      return func;
        func = func->next;
    }
    return NULL;
}

int VarFind (char *name) {
    TVar *v = Gvar;
    int i = 0;
    while(v->name) {
        if (!strcmp(v->name, name))
      return i;
        v++;
        i++;
    }
    return -1;
}

static int see (LEXER *l) {
    char *s = l->text+l->pos;
    while (*s) {
        if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
            s++;
        } else {
            if (s[0]=='=' && s[1]=='=') return TOK_EQUAL_EQUAL;
            if (s[0]=='+' && s[1]=='+') return TOK_PLUS_PLUS;
            if (s[0]=='+' && s[1]=='=') return TOK_PLUS_EQUAL;
            if (s[0]=='-' && s[1]=='-') return TOK_MINUS_MINUS;
            return *s;
        }
    }
    return 0;
}

static F_STRING *fs_new (char *s) {
    static int count = 0;
    F_STRING *p = fs, *n;

    while (p) {
        if (!strcmp(p->s,s)) return p;
        p = p->next;
    }

    if ((n = (F_STRING*)malloc(sizeof(F_STRING)))==NULL) return NULL;
    n->s = strdup(s);

//printf ("FIXED: %p\n", &n->s);

    n->i = count++;
    // add on top
    n->next = fs;
    fs = n;

    return n;
}

void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printf ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_LONG)   printf ("Gvar[%d](%s) = %ld\n", i, v->name, v->value.l);
            else
            if (v->type==TYPE_FLOAT) printf ("Gvar[%d](%s) = %f\n", i, v->name, v->value.f);
            else printf ("Gvar[%d](%s)\n", i, v->name);
            v++; i++;
        }
        } break;

    default:
        printf ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}

//
// Set VM CallBack Function
//
void lib_SetCall (OBJECT *o, char *name) {
    if (o && name) {
        // linked list:
        TFunc *func = Gfunc;
        while (func) {
            if ((func->name[0]==name[0]) && !strcmp(func->name, name)) {
                app_SetCallVM (o, (VM*)(func->code));
                break;
            }
            func = func->next;
        }
    }
}

void Erro (char *format, ...) {
    char msg[1024] = { 0 };
    va_list ap;

    va_start (ap,format);
    vsprintf (msg, format, ap);
    va_end (ap);
    if ((strlen(strErro) + strlen(msg)) < STR_ERRO_SIZE)
        strcat (strErro, msg);
    erro++;
}
char *ErroGet (void) {
    if (strErro[0])
        return strErro;
    else
        return NULL;
}
void ErroReset (void) {
    erro = 0;
    strErro[0] = 0;
}

