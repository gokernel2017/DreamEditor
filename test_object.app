
int a = 100;

OBJECT button;

function hello (msg) {
    a = a + 1;
    a; // display value
}

  button = app_NewButton (0, 12345, 10, 200, 0);

  SetCall (button, "hello");

