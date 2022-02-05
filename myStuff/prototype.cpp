#include "curses.h"
#include <stdexcept>
#include <Windows.h>
#include <cctype>
#include <ctime>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

// MUST "DEALLOCATE" C-STYLE STRINGS!!!!!

bool running {false};
int anchorTime{time(NULL)};
int countAsOfLastAnchor{0};
int countNow{0};
float multiplier{1};
int rightwardness{0};

WINDOW *notices {};
WINDOW *inputSpace{};
WINDOW *firstDigit {};
WINDOW *secondDigit {};
WINDOW *thirdDigit {};
WINDOW *fourthDigit {};
WINDOW *fifthDigit {};
WINDOW *sixthDigit {};
WINDOW *seventhDigit {};
WINDOW *eighthDigit {};

WINDOW digits[8];

std::string oneOne{"    111\n  11111\n    111\n    111\n    111\n    111\n    111\n    111\n    111\n  1111111"};
std::string oneTwo{   "22222\n 222   222\n222     222\n      222\n     222\n   222\n 222\n222\n222\n22222222222"};
std::string oneThree{"  333333\n333    333\n3        33\n        333\n     333\n     3333\n       333\n3        33\n333    333\n   3333"};
std::string oneFour{"  44    44\n 44     44\n44      44\n44      44\n44      44\n44444444444\n        44\n        44\n        44\n        44"};
std::string oneFive{" 5555555555\n55\n55\n55\n  555555\n       555\n        555\n         55\n555   555\n  5555"};
std::string oneSix{"    666\n  666\n 66\n66\n66   666\n6666   666\n666     666\n666     666\n 666   666\n   6666"};
std::string oneSeven{"77777777777\n77      777\n        77\n       77\n      77\n     77\n    77\n   77\n  77\n 77"};
std::string oneEight{"   8888\n 88    88\n88      88\n 88    88\n  888888\n 88    88\n88      88\n88      88\n 88    88\n   8888"};
std::string oneNine{"   9999\n 99    99\n99      99\n 99    999\n   999 99\n      99\n     99\n    99\n   99\n  99"};
std::string oneZero{"    000\n 000   000\n00       00\n00       00\n00       00\n00       00\n00       00\n00       00\n00       00\n 000   000\n    000"};
std::string oneColon{"\n\n\n     &\n    &&\n\n     &\n    &&"};

void badInput(std::string customMessage = "") {mvprintw(0, 0, "That's not a properly formatted command. %s", customMessage.c_str());}

void settup () {
    firstDigit = newwin(10,11,3,0);
    secondDigit = newwin(10,11,3,12);
    thirdDigit = newwin(10,11,3,24);
    fourthDigit = newwin(10,11,3,36);
    fifthDigit = newwin(10,11,3,48);
    sixthDigit = newwin(10,11,3,60);
    seventhDigit = newwin(10,11,3,72);
    eighthDigit = newwin(10,11,3,84);
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
    mvprintw(0,0,"");
    clrtoeol();
    mvprintw(0, 0, asString.c_str());
    mvprintw(1,rightwardness, "");
}

void showCount () {
    // wipeTime();
    std::string hours {std::to_string(countNow / 3600)};
    std::string minutes {std::to_string(countNow % 3600 / 60)};
    std::string seconds {std::to_string(countNow % 60)};
    std::string formattedTime{hours + ":" + minutes + ":" + seconds};
    mvprintw(2,0,formattedTime.c_str());
    // std::string numeral{};
    // for (int i{0}; i < 8; ++i) {
    //     switch (formattedTime[i]) {
    //         case '0':
    //             numeral = oneZero;
    //             break;
    //         case '1':
    //             numeral = oneOne;
    //             break;
    //         case '2':
    //             numeral = oneOne;
    //             break;
    //         case '3':
    //             numeral = oneOne;
    //             break;
    //         case '4':
    //             numeral = oneOne;
    //             break;
    //         case '5':
    //             numeral = oneOne;
    //             break;
    //         case '6':
    //             numeral = oneOne;
    //             break;
    //         case '7':
    //             numeral = oneOne;
    //             break;
    //         case '8':
    //             numeral = oneOne;
    //             break;
    //         case '9':
    //             numeral = oneOne;
    //             break;
    //         case ':':
    //             numeral = oneOne;
    //             break;             
    //     }
    // }
    mvprintw(1, rightwardness,"");
}

