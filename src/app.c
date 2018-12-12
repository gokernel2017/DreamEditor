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
// The Main Core:
//
// START DATE: 04/11/2018 - 07:00
//
//-------------------------------------------------------------------
//
#include "app.h"

#define DIALOG_TEXT_SIZE  100
#define ID_NO             0
#define ID_YES            1
#define ID_OK             2

struct OBJECT { // opaque struct
    void      *data; // any information about object
    int       (*proc) (OBJECT *o, int msg, int value);
    void      (*call) (int msg); // user function callback
    VM        *vm_call;
    short     x;
    short     y;
    SDL_Rect  rect; //  real position from the gui ... this is computed from parent
    int       id;
    char      type;
    char      focus;
    char      visible;
    OBJECT    *parent;
    OBJECT    *first;
    OBJECT    *next;
};

typedef struct {
    char  text [DIALOG_TEXT_SIZE];
    int   fg;
    int   bg;
}DATA_DIALOG;

static void draw_bg (void);
static void app_ObjectMouseFind (OBJECT *o);
static void app_ObjectDrawAll (OBJECT *o);
static void app_UpdatePos (OBJECT *obj);

//-----------------------------------------------
//-----------------  VARIABLES  -----------------
//-----------------------------------------------
//
SDL_Surface *screen;
int key_ctrl;
int key_shift;
int key;
int keysym;
int mx, my; // mouse_x, mouse_y

static int running = 0, id_object;
DATA_DIALOG dialog_data;

// Dialog:
//-----------------------------------------------
static OBJECT * dialog_root = NULL;
static OBJECT * dialog      = NULL;
static OBJECT * dlgYES      = NULL;
static OBJECT * dlgNO       = NULL;
static OBJECT * dlgOK       = NULL;
//
static OBJECT *file_dialog_root;
static OBJECT *file_dialog;
static OBJECT *file_dialog_EDIT;
static OBJECT *file_dialog_OK;
static OBJECT *file_dialog_CANCEL;
//-----------------------------------------------
static OBJECT * root         = NULL; // the root object
static OBJECT * current      = NULL; // the current object
static OBJECT * mouse_find   = NULL; // object on mouse
static OBJECT * object_focus = NULL; // focused object
static OBJECT * object_click = NULL;
//-----------------------------------------------

static int
    state,
    quit,
    dialog_quit, dialog_ret
    ;

void _call_ (void) {
    SDL_Delay (10);
}
void (*CallBack) (void) = _call_;

static int proc_null (OBJECT *o, int msg, int i) {
    return 0;
}

int app_Init (int argc, char **argv) {
    static int init = 0;
    int w = 0, h = 0, flags = SDL_HWSURFACE, i;

    if (init) return 1;
    init = 1;
    SDL_Init (SDL_INIT_VIDEO);
    SDL_WM_SetCaption ("Application API:", NULL);
    #ifdef _WIN32
    SDL_putenv ("SDL_VIDEO_CENTERED=center");
    #endif

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-w") && argc > i)
            w = atoi(argv[i+1]);
        if (!strcmp(argv[i], "-h") && argc > i)
            h = atoi(argv[i+1]);
        if (!strcmp(argv[i], "-noframe"))
           flags |= SDL_NOFRAME;

    }
    if (w <= 0) w = 800;
    if (h <= 0) h = 600;

    screen = SDL_SetVideoMode (w, h, 16, flags);

    SDL_EnableUNICODE (1);
    SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // For keypressed

    if ((root = app_ObjectNew (proc_null,0,0,0,0,0,0,NULL)) == NULL)
  return 0;

    atexit (SDL_Quit);

    return 1;
}

