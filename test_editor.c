//-------------------------------------------------------------------
//
// Simple Editor Example:
//
// FILE:
//   test_editor.c
//
// COMPILE:
//   gcc test_editor.c -o test_editor libapp.a -lSDL -Wall
//
//-------------------------------------------------------------------
//
#include "src/app.h"

#define ID_EDITOR   1000

OBJECT *zero = NULL;
OBJECT *ed = NULL;

void CreateInterface (void) {
    zero = app_GetRoot ();
    if (zero) {
        ed = app_NewEditor (zero, ID_EDITOR, 150, 150, "\nint a = 100, b = 250;\n\n  a + b;\n\n // To Run Script: CTRL + R ...\n", 5000);
        if (ed) {
            app_SetSize (ed, 600, 400);
            app_SetFocus (ed);
        }
    }
}


int main (int argc, char **argv) {
    if (app_Init(argc,argv)) {
        CreateInterface();
        app_Run (NULL);
    }
    return 0;
}

