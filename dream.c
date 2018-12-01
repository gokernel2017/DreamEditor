//-------------------------------------------------------------------
//
// THANKS TO:
// ----------------------------------------------
//
// 01 : God the creator of the heavens and the earth in the name of Jesus Christ.
//
// ----------------------------------------------
//
// Dream Editor:
//
// COMPILE:
//   gcc dream.c -o dream libapp.a -lSDL -Wall
//
// USAGE:
//   dream <FileName.txt>
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "src/app.h"

#define DREAM_VERSION         0
#define DREAM_VERSION_SUB     91
#define DREAM_VERSION_PATCH   0

#define ID_BUTTON1    1000  // FuncList
#define ID_BUTTON2    1001  // Templat
#define ID_BT_ABOUT   1002  // About
#define ID_EDIT       1003
#define ID_EDITOR     1004
#define ID_SHELL      1005
#define ID_CONSOLE    1006

#ifdef WIN32
    #define EDITOR_DIR            "c:\\DreamEditor\\"
    #define EDITOR_DIR_TEMPLATE   "c:\\DreamEditor\\template\\"
#endif
#ifdef __linux__
    #define EDITOR_DIR            "/etc/DreamEditor/"
    #define EDITOR_DIR_TEMPLATE   "/etc/DreamEditor/template/"
#endif

OBJECT *button1, *button2, *bt_about, *edit, *editor, *shell, *console;
MENU *menu;
char *text = NULL;
char *FileName = NULL;
int find_pos;
int value = 1;

int isinit (char *name, char *string) {
    while (*name) {
        if (*name != *string) return 0;
        name++;
        string++;
    }
    return 1;
}

void call_menu (MENU *m, char *text) {
    printf ("MENU INDEX = %d\n", m->index);
}

void call_button1 (ARG *a) {
    if (menu) {
        char *ret;
        app_EditorListFunction (editor, menu);
        if ((ret = app_Menu (menu, 3+50, 33, NULL))) {
            app_EditorFindString (editor, ret, 0);
        }
        app_ObjectUpdate (editor);
    }
}
void call_button2 (ARG *a) {
    char buf[1024];
    char *ret;

    if (!menu) return;
    app_MenuItenClear (menu);

#ifdef WIN32
    {
    int done;
    struct _finddata_t find;

    sprintf (buf, "%s%s", EDITOR_DIR_TEMPLATE, "*.*");

    if ((done = _findfirst (buf, &find))==-1) {
        sprintf (buf, "DIR NOT FOUND: '%s'", EDITOR_DIR_TEMPLATE);
        app_ShowDialog (buf, DIALOG_OK);
        return;
    }
    do {
        if (!(find.attrib & _A_SUBDIR)) {
            if (strstr(find.name, ".tem")) {
                sprintf (buf, "%s%s", EDITOR_DIR_TEMPLATE, find.name);
                app_MenuItenAdd (menu, buf);
            }
        }
    } while ( !_findnext(done, &find) );
    _findclose(done);

    }
#endif // WIN32


#ifdef __linux__
    {
    DIR *dir;
    struct dirent *entry;
    struct stat s;

    sprintf (buf, "%s", EDITOR_DIR_TEMPLATE);

    if ((dir = opendir(buf))==NULL) {
        sprintf (buf, "DIR NOT FOUND: '%s'", EDITOR_DIR_TEMPLATE);
        app_ShowDialog (buf, DIALOG_OK);
        return;
    }
    for (;;) {
        entry = readdir(dir);
        if (!entry) break;
        if ( !(stat(entry->d_name, &s) == 0 && S_ISDIR(s.st_mode)) ) {
            if (strstr(entry->d_name, ".tem")) {
                sprintf (buf, "%s%s", EDITOR_DIR_TEMPLATE, entry->d_name);
                app_MenuItenAdd (menu, buf);
            }
        }
    }// for (;;)
    closedir (dir);
    }
#endif // __linux__

    if ((ret = app_Menu (menu, 106, 33, NULL))) {
        FILE *fp;
        if ((fp = fopen (ret, "r")) != NULL) {
            int c;
            DATA_EDITOR *data = app_GetData (editor);
            while ((c = fgetc(fp)) != EOF) {
                if (data->len < data->size-2) {
                    app_EditorInsertChar (data->text, data->pos, c);
                    data->len++;
                    data->pos++;
                }
            }
            data->saved = 0;
            fclose(fp);
        }
    }

    app_SendMessage (editor, MSG_KEY, 0);
    app_ObjectUpdate (editor);

}// call_button2 ()

