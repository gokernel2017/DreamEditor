//-------------------------------------------------------------------
//
// Big Loop Example:
//
// USAGE:
//   dream loop_big.app
//
// CTRL + R: Run Script.
//
//-------------------------------------------------------------------
//
int i;

  printf ("\nBig Loop (100 000 000), Please Wait ...\n");
  i = 0;
  for (;;) {
      i++;
      if (i >= 100000000) {
          break;
      }
  }
  printf ("Now Value i: %d\n", i);