void app_UpdateGui (OBJECT *o) {
    SDL_Event ev;
    int ret;

    while (SDL_PollEvent(&ev)) {
    switch (ev.type) {
    case SDL_MOUSEMOTION:
        mx = ev.motion.x;
        my = ev.motion.y;

        //---------------------------------------
        // set: mouse_find
        //---------------------------------------
        mouse_find = NULL;
        app_ObjectMouseFind (o); // <<<<<<< set mouse_find >>>>>>>

        //-----------------------------------------------
        //-----------  MSG_ENTER / MSG_LEAVE  -----------
        //-----------------------------------------------
        //
        // set object 'current':
        //
        //-----------------------------------------------
        if (mouse_find != current) {
            // MSG_ENTER
            if (mouse_find && (ret = mouse_find->proc (mouse_find,MSG_ENTER,0))) {
                // redraw and update
                mouse_find->proc (mouse_find, MSG_DRAW, 1);
                SDL_UpdateRect (screen, mouse_find->rect.x, mouse_find->rect.y, mouse_find->rect.w, mouse_find->rect.h);
                // send callback : MSG_ENTER
                if (ret == RET_CALL && mouse_find->call) {
                    mouse_find->call (MSG_ENTER);
                }
            }
            // MSG_LEAVE
            if (current && (ret = current->proc (current,MSG_LEAVE,0))) {
                // redraw and update
                current->proc (current, MSG_DRAW, 0);
                SDL_UpdateRect (screen, current->rect.x, current->rect.y, current->rect.w, current->rect.h);
                // send callback : MSG_LEAVE
                if (ret == RET_CALL && current->call) {
                    current->call (MSG_LEAVE);
                }
            }

            current = mouse_find;

        }// if (mouse_find != current)

        break;//: case SDL_MOUSEMOTION:

    //-----------------------------------------------
    // On Mouse Click ENTER: MSG_FOCUS
    //-----------------------------------------------
    case SDL_MOUSEBUTTONDOWN:
        mx = ev.button.x;
        my = ev.button.y;

        if (current) {

            //
            // set object clicked ... to use in event: SDL_MOUSEBUTTONUP.
            //
            object_click = current;

            if (current != object_focus && current->proc (current, MSG_FOCUS, 0)) {
                // draw the old
                if (object_focus) {
                    object_focus->focus = 0;
                    object_focus->proc (object_focus, MSG_DRAW, 0);
                    SDL_UpdateRect (screen, object_focus->rect.x, object_focus->rect.y, object_focus->rect.w, object_focus->rect.h);
                }
                current->focus = 1;
                // now set object_focus:
                object_focus = current;
                object_focus->proc (object_focus, MSG_DRAW, 1);
                SDL_UpdateRect (screen, object_focus->rect.x, object_focus->rect.y, object_focus->rect.w, object_focus->rect.h);
            }
            if (current->proc (current, MSG_MOUSE_DOWN, 0) == RET_CALL && current->call) {
                id_object = current->id;
                current->call (MSG_MOUSE_DOWN);
            }
        }// if (current)

        break;// case SDL_MOUSEBUTTONDOWN:

    case SDL_MOUSEBUTTONUP:

        if (object_click && object_click == current) {
            if (object_click->proc (object_click, MSG_MOUSE_UP, 0) == RET_CALL) {
                id_object = object_click->id;
                if (object_click->call) {
                    object_click->call (MSG_MOUSE_UP);
                }
                if (object_click->vm_call) {
                    vm_simule_push_long (MSG_MOUSE_UP);
                    vm_Run (object_click->vm_call);
                }
            }
        }
        object_click = NULL;

        break;// case SDL_MOUSEBUTTONUP:

    case SDL_KEYDOWN:
        keysym = ev.key.keysym.sym;
        if ((key = ev.key.keysym.unicode)==0)
            key = ev.key.keysym.sym;

        if (key == SDLK_RCTRL || key == SDLK_LCTRL) {
            key_ctrl = 1;
        }
        else
        if (key == SDLK_RSHIFT || key == SDLK_LSHIFT) {
            key_shift = 1;
        }
        else
        if (key == SDLK_ESCAPE && o == root) {
            quit = app_ShowDialog("Application API - Exit ?", 0);
        }

        // !!!!!!! To Linux !!!!!!!
        if (ev.key.keysym.sym == SDLK_BACKSPACE)
            key = SDLK_BACKSPACE;

        if (object_focus && object_focus->visible && object_focus->focus) {
            if ((ret = object_focus->proc (object_focus, MSG_KEY, key))) {
                // redraw and update
                object_focus->proc (object_focus, MSG_DRAW, 1);
                SDL_UpdateRect (screen, object_focus->rect.x, object_focus->rect.y, object_focus->rect.w, object_focus->rect.h);
                if (ret == RET_CALL && object_focus->call) {
                    object_focus->call (MSG_KEY);
                }
            }
        }

        break; // case SDL_KEYDOWN:

    case SDL_KEYUP: {
        int k;
        if ((k = ev.key.keysym.unicode)==0)
            k = ev.key.keysym.sym;

        if (k == SDLK_RCTRL || k == SDLK_LCTRL) {
            key_ctrl = 0;
        }
        else
        if (k == SDLK_RSHIFT || k == SDLK_LSHIFT) {
            key_shift = 0;
        }

        } break; // case SDL_KEYUP:

    }// switch (ev.type)
    }// while (SDL_PollEvent(&ev))

    if (state==RET_REDRAW_ALL) {
        state = 0;
        if (o == root)
            draw_bg ();
        app_ObjectDrawAll (o);
        SDL_Flip (screen);
    }

}// app_UpdateGui ()