//
// Line edit: CallBack
//
void call_edit (ARG *a) {
    if (a->key == SDLK_RETURN) {
        find_pos = app_EditorFindString (editor, app_EditGetText(edit), find_pos);
        if (find_pos != -1) {
            find_pos++;
            app_ObjectUpdate (editor);
        }
        else find_pos = 0;
    }
    else
    if (a->key == SDLK_TAB) {
        app_SetFocus (editor);
        app_ObjectUpdate (editor);
    }
}

void call_console (ARG *a) {
    if (a->msg == MSG_MOUSE_DOWN) {
        char *s;
        if ((s = app_ConsoleTextChanged(console))) {
            app_EditorInsertText (editor, s);
        }
        if (editor)
            app_ObjectSetTop (editor);
        return;
    }
}

//
// Editor Mult Line: CallBack
//
void call_editor (ARG *a) {
    char buf [1024];

    if (a->msg == MSG_MOUSE_DOWN) {
        if (console)
            app_ObjectSetTop (console);
        return;
    }

    //
    // CTRL + A: complete text
    //
    if (key_ctrl && a->key == CTRL_KEY_A) {
        DATA_EDITOR *data = app_GetData(editor);
        FILE *fp;
        sprintf (buf, "%scomplete", EDITOR_DIR); // c:\editor\complete  OR  /usr/editor/complete
        if (data && menu && (fp = fopen(buf, "r"))) {
            SDL_Rect r;
            char name[1024];
            int i = data->pos-1, count = 0;
            int pos_x, pos_y;
            int w = menu->w;
            int h = menu->h;
            char *ret; // menu return text
            app_GetRect (editor, &r);
            menu->w = 250; menu->h = 129;
            app_MenuItenClear (menu);
            for(;;) {
                int c = data->text[i];
                if (i==0) break;
                if (c<=' ' || c=='(') { i++; break; }
                i--;
            }
            while (i < data->pos) {
                int ch = data->text[i++];
                if (ch > 32) {
                    name[count++] = ch;
                }
                if (count >= sizeof(name)) break;
            }
            name[count] = 0;
            //printf ("name(%s)\n", name);
		        while (fgets(buf, sizeof(buf), fp) != NULL) {
                if (*buf == *name && isinit(name, buf))
                    app_MenuItenAdd (menu, buf);
            }
            fclose(fp);
            //-------------------------------------------------------
            // set menu position: x, y
            //
            pos_x = 70+r.x+data->col*8;
            pos_y = (r.y+data->line_pos*EDITOR_LINE_DISTANCE)+EDITOR_LINE_DISTANCE+3;
            if (pos_x + menu->w >= screen->w) {
                pos_x -= menu->w;
            }
            if (pos_y + menu->h >= screen->h) {
                pos_y -= menu->h + EDITOR_LINE_DISTANCE;
            }
            //-------------------------------------------------------
            if ((ret = app_Menu (menu, pos_x, pos_y, NULL))) {
                char *s = ret+strlen(name);
                while (*s && *s != '\n') {
                    if (data->len < data->size-2) {
                        app_EditorInsertChar (data->text, data->pos, *s);
                        data->len++;
                        data->pos++;
                        data->col++;
                    }
                    s++;
                }
                data->saved = 0;
            }
            menu->w = w; menu->h = h;
            app_ObjectUpdate (editor);
        }
        else {
            sprintf (buf, "FILE NOT FOUND: '%scomplete'", EDITOR_DIR);
            app_ShowDialog (buf, DIALOG_OK);
        }
    }
    else
    //
    // CTRL + S: Save the text
    //
    if (key_ctrl && a->key == CTRL_KEY_S) {
        DATA_EDITOR *data = app_GetData (editor);
        if (data) {
            if (data->FileName[0]==0) {
                char buf [1024];
                // copy the current directory name to object edit.
                getcwd (buf, sizeof(buf)-1);
                if (app_FileDialog ("Save File:", buf)) {
                    sprintf (data->FileName, "%s", buf);
                }
                else return;
            }
            FILE *fp;
            char *s = data->text;
            if ((fp = fopen (data->FileName, "w")) != NULL) {
                while (*s) {
                    fputc (*s, fp);
                    s++;
                }
                fclose (fp);
                data->saved = 1;
                app_ObjectUpdate (editor);
//                printf ("\nOK Saved: '%s'\n", data->FileName);
            }
            else {
                printf ("\nInvalid File Name: '%s'\nPlease Edit a Valid File Name !", data->FileName);
                data->FileName[0] = 0;
            }
        }
    }
    else
    //
    // CTRL + O: Insert The Text File ( C:\editor\o or /usr/editor/o ) in EDITOR.
    //
    if (key_ctrl && a->key == CTRL_KEY_O) {
        DATA_EDITOR *data = app_GetData (editor);
        int c;
        FILE *fp;
        sprintf (buf, "%so", EDITOR_DIR);
        if ((fp = fopen (buf, "r")) != NULL) {
            while ((c = fgetc(fp)) != EOF) {
                if (data->len < data->size-2) {
                    app_EditorInsertChar (data->text, data->pos, c);
                    data->len++;
                    data->pos++;
                }
            }
            fclose(fp);
            data->saved = 0;
            app_SendMessage (editor, MSG_KEY, 0);
            app_ObjectUpdate (editor);
        }
        else {
            sprintf (buf, "FILE NOT FOUND: '%so'", EDITOR_DIR);
            app_ShowDialog (buf, DIALOG_OK);
        }
    }
}

