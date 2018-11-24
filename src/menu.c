//-------------------------------------------------------------------
//
// OBJECT Menu Implementation:
//
//-------------------------------------------------------------------
//
#include "app.h"

static char *return_menu_text = NULL;

MENU * app_MenuCreate (int w, int h) {
    MENU *m;
    int i = 0, y = 0;
    if ((m = (MENU*) malloc (sizeof(MENU))) == NULL)
  return NULL;
    m->w = w;
    m->h = h;
    m->index = 0;
    m->top = 0;
    m->pos_y = 0;
    m->count = 0;
    m->button_h = 25;
    m->iten_first = NULL;
    // adjust size h
    for (i = 0; i < 100; i++) {
        if (y >= h) break;
        y += m->button_h;
    }
    m->h = (i * m->button_h)+4;
    return m;
}

void app_MenuItenAdd (MENU *m, char *text) {
    if (m && text) {
        MENU_ITEN *iten = (MENU_ITEN*) malloc (sizeof(MENU_ITEN));
        if (iten) {
            iten->text = strdup (text);
            iten->next = NULL;
            if (!m->iten_first) {
                m->iten_first = iten;
            } else {
                MENU_ITEN *aux = m->iten_first;
                while (aux->next != NULL) {
                    aux = aux->next;
                }
                aux->next = iten;
            }
            m->count++;
        }
    }
}

void app_MenuItenClear (MENU *m) {
    if (m) {
        MENU_ITEN *info;
        while (m->iten_first) {
            info = m->iten_first->next;

            if (m->iten_first->text)
                free (m->iten_first->text);

            free (m->iten_first);

            m->iten_first = info;
        }
        m->index = 0;
        m->top = 0;
        m->pos_y = 0;
        m->count = 0;
        m->iten_first = NULL;
    }
}

static void MenuDraw (MENU *m, int x, int y, void (*call) (MENU *menu, char *text)) {
    MENU_ITEN *iten;
    int pos_y = y, top = 0;
    SDL_FillRect (screen, &(SR){ x+1, y+1, m->w-2, m->h-2 }, MRGB(255,251,198)); // bg
    DrawRectR (screen, x, y, m->w, m->h, COLOR_ORANGE);

    // bg index
    SDL_FillRect (screen, &(SR){ x+2, y + 2 + m->pos_y, m->w-4, m->button_h }, COLOR_WHITE); // bg first iten
    DrawRectR (screen, x+2, y + 2 + m->pos_y, m->w-4, m->button_h, COLOR_ORANGE); // border first iten

    iten = m->iten_first;
    while (iten) {
        if (pos_y > (y+m->h)-m->button_h) break;
        if (top >= m->top) {
            char *s = iten->text;
            int xx = x+10;
            while (*s) {
                if (xx > (x+m->w)-20) break;
                DrawChar (screen, *s, xx, pos_y+8, COLOR_ORANGE);
                xx += 8;
                s++;
            }
            pos_y += m->button_h;
            if (top == m->index) {
                return_menu_text = iten->text;
            }
        }
        top++;
        iten = iten->next;
    }
    if (call)
        call (m, return_menu_text);
}

//int count;
char * app_Menu (MENU *m, int x, int y, void (*call) (MENU *menu, char *text)) {
    int quit = 0;
    if (!m) return NULL;
    return_menu_text = NULL;
    MenuDraw (m,x,y,call);
    SDL_UpdateRect (screen, x, y, m->w, m->h);
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_KEYUP: {
                int k = e.key.keysym.sym;
                if (k == SDLK_RCTRL || k == SDLK_LCTRL) {
                    key_ctrl = 0;
                }
                else
                if (k == SDLK_RSHIFT || k == SDLK_LSHIFT) {
                    key_shift = 0;
                }
                } break;

            case SDL_KEYDOWN: {
                int k;
                if ((k = e.key.keysym.unicode)==0)
                    k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE) {
              return NULL;
                }
                else if (k == SDLK_RETURN) {
                    quit = 1;
                }
                else if (k == SDLK_UP && m->index > 0) {
                    m->index--;
                    if (m->pos_y > 0) // If iten selected in top!
                        m->pos_y -= m->button_h;
                    else
                        m->top--;
                }
                else if (k == SDLK_DOWN && m->index < m->count-1) {
                    m->index++;
                    if (m->pos_y < m->h - m->button_h - 10) // If iten selected in botton!
                        m->pos_y += m->button_h;
                    else
                        m->top++;
                }
                MenuDraw (m,x,y,call);
                SDL_UpdateRect (screen, x, y, m->w, m->h);
                } break; // case SDL_KEYDOWN:

            case SDL_MOUSEMOTION:
                if (e.motion.x > x && e.motion.x < x+m->w && e.motion.y > y && e.motion.y < y+m->h) {
                    int pos_y = 0, top = m->top;
                    for (;;) {
                        if (pos_y > m->h-5 || top >= m->count)
                      break;
                        if (m->index != top && e.motion.y > y+pos_y && e.motion.y < y+pos_y+m->button_h) {
                            m->index = top;
                            // DEBUG := printf ("index: %d, count: %d\n", m->index, count++);
                            m->pos_y = pos_y;
                            MenuDraw (m,x,y,call);
                            SDL_UpdateRect (screen, x, y, m->w, m->h);
                            break;
                        }
                        pos_y += m->button_h;
                        top++;
                    }
                }
                break; // case SDL_MOUSEMOTION:

                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == 4 && m->index > 0) { // SDLK_UP
                        m->index--;
                        if (m->pos_y > 0) // If iten selected in top!
                            m->pos_y -= m->button_h;
                        else
                            m->top--;
                        MenuDraw (m,x,y,call);
                        SDL_UpdateRect (screen, x, y, m->w, m->h);
                    }
                    else if (e.button.button == 5 && m->index < m->count-1) { // SDLK_DOWN
                        m->index++;
                        if (m->pos_y < m->h - m->button_h - 10) // If iten selected in botton!
                            m->pos_y += m->button_h;
                        else
                            m->top++;
                        MenuDraw (m,x,y,call);
                        SDL_UpdateRect (screen, x, y, m->w, m->h);
                    }
                    else
                    if (e.button.button == 1 || e.button.button == 3) {
                        if (e.button.x > x && e.button.x < x+m->w && e.button.y > y && e.button.y < y+m->h) {
                            if (e.button.y > y+m->pos_y && e.button.y < y+m->pos_y+m->button_h)
                                return return_menu_text;
                        } else {
                            return NULL;
                        }
                    }
                    break; // case SDL_MOUSEBUTTONDOWN:

            }// switch (e.type)
            SDL_Delay(10);
        }
    }
    return return_menu_text;
}