void app_Run (void (*call) (void)) {

    if (call)
        CallBack = call;

    app_UpdatePos (root);
    running = 1;
    state = RET_REDRAW_ALL;
    quit = 0;

    while (!quit) {

        app_UpdateGui (root);
        CallBack ();

    }
}

static void draw_bg (void) {
    int color1 = SDL_MapRGB (screen->format, 254,238,204);
    int color2 = SDL_MapRGB (screen->format, 0,130,214);
    int hh = screen->h/3;

    SDL_FillRect (screen, &(SR){ 1, 1, screen->w-1, hh }, color1);
//    SDL_FillRect (screen, &(SR){ 1, hh, screen->w-1, hh }, COLOR_BLUE2);
    SDL_FillRect (screen, &(SR){ 1, hh, screen->w-1, hh }, color2);
    SDL_FillRect (screen, &(SR){ 1, hh+hh, screen->w-1, hh }, color1);

    SDL_FillRect (screen, &(SR){ 1, hh-20, screen->w-1, 20 }, COLOR_ORANGE);
    SDL_FillRect (screen, &(SR){ 1, hh+hh, screen->w-1, 20 }, COLOR_ORANGE);

    DrawText (screen, "To Exit Press The KEY: ESC", 100, (screen->h/2), COLOR_WHITE);
}

OBJECT * app_ObjectNew (
    int (*proc) (OBJECT *o, int msg, int value),
    int   x, int y, int w, int h,
    int   id,
    char  type,
    void *data
) {
    OBJECT *o;

    if ((o = (OBJECT*)malloc(sizeof(OBJECT)))==NULL)
  return NULL;
    o->proc = proc;
    o->call = NULL;
    o->vm_call = NULL;
    o->x = x;
    o->y = y;
    o->rect.w = w;
    o->rect.h = h;
    o->id = id;
    o->type = type;
    o->focus = 0;
    o->visible = 1;
    o->parent = NULL;
    o->first = NULL;
    o->next = NULL;
    o->data = data;
    return o;
}

void * app_GetData (OBJECT *o) {
    return o->data;
}

void app_SetDataNULL (OBJECT *o) {
    o->data = NULL;
}

void app_GetRect (OBJECT *o, SDL_Rect *rect) {
    *rect = o->rect;
}
int app_GetType (OBJECT *o) {
    return o->type;    
}


int app_Focused (OBJECT *o) {
    return o->focus;
}

void app_ObjectAdd (OBJECT *o, OBJECT *sub) {

    if (o==NULL) o = root;

    if (o && sub) {
        sub->parent = o;
        if (!o->first) {
            o->first = sub; // the index to first object
        } else {
            OBJECT *aux = o->first; // the index to first object

            while (aux->next != NULL)
                aux = aux->next;
            aux->next = sub;
        }
        state = RET_REDRAW_ALL;
        if (running)
            app_UpdatePos (o);
    }
}

OBJECT * app_GetByID (int id) {
    OBJECT *sub = root->first;
    while (sub) {
        if (sub->id == id)
      return sub;
        if (sub->first) {
            app_GetByID (id);
        }
        sub = sub->next;
    }
    return NULL;
}

static void app_ObjectMouseFind (OBJECT *obj) {
    OBJECT *sub = obj->first;
    while (sub) {
        if (sub->visible && mx > sub->rect.x && mx < sub->rect.x+sub->rect.w && my > sub->rect.y && my < sub->rect.y+sub->rect.h) {
            mouse_find = sub;
            if (sub->first) {
                app_ObjectMouseFind (sub);
            }
        }
        sub = sub->next;
    }
}
static void app_ObjectDrawAll (OBJECT *obj) {
    OBJECT *sub = obj->first;
    while (sub) {
        if (sub->visible) {
            sub->proc (sub, MSG_DRAW, 0);
            if (sub->first) {
                app_ObjectDrawAll (sub);
            }
        }
        sub = sub->next;
    }
}

void app_ObjectUpdate (OBJECT *o) {
    if (o && o->visible) {
        o->proc (o,MSG_DRAW,1);
        SDL_UpdateRect (screen, o->rect.x, o->rect.y, o->rect.w, o->rect.h);
    }
}

