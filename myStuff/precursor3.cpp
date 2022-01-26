#include "curses.h"
#include <Windows.h>
#include <ctime>
#include <string>

bool run {true};

void processCommand (std::string line) {
    
}

int main () {
    WINDOW *myWindow{initscr()};
    raw();
    nodelay(stdscr, true);
    bool running{true};
    int anchorTime{time(NULL)};
    int sumTime{0};
    int lastCharHit{};
    int rightwardness{0};
    while (lastCharHit != 'q') {
        lastCharHit = mvwgetch(stdscr, 1, rightwardness);
        if (running) {     
            int toPrint{sumTime + difftime(time(NULL), anchorTime)};    
            mvprintw(0, 0, "%i", toPrint);
        }
        if (lastCharHit == ' ') {
            running = !running;
            if (running == false) {
                sumTime = sumTime + difftime(time(NULL), anchorTime);                
            }
            else {
                anchorTime = time(NULL);
            }
        }
        // if (lastCharHit == 13) {
        //     rightwardness = 0;
        //     mvprintw(1, rightwardness, "");            
        //     clrtoeol();
        // }
        // else if (lastCharHit != ERR) {
        //     mvprintw(1, ++rightwardness - 1, "%c", lastCharHit);
        // }        
        refresh();
        Sleep(10);
    }
    endwin();
    return 0;
}