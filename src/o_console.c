//-------------------------------------------------------------------
//
// OBJECT Console Implementation:
//
//-------------------------------------------------------------------
//
#include "app.h"

#define CONSOLE_ITEN_MAX    10000
#define DISTANCE            17  // line distance
#define COLOR_DELETE        25388

typedef struct ITEN ITEN;

typedef struct {
    char  text[CONSOLE_TEXT_SIZE+1];
    int   top; // line top
    int   count;
    int   col; // d2
    int   text_changed; // is true on click in line number
    ITEN  *iten_top; // the first displayed iten in top
    ITEN  *iten_first;
    ITEN  *iten_last;
}DATA_CONSOLE;

struct ITEN {
    char  *text;
    int   color;
    ITEN  *prev;
    ITEN  *next;
};

SDL_Rect r;

static void thanks (OBJECT *o);
void app_ConsoleDeteteIten (DATA_CONSOLE *data, ITEN *iten);

int console_get_line_text (DATA_CONSOLE *data) {
    int pos_y = r.y+8;
    ITEN *iten = data->iten_top;
    while (iten) {
        if (pos_y > (r.y+r.h)-50)
      break;
        if (my > pos_y && my < pos_y+15) {
            char *s = iten->text;
            int count = 0;
            while (*s && *s != '\n') {
                data->text[count] = *s++;
                if (count < CONSOLE_TEXT_SIZE)
                    count++;
            }
            data->text[count] = 0;
            data->col = count;
            return 1;
        }
        pos_y += DISTANCE;
        iten = iten->next;
    }
    return 0;
}

