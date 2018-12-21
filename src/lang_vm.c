//-------------------------------------------------------------------
//
// THANKS TO:
// ----------------------------------------------
//
//   01 : God the creator of the heavens and the earth in the name of Jesus Christ.
//
// ----------------------------------------------
//
// THIS FILE IS PART OF APPLICATION API:
//
// VM - The Virtual Machine Implementation:
//
// START DATE: 04/11/2018 - 07:00
//
//-------------------------------------------------------------------
//
#include "lang_vm.h"

#define STACK_SIZE    1024

static VALUE    stack [STACK_SIZE];
static VALUE  * sp = stack;
static VALUE    eax;
static int      callvm_stage2_position = 0;
static int			flag;

void callvm (VM *vm) {
    vm_Run (vm);
}

void vm_simule_push_long (long value) {
    sp++;
    sp->l = value;
}

VALUE * vm_Run (VM *vm) {

    vm->ip = 0;

    for (;;) {
    switch (vm->code[vm->ip++]) {

case OP_PUSH_LONG: {
    sp++;
    sp->l = *(long*)(vm->code+vm->ip);
    vm->ip += sizeof(long);
    } continue;

case OP_PUSH_FLOAT: {
    sp++;
    sp->f = *(float*)(vm->code+vm->ip);
    vm->ip += sizeof(float);
    } continue;

case OP_PUSH_VAR: {
    UCHAR i = (UCHAR)vm->code[vm->ip++];
    sp++;
    switch (Gvar[i].type) {
    case TYPE_LONG:    sp->l = Gvar[i].value.l; break;
    case TYPE_FLOAT:   sp->f = Gvar[i].value.f; break;
    case TYPE_POINTER: sp->p = Gvar[i].value.p; break;
    }
    } continue;

case OP_PUSH_STRING: {
    char *s = *(void**)(vm->code+vm->ip);
    vm->ip += sizeof(void*);
    sp++;
    sp->s = s;
    } continue;

case OP_POP_VAR: {
    UCHAR i = (UCHAR)vm->code[vm->ip++];
    switch (Gvar[i].type) {
    case TYPE_LONG:  Gvar[i].value.l = sp->l; break;
    case TYPE_FLOAT: Gvar[i].value.f = sp->f; break;
    }
    sp--;
    } continue;

case OP_MUL_LONG: sp[-1].l *= sp[0].l; sp--; continue;
case OP_DIV_LONG: sp[-1].l /= sp[0].l; sp--; continue;
case OP_ADD_LONG: sp[-1].l += sp[0].l; sp--; continue;
case OP_SUB_LONG: sp[-1].l -= sp[0].l; sp--; continue;

case OP_MUL_FLOAT: sp[-1].f *= sp[0].f; sp--; continue;
case OP_DIV_FLOAT: sp[-1].f /= sp[0].f; sp--; continue;
case OP_ADD_FLOAT: sp[-1].f += sp[0].f; sp--; continue;
case OP_SUB_FLOAT: sp[-1].f -= sp[0].f; sp--; continue;

case OP_POP_EAX: {
    eax = sp[0];
    sp--;
    } continue;

case OP_PRINT_EAX: {
    UCHAR i = (UCHAR)vm->code[vm->ip++];
    switch (i) {
    case TYPE_LONG:  printf ("%ld\n", eax.l); break;
    case TYPE_FLOAT: printf ("%f\n", eax.f); break;
    }
    } continue;

case OP_MOV_EAX_VAR: {
    UCHAR i = (UCHAR)vm->code[vm->ip++];
//    Gvar[i].value.l = eax.l;
    Gvar[i].value = eax;
    } continue;


case OP_CMP_LONG:
    sp--;
    flag = (int)(sp[0].l - sp[1].l);
    sp--;
    continue;

//
// simple_language_0.9.0
//
case OP_JUMP_JMP:
    vm->ip = *(unsigned short*)(vm->code+vm->ip);
    continue;

case OP_JUMP_JE: // !=
    if (!flag)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JUMP_JNE: // ==
    if (flag)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JUMP_JGE: // =<
    if (flag >= 0)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JUMP_JLE: // >=
    if (flag <= 0)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JUMP_JG:
    if (flag > 0)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JUMP_JL:
    if (flag < 0)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

//
// call a C Function
//
case OP_CALL:
    {
    int (*func)() = *(void**)(vm->code+vm->ip);
    float (*func_float)() = *(void**)(vm->code+vm->ip);
    vm->ip += sizeof(void*);
    UCHAR arg_count = (UCHAR)(vm->code[vm->ip++]);
    UCHAR return_type = (UCHAR)(vm->code[vm->ip++]);

    switch (arg_count) {
    case 0: // no argument
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func ();
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float ();
        else
            eax.l = func ();
        break; //: case 0:

    case 1: // 1 argument
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[0]);
        else
            eax.l = func (sp[0]);
        sp--;
        break; //: case 1:

    case 2: // 2 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-1], sp[0]);
        else
            eax.l = func (sp[-1], sp[0]);
        sp -= 2;
        break; //: case 2:

    case 3: // 3 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-2], sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-2], sp[-1], sp[0]);
        else
            eax.l = func (sp[-2], sp[-1], sp[0]);
        sp -= 3;
        break; //: case 3:

    case 4: // 4 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-3], sp[-2], sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-3], sp[-2], sp[-1], sp[0]);
        else
            eax.l = func (sp[-3], sp[-2], sp[-1], sp[0]);
        sp -= 4;
        break; //: case 4:

    case 5: // 5 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-4], sp[-3], sp[-2], sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-4], sp[-3], sp[-2], sp[-1], sp[0]);
        else
            eax.l = func (sp[-4], sp[-3], sp[-2], sp[-1], sp[0]);
        sp -= 5;
        break; //: case 5:

    }//: switch (arg_count)

    } continue; //: case OP_CALL:


// call a VM Function
//
case OP_CALL_VM: {
    VM *local = *(void**)(vm->code+vm->ip);
    vm->ip += sizeof(void*);
    UCHAR arg_count = (UCHAR)(vm->code[vm->ip++]); //printf ("CALL ARG_COUNT = %d\n", arg_count);
    UCHAR return_type = (UCHAR)(vm->code[vm->ip++]);

    switch(arg_count){
    case 1: local->arg[0] = sp[0]; sp--; break;
    case 2:
        local->arg[0] = sp[-1];
        local->arg[1] = sp[0];
        sp -= 2;
        break;
    case 3:
        local->arg[0] = sp[-2];
        local->arg[1] = sp[-1];
        local->arg[2] = sp[0];
        sp -= 3;
        break;
    case 4:
        local->arg[0] = sp[-3];
        local->arg[1] = sp[-2];
        local->arg[2] = sp[-1];
        local->arg[3] = sp[0];
        sp -= 4;
        break;
    case 5:
        local->arg[0] = sp[-4];
        local->arg[1] = sp[-3];
        local->arg[2] = sp[-2];
        local->arg[3] = sp[-1];
        local->arg[4] = sp[0];
        sp -= 5;
        break;

    }//: switch(arg_count)

    if (local == vm) {

        //-----------------------------------------------------------
        //
        // here is position the next opcode BEFORE of recursive function.
        //
        //-----------------------------------------------------------
        //
        callvm_stage2_position = local->ip;
        vm->ip = 0;
        //printf ("Todo antes da FUNCAO pos(%d) RECURSIVA CODE: %d\n", callvm_stage2_position, local->code[callvm_stage2_position]);
    } else {
        //printf ("PRIMEIRA VEZ EXECUTANDO\n");
        callvm_stage2_position = 0;
        local->ip = 0;
    }

    callvm (local);

    local->ip = callvm_stage2_position;
    //local->ip = a->ip - 5;

    } continue; //: case OP_CALL_VM:

case OP_HALT:
    vm->ip = 0;
    //printf ("VM sp: %d\n", (int)(sp - stack));
    return sp;
    }// switch (vm->code[vm->ip++])
    }// for (;;)

}// vm_run ()

VM * vm_New (unsigned int size) {
    VM *vm = (VM*)malloc (sizeof(VM));
    if (vm && (vm->code = (UCHAR*)malloc(size)) != NULL) {
        vm->p = vm->code;
        vm->label = NULL;
        vm->jump = NULL;
        vm->size = size;
        vm->ip = 0;
        vm->local = NULL;
        vm->local_count = 0;
        return vm;
    }
    return NULL;
}

void vm_Reset (VM *vm) {

    vm->p = vm->code;

    // reset ASM_label:
    while (vm->label != NULL) {
        VM_label *temp = vm->label->next;
        if (vm->label->name)
            free (vm->label->name);
        free (vm->label);
        vm->label = temp;
    }
    // reset ASM_jump:
    while (vm->jump != NULL) {
        VM_jump *temp = vm->jump->next;
        if (vm->jump->name)
            free(vm->jump->name);
        free (vm->jump);
        vm->jump = temp;
    }

    vm->label = NULL;
    vm->jump  = NULL;
    vm->ip = 0;
    vm->local_count = 0;
    //a->len = 0;
}

int vm_GetLen (VM *vm) {
    return (vm->p - vm->code);
}

void vm_Label (VM *vm, char *name) {
    if (name) {
        VM_label *lab;
        VM_label *l = vm->label;

        // find if exist:
        while (l) {
            if (!strcmp(l->name, name)) {
                printf ("Label Exist: '%s'\n", l->name);
                return;
            }
            l = l->next;
        }

        if ((lab = (VM_label*)malloc(sizeof(VM_label))) != NULL) {
            lab->name = strdup (name);
            lab->pos  = (vm->p - vm->code); // the index

            // add on top:
            lab->next = vm->label;
            vm->label = lab;
        }
    }
}

