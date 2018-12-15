//-------------------------------------------------------------------
//
// Basic Example:
//
// FILE:
//   basic.c
//
// COMPILE:
//   gcc basic.c -o basic libapp.a -lSDL -Wall
//
//-------------------------------------------------------------------
//
#include "src/app.h"

#define ID_BUTTON   1000

OBJECT *zero = NULL;
OBJECT *button;
int count;

void call_button (int msg) {
    printf ("Count: %d\n", count++);
}

void CreateInterface (void) {
    zero = app_GetRoot ();
    if (zero) {
        button = app_NewButton (zero, ID_BUTTON, 100, 100, "Hello");
        if (button) {
            app_SetCall (button, call_button);
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

