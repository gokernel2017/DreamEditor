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
// The Main Header:
//
// START DATE: 04/11/2018 - 07:00
//
//-------------------------------------------------------------------
//
#ifndef _APP_H_
#define _APP_H_

#include <SDL/SDL.h>

#ifdef WIN32
    #include "io.h"
    #undef main
#else
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
#endif

// LANGUAGE:
#include "lang_lex.h"
#include "lang_vm.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
//#define LIBIMPORT         extern
#define BMP               SDL_Surface
#define SEND              app_SendMessage
//
#define COLOR_ORANGE      64512
#define CONSOLE_BG        8 // blue
#define SR                struct SDL_Rect
#define MRGB(r,g,b)       SDL_MapRGB(screen->format, r, g, b)
#define COLOR_BORDER      11184810
#define COLOR_PURPURE     12402852
#define COLOR_WHITE       16777215
#define COLOR_BLUE2       33237
#define COLOR_RED         63488
#define COLOR_OBG         15724527  // object bg
#define COLOR_WORD        2047
#define COLOR_GREEN       2016

// CTRL + KEY:
#define CTRL_KEY_A        1   // CTRL + A
#define CTRL_KEY_C        3   // CTRL + C
#define CTRL_KEY_O        15  // CTRL + O
#define CTRL_KEY_S        19  // CTRL + S
#define CTRL_KEY_Y        25  // CTRL + Y
#define CTRL_KEY_V        22  // CTRL + V

//
// object send message
#define MSG_DRAW          1
#define MSG_KEY           2
#define MSG_CHAR          3   // fucused object editable
#define MSG_FOCUS         4   // set focus
#define MSG_MOUSE_DOWN    5   // click on enter
#define MSG_MOUSE_UP      6   // click on leave
#define MSG_ENTER         7   // on mouse enter
#define MSG_LEAVE         8   // on mouse leave
#define MSG_FREE          9
// "proc_object" return:
#define RET_REDRAW_ALL    10  // redraw all objects
#define RET_REDRAW        11  // redraw the current object
#define RET_CALL          12

#define DIALOG_OK         1
//
#define EDITOR_FILE_NAME_SIZE 255
#define EDITOR_LINE_DISTANCE  17
#define CONSOLE_TEXT_SIZE     1024

enum {
    OBJECT_TYPE_OBJECT = 1,
    OBJECT_TYPE_BUTTON,
    OBJECT_TYPE_EDIT,
    OBJECT_TYPE_EDITOR,
//    OBJECT_TYPE_MENU
    OBJECT_TYPE_CONSOLE
};
enum {
    FUNC_TYPE_NATIVE_C = 0,
    FUNC_TYPE_VM,
    FUNC_TYPE_MODULE     // .dll | .so
};

//-----------------------------------------------
//-------------------  STRUCT  ------------------
//-----------------------------------------------
//
typedef struct OBJECT       OBJECT; // opaque struct in file: "app.c"
//typedef struct ARG          ARG;    // OBJECT Function Callback Argument
typedef struct DATA_EDITOR  DATA_EDITOR;
typedef struct TRect        TRect;
typedef struct MENU         MENU;
typedef struct MENU_ITEN    MENU_ITEN;
// LANGUAGE:
typedef struct TFunc        TFunc;

/*
struct ARG { // OBJECT Function Callback Argument
    int   msg;
    int   id;
    int   x;
    int   y;
    int   key;
};
*/
struct TRect {
    short   x,  y;
    short   w,  h;
};
struct MENU {
    int   w;
    int   h;
    int   index;
    int   top;
    int   pos_y;
    int   count;
    int   button_h;
    MENU_ITEN *iten_first;
};
struct MENU_ITEN {
    char  *text;
    MENU_ITEN *next;
};
struct DATA_EDITOR {
    char  *text; // use malloc (size)
    char  FileName [EDITOR_FILE_NAME_SIZE];
    int   pos;      // position in text: text [ pos ];
    int   col;      // x cursor position
    int   scroll;
    int   line;
    int   line_top; // first line displayed
    int   line_pos;
    int   line_count;
    //
    int   line_ini;
    int   line_len;
    //
    int   sel_start;  // text selected ... WHITH SHIFT KEY
    int   sel_len;
    //
    int   len;        // string len
    int   size;       // memory size text alloc
    int   saved;
    int   bg;         // bg color
};
struct TFunc {
    char    *name;
    char    *proto; // prototype
    UCHAR   *code;  // the function on JIT MODE | or VM in VM MODE
    int     type;   // FUNC_TYPE_NATIVE_C = 0, FUNC_TYPE_COMPILED, FUNC_TYPE_VM
    int     len;
    TFunc   *next;
};

//-----------------------------------------------
//-----------------  VARIABLES  -----------------
//-----------------------------------------------
//
LIBIMPORT SDL_Surface *screen;
LIBIMPORT int key_ctrl;
LIBIMPORT int key_shift;
LIBIMPORT int key;
LIBIMPORT int keysym;
LIBIMPORT int mx, my; // mouse_x, mouse_y