void newStart () {
    countAsOfLastAnchor = countNow;
    anchorTime = time(NULL);    
    showCount();
}

void enactTimeSumChange (std::string timeChange) {
    std::string timeChangeError{"Time modifications need to be in either '+/-##' format (for minutes) or '+/-##:##' format (for hours:minutes)."};
    int i{1};
// Itterate until something that's NOT a number.
    while (std::isdigit(timeChange[i]) && i < timeChange.length()) {  
        ++i;      
    }
    if (timeChange[i] == ':') {
// Remember that substr()'s second parameter is the LENGTH of the substring, not the index of its end.
        std::string hoursAsString{timeChange.substr(0, i)};
        int hours{std::stoi(hoursAsString)};
        int j {i + 1};
        while (std::isdigit(timeChange[j]) && j < timeChange.length()) {  
            ++j;      
        }
        if (j < timeChange.length()) {
            badInput(timeChangeError);           
        }
        else {
            int minutes{std::stoi(timeChange.substr(i + 1, j - i - 1))};
            countNow += (hours * 60 + minutes) * 60;
        }
    }
    else if (i < timeChange.length()) {
        badInput(timeChangeError);
    }    
    else {
        countNow += std::stoi(timeChange) * 60;
    }
    newStart();
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
            throw std::invalid_argument("Something went wrong!");
    }
    newStart();
    try {
        multiplier = std::stof(newMultiplier);
        showTimeFactor();
    }
    catch (const std::invalid_argument& whoops) {
        badInput("Changes to the time muliplier should look like this: 'u1' or 'd3.33'.");
    }
}

void processCommand () {
    std::string command{};
    char charInQuestion{};
    rightwardness = 0;    
    while ((charInQuestion = mvinch(1, rightwardness)) != ' ') {
        command += charInQuestion;
        ++rightwardness;
        }  
    rightwardness = 0;
    mvprintw(1, rightwardness, "");            
    clrtoeol();
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
            badInput();
    }
}

int main () {
    char fileName [21] {"acCOUNTant_state.txt"};
    WINDOW *myWindow{initscr()};
    raw();
    nodelay(stdscr, true);
    int lastCharHit{};
    int i{};
    std::fstream readerWriter {};
// This is necessary because fstream normally doesn't create a new file if its target doesn't exist. We also need to supply a value to be read a few lines later.
    if (std::filesystem::exists(fileName) == false) {
        readerWriter.open(fileName, std::fstream::out);
        readerWriter << "0";
        readerWriter.close();
    }
    readerWriter.open(fileName, std::fstream::out | std::fstream::in);
    std::string temp{};
    readerWriter >> temp;
    countNow = countAsOfLastAnchor = std::stoi(temp);
    showTimeFactor();
    showCount();
    while (lastCharHit != 'q') {
        lastCharHit = mvgetch(1, rightwardness);
        if (running) {
            countNow = countAsOfLastAnchor + difftime(time(NULL), anchorTime) * multiplier;
            showCount();
            if (i % 500 == 0) {
                readerWriter.seekp(0);
                if (!(readerWriter << countNow << "         ")) {
                    throw std::runtime_error("Problem writing to file.");
                }
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
            processCommand();
        }
        else if (lastCharHit != ERR) {
            if (mvinch(0, 0) != 'x') {
                showTimeFactor();
            }
            mvprintw(1, ++rightwardness - 1, "%c", lastCharHit);
        }      
        refresh();
        i = (i + 1) % 1000;
        Sleep(10);
    }
    endwin();
    return 0;
}