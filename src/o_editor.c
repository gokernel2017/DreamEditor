//-------------------------------------------------------------------
//
// OBJECT Editor Implementation:
//
//-------------------------------------------------------------------
//
#include "app.h"

//#define LINE_DISTANCE           15

#define SELECTED_COLOR      33808

// To state of sintax color
// CODE FROM: SciTE100 Editor 1.0
#define STATE_DEFAULT       0
#define STATE_COMMENT       1
#define STATE_LINE_COMMENT  2
#define STATE_DOC_COMMENT   3
#define STATE_NUMBER        4
#define STATE_WORD          5
#define STATE_STRING        6
#define STATE_CHAR          7
#define STATE_PUNCT         8
#define STATE_PRE_PROC      9
#define STATE_OPERATOR      10
#define STATE_IDENTIFIER    11

//-----------------------------------------------
// text color state:
//-----------------------------------------------
//
#define C_DEFAULT               64515 // orange
#define C_COMMENT               63519
#define C_COMMENT_LINE          63518
#define C_STRING                65504
#define C_PRE_PROC              2016
#define C_WORD                  2047

VM *main_vm = NULL;

static char         *str;
static SDL_Rect     r;
static int          state, color;
static int          is_reserved_word;
static char         is_selected; // selected text(SHIFT PRESSED)
static char         *selected_text = NULL;
int C_RED = 12345;

int count;

char iswordchar (char ch) {
    return isalnum(ch) || ch == '.' || ch == '_';
}

//-------------------------------------------------------------------
// This function is part of Scintilla:
//
// Copyright 1998-1999 by Neil Hodgson <neilh@hare.net.au>
// The License.txt file describes the conditions under which this software may be distributed.
//
// PROJECT  : SciTE100 Editor - 1.0
// FILE     : KeyWords.cc
// FUNCTION : inline bool isoperator(char ch)
//
//-------------------------------------------------------------------
//
char isoperator (char ch) {

  if (isalnum(ch)) return 0;

	// '.' left out as it is used to make up numbers
  if (ch == '%' || ch == '^' || ch == '&' || ch == '*' ||
     ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
     ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
     ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
     ch == '<' || ch == '>' || ch == ',' || ch == '/' ||
     ch == '?' || ch == '!' || ch == '.' || ch == '~'
  )
	return 1;

	return 0;
}

void app_EditorInsertChar (char *string, register int index, int ch) {
  register int x = strlen (string);

    while (x >= index){
        string[x+1] = string[x];
        x--;
    }
    string[index] = ch;
}

//-------------------------------------------------------------------
// Find a string in object EDITOR.
//-------------------------------------------------------------------
int app_EditorFindString (OBJECT *o, char *str, int start) {

    if (str && app_GetType(o) == OBJECT_TYPE_EDITOR) {
        DATA_EDITOR *data = app_GetData (o);
        int c, char1=0, index=0, pos=-1;
        char *find, *text = data->text;
        char *string = data->text;

        text += start;
        find = strstr(text, str);

        if (find) {

            pos = strlen(string)-strlen(find);

            for (c = 0; string[c]; c++) {
                if (string[c] == '\n') {
                    char1 = c + 1;  // Primeira LETRA A SER IMPRESSA
                    index++;    // LINHA
                }
                if (pos==c) break;
            }

            data->pos   = pos;      // Posicao real PARA INSERIR A LETRA
            //ed->char1 = char1;  // Primeira LETRA A SER IMPRESSA
            data->col = pos - char1;
            data->line = data->line_top = index;
            data->line_pos = 0;

//        ed->sel_start = pos;
//        ed->sel_len   = strlen(str)-1;
          return pos;
      }
  }

  return -1;

} // app_EditorFindString ()


void app_EditorSetFileName (OBJECT *o, char *FileName) {
    DATA_EDITOR *data = app_GetData (o);
    if (data && strlen(FileName) < EDITOR_FILE_NAME_SIZE-1) {
        sprintf (data->FileName, "%s", FileName);
    }
}

