//-------------------------------------------------------------------
//
// OBJECT Edit Line Implementation:
//
//-------------------------------------------------------------------
//
#include "app.h"

typedef struct {
    char    *text;  // The text displayed.
    int     fg;     // The text color
    int     bg;     // The BackGround color
    int     d1;     // Len of memory alloc
    int     d2;     // Position of cursor
}DATA_EDIT;

SDL_Rect r;

int proc_edit (OBJECT *o, int msg, int value) {
    DATA_EDIT *data = app_GetData(o);
    int l, p, w, x;
    int b;
    int scroll;
    char *s = data->text;
    char buf[2];
    int cursor;

    if (!data) return 0;

    app_GetRect(o, &r);

    l = strlen(s);
    if (data->d2 > l) data->d2 = l;

    // calculate maximal number of displayable characters
    b = x = 0;
    if (data->d2 == l)  {
        buf[0] = ' '; buf[1] = 0;
        x = 8; //text_length(font, buf);
    }

    buf[1] = 0;
    for (p=data->d2; p>=0; p--) {
        buf[0] = s[p];
        b++;
        x += 8;//text_length(font, buf);
        if (x > r.w) break;
    }

    if (x <= r.w) {
        b = l; scroll = 0;
    }
    else {
        b--; scroll = 1;
    }

    switch (msg) {
    case MSG_DRAW:
        x = 0;

        if (scroll) {
            p = data->d2-b+1;
            b = data->d2;
        }
        else p = 0;

        SDL_FillRect (screen, &(SR) { r.x+1, r.y+1, r.w-2, r.h-2 }, data->bg);

        cursor = p;
        for (; p<=b; p++) {
            buf[0] = s[p] ? s[p] : ' ';
            w = 8;//text_length(font, buf);
            if (x+w >= r.w)
          break;
 	          DrawChar (screen, buf[0], 3+r.x+x, r.y+7, data->fg);
            x += w;
        }

        if (app_Focused(o) || value) {
            // cursor
            DrawVline (screen, 2+r.x+ (data->d2*8) - cursor*8 , r.y+2, r.y+r.h-3, data->fg);
            // ! double border
//            DrawRect (screen, r.x+1, r.y+1, r.w-2, r.h-2, data->fg ); // Border
            DrawRect (screen, r.x+1, r.y+1, r.w-3, r.h-3, COLOR_ORANGE); // border
        }

        DrawRectR (screen, r.x, r.y, r.w, r.h, data->fg); // border

        break;//: case MSG_DRAW

    case MSG_ENTER:
    case MSG_LEAVE:
        return 1;
/*
    case MSG_MOUSE_DOWN: {
        int x = o->px+2, i;
        for(i=0;i<=l;i++) {
            if (mx >= x && mx <= x+8) {
                if (scroll) p = data->d2-b+1; else p = 0;
                data->d2 = i+p;
                // draw this and display:
                o->proc (o, MSG_DRAW, 0);
                SDL_UpdateRect (screen, o->px, o->py, o->w, o->h);
          return 0;
            }
            x += 8; if (x > o->px+o->w) break;
        }
        } return 0;
*/
    case MSG_FOCUS:
        return 1; // object focused ok

    case MSG_KEY: {

        if (value > 280)
      return 0;

        if (value==SDLK_UP) {
            // none
        }
        else if (value==SDLK_DOWN) {
            // none
        }
        else if (value==SDLK_LEFT) {
            if (data->d2 > 0) data->d2--;
        }
        else if (value==SDLK_RIGHT) {
            if (data->d2 < l) data->d2++;
        }
        else if (value==SDLK_HOME) {
            data->d2 = 0;
        }
        else if (value==SDLK_END) {
            data->d2 = l;
        }
        else if (value==SDLK_DELETE) {
            if (data->d2 < l)
                for (p=data->d2; s[p]; p++)
                    s[p] = s[p+1];
        }
        else if (value==SDLK_BACKSPACE) {
            if (data->d2 > 0) {
                data->d2--;
                for (p=data->d2; s[p]; p++)
                    s[p] = s[p+1];
            }
        }
        else if (value == SDLK_RETURN || value == SDLK_TAB) {
            // bla bla bla ...
            return RET_CALL;
        }
        else {
            value &= 0xff;

            if ((value >= 32) && (value <= 255)) {
                if (l < data->d1) {
                    while (l >= data->d2) {
                        s[l+1] = s[l];
		                    l--;
                    }
                    s[data->d2] = value;
                    data->d2++;
                }
            }
            else return 0;
        }

//        if (o->callback)
//            o->callback (o, MSG_CHAR, value);

        // draw this and display: 
        app_ObjectUpdate (o);
//        o->proc (o, MSG_DRAW, 0);
//        SDL_UpdateRect (screen, o->px, o->py, o->w, o->h);

//        if (object_focus)
//            SDL_UpdateRect (screen, object_focus->px, object_focus->py, object_focus->w, object_focus->h);

        } break; // case MSG_CHAR:

    }// switch(msg)

    return 0;

}// proc_edit ()

OBJECT *app_NewEdit (OBJECT *parent, int id, int x, int y, char *text, int size) {
    OBJECT *o;
    DATA_EDIT *data;

    if ((data = (DATA_EDIT*)malloc(sizeof(DATA_EDIT))) == NULL)
  return NULL;

    const int len = strlen (text);

    data->fg = COLOR_ORANGE;
    data->bg = COLOR_WHITE;
    if (size <= len)
        size = len+1;
    data->d1 = size;
    data->d2 = 0;
    data->text = (char*)malloc (size);

    if (text)
        strcpy (data->text, text);

    o = app_ObjectNew (proc_edit, x, y, 320, 28, id, OBJECT_TYPE_EDIT, data);

    app_ObjectAdd (parent, o);

    return o;
}

char * app_EditGetText (OBJECT *o) {
    if (app_GetType(o) == OBJECT_TYPE_EDIT) {
        DATA_EDIT *data = app_GetData(o);
        if (data && data->text)
            return data->text;
    }
    return NULL;
}

