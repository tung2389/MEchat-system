// client.c

#include <stdio.h>

int main(int argc, char **argv)
{   
    if (argc < 2) 
    {
      fprintf(stderr, "client: You must specify the server hostname on the command line.");
      exit(1);
    }

 
  
  return 0;
}