static void SetTextColor (void) {
    register char ch   = str[0];
    register char next = str[1];

    //---------------------------------------------------------------
    // CODE BASED: SciTE100 Editor 1.0
    //
    //     FILE: KeyWords.cc
    // FUNCTION: static void ColouriseCppDoc ( ... );
    //
    //   state := text_color
    //
    //---------------------------------------------------------------
    //
    if (state == STATE_DEFAULT) {

        color = C_DEFAULT;

        if (ch == '/' && next == '*') {  // COMMENT
            state = STATE_COMMENT; color = C_COMMENT;
        } else if (ch == '/' && next == '/') {  // LINE_COMMENT
            state = STATE_LINE_COMMENT; color = C_COMMENT;
        } else if (ch == '\"') { // STRING
            state = STATE_STRING; color = C_STRING;
        } else if (ch =='\'') { // CHAR
            state = STATE_CHAR; color = C_RED;
        } else if (ch == '#') { // PRE_PROC
            state = STATE_PRE_PROC; color = C_PRE_PROC;
        }
    } else {
        if (state == STATE_PRE_PROC){
            if ((ch == '\r' || ch == '\n') && (str[-1] != '\\')) {
                state = STATE_DEFAULT; color = C_DEFAULT;
            }
            if (ch=='/' && next=='/') {
                state = STATE_LINE_COMMENT; color = C_COMMENT;
            }
        } else if (state==STATE_COMMENT && str[-2]=='*' && str[-1]=='/'){
            state = STATE_DEFAULT; color = C_DEFAULT;
        } else if (state==STATE_LINE_COMMENT && (ch == '\r' || ch == '\n')) {
            state = STATE_DEFAULT; color = C_DEFAULT;
        } else if (state == STATE_STRING && (ch == '"'|| ch=='\n')) {
            state = STATE_DEFAULT; //color = data->color;
        } else if (state == STATE_CHAR && (ch =='\'' || ch=='\n')){
            state = STATE_DEFAULT; color = C_DEFAULT;
        }
    }
}

