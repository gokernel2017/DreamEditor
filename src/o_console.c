//-------------------------------------------------------------------
//
// OBJECT Console Implementation:
//
//-------------------------------------------------------------------
//
#include "app.h"

#define DISTANCE            17  // line distance
#define CONSOLE_TEXT_SIZE   1024

typedef struct ITEN ITEN;

typedef struct {
    char  text[CONSOLE_TEXT_SIZE+1];
    int   line_top;
    int   count;
    int   col; // d2
    ITEN  *iten_first;
    ITEN  *iten_last;
}DATA_CONSOLE;

struct ITEN {
    char  *text;
    int   color;
    ITEN  *next;
};


int proc_console (OBJECT *o, int msg, int value) {
    DATA_CONSOLE *data = app_GetData(o);
    SDL_Rect r;
    int l, p, w, x;
    int b;
    int scroll;
    char *s = data->text;
    char buf[2];
    int cursor;

    if (!data) return 0;

    app_GetRect(o, &r);

    l = strlen(s);
    if (data->col > l) data->col = l;

    // calculate maximal number of displayable characters
    b = x = 0;
    if (data->col == l)  {
        buf[0] = ' '; buf[1] = 0;
        x = 8; //text_length(font, buf);
    }

    buf[1] = 0;
    for (p=data->col; p>=0; p--) {
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
    case MSG_DRAW: {
        ITEN *iten = data->iten_first;
        char buf[20];
        int top = 0, pos_y;


        SDL_FillRect (screen, &(SR) {r.x+1, r.y+1, r.w-2, r.h-2 }, 0);
        DrawVline (screen, r.x+52, r.y, r.y+r.h-30, COLOR_ORANGE);

        pos_y = r.y;
        while (iten) {
            if (pos_y > (r.y+r.h)-50)
          break;
            if (top >= data->line_top) {
                char *s = iten->text;
                int x = r.x+64;
                sprintf (buf, "%04d", top+1);
                DrawText (screen, buf, r.x+10, pos_y+10, COLOR_ORANGE);
                while (*s) {
                    if (x > (r.x+r.w)-16) break;
                    DrawChar (screen, *s, x, pos_y+10, iten->color);
                    x += 8;
                    s++;
                }
                pos_y += DISTANCE;
            }
            top++;
            iten = iten->next;
        }
        DrawHline (screen, r.x, r.y+r.h-28, r.x+r.w-1, COLOR_ORANGE);
        //DrawText (screen, data->text, r.x+10, r.y+r.h-21, COLOR_GREEN);
        DrawRectR (screen, r.x, r.y, r.w, r.h, COLOR_ORANGE); // border
        sprintf (buf, "%d", data->count);
        DrawText (screen, buf, (r.x+r.w)-(strlen(buf)*8)-10, r.y+10, COLOR_ORANGE);

        // draw edit text:
        //-----------------------------------------------------------
        x = 0;

        if (scroll) {
            p = data->col-b+1;
            b = data->col;
        }
        else p = 0;

        cursor = p;
        for (; p<=b; p++) {
            buf[0] = s[p] ? s[p] : ' ';
            w = 8;//text_length(font, buf);
            if (x+w >= r.w)
          break;
 	          DrawChar (screen, buf[0], 10+r.x+x, r.y+r.h-21, COLOR_GREEN);
            x += w;
        }

        if (app_Focused(o) || value) {
            // cursor
//            DrawVline (screen, 2+r.x+ (data->col*8) - cursor*8 , r.y+2, r.y+r.h-3, COLOR_WHITE);
            DrawVline (screen, 9+r.x+ (data->col*8) - cursor*8 , r.y+r.h-25, r.y+r.h-2, COLOR_WHITE);
        }

        } break;

    case MSG_FOCUS:
        return 1; // object focused ok

    case MSG_KEY:

        if (value > 280)
      return 0;

        if (value == SDLK_UP) {
            if (data->line_top >= 1) {
                data->line_top--;
                app_ObjectUpdate(o);
            }
        }
        else if (value == SDLK_DOWN) {
            if (data->line_top < data->count-1) {
                data->line_top++;
                app_ObjectUpdate(o);
            }
        }
        else if (value == SDLK_LEFT) {
            if (data->col > 0) data->col--;
        }
        else if (value == SDLK_RIGHT) {
            if (data->col < l) data->col++;
        }
        else if (value == SDLK_HOME) {
            data->col = 0;
        }
        else if (value == SDLK_END) {
            data->col = l;
        }
        else if (value == SDLK_DELETE) {
            if (data->col < l)
                for (p=data->col; s[p]; p++)
                    s[p] = s[p+1];
        }
        else if (value == SDLK_BACKSPACE) {
            if (data->col > 0) {
                data->col--;
                for (p=data->col; s[p]; p++)
                    s[p] = s[p+1];
            }
        }
        else if (value == SDLK_RETURN) {
            FILE *fp;
            char buf[1024];
            if (!strcmp(data->text, "clear") || !strcmp(data->text, "cls")) {
                app_ConsoleClear (o);
                app_ObjectUpdate (o);
                return 0;
            }
            sprintf (buf, "%s 2>&1", data->text);
            data->line_top = data->count;
            app_ConsoleAdd (o, data->text, COLOR_GREEN);
            if ((fp = popen (buf, "r")) != NULL) {
                while (fgets(buf, sizeof(buf), fp) != NULL) {
                    app_ConsoleAdd (o, buf, COLOR_ORANGE);
                }
                pclose (fp);
                app_ObjectUpdate (o);
            }
        }
        else {
            value &= 0xff;

            if ((value >= 32) && (value <= 255)) {
                if (l < CONSOLE_TEXT_SIZE) {
                    while (l >= data->col) {
                        s[l+1] = s[l];
		                    l--;
                    }
                    s[data->col] = value;
                    data->col++;
                }
            }
        }

        // draw this and display: 
        app_ObjectUpdate (o);
        break;

    case MSG_MOUSE_DOWN:
        return RET_CALL;

    }
    return 0;
}

