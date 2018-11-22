//-------------------------------------------------------------------
//
// OBJECT Button Implementation:
//
// 1 - CALLBACK ENABLE ( MSG_MOUSE_UP ):
//
//-------------------------------------------------------------------
//
#include "app.h"

typedef struct {
    char  *text;
    int   fg;
}DATA_BUTTON;

int proc_button (OBJECT *o, int msg, int value) {
    DATA_BUTTON *data = app_GetData (o);

    switch (msg) {
    case MSG_DRAW: {
        SDL_Rect r;
        int i;

        app_GetRect (o, &r);

        for (i = 0; i < r.h-2; i++) {
            //        c2                  c1
            int _r = (236 * i / r.h) + (254 * (r.h - i) / r.h);
            int _g = (236 * i / r.h) + (254 * (r.h - i) / r.h);
            int _b = (236 * i / r.h) + (254 * (r.h - i) / r.h);

            int color = MRGB(_r,_g,_b);
            DrawHline (screen, r.x+1, (r.y+i)+1, r.x+r.w-2, color);
        }

        DrawRectR (screen, r.x, r.y, r.w, r.h, COLOR_ORANGE);

        if (value)
            DrawRect (screen, r.x+1, r.y+1, r.w-3, r.h-3, COLOR_ORANGE);

        int x = (r.x + r.w / 2) - ((strlen(data->text)*8)/2);
        DrawText (screen, data->text, x+1, (r.y+r.h/2)-6, data->fg);

        } break;


//    case MSG_MOUSE_DOWN:
    case MSG_MOUSE_UP:
        return RET_CALL;


    case MSG_FOCUS:
        return 0; // object no focused


    case MSG_ENTER:
    case MSG_LEAVE:
        return 1;         // enable to REdraw
//        return RET_CALL;  // enable to callback

    }// switch (msg)

    return 0;
}

OBJECT * app_NewButton (OBJECT *parent, int id, int x, int y, char *text) {
    OBJECT *o;
    DATA_BUTTON *data;

    if ((data = (DATA_BUTTON*)malloc(sizeof(DATA_BUTTON))) == NULL)
  return NULL;

    if (text)
        data->text = strdup (text);
    else
        data->text = strdup ("Button");
    data->fg = COLOR_ORANGE;

    o = app_ObjectNew (proc_button, x, y, 100, 28, id, OBJECT_TYPE_BUTTON, data);

    app_ObjectAdd (parent, o);

    return o;
}

