//-------------------------------------------------------------------
// Dialog Script Example:
//
// USAGE:
//   dream dialog.app
//
// To Run This Script ... IN EDITOR | PRESS KEY ( CTRL + R ):
//
//-------------------------------------------------------------------
//
int a = 100;

OBJECT dialog, button_add, button_sub;

function call_button_add (msg) {
    a = a + 1;
    a; // display value;
}

function call_button_sub (msg) {
    a = a - 1;
    a; // display value;
}


  if (!dialog) {

      printf ("script: PRIMEIRA VEZ ... Dialog criando com IF\n");

      dialog = DialogNew (100, 100, 600, 400);

      button_add = app_NewButton (dialog, 12345, 100, 100, "ADD");
      button_sub = app_NewButton (dialog, 12345, 250, 100, "SUB");

      SetCall (button_add, "call_button_add");
      SetCall (button_sub, "call_button_sub");
  }

  if (dialog) {
      printf ("script: Dialog EXECUTANDO ... test IF\n");
      DialogRun (dialog, "Main Dialog TITLE - Exit ?");
  }


