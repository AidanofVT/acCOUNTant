#include "curses.h"
#include <stdexcept>
#include <Windows.h>
#include <cctype>
#include <ctime>
#include <fstream>
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

void wipeTime () {
    mvprintw(2, 0, "");       
    clrtoeol();
}

void showTimeFactor () {
    std::string asString{"x" + std::to_string(multiplier)};
    while (asString[asString.length() - 1] == '0' || asString[asString.length() - 1] == '.') {
        asString.erase(asString.length() - 1);
    }
    mvprintw(0, 0, asString.c_str());
}

void showCount (int timeInSeconds) {
    std::string hours {std::to_string(countNow / 3600)};
    std::string minutes {std::to_string(countNow % 3600 / 60)};
    std::string seconds {std::to_string(countNow % 60)};
    std::string formattedTime{hours + ":" + minutes + ":" + seconds};    
    mvprintw(2, 0, formattedTime.c_str());
    mvprintw(1, rightwardness,"");
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

void enactTimeSumChange (std::string timeChange) {
    std::string timeChangeError{"Uh-uh: time insertions need to be in either ## (for minutes) or ##:## (for hour:minute) format."};
    int i{1};
// Itterate until something that's NOT a number.
    for (; std::isdigit(timeChange[i]); ++i) {        
    }
    if (timeChange[i] == ':') {
        int firstNumber{};
        int j {i + 1};
        for (; std::isdigit(timeChange[j]); ++j) {        
        }
        if (timeChange[j] == NULL) {

        }
        else {
            throw timeChangeError;
        }
    }
    else if (timeChange[i] == NULL) {
        int minutes {std::stoi(timeChange)};
        int seconds {minutes * 60};
        countAsOfLastAnchor += seconds;
        wipeTime();
    }    
    else {
        throw timeChangeError;
    }    
    switch (timeChange[0]) {
        case '+':
            break;
        case '-':
            break;
        default:
            throw "Something went wrong!";
    }
}

void enactMultiplierChange (std::string newMultiplier) {
    switch (newMultiplier[0]) {
        case 'u':
            newMultiplier.erase(0,1);
            break;
        case 'd':
            newMultiplier.replace(0, 1, "-");
            break;
        default:
            throw "Something went wrong!";
    }
    newStart();
    try {
        multiplier = std::stof(newMultiplier);
    }
    catch (const std::invalid_argument& whoops) {
        mvprintw(0, 1, "Nope! Changes to the time muliplier should look like this: 'u1' or 'd3.33'.");
    }
    showTimeFactor();
}

void processCommand (std::string command) {
    switch (command[0]) {
        case 'u':
        case 'd':
            enactMultiplierChange(command);
            break;
        case '+':
        case '-':
            enactTimeSumChange(command);
            break;
        default:
            throw "Something went wrong!";
    }
}

int main () {
    WINDOW *myWindow{initscr()};
    raw();
    nodelay(stdscr, true);
    std::fstream readerWriter {"acCOUNTant_state.txt"};
    char temp [10];
    readerWriter.getline(temp, 10);
    countAsOfLastAnchor = std::stoi(temp);
    readerWriter.clear();
    int lastCharHit{};
    int i{};
    showTimeFactor();
    while (lastCharHit != 'q') {
        lastCharHit = mvgetch(1, rightwardness);
        if (running) {
            countNow = countAsOfLastAnchor + difftime(time(NULL), anchorTime) * multiplier;
            wipeTime();
            showCount(countNow);
            if (i % 500 == 0) {
                readerWriter.seekp(0);
                readerWriter << countNow << "         ";
                readerWriter.flush();
            }
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
        i = (i + 1) % 1000;
        Sleep(10);
    }
    endwin();
    return 0;
}