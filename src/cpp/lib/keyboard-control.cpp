#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {

    //DO whatever you like to do with this charecter ..

    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}


main( int argc, char** argv )
{
        char *input = argv[0];
        int nomor = argc;
        pid_t pid = 0;

        int x=0;
        while(1)
        {
                if(kbhit()){
                        //clearing the buffer
                        char ch = getchar();
                        printf("you hit keyboard and key = %c\n", ch);
                }
        }
}
