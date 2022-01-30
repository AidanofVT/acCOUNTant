#include "curses.h"
#include <Windows.h>
#include <ctime>
#include <string>

// MUST "DEALLOCATE" C-STYLE STRINGS!!!!!

bool running {false};
int anchorTime{time(NULL)};
int countAsOfLastAnchor{0};
int countNow{0};
float multiplier{1};
int rightwardness{0};

void newStart () {
    countAsOfLastAnchor = countNow;
    anchorTime = time(NULL);
}

std::string takeCommand () {
    std::string toReturn{};
    char charInQuestion{};
    rightwardness = 0;    
    while ((charInQuestion = mvinch(1, rightwardness)) != ' ') {
        toReturn += charInQuestion;
        ++rightwardness;
        }  
    rightwardness = 0;
    mvprintw(1, rightwardness, "");            
    clrtoeol();
    return toReturn;
}

void enactTimeSumChange (std::string timeChange) {}

void enactMultiplierChange (std::string newMultiplier) {
    switch (newMultiplier[0]) {
        case 'u':
            newMultiplier.erase(0,1);
            break;
        case 'd':
            newMultiplier.replace(0, 1, "-");
            break;
    }
    newStart();
    multiplier = std::stof(newMultiplier);
}

void processCommand (std::string command) {
    switch (command[0]) {
        case 'u':
        case 'd':
            enactMultiplierChange(command);
            break;
    }
}

int main () {
    WINDOW *myWindow{initscr()};
    raw();
    nodelay(stdscr, true);
    int lastCharHit{};
    while (lastCharHit != 'q') {
        lastCharHit = mvgetch(1, rightwardness);
        if (running) {
            countNow = countAsOfLastAnchor + difftime(time(NULL), anchorTime) * multiplier;
            mvprintw(0, 0, "");            
            clrtoeol();
            mvprintw(0, 0, "%i", countNow);
            mvprintw(1, rightwardness,"");
        }
        if (lastCharHit == ' ') {            
            running = !running;
            if (running == true) {
                newStart();               
            }
            mvprintw(1, rightwardness,"");
        }
        else if (lastCharHit == 13) {
            processCommand(takeCommand());
        }
        else if (lastCharHit != ERR) {
            mvprintw(1, ++rightwardness - 1, "%c", lastCharHit);
        }      
        refresh();
        Sleep(10);
    }
    endwin();
    return 0;
}