//-----------------------------------------------
//-----------------  PUBLIC API  ----------------
//-----------------------------------------------
//
// app.c | The Main Core:
//
LIBIMPORT int       app_Init          (int argc, char **argv);
LIBIMPORT void      app_Run           (void (*call) (void));
LIBIMPORT void    * app_GetData       (OBJECT *o);
LIBIMPORT void      app_SetDataNULL   (OBJECT *o);
LIBIMPORT void      app_GetRect       (OBJECT *o, SDL_Rect *rect);
LIBIMPORT int       app_GetType       (OBJECT *o);
LIBIMPORT OBJECT  * app_GetByID       (int id);
LIBIMPORT char    * app_FileOpen      (const char *FileName);
LIBIMPORT void      app_SetFocus      (OBJECT *o);
LIBIMPORT void      app_SetSize       (OBJECT *o, int w, int h);
LIBIMPORT void      app_SetVisible    (OBJECT *o, int visible);
LIBIMPORT int       app_Focused       (OBJECT *o);
LIBIMPORT void      app_SetCall       (OBJECT *o, void (*call) (int msg));
LIBIMPORT void      app_ObjectAdd     (OBJECT *o, OBJECT *sub);
LIBIMPORT void      app_ObjectUpdate  (OBJECT *o); // draw and display
LIBIMPORT void      app_ObjectSetTop  (OBJECT *o);
LIBIMPORT int       app_ShowDialog    (char *text, int ok);
LIBIMPORT int       app_FileDialog    (char const *title, char path[1024]);
LIBIMPORT int       app_SendMessage   (OBJECT *o, int msg, int value);
//
LIBIMPORT OBJECT * app_ObjectNew (
    int   (*proc) (OBJECT *o, int msg, int value),
    int   x, int y, int w, int h,
    int   id,
    char  type,
    void  *data
    );

LIBIMPORT OBJECT * app_NewButton  (OBJECT *parent, int id, int x, int y, char *text);
LIBIMPORT OBJECT * app_NewEdit    (OBJECT *parent, int id, int x, int y, char *text, int size);
LIBIMPORT OBJECT * app_NewEditor  (OBJECT *parent, int id, int x, int y, char *text, int size);
LIBIMPORT OBJECT * app_NewMenu    (OBJECT *parent, int id, int x, int y);
LIBIMPORT OBJECT * app_NewConsole (OBJECT *parent, int id, int x, int y, char *text);

LIBIMPORT void app_EditorInsertChar (char *string, register int index, int ch);
LIBIMPORT void app_EditorInsertText (OBJECT *o, char *text);

// Editor Functions:
//
LIBIMPORT void    app_EditorSetFileName (OBJECT *o, char *FileName);
LIBIMPORT int     app_EditorFindString (OBJECT *o, char *str, int start);
LIBIMPORT void    app_EditorListFunction (OBJECT *o, MENU *menu);
LIBIMPORT void    app_EditorFree (OBJECT *o);


// Edit Functions:
LIBIMPORT void    app_EditSetText (OBJECT *o, char *text);
LIBIMPORT char  * app_EditGetText (OBJECT *o);

LIBIMPORT char *  app_ConsoleTextChanged (OBJECT *o);
LIBIMPORT void    app_ConsoleAdd (OBJECT *o, char *text, int color);
LIBIMPORT void    app_ConsoleClear (OBJECT *o);
//
// menu.c
//
LIBIMPORT MENU  * app_MenuCreate (int w, int h);
LIBIMPORT void    app_MenuItenAdd (MENU *menu, char *text);
LIBIMPORT void    app_MenuItenClear (MENU *menu);
LIBIMPORT char *  app_Menu (MENU *menu, int x, int y, void (*call) (MENU *menu, char *text));

//
// draw.c | Drawing Primitive:
//
LIBIMPORT void  DrawPixel   (BMP *bmp, int x, int y, int color);
LIBIMPORT void  DrawHline   (BMP *bmp, int x1, int y, int x2, int color);
LIBIMPORT void  DrawVline   (BMP *bmp, int x, int y1, int y2, int color);
LIBIMPORT void  DrawRect    (BMP *bmp, int x, int y, int w, int h, int color);
LIBIMPORT void  DrawRectR   (BMP *bmp, int x, int y, int w, int h, int color);
LIBIMPORT void  DrawChar    (BMP *bmp, char ch, int x, int y, int color);
LIBIMPORT void  DrawText    (BMP *bmp, char *text, int x, int y, int color);
LIBIMPORT void  DrawWindow  (SDL_Rect *rect);


// ... TEMP ...
//
LIBIMPORT void app_PrintData (OBJECT *o);


//-----------------------------------------------
//------------------  LANGUAGE  -----------------
//-----------------------------------------------
//
LIBIMPORT VM    * app_LangInit  (unsigned int size);
LIBIMPORT int     app_LangParse (LEXER *l, VM *vm, char *text, char *name);
LIBIMPORT void    CreateVarLong (char *name, int value);
LIBIMPORT TFunc * FuncFind      (char *name);
LIBIMPORT int     VarFind       (char *name);
// erro:
LIBIMPORT void    Erro          (char *format, ...);
LIBIMPORT char  * ErroGet       (void);
LIBIMPORT void    ErroReset     (void);


#ifdef __cplusplus
}
#endif
#endif // ! _APP_H_

