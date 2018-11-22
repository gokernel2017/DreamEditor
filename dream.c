//-------------------------------------------------------------------
//
// Dream Editor:
//
// COMPILE:
//   gcc dream.c -o dream libapp.a -lSDL -Wall
//
// USAGE:
//   dream <FileName.txt>
//
// TO SAVE THE TEXT (in editor):
//   CTRL + S
//
//-------------------------------------------------------------------
//
#include "src/app.h"

#ifdef WIN32
    #include "io.h"
#else
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
#endif

#define ID_BUTTON1    1000  // FuncList
#define ID_BUTTON2    1001  // Templat
#define ID_BUTTON3    1002  // About
#define ID_EDIT       1003
#define ID_EDITOR     1004
#define ID_SHELL      1005

#ifdef WIN32
    #define EDITOR_DIR            "c:\\DreamEditor\\"
    #define EDITOR_DIR_TEMPLATE   "c:\\DreamEditor\\template\\"
#endif
#ifdef __linux__
    #define EDITOR_DIR            "/etc/DreamEditor/"
    #define EDITOR_DIR_TEMPLATE   "/etc/DreamEditor/template/"
#endif

OBJECT *button1, *button2, *button3, *edit, *editor, *shell;
MENU *menu;
char *text = NULL;
char *FileName = NULL;
int find_pos;
int value = 1;

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

//
// Editor Mult Line: CallBack
//
void call_editor (ARG *a) {
    char buf [1024];

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
            menu->w = 200; menu->h = 129;
            app_MenuItenClear (menu);
            for(;;) {
                int c = data->text[i];
                if (i==0) break;
                if (c<=' ' || c=='(') { i++; break; }
                i--;
            }
            while (i < data->pos) {
                name[count++] = data->text[i++];
                if (count >= sizeof(name)) break;
            }
            name[count] = 0;
//            printf ("name(%s)\n", name);
		        while (fgets(buf, sizeof(buf), fp) != NULL) {
                if (*buf == *name && strstr(buf, name))
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
                char buf [255] = { 0 };
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
                printf ("\nOK Saved: '%s'\n", data->FileName);
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
        system (app_EditGetText(shell));
    }
}

void CreateInterface (void) {

    button1 = app_NewButton (NULL, ID_BUTTON1, 3, 3, "FuncList");
    button2 = app_NewButton (NULL, ID_BUTTON2, 106, 3, "Template");
    button3 = app_NewButton (NULL, ID_BUTTON3, 106+103, 3, "About");
    edit = app_NewEdit (NULL, ID_EDIT, 209+103, 3, "Find Text | FILE NAME", EDITOR_FILE_NAME_SIZE-2);
    app_SetSize (edit, 486, 0);

    if (text && FileName) {
        editor = app_NewEditor (NULL, ID_EDITOR, 3, 33, text, 50000);
        app_EditorSetFileName (editor, FileName);
    } else {
        editor = app_NewEditor (NULL, ID_EDITOR, 3, 33,
        " Uriel Editor:\n   CTRL + S: Save The Text\n",
        50000
        );
    }
    shell = app_NewEdit (NULL, ID_SHELL, 3, screen->h-30, "gcc -v", EDITOR_FILE_NAME_SIZE-2);
    app_SetSize (shell, screen->w-6, 28);

//    app_SetSize (editor, screen->w-6, screen->h-35);
    app_SetSize (editor, screen->w-6, screen->h-66);
    app_SetFocus (editor);
    app_SetCall (editor, call_editor);

    app_SetCall (button1, call_button1);
    app_SetCall (button2, call_button2);
    app_SetCall (edit, call_edit);
    app_SetCall (shell, call_shell);

    menu = app_MenuCreate (400, 250);
}

void Finalize (void) {
    if (text) free (text);
    if (menu) {
        app_MenuItenClear (menu);
        free (menu);
    }
}

int main (int argc, char **argv) {
    if (app_Init(argc,argv)) {

        if (argc >= 2 && (text = app_FileOpen(argv[1])) != NULL) {
            FileName = argv[1];
        }
        CreateInterface ();
        app_Run (NULL);
        Finalize();
    }
    printf ("Exiting With Sucess !\n");
    return 0;
}
// 353