void call_shell (ARG *a) {
    if (a->key == SDLK_RETURN && shell) {
        FILE *fp;
        char buf[1024];
        sprintf (buf, "%s 2>&1", app_EditGetText (shell));
//        printf ("BUF(%s)\n", buf);
        if ((fp = popen (buf, "r")) != NULL) {
            while (fgets(buf, sizeof(buf), fp) != NULL) {
                printf ("%s", buf); // aqui poderia ja armazenar ( buf ) ...
            } 
            pclose (fp);
        }
    }
}

/*
void console_store (void) {
    FILE *fp;
    char buf[1024];
    app_ConsoleAdd (console, " ", COLOR_WORD);
    app_ConsoleAdd (console, "gcc -v", COLOR_GREEN);
    if ((fp = popen ("gcc -v 2>&1", "r")) != NULL) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            app_ConsoleAdd (console, buf, COLOR_ORANGE);
        }
        pclose (fp);
    }
    app_ConsoleAdd (console, " ", COLOR_WORD);
    app_ConsoleAdd (console, "dir", COLOR_GREEN);
    if ((fp = popen ("dir 2>&1", "r")) != NULL) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            app_ConsoleAdd (console, buf, COLOR_ORANGE);
        }
        pclose (fp);
    }
}
*/

void call_bt_about (ARG *a) {
#define WIDTH   450
#define HEIGHT  300
    SDL_Event e;
    char buf[1024];
    int x = (screen->w/2)-WIDTH/2;
    int y = (screen->h/2)-HEIGHT/2;
    SDL_FillRect (screen, &(SR) { x+1, y+1, WIDTH-2, HEIGHT-2}, COLOR_WHITE);
    DrawRectR (screen, x, y, WIDTH, HEIGHT, COLOR_ORANGE);
    sprintf (buf, "Dream Editor - Version: %d.%d.%d", DREAM_VERSION, DREAM_VERSION_SUB, DREAM_VERSION_PATCH);
    DrawText (screen, buf, (screen->w/2)-(strlen(buf)*8)/2, y+20, COLOR_ORANGE);
    DrawText (screen, "KEY USAGE:", x+15, y+50, COLOR_ORANGE);
    DrawText (screen, "CTRL + S: Save the text", x+15+16, y+70, COLOR_ORANGE);
    DrawText (screen, "CTRL + C: Copy the selected text", x+15+16, y+90, COLOR_ORANGE);
    DrawText (screen, "CTRL + V: Paste the text", x+15+16, y+110, COLOR_ORANGE);
    DrawText (screen, "CTRL + A: Complete Words from MENU List", x+15+16, y+130, COLOR_ORANGE);
    DrawText (screen, "CTRL + Y: Delete Line", x+15+16, y+150, COLOR_ORANGE);
    //
    DrawText (screen, "https://github.com/gokernel2017/DreamEditor", x+15, y+250, COLOR_ORANGE);
    DrawText (screen, "BY: Francisco - gokernel@hotmail.com", x+15, y+270, COLOR_ORANGE);
    SDL_UpdateRect (screen, x, y, WIDTH, HEIGHT);
    for (;;) {
        if (SDL_PollEvent(&e) && (e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP))
            break;
        SDL_Delay(10);
    }
    app_ObjectUpdate (editor);
}