int proc_editor (OBJECT *o, int msg, int value) {
    DATA_EDITOR *data = app_GetData (o);
/*
    if (data==NULL) {
//        printf ("Editor Data NOT FOUND\n");
//        printf ("data ponteiro %p\n", &data);
        return 0;
    }
*/
    str = data->text;
    app_GetRect (o, &r);

    switch (msg) {

    case MSG_DRAW: {
        char buf[1024];
        int line_top = 0, i = 1;
        int pos_x = (r.x + 70) - data->scroll*8;
        int pos_y = r.y + 5;

        SDL_FillRect (screen, &r, data->bg); // bg
        SDL_FillRect (screen, &(SR){ r.x, r.y, 61, r.h}, MRGB(240,240,240)); // lines numbers bg
        DrawRect (screen, r.x, r.y, r.w, r.h, COLOR_ORANGE); // border

        data->line_count = 0;
        state = STATE_DEFAULT;
        color = C_DEFAULT;

        //-------------------------------
        // Get the FIRST char DISPLAYED
        // ed->line_count,
        //-------------------------------
        while (*str) {
            if (line_top == data->line_top)
          break;
            if (*str == '\n') { // <-- new line
                data->line_count++;
                if (line_top != data->line_top)
                    line_top++;
            }
            SetTextColor ();
            str++;
        }

        // NOW DRAW THE TEXT ( DrawChar(...) )
        //
        while (*str) {
            // size h:
            if (pos_y > (r.y + r.h)-EDITOR_LINE_DISTANCE)
          break;

            SetTextColor();

            // Draw char in area of editor
            if (pos_x < r.x+r.w-8) {

                // Selected TEXT - with SHIFT KEY:
                if (is_selected) {
                    int i = (int)(str-data->text);
                    if (i >= data->sel_start && i < data->sel_start+data->sel_len) {
                        SDL_FillRect (screen, &(SR){ pos_x, pos_y, 8, 15 }, SELECTED_COLOR);
                    }
                } 

                if (state == STATE_DEFAULT) { // ! is state not STATE_COMMENT

                  // CODE FROM: SciTE100 Editor 1.0
                  // If == '(' or ')' or '-' or '+' or ',' or '.' ... etc
                  if (isoperator(*str))
                      color = COLOR_WHITE;

                    //------- Color RESERVEDs WORDs: Language C -------
                    // Gets CHARs: SPACE, NEW LINE, '(', ';'
                    // In START of RESERVED WORDs
                    if (!is_reserved_word && !iswordchar(str[-1])) {

                        // If text[ch] == (First char of RESERVEDs WORDs): [b]reak, [c]ase, [s]truct ... etc
                        if ((*str >= 'a' && *str <= 'g') || *str=='i' || *str=='l' || *str=='o' || (*str >= 'r' && *str <= 'v') || *str=='w') {
                            char *s = str;
                            int count = 0, i;
                            char word [20];
                            char *WORDS[] = { "break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while",
                                              "and","end","function","in","local","or","repeat","then",0};
                            while (*s) {
                                word[count++] = *s++;
                                if (!iswordchar(*s) || count > 8) break;
                            }
                            word[count] = 0;
                            for (i = 0; WORDS[i]; i++)
                                if (!strcmp(WORDS[i], word)) {
                                    is_reserved_word = count; // <-- HERE: increment from size of WORD.
                              break;
                                }
                        }

                    }// if (!is_reserved_word && !iswordchar(str[-1]))
                    if (is_reserved_word) {
                        color = C_WORD; // Torn color of sintax
                        is_reserved_word--;
                    }

                } // if (state == STATE_DEFAULT)

                DrawChar (screen, *str, pos_x, pos_y, color);

            }// if (pos_x < r.x+r.w-8)
            pos_x += 8;

            if (*str == '\n') { // <-- New line
                // draw lines numbers
                sprintf (buf, "%04d", data->line_top + i); i++;
								DrawText (screen, buf, r.x+4, pos_y, COLOR_ORANGE);

                data->line_count++;
                pos_x = (r.x + 70) - data->scroll*8;
                pos_y += EDITOR_LINE_DISTANCE;
            }
            
            str++;

        }// while (*str)

        // Get "ed->line_count": continue incremeting "count_ch" at END(str)
        while (*str) {
            if (*str == '\n') data->line_count++;
            str++;
        }

        // draw cursor position
        DrawRect (screen,  (r.x+69+data->col*8) -data->scroll*8, r.y+4+data->line_pos * EDITOR_LINE_DISTANCE, 9, 14, COLOR_WHITE);
        DrawVline (screen, (r.x+69+data->col*8) -data->scroll*8, r.y+1, r.y+r.h-2, COLOR_WHITE);

        // display: LINE NUMBER, COL, ...
        //
        SDL_FillRect (screen, &(SR){ r.x+1, r.y+r.h-EDITOR_LINE_DISTANCE, r.w-2, EDITOR_LINE_DISTANCE}, 0); // BG: LINE: COL:
        DrawHline (screen, r.x+1, r.y+r.h-EDITOR_LINE_DISTANCE, r.x+r.w, COLOR_WHITE);
        if (data->FileName[0])
            sprintf (buf, "LINE: %d/%d  COL: %d - LEN/SIZE(%d/%d) | %d('%c' = %d) | FILE: %s", data->line+1, data->line_count, data->col+1, data->len, data->size, data->pos, data->text[data->pos], data->text[data->pos], data->FileName);
        else
            sprintf (buf, "LINE: %d/%d  COL: %d - LEN/SIZE(%d/%d) | %d('%c' = %d) | FILE: noname", data->line+1, data->line_count, data->col+1, data->len, data->size, data->pos, data->text[data->pos], data->text[data->pos]);
        if (data->saved)
            DrawText (screen, buf, r.x+5, r.y+r.h-15, COLOR_WHITE);
        else
            DrawText (screen, buf, r.x+5, r.y+r.h-15, C_COMMENT);

        } break; // case MSG_DRAW:

    case MSG_FOCUS:
        return 1; // object focused ok

    case MSG_KEY: {
        int count;

        // CTRL + R
        if (key_ctrl && keysym == 'r') {
            if (main_vm) {
                LEXER lexer;
                if (app_LangParse (&lexer, main_vm, data->text, "EDITOR") == 0) {
                    vm_Run(main_vm);
                }
                else app_ShowDialog (ErroGet(), DIALOG_OK);
            }
            return 0;
        }

        if (value == SDLK_UP && data->line > 0) {
            if (data->line_pos > 0)
                data->line_pos--;
            else
                data->line_top--;
            data->line--;
        }
        else if (value == SDLK_DOWN && data->line < data->line_count-1) {
            // if last line(DISPLAYED)
            if ((data->line_pos*EDITOR_LINE_DISTANCE)+34 >= r.h-14 && (data->line_pos*EDITOR_LINE_DISTANCE)+34 <= r.h+14)
                data->line_top++;
            else
                data->line_pos++;

            data->line++;
        }
        else if (value == SDLK_LEFT && data->col > 0) { // <--
            data->pos--; data->col--;
            if (data->scroll > 0)
                data->scroll--;
        }
        else if (value == SDLK_RIGHT && data->pos < data->len-1) { // -->
            if (str[data->pos] == '\n') { // if "current char" = new line

                // if last line(DISPLAYED)
                if ( (data->line_pos*EDITOR_LINE_DISTANCE)+34 >= r.h-14 && (data->line_pos*EDITOR_LINE_DISTANCE)+34 <= r.h+14 )
                    data->line_top++;
                else
                    data->line_pos++;

                data->line++; data->col = 0; // CURSOR(Linha AZUL) no inicio da LINHA
            } else {
                data->col++;
                if (data->col*8 > r.w-88) {
                    data->scroll++;
//printf ("scroll: %d\n", data->scroll);
                }
            }

            data->pos++;
        }
        else if (value == SDLK_PAGEUP){ // PAGINA A CIMA: O mesmo que SETA A ACIMA * Numero de LINHAS VISIVEIS
            for (count=0; count < (r.h/EDITOR_LINE_DISTANCE)-2; count++)
            if (data->line > 0) {
                if ( data->line_pos > 0 ) data->line_pos--; else data->line_top--;
                data->line--;
            }
        }
        else if (value == SDLK_PAGEDOWN) {// PAGINA A BAIXO: O mesmo que SETA A BAIXO * Numero de LINHAS VISIVEIS
            for (count=0; count < (r.h/EDITOR_LINE_DISTANCE)-2; count++)
            if (data->line < data->line_count) {
                if ( (data->line_pos*EDITOR_LINE_DISTANCE)+34 >= r.h-14 && (data->line_pos*EDITOR_LINE_DISTANCE)+34 <= r.h+14)
                    data->line_top++;
                else
                    data->line_pos++;

                data->line++;
            }
        }
        else if (value == SDLK_DELETE && data->pos < data->len-1) {
            data->len--;  
            data->saved = 0;
            for (count = data->pos; data->text[count]; count++)
                data->text[count] = data->text[count+1];
        }
        else if (value == SDLK_BACKSPACE && data->col > 0) {
            data->saved = 0;
            data->pos--; data->col--; data->len--;
            for (count = data->pos; data->text[count]; count++)
                data->text[count] = data->text[count+1];
        }
        else if (value == SDLK_TAB && data->len < data->size-2) {
            data->saved = 0;
            app_EditorInsertChar (data->text, data->pos, ' ');
            app_EditorInsertChar (data->text, data->pos, ' ');
            data->col += 2; data->pos += 2; data->len += 2;
        }
        else
        //-----------------------------------------------------------
        // INSERT CHAR
        //-----------------------------------------------------------
        if (data->len < data->size-1 && (value==SDLK_RETURN || (value >= 32 && value <= 126))) {
            if (value==SDLK_RETURN) {
                app_EditorInsertChar (data->text, data->pos, '\n');
              // if last line(DISPLAYED)
              //if ( (ED->line_pos*15)+15 >= O->h-12 && (ED->line_pos*15)+15 <= O->h+12 )
              if ( (data->line_pos*EDITOR_LINE_DISTANCE)+34 >= r.h-14 && (data->line_pos*EDITOR_LINE_DISTANCE)+34 <= r.h+14)
                  data->line_top++;
              else
                  data->line_pos++;

              data->line++; data->col = 0;
              data->text[ data->pos ] = '\n'; // New line: char 10
            } else {
                app_EditorInsertChar (data->text, data->pos, value);
                data->col++; if (data->col*8 > r.w-40) data->scroll++;// Increment the CURSOR
            }

            data->len++;
            data->pos++; // Increment New POSITION
            data->saved = 0;
        }

        //--------------------------------------------------------------
        // ED->POS: < < < < < < < <     S T A R T     > > > > > > > > >
        // Get New position of: "ED->POS"
        //--------------------------------------------------------------
        data->line_ini = data->line_len = count = 0;

        //----------------------------------------------
        // line_ini
        // Get Position of START of LINE NOW
        //----------------------------------------------
        while (*str != 0 && count != data->line) {
            if (*str=='\n') count++;
            data->line_ini++;
            str++;
        }

        //----------------------------------------------
        // line_len
        // Get the SIZE of the LINE NOW
        //----------------------------------------------
        while (*str != 0 && *str != '\n'){
            data->line_len++;
            str++;
        }

        if (data->col > data->line_len)
            data->col = data->line_len;

        data->pos = data->line_ini + data->col; // <-- CODE HERE
        //-------------------------------------------------------------
        // ED->POS: < < < < < < < < <     E N D     > > > > > > > > > >
        // Get New position of: "ED->POS"
        //-------------------------------------------------------------

        if (value == SDLK_HOME) {
            data->pos = data->line_ini; data->col = 0;
        }
        else if (value == SDLK_END) {
            data->pos = data->line_ini + data->line_len;
            data->col = data->line_len;
            data->scroll = data->line_len + 70 - (r.w/8);
        }

        // if "data->col" NAO ULTRAPASSA O "tamanho->w"
        if (data->col*8 < r.w-88)
            data->scroll = 0;

        if (key_ctrl) {
            if (value == CTRL_KEY_C && is_selected) {
                int i = data->sel_start;
                int count = 0;

                if (selected_text)
                    free (selected_text);

                if ((selected_text = malloc(data->sel_len+2)) != NULL) {
                    for (i = data->sel_start; i < data->sel_start+data->sel_len; i++) {
                        selected_text[count++] = data->text[i];
                    }
                    selected_text[count] = 0;
                }
            }
            else
            if (value == CTRL_KEY_V && selected_text) {
                char *s = selected_text;
                while (*s) {
                    if (data->len < data->size-1) {
                        app_EditorInsertChar (data->text, data->pos, *s);
                        data->len++;
                        data->pos++;
                        data->col++;
                    }
                    s++;
                }
            }
            else
            if (value == CTRL_KEY_Y) { // delete the line
                int _loop_, i;
                data->pos = data->line_ini; data->col = data->saved = 0;

                for (_loop_= 0; _loop_ <= data->line_len; _loop_++)
                    for (i = data->pos; data->text[i]; i++)
                        data->text[i] = data->text[i+1];

                data->len = strlen(data->text);
            }
        }
        else if (key_shift) {
            if (!is_selected) { data->sel_start = data->pos; }
            data->sel_len = abs(data->pos - data->sel_start);
            is_selected = 1;
        } else { is_selected = 0; data->sel_len = 0; }

        return RET_CALL;
        } break; // case MSG_KEY:

    case MSG_FREE:
        if (data) {
//printf ("MSG FREE --- data ponteiro %p\n", &data);

//            free (data->text);
//            data->text = NULL;
//            free (data);
            //data = NULL;
//            app_SetDataNULL (o);
        }
        break;

    case MSG_MOUSE_DOWN:
        return RET_CALL;

    }// switch (msg)

    return 0;

}// proc_editor()

