#include "curses.h"
#include <Windows.h>
#include <ctime>
#include <cstdio>
#include <string>

int main () {
    WINDOW *myWindow{initscr()};
    raw();
    nodelay(stdscr, true);
    int thisTime{};
    int i{0};
    int rightwardness{0};
    while (thisTime != 'q') {
        if (++i / 50 % 2 == 0) {
            mvprintw(0,0,"tick");
        }
        else {
            mvprintw(0,0,"tock");
        }
        refresh();
        thisTime = mvwgetch(stdscr, 1, rightwardness);
        if (thisTime != ERR) {
            mvprintw(1, ++rightwardness, "%c", thisTime);
        }
        Sleep(10);
    }
    endwin();
    return 0;
}