static void app_UpdatePos (OBJECT *obj) {
    OBJECT *o = obj->first;
    while (o) {
        OBJECT *par = o;
        int x, y;
        x = par->x;
        y = par->y;
        while (par->parent) {
            par = par->parent;
            x += par->x;
            y += par->y;
        }
        //-----------------
        // update position:
        //-----------------
        o->rect.x = x;
        o->rect.y = y;
        //-----------------

        if (o->first) {
            app_UpdatePos (o);
        }

    o = o->next;
    }
}

void app_SetSize (OBJECT *o, int w, int h) {
    if (w > 0) o->rect.w = w;
    if (h > 0) o->rect.h = h;
}

void app_SetFocus (OBJECT *o) {
    if (o && o->visible) {
        // set old UNFOCUS
        if (object_focus)
            object_focus->focus = 0;
        o->focus = 1;
        object_focus = o;
    }
}

void app_SetCall (OBJECT *o, void (*call) (int msg)) {
    if (o)
        o->call = call;
}
void app_SetCallVM (OBJECT *o, VM *vm) {
    if (o && vm)
        o->vm_call = vm;
}


void app_SetVisible (OBJECT *o, int visible) {
    o->visible = visible;
}

int proc_dialog (OBJECT *o, int msg, int value) {
    if (msg == MSG_DRAW) {
        SDL_Rect r;
        int x;
        char *s = dialog_data.text;

        app_GetRect(o, &r);
        SDL_FillRect (screen, &(SR){ r.x+1, r.y+1, r.w-2, r.h-2 }, dialog_data.bg);
        DrawRectR (screen, r.x, r.y, r.w, r.h, dialog_data.fg);
        DrawRect (screen, r.x+1, r.y+1, r.w-3, r.h-3, dialog_data.fg);
        //DrawWindow (&r);
        x = r.x+12;
        while (*s) {
            DrawChar (screen, *s, x, r.y+12, dialog_data.fg);
            x += 8;
            if (x > r.x+r.w) break;
            s++;
        }
        return 0;
    }
    if (msg == MSG_KEY) {
        if (value == 'Y' || value == 'y' || value == SDLK_RETURN) {
            dialog_quit = 1;
            dialog_ret = 1;
        }
        if (value == 'N' || value == 'n' || value == SDLK_ESCAPE) {
            dialog_quit = 1;
            dialog_ret = 0;
        }
    }

    return 0;
}

void call_dialog (int msg) {
    dialog_quit = 1;
    dialog_ret = id_object;
}

int app_ShowDialog (char *text, int ok) {

    if (dialog_root == NULL) {
        if ((dialog_root = app_ObjectNew (proc_null,0,0,0,0,0,0,NULL)) != NULL) {
            dialog_data.fg = COLOR_ORANGE;
            dialog_data.bg = COLOR_WHITE;
            dialog = app_ObjectNew (proc_dialog,(screen->w/2)-250,(screen->h/2)-50,500,100,0,0,&dialog_data);
            app_ObjectAdd (dialog_root, dialog);
            dlgYES = app_NewButton (dialog, ID_YES, 142, 55, "YES");
            dlgNO  = app_NewButton (dialog, ID_NO,  258, 55, "NO");
            dlgOK  = app_NewButton (dialog, ID_OK,  200, 55, "OK");
            app_SetCall (dlgYES, call_dialog);
            app_SetCall (dlgNO, call_dialog);
            app_SetCall (dlgOK, call_dialog);
        }
    }

    if (dialog_root && dialog) {
        OBJECT *old_focus;

        app_UpdatePos (dialog_root);
        state = RET_REDRAW_ALL;
        dialog_ret = 0;
        dialog_quit = 0;
        dialog->visible = 1;

        if (ok) { // Show with OK BUTTON
            dlgOK->visible = 1;
            dlgYES->visible = 0;
            dlgNO->visible = 0;
        } else {
            dlgOK->visible = 0;
            dlgYES->visible = 1;
            dlgNO->visible = 1;
        }

        if (text && strlen(text) < DIALOG_TEXT_SIZE-1) {
            sprintf (dialog_data.text, "%s", text);
        } else {
            dialog_data.text[0] = 0;
        }

        current = mouse_find = NULL;
        old_focus = object_focus; // save object focus
        app_SetFocus (dialog);

        while (!dialog_quit) {

            app_UpdateGui (dialog_root);
            SDL_Delay (10);

        }
        key = 0;

        current = mouse_find = NULL;
        object_focus = old_focus; // restore object focus
        if (object_focus)
            app_SetFocus (object_focus);
        dialog->visible = 0;
        quit = 0;

        state = RET_REDRAW_ALL;
        
    }

    return dialog_ret;
}