OBJECT * app_NewEditor (OBJECT *parent, int id, int x, int y, char *text, int size) {
    OBJECT *o;
    DATA_EDITOR *data;

    if ((data = (DATA_EDITOR*)malloc(sizeof(DATA_EDITOR))) == NULL)
  return NULL;

    if ((data->text = malloc(size))==NULL) {
        printf ("ERRO: OBJECT EDITOR | text not allocated\n");
        return NULL;
    } else {
        printf ("OBJECT EDITOR | text allocated\n");
    }
    data->len = 0;
    if (text) {
/*
        int i = 0;
        while (text[i]) {
            if (i >= size-2) break;
            data->text[i] = text[i];
            i++;
        }
        data->text[i] = 0;
        data->len = i;
        data->saved = 1;
*/
        data->len = strlen(text);
        strcpy (data->text, text);
    } else {
        data->text[0] = ' ';
        data->text[1] = 0;
        data->text[2] = 0;
    }

    data->FileName[0] = 0;
    data->pos = 0;
    data->col = 0;
    data->scroll = 0;
    data->line = 0;
    data->line_top = 0;
    data->line_pos = 0;
    data->line_count = 0;
    data->saved = 1;
    //
    data->line_ini = 0;
    data->line_len = 0;
    data->sel_start = 0;  // text selected ... WHITH SHIFT KEY
    data->sel_len = 0;
    //
    data->size = size; // memory size text alloc
    data->bg = 8;

    o = app_ObjectNew (proc_editor, x, y, 320, 245, id, OBJECT_TYPE_EDITOR, data);

    app_ObjectAdd (parent, o);

    if (main_vm == NULL) {
        if ((main_vm = app_LangInit(VM_DEFAULT_SIZE))) {
            printf ("OBJECT EDITOR: VM (Virtual Machine) Created !!!\n");
        } else {
            printf ("OBJECT EDITOR: VM (Virtual Machine) NOT FOUND ... SORRY.\n");
        }
    }

    return o;
}