int proc_console (OBJECT *o, int msg, int value) {
    DATA_CONSOLE *data = app_GetData(o);
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
        ITEN *iten = data->iten_top;
        char sbuf[20];
        int top = data->top;
        int pos_y;

        SDL_FillRect (screen, &(SR) {r.x+1, r.y+1, r.w-2, r.h-2 }, 0);
        DrawVline (screen, r.x+52, r.y, r.y+r.h-30, COLOR_ORANGE);

        pos_y = r.y;
        while (iten) {
            char *s = iten->text;
            int x = r.x+64;
            if (pos_y > (r.y+r.h)-50)
          break;
            sprintf (sbuf, "%04d", top+1);
            DrawText (screen, sbuf, r.x+10, pos_y+10, COLOR_ORANGE);
            while (*s) {
                if (x > (r.x+r.w)-16) break;
                DrawChar (screen, *s, x, pos_y+10, iten->color);
                x += 8;
                s++;
            }
            pos_y += DISTANCE;
            top++;
            iten = iten->next;
        }
        DrawHline (screen, r.x, r.y+r.h-28, r.x+r.w-1, COLOR_ORANGE);
        //DrawText (screen, data->text, r.x+10, r.y+r.h-21, COLOR_GREEN);
        DrawRectR (screen, r.x, r.y, r.w, r.h, COLOR_ORANGE); // border
        sprintf (sbuf, "%d", data->count);
        DrawText (screen, sbuf, (r.x+r.w)-(strlen(sbuf)*8)-10, r.y+10, COLOR_ORANGE);

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

        if (value > 281)
      return 0;

        if (value == SDLK_UP) {
            if (data->top >= 1) {
                data->top--;
                data->iten_top = data->iten_top->prev;
            }
        }
        else if (value == SDLK_DOWN) {
            if (data->top < data->count-1) {
                data->top++;
                data->iten_top = data->iten_top->next;
            }
        }
        else if (value == SDLK_PAGEUP) {
            int lines = ((r.h-28) / DISTANCE)-1;
            while (lines--) {
                if (data->top >= 1) {
                    data->top--;
                    data->iten_top = data->iten_top->prev;
                }
            }
        }
        else if (value == SDLK_PAGEDOWN) {
            int lines = ((r.h-28) / DISTANCE)-1;
            while (lines--) {
                if (data->top < data->count-1) {
                    data->top++;
                    data->iten_top = data->iten_top->next;
                }
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
            app_ConsoleAdd (o, data->text, COLOR_GREEN);
            while (data->top != data->count-1) {
                data->top++;
                data->iten_top = data->iten_top->next;
            }
            if ((fp = popen (buf, "r")) != NULL) {
                while (fgets(buf, sizeof(buf), fp) != NULL) {
                    app_ConsoleAdd (o, buf, COLOR_ORANGE);
                }
                pclose (fp);
            }
            data->text[0] = 0;
            data->col = 0;
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

    case MSG_MOUSE_DOWN: {
        data->text_changed = 0;
        if (mx < r.x+52) {
            if (console_get_line_text (data)) {
                data->text_changed = 1;
                app_ObjectUpdate (o);
            }
        }
        return RET_CALL;
        } break;
    }
    return 0;
}

char * app_ConsoleTextChanged (OBJECT *o) {
    DATA_CONSOLE *data = app_GetData(o);
    if (data && data->text_changed)
        return data->text;
    return NULL;
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

    data->top = 0;
    data->count = 0;
    data->col = 0;
    data->text_changed = 0;
    data->iten_first = NULL;
    data->iten_last = NULL;

    o = app_ObjectNew (proc_console, x, y, 400, 300, id, OBJECT_TYPE_CONSOLE, data);
    thanks (o);

    app_ObjectAdd (parent, o);

    return o;
}

void app_ConsoleAdd (OBJECT *o, char *text, int color) {
    DATA_CONSOLE *data = app_GetData(o);
    ITEN *iten;
    if (data && data->count < CONSOLE_ITEN_MAX && text && (iten =(ITEN*)malloc(sizeof(ITEN))) != NULL) {
        if ((iten->text = strdup (text)) != NULL) {
            iten->color = color;
            iten->prev = NULL;
            iten->next = NULL;

            // ADICIONE O PRIMEIRO ITEN NO INICIO
            if (data->iten_first == NULL) {
                data->iten_top = iten;
                data->iten_first = iten;
                data->iten_last = iten;
            } else {
                // ADICIONE O ITEN NO FINAL
                iten->prev = data->iten_last;
                data->iten_last->next = iten;
                data->iten_last = iten;
            }
            data->count++;
        }
    }
}

void app_ConsoleClear (OBJECT *o) {
    DATA_CONSOLE *data = app_GetData(o);
    if (data) {
        ITEN *info;
        while (data->iten_first) {
            info = data->iten_first->next;
            if (data->iten_first->text) {
                free (data->iten_first->text);
            }
            free (data->iten_first);
            data->iten_first = info;
        }
        data->top = 0;
        data->count = 0;
        data->col = 0;
        data->text_changed = 0;
        data->iten_top = NULL;
        data->iten_first = NULL;
        data->iten_last = NULL;
        thanks (o);
    }
}

int isEmpty (DATA_CONSOLE *data) {
    return (data->iten_first == NULL);
}

void app_ConsoleDeteteIten (DATA_CONSOLE *data, ITEN *iten) {
    ITEN *prev;
    ITEN *next;
    if (isEmpty(data)) {
        return;
    }
    prev = iten->prev;
    next = iten->next;
    if (prev != NULL) {
        if (next != NULL) {
            // Both the previous and next links are valid, so just bypass "link" without altering "list" at all.
            prev->next = next;
            next->prev = prev;
        } else {
            // Only the previous link is valid, so "prev" is now the last link in "list".
            prev->next = 0;
            data->iten_last = prev;
        }
    } else {
        if (next != NULL) {
            // Only the next link is valid, not the previous one, so "next" is now the first link in "list".
            next->prev = 0;
            data->iten_first = next;
        } else {
            // Neither previous nor next links are valid, so the list is now empty.
            data->iten_first = NULL;
            data->iten_last = NULL;
        }
    }
    if (iten->text)
        free (iten->text);
    free (iten);
}


/*

void delete_NODE(linked_list * list, NODE * node) {
    NODE * prev;
    NODE * next;
    if (isEmpty(list)) {
        return;
    }
    prev = node->prev;
    next = node->next;
    if (prev != NULL) {
        if (next != NULL) {
            // Both the previous and next links are valid, so just bypass "link" without altering "list" at all.
            prev->next = next;
            next->prev = prev;
        } else {
            // Only the previous link is valid, so "prev" is now the last link in "list".
            prev->next = 0;
            list->last = prev;
        }
    } else {
        if (next != NULL) {
            // Only the next link is valid, not the previous one, so "next" is now the first link in "list".
            next->prev = 0;
            list->first = next;
        } else {
            // Neither previous nor next links are valid, so the list is now empty.
            list->first = 0;
            list->last = 0;
        }
    }
    free(node);
}

int SDL_LListRemove(SDL_LinkedList *list,SDL_LinkedListElem *elem)
{
    if(elem->prev != NULL)
        elem->prev->next = elem->next;
    else
        list->first = elem->next;
    
    if(elem->next != NULL)
        elem->next->prev = elem->prev;
    else
        list->last = elem->prev;

    return 1;
}

*/

static void thanks (OBJECT *o) {
    app_ConsoleAdd (o, "THANKS TO", COLOR_GREEN);
    app_ConsoleAdd (o, "  01: God the creator of the heavens and the earth in the name of Jesus Christ.", COLOR_WORD);
    app_ConsoleAdd (o, "  02: webton  - member of unidev (www.undev.com.br).", COLOR_WORD);
    app_ConsoleAdd (o, "  03: Sam. L. - member of VOL (www.vivaolinux.com.br).", COLOR_WORD);
    //  
    app_ConsoleAdd (o, " ", COLOR_ORANGE);
    app_ConsoleAdd (o, "Console Version 0.90.0", COLOR_ORANGE);
    app_ConsoleAdd (o, " ", COLOR_ORANGE);
    app_ConsoleAdd (o, "To clear the lines type: 'clear' or 'cls'.", COLOR_ORANGE);
}

