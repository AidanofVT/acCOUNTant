#include "curses.h"
#include <Windows.h>
#include <ctime>
#include <string>

void processCommand (std::string line) {

}

int main () {
    WINDOW *myWindow{initscr()};
    raw();
    nodelay(stdscr, true);
    int lastCharHit{};
    int i{0};
    int rightwardness{0};
    while (lastCharHit != 'q') {
        if (++i / 50 % 2 == 0) {
            mvprintw(0,0,"tick");
        }
        else {
            mvprintw(0,0,"tock");
        }
        mvprintw(1, 0, "%i", time(NULL));
        lastCharHit = mvwgetch(stdscr, 2, rightwardness);
        if (lastCharHit == 13) {
            rightwardness = 0;
            mvprintw(2,rightwardness, "");
            
            clrtoeol();
        }
        else if (lastCharHit != ERR) {
            mvprintw(2, ++rightwardness, "%c", lastCharHit);
        }        
        refresh();
        Sleep(10);
    }
    endwin();
    return 0;
}