void emit_begin (VM *vm) {
}

void emit_end (VM *vm) {
    VM_label *label = vm->label;

    emit_halt(vm);

    //-----------------------
    // change jump:
    //-----------------------
    //
    while (label) {

        VM_jump *jump = vm->jump;

        while (jump) {
            if (!strcmp(label->name, jump->name)) {
                *(unsigned short*)(vm->code+jump->pos) = label->pos;
            }
            jump = jump->next;
        }
        label = label->next;
    }
}

void emit_halt (VM *vm) {
    *vm->p++ = OP_HALT;
}

void emit_push_long (VM *vm, long value) {
    *vm->p++ = OP_PUSH_LONG;
    *(long*)vm->p = value;
    vm->p += sizeof(long);
}
void emit_push_float (VM *vm, float value) {
    *vm->p++ = OP_PUSH_FLOAT;
    *(float*)vm->p = value;
    vm->p += sizeof(float);
}

void emit_push_var (VM *vm, UCHAR i) {
    *vm->p++ = OP_PUSH_VAR;
    *vm->p++ = i;
}
void emit_pop_var (VM *vm, UCHAR i) {
    *vm->p++ = OP_POP_VAR;
    *vm->p++ = i;
}

void emit_mul_long (VM *vm) {
    *vm->p++ = OP_MUL_LONG;
}
void emit_div_long (VM *vm) {
    *vm->p++ = OP_DIV_LONG;
}
void emit_add_long (VM *vm) {
    *vm->p++ = OP_ADD_LONG;
}
void emit_sub_long (VM *vm) {
    *vm->p++ = OP_SUB_LONG;
}

void emit_add_float (VM *vm) {
    *vm->p++ = OP_ADD_FLOAT;
}

void emit_pop_eax (VM *vm) {
    *vm->p++ = OP_POP_EAX;
}
void emit_print_eax (VM *vm, UCHAR type) {
    *vm->p++ = OP_PRINT_EAX;
    *vm->p++ = type;
}

void emit_mov_eax_var (VM *vm, UCHAR index) {
    *vm->p++ = OP_MOV_EAX_VAR;
    *vm->p++ = index;
}

void emit_push_string (VM *vm, char *s) {
    *vm->p++ = OP_PUSH_STRING;
    *(void**)vm->p = s;
    vm->p += sizeof(void*);
}

void emit_cmp_long (VM *vm) {
    *vm->p++ = OP_CMP_LONG;
}

void emit_jump_jmp (VM *vm, char *name) {
    VM_jump *jump;

    if (name && (jump = (VM_jump*)malloc (sizeof(VM_jump))) != NULL) {

        *vm->p++ = OP_JUMP_JMP;

        jump->name = strdup (name);
        jump->pos  = (vm->p - vm->code); // the index

        // add on top:
        jump->next = vm->jump;
        vm->jump = jump;

        // to change ...
        *(unsigned short*)vm->p = (jump->pos+2); // the index
        vm->p += sizeof(unsigned short);
    }
}

static void conditional_jump (VM *vm, char *name, UCHAR type) {
    VM_jump *jump;

    if (name && (jump = (VM_jump*)malloc (sizeof(VM_jump))) != NULL) {

        *vm->p++ = type;

        jump->name = strdup (name);
        jump->pos  = (vm->p - vm->code); // the index

        // add on top:
        jump->next = vm->jump;
        vm->jump = jump;

        // to change ...
        *(unsigned short*)vm->p = (jump->pos+2); // the index
        vm->p += sizeof(unsigned short);
    }
}

void emit_jump_je (VM *vm, char *name) {
    conditional_jump (vm, name, OP_JUMP_JE);
}
void emit_jump_jne (VM *vm, char *name) {
    conditional_jump (vm, name, OP_JUMP_JNE);
}

void emit_jump_jl (VM *vm, char *name) {
    conditional_jump (vm, name, OP_JUMP_JL);
}
void emit_jump_jg (VM *vm, char *name) {
    conditional_jump (vm, name, OP_JUMP_JG);
}


void emit_jump_jle (VM *vm, char *name) {
    conditional_jump (vm, name, OP_JUMP_JLE);
}
void emit_jump_jge (VM *vm, char *name) {
    conditional_jump (vm, name, OP_JUMP_JGE);
}

void emit_call (VM *vm, void *func, UCHAR arg_count, UCHAR return_type) {
    *vm->p++ = OP_CALL;
    *(void**)vm->p = func;
    vm->p += sizeof(void*);
    *vm->p++ = arg_count;
    *vm->p++ = return_type;
}

void emit_call_vm (VM *vm, void *func, UCHAR arg_count, UCHAR return_type) {
    *vm->p++ = OP_CALL_VM;
    *(void**)vm->p = func;
    vm->p += sizeof(void*);
    *vm->p++ = arg_count;
    *vm->p++ = return_type;
}

