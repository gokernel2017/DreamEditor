//-------------------------------------------------------------------
//
// Word ( for ) Example:
//
// USAGE:
//   dream for.app
//
// CTRL + R: Run Script.
//
//-------------------------------------------------------------------
//
int i;

  printf ("\nWord for example: 0 ... 20.\n");
  i = 0;
  for (;;) {
      printf ("Value i: %d\n", i);
      i++;
      if (i > 20) {
          break;
      }
  }