void app_EditorListFunction (OBJECT *o, MENU *menu) {
    DATA_EDITOR *data = app_GetData (o);
    if (app_GetType(o) == OBJECT_TYPE_EDITOR && data) {
        char *text = data->text;
        register int ch = 0;
        unsigned char comment = 0, comment_line = 0, string = 0;
//        int ret;

        if (!menu)
      return;

        app_MenuItenClear (menu);

        while (text[ch]) {

            if(text[ch]=='/' && text[ch+1]=='*') comment = 1;
            else
            if(text[ch-1]=='*' && text[ch]=='/') comment = 0;

            if(!comment){
                if(text[ch]=='\n') comment_line = 0;
                else
                if(text[ch]=='/' && text[ch+1]=='/') comment_line = 1;
            }

            if (!comment && !comment_line)
            if (text[ch]=='{' && (
               text[ch-1]==')'  ||  // "){"
               text[ch-2]==')'  ||  // ") {" or ")\n{"
               text[ch-3]==')'  )   // ") \n{"
            ) {
                int v;
                int pos = ch;

                //---------------------------------------------------
                // Decrement 'pos' to start of line of function name.
                //---------------------------------------------------
                v = 0;
                while (text[pos]) {

                    //---------------------------------------------------
                    // BECAUSE THIS: void any_func ( int (*proc)(int), int any) {
                    //---------------------------------------------------
                    if(text[pos]=='(') v++;
                    if(text[pos]==')') v--;

                    if(pos<ch-3)
                    if(v==0 && text[pos]=='\n'){ // The start of line
                      char buf[1024];
                      int cc = 0;

                        // Copy the function name to buf
                        for(v=pos+1; v != ch+1; v++) {
                            if(cc<1020) buf[cc++] = text[v];
                        }
                        buf[cc] = 0;
//                        printf ("FUNCTION(%s)\n", buf);
                        app_MenuItenAdd (menu, buf);

                    break;
                    }
                    pos--;
                }

                //---------------------------------------------------
                // Increment to end of function!
                //---------------------------------------------------
                v = 0;
                while (text[ch]) {
                    // For now only line comment.
                    if(text[ch]=='\n') comment_line = 0;
                    else
                    if(text[ch]=='/' && text[ch+1]=='/') comment_line = 1;

                    if(!string && (text[ch]==34/*"*/ || text[ch]==39/*'*/))
                        string = 1;
                    else if(string && (text[ch]==34/*"*/ || text[ch]==39/*'*/))
                        string = 0;

                    if(!comment_line && !string){
                        if(text[ch]=='{') v++;
                        if(text[ch]=='}') v--;
                    if(v==0) break;
                    }
                    ch++;
                }
            }
            ch++;
        }// while(text[ch])
    }
}//END: AS_medit_menu_function()


void app_EditorInsertText (OBJECT *o, char *text) {
    DATA_EDITOR *data = app_GetData(o);
    char *s = text;
    while (*s) {
        if (data->len < data->size-1) {
            app_EditorInsertChar (data->text, data->pos, *s);
            data->len++;
            data->pos++;
        }
        s++;
    }
    if (data->len < data->size-1) {
        app_EditorInsertChar (data->text, data->pos, '\n');
        data->len++;
        data->pos++;
    }
    SEND (o, MSG_KEY, SDLK_RIGHT);
}

void app_EditorFree (OBJECT *o) {
    DATA_EDITOR *data = app_GetData (o);
    if (app_GetType(o) == OBJECT_TYPE_EDITOR && data) {
        if (data->text)
            free (data->text);
        free (data);
        free (o);
        o = NULL;
        data = NULL;
    }
}