OBJECT * app_NewConsole (OBJECT *parent, int id, int x, int y, char *text) {
    OBJECT *o;
    DATA_CONSOLE *data;

    if ((data = (DATA_CONSOLE*)malloc(sizeof(DATA_CONSOLE))) == NULL)
  return NULL;

    if (text && strlen(text) < CONSOLE_TEXT_SIZE)
        sprintf(data->text, "%s", text);
    else
        data->text[0] = 0;

    data->line_top = 0;
    data->count = 0;
    data->iten_first = NULL;
    data->iten_last = NULL;

    o = app_ObjectNew (proc_console, x, y, 400, 300, id, OBJECT_TYPE_CONSOLE, data);

    app_ObjectAdd (parent, o);

    return o;
}

void app_ConsoleAdd (OBJECT *o, char *text, int color) {
    DATA_CONSOLE *data = app_GetData(o);
    ITEN *iten;
    if (data && text && (iten =(ITEN*)malloc(sizeof(ITEN))) != NULL) {
        iten->text = strdup (text);
        iten->color = color;
        iten->next = NULL;

         // ADICIONE O PRIMEIRO ITEN NO INICIO
        if (data->iten_first == NULL) {
            data->iten_first = iten;
            data->iten_last = iten;
        } else {
        // ADICIONE O ITEN NO FINAL
            data->iten_last->next = iten;
            data->iten_last = data->iten_last->next;
        }
        data->count++;
    }
}

void app_ConsoleClear (OBJECT *o) {
    DATA_CONSOLE *data = app_GetData(o);
    if (data) {
        ITEN *info;
printf ("Console clear:\n");
        while (data->iten_first) {
            info = data->iten_first->next;

            if (data->iten_first->text) {
                printf ("TEXT(%s):\n", data->iten_first->text);
                free (data->iten_first->text);
            }
            free (data->iten_first);
            data->iten_first = info;
        }
        data->line_top = 0;
        data->count = 0;
        data->iten_first = NULL;
        data->iten_last = NULL;
    }
}