void CreateInterface (void) {

    button1 = app_NewButton (NULL, ID_BUTTON1, 3, 3, "FuncList");
    button2 = app_NewButton (NULL, ID_BUTTON2, 106, 3, "Template");
    bt_about = app_NewButton (NULL, ID_BT_ABOUT, 106+103, 3, "About");
    edit = app_NewEdit (NULL, ID_EDIT, 209+103, 3, "Find Text", EDITOR_FILE_NAME_SIZE-2);
    app_SetSize (edit, 486, 0);

    console = app_NewConsole (NULL, ID_CONSOLE, 3, 200, "Console");
    app_SetSize (console, screen->w-6, screen->h-200);
    app_SetCall (console, call_console);

    if (text && FileName) {
        editor = app_NewEditor (NULL, ID_EDITOR, 3, 33, text, 50000);
        app_EditorSetFileName (editor, FileName);
    } else {
        editor = app_NewEditor (NULL, ID_EDITOR, 3, 33,
        " Dream Editor:\n   CTRL + S: Save The Text\n",
        50000
        );
    }

    app_SetSize (editor, screen->w-6, screen->h-65);
    app_SetFocus (editor);
    app_SetCall (editor, call_editor);

    app_SetCall (button1, call_button1);
    app_SetCall (button2, call_button2);
    app_SetCall (bt_about, call_bt_about);
    app_SetCall (edit, call_edit);
//    app_SetCall (shell, call_shell);

    menu = app_MenuCreate (400, 250);
}

void Finalize (void) {
    if (text) free (text);
    if (menu) {
        app_MenuItenClear (menu);
        free (menu);
    }
    SEND (editor, MSG_FREE, 0);
}

int main (int argc, char **argv) {

    if (app_Init(argc,argv)) {

        if (argc >= 2 && (text = app_FileOpen(argv[1])) != NULL) {
            FileName = argv[1];
            printf ("agora STR ponteiro %p = %d\n", &text, (int)text);
        }
        CreateInterface ();
        app_PrintData(editor);
printf ("cinza: %d\n", MRGB(100,100,100));
        app_Run (NULL);
        Finalize();
    }
    printf ("Exiting With Sucess !\n");
    return 0;
}
// 353

