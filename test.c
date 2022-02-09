#include<ncurses.h>
#include<stdlib.h>

//--------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------
void printing();
void moving_and_sleeping();
void colouring();

//--------------------------------------------------------
// FUNCTION main
//--------------------------------------------------------
int main(void)
{
    initscr();

    addstr("-----------------\n| codedrome.com |\n| ncurses Demo  |\n-----------------\n\n");
    refresh();

    //printing();

    //moving_and_sleeping();

    //colouring();

    addstr("\npress any key to exit...");
    refresh();

    getch();

    endwin();

    return EXIT_SUCCESS;
}