void call_edit_file_dialog (int msg) {
    if (key == SDLK_RETURN) {
        dialog_ret = 1;
        dialog_quit = 1;
    }
    else
    if (key == SDLK_ESCAPE) {
        dialog_ret = 0;
        dialog_quit = 1;
    }
}

int app_FileDialog (char const *title, char path[1024]) {
    if (file_dialog_root == NULL) {
        if ((file_dialog_root = app_ObjectNew (proc_null,0,0,0,0,0,0,NULL)) != NULL) {
            dialog_data.fg = COLOR_ORANGE;
            dialog_data.bg = COLOR_WHITE;
            file_dialog = app_ObjectNew (proc_dialog,(screen->w/2)-250,(screen->h/2)-50,500,110,0,0,&dialog_data);
            app_ObjectAdd (file_dialog_root, file_dialog);

            file_dialog_EDIT = app_NewEdit (file_dialog, 0, 12, 30, "File Name Edit", 1024);
            file_dialog_OK = app_NewButton (file_dialog, ID_YES, 142, 70, "OK");
            file_dialog_CANCEL  = app_NewButton (file_dialog, ID_NO,  258, 70, "CANCEL");
            //
            app_SetCall (file_dialog_EDIT, call_edit_file_dialog);
            app_SetCall (file_dialog_OK, call_dialog);
            app_SetCall (file_dialog_CANCEL, call_dialog);
            app_SetSize (file_dialog_EDIT, file_dialog->rect.w-27, 0);
        }
    }

    if (file_dialog_root && file_dialog) {
        OBJECT *old_focus;

        app_UpdatePos (file_dialog_root);
        state = RET_REDRAW_ALL;
        dialog_ret = 0;
        dialog_quit = 0;

        if (path)
            app_EditSetText (file_dialog_EDIT, path);

        if (title && strlen(title) < DIALOG_TEXT_SIZE-1) {
            sprintf (dialog_data.text, "%s", title);
        } else {
            dialog_data.text[0] = 0;
        }

        current = mouse_find = NULL;
        old_focus = object_focus; // save object focus
        app_SetFocus (file_dialog_EDIT);

        while (!dialog_quit) {
            app_UpdateGui (file_dialog_root);
            SDL_Delay (10);
        }
        key = 0;

        current = mouse_find = NULL;
        object_focus = old_focus; // restore object focus
        if (object_focus)
            app_SetFocus (object_focus);
        quit = 0;
        state = RET_REDRAW_ALL;
    }
    
    sprintf (path, "%s", app_EditGetText(file_dialog_EDIT));

    return dialog_ret;
}

char * app_FileOpen (const char *FileName) {
    FILE *fp;

    if ((fp = fopen (FileName, "r")) != NULL) {
        char *str;
        int size, i;

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        str = (char *)malloc (size + 5);
        if(!str){
            fclose (fp);
            return NULL;
        }
        i = fread(str, 1, size, fp);
        fclose(fp);
        str[i] = 0;
        str[i+1] = 0;

printf ("FileOpen STR ponteiro %p = %d\n", &str, (int)str);

        return str;
    }
    else printf ("File Not Found: '%s'\n", FileName);

    return NULL;
}

int app_SendMessage (OBJECT *o, int msg, int value) {
    if (o->visible)
        return o->proc (o,msg,value);
    return 0;
}

void app_ObjectSetTop (OBJECT *o) {
    OBJECT *aux     = root->first;
    OBJECT *aux_pre = aux; // Node previous to 'p'

    if (o == root->first) // If 'p' node is yet the first node, nothing to do
  return;

        // The aim of the loop is to get the node previous to 'p'
    while (aux != o) {
        aux_pre = aux;
        aux     = aux->next;
    } // At the end of this loop, 'aux' is the node 'p' and 'aux_pre' is the node previous to 'p'

    aux_pre->next = o->next;  // Links 'p' previous and next nodes
    o->next = root->first;   // 'p' next node is now the previous first node
    root->first = o;         // The first node becomes 'p'

//    state = RET_REDRAW_ALL;

}//END: AS_win_set_top()

