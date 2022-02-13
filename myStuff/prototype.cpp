#include "curses.h"
#include <stdexcept>
#include <Windows.h>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

bool running {false};
int anchorTime{time(NULL)};
int countAsOfLastAnchor{0};
int countNow{0};
float multiplier{1};
int rightwardness{0};
char fileName [21] {"acCOUNTant_state.txt"};
std::fstream readerWriter {fileName};

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
WINDOW *digits [8] {};

std::string oneOne  {"    111\n  11111\n    111\n    111\n    111\n    111\n    111\n    111\n    111\n  1111111"};
std::string oneTwo  {"   22222\n 222   222\n222     222\n      222\n     222\n   222\n 222\n222\n222\n22222222222"};
std::string oneThree{"  333333\n333    333\n3        33\n        333\n     333\n     3333\n       333\n3        33\n333    333\n   3333"};
std::string oneFour {"  44    44\n 44     44\n44      44\n44      44\n44      44\n44444444444\n        44\n        44\n        44\n        44"};
std::string oneFive {" 5555555555\n55\n55\n55\n  555555\n       555\n        555\n         55\n555   555\n  5555"};
std::string oneSix  {"    666\n  666\n 66\n66\n66   666\n6666   666\n666     666\n666     666\n 666   666\n   6666"};
std::string oneSeven{"77777777777\n77      777\n        77\n       77\n      77\n     77\n    77\n   77\n  77\n 77"};
std::string oneEight{"   8888\n 88    88\n88      88\n 88    88\n  888888\n 88    88\n88      88\n88      88\n 88    88\n   8888"};
std::string oneNine {"   9999\n 99    99\n99      99\n 99    999\n   999 99\n      99\n     99\n    99\n   99\n  99"};
std::string oneZero {"    000\n 000   000\n00       00\n00       00\n00       00\n00       00\n00       00\n00       00\n 000   000\n    000"};
std::string oneColon{"\n\n\n    &\n   &&\n\n    &\n   &&"};

void badInput(std::string customMessage = "") {mvprintw(0, 0, "That's not a properly formatted command. %s", customMessage.c_str());}

void settup () {
    initscr();
    raw();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    digits [0] = firstDigit = newwin(10,12,2,0);
    digits [1] = secondDigit = newwin(10,12,2,12);
    digits [2] = thirdDigit = newwin(10,12,2,24);
    digits [3] = fourthDigit = newwin(10,12,2,36);
    digits [4] = fifthDigit = newwin(10,12,2,48);
    digits [5] = sixthDigit = newwin(10,12,2,60);
    digits [6] = seventhDigit = newwin(10,12,2,72);
    digits [7] = eighthDigit = newwin(10,12,2,84);
    refresh();
}

void wipeTime () {
    for (WINDOW *inQuestion : digits) {
        wclear(inQuestion);
// I don't understand why it has to be this, not normal refresh() at the end, but if I do it that way then it doesn't work.
        wrefresh(inQuestion);
    }
}

int parseTime(std::string input) {
    int i{0};
// Itterate until something that's NOT a number.
    while (std::isdigit(input[i]) && i < input.length()) {  
        ++i;      
    }
    if (input[i] == ':') {
// This is the case that the input ends in a colon.
        if (i == input.length() - 1) {
            return -616616;
        }
// Remember that substr()'s second parameter is the LENGTH of the substring, not the index of its end.
        std::string hoursAsString{input.substr(0, i)};
        int hours{std::stoi(hoursAsString)};
        int j {i + 1};
        while (std::isdigit(input[j]) && j < input.length()) {  
            ++j;      
        }
        if (j < input.length()) {
            return -616616;           
        }
        else {
            int minutes{std::stoi(input.substr(i + 1, j - i - 1))};
            return (hours * 60 + minutes) * 60;
        }
    }
    else if (i < input.length()) {
        return -616616;
    }    
    else {
        return std::stoi(input) * 60;
    }
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
    wipeTime();
    std::string hours {std::to_string(countNow / 3600)};
    std::string minutes {std::to_string(countNow % 3600 / 60)};
    std::string seconds {std::to_string(countNow % 60)};
    std::string formattedTime{hours + ":" + minutes + ":" + seconds};
    std::string numeral{};
    for (int i{0}; i < formattedTime.length(); ++i) {
        switch (formattedTime[i]) {
            case '0':
                numeral = oneZero;
                break;
            case '1':
                numeral = oneOne;
                break;
            case '2':
                numeral = oneTwo;
                break;
            case '3':
                numeral = oneThree;
                break;
            case '4':
                numeral = oneFour;
                break;
            case '5':
                numeral = oneFive;
                break;
            case '6':
                numeral = oneSix;
                break;
            case '7':
                numeral = oneSeven;
                break;
            case '8':
                numeral = oneEight;
                break;
            case '9':
                numeral = oneNine;
                break;
            case ':':
                numeral = oneColon;
                break;             
        }
        wprintw(digits[i], numeral.c_str());
        wrefresh(digits[i]);
    }
}

void newStart () {
    countAsOfLastAnchor = countNow;
    anchorTime = time(NULL);    
    showCount();
}

void writeToFile () {
    readerWriter.seekp(0);
    if (!(readerWriter << countNow << "         ")) {
        throw std::runtime_error("Problem writing to file.");
    }
    readerWriter.flush();
}

void enactSetCount (std::string newTime) {
    int parsed {parseTime(newTime.substr(1))};
    if (parsed == -616616) {
        badInput("Time modifications need to be in either '+/-##' format (for minutes) or '+/-##:##' format (for hours:minutes).");
    }
    else {
        countNow = parsed;
        writeToFile();
        newStart();
    }
    running = false;
}

void enactAddDeductFromCount (std::string timeChange) {
    int parsed {parseTime(timeChange)};
    if (parsed == 0) {
        badInput("Time modifications need to be in either '+/-##' format (for minutes) or '+/-##:##' format (for hours:minutes).");
    }
    else {
        countNow += parsed;
        newStart();
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
            enactAddDeductFromCount(command);
            break;
        case 's':
            enactSetCount(command);
            break;
        default:
            badInput();
    }
}

int main () {
    WINDOW *myWindow{initscr()};
    settup();
    int lastCharHit{};
    int i{};
// This is necessary because fstream normally doesn't create a new file if its target doesn't exist. We also need to supply a value to be read a few lines later.
    if (readerWriter.peek() == EOF) {
        readerWriter.open(fileName, std::fstream::out);
        readerWriter << "0";
        readerWriter.clear();
        readerWriter.close();
        readerWriter.open(fileName);
    }
    std::string temp{};
    readerWriter >> temp;
    readerWriter.clear();
    countNow = countAsOfLastAnchor = std::stoi(temp);
    int previousCount{countNow};
    showTimeFactor();
    showCount();
    while (lastCharHit != 'q') {
        lastCharHit = mvgetch(1, rightwardness);
        if (running) {
            countNow = countAsOfLastAnchor + difftime(time(NULL), anchorTime) * multiplier;
            if (countNow != previousCount) {                            
                showCount();
                previousCount = countNow;
            }
            if (i % 500 == 0) {
                writeToFile();
            }
        }
        if (lastCharHit == ' ') {            
            running = !running;
            if (running == true) {
                newStart();               
            }
            mvprintw(1, rightwardness,"");
        }
        else if (lastCharHit == 13 || lastCharHit == PADENTER) {
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