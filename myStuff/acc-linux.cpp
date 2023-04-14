#include "ncurses.h"
#include "panel.h"
#include <stdexcept>
#include <math.h>
#include <chrono>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <unistd.h>

bool running {false};
int enter_keycode{};
long anchorTime{};
int countAsOfLastAnchor{0};
int countNow{0};
float multiplier{1};
int rightwardness{0};
bool manualVisible{false};
char fileName [21] {"acCOUNTant_state.txt"};
std::fstream readerWriter {fileName};

WINDOW *notices {};
WINDOW *inputSpace{};
WINDOW *minusSign{};
WINDOW *manual{};
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

void badInput(std::string customMessage = "That's not a properly formatted command. Enter 'help' for instructions.") {mvprintw(0, 0, "%s", customMessage.c_str());}

void settup () {
    initscr();
    raw();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    manual = newwin(20, 110, 2, 0);
    minusSign = newwin(2, 7, 6, 1);    
    digits [0] = firstDigit = newwin(10,12,2,10);
    digits [1] = secondDigit = newwin(10,12,2,22);
    digits [2] = thirdDigit = newwin(10,12,2,34);
    digits [3] = fourthDigit = newwin(10,12,2,46);
    digits [4] = fifthDigit = newwin(10,12,2,58);
    digits [5] = sixthDigit = newwin(10,12,2,70);
    digits [6] = seventhDigit = newwin(10,12,2,82);
    digits [7] = eighthDigit = newwin(10,12,2,94);
    refresh();
}

long timeAsSeconds () {
    using namespace std::chrono;
    static time_point<system_clock> timeNow {system_clock::now()};
    static seconds nowSeconds {duration_cast<seconds>(timeNow.time_since_epoch())};
    return nowSeconds.count();
}

void wipeTime () {
    for (WINDOW *inQuestion : digits) {
        wclear(inQuestion);
// I don't understand why it has to be this, not normal refresh() at the end, but if I do it that way then it doesn't work.
        wrefresh(inQuestion);
    }
}

int parseTime(std::string input) {
    int errorNumber(-616616);
    int i{0};
    if (input[0] == '-' || input[0] == '+') {
        i = 1;
    }
// Itterate until something that's NOT a number.
    while (std::isdigit(input[i]) && i < input.length()) {  
        ++i;      
    }
    if (input[i] == ':') {
// This is the case that the input ends in a colon.
        if (i == input.length() - 1) {
            return errorNumber;
        }
// Remember that substr()'s second parameter is the LENGTH of the substring, not the index of its end.
        std::string hoursAsString{input.substr(0, i)};
        int hours{std::stoi(hoursAsString)};
        int j {i + 1};
        while (std::isdigit(input[j]) && j < input.length()) {  
            ++j;      
        }
        if (j < input.length()) {
            return errorNumber;           
        }
        else {
            int minutes{std::stoi(input.substr(i + 1, j - i - 1))};
            return (hours * 60 + minutes) * 60;
        }
    }
    else if (i < input.length()) {
        return errorNumber;
    }    
    else {
        int outcome{std::stoi(input) * 60};
        int temp{std::abs(outcome)};
        if (temp > 359999) {
            badInput("You either entered an extremely large number, or accidentally typed a non-number character.");
            return errorNumber;
        }
        else {
            return outcome;
        }
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
//Absolute values are used here because having minus signs messes things up. Negativity is dealt with at the bottom.
    std::string hours {std::to_string(abs(countNow / 3600))};
    std::string minutes {std::to_string(abs(countNow % 3600 / 60))};
    std::string seconds {std::to_string(abs(countNow % 60))};
    std::string formattedTime{hours + ":" + minutes + ":" + seconds};
    // std::cout << formattedTime << '\n';
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
    if (countNow < 0 && mvwinch(minusSign, 0, 0) != '_') {            
        wprintw(minusSign, "______\n------");
    }
    else if (countNow >= 0 && mvwinch(minusSign, 0, 0) != ' ') {
        wclear(minusSign);
    }
    wrefresh(minusSign); 
}

void newStart () {
    countAsOfLastAnchor = countNow;
    anchorTime = timeAsSeconds();
    // anchorTime = time(NULL);
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
        badInput("Time needs to be in either '+/-##' format (for minutes) or '+/-##:##' format (for hours:minutes).");
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
    if (parsed == -616616) {
        badInput("Time modifications need to be in either '+/-##' format (for minutes) or '+/-##:##' format (for hours:minutes).");
    }
    else {
        countNow += parsed;
        writeToFile();
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
        badInput();
    }
}

void toggleInstructions () {
    if (manualVisible == false) {
        mvprintw(0,0,"");
        clrtoeol();
        wprintw(manual,
            "This is acCOUNTant, a time-management timer.\n"
            "Its intended use is to maintain a 'bank' of time which grows with some tasks and is diminished by others. \n\n"
            
            "The clock can be started or stopped at any time by pressing the spacebar.\n\n"

            "To change how fast the clock counts up, type and enter 'u{multiplier}'\n"
            "So, 'u2' to count up two seconds per second, or 'u.5' to count up half a second per second.\n\n"
        );
        wprintw(manual,
            "If you'd like acCOUNTant to count down instead, like a timer instead of a stopwatch, type and enter 'd{multiplier}'\n"
            "So, 'd2' to count down two seconds per second, or 'd.5' to count down half a second per second.\n\n"

            "You can also manually add or deduct time, using either '+/-##' format (for minutes) or '+/-##:##' format (for hours:minutes).\n"
            "So, '+1' to add one minute to the count, or '-1:1' to deduct one hour and one minute.\n\n"
        );
        wprintw(manual,
            "Finally, if you'd like to directly set the time, you can do so with 's{time}'\n"
            "This uses the same format as additions/deductions, but without the plus/minus signs. 's0' or 's0:0' will both set the time to zero.\n\n"

            "I hope this system helps you as much as it has helped me.\n"
            "-Aidan"
        );
        wrefresh(manual);
        manualVisible = true;
    }
    else {
        wclear(manual);
        wrefresh(manual);
        showCount();
        showTimeFactor();
        manualVisible = false;
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
        case 'h':
            if (command == "help" && manualVisible == false) {
                toggleInstructions();
            }
            else {
                badInput();
            }
            break;
        default:
            badInput();
    }
}

int main () {
    atexit(writeToFile);
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
    printw("Type and enter 'help' for a list of commands.");
    showCount();
    while (lastCharHit != 'q') {
        lastCharHit = mvgetch(1, rightwardness);
        if (running) {
            countNow = countAsOfLastAnchor + difftime(timeAsSeconds(), anchorTime) * multiplier;
            if (countNow != previousCount) {
                // if (signbit(countNow) != signbit(previousCount)) {
                //     MessageBox(GetConsoleWindow(), (LPCTSTR)"It's time.", (LPCTSTR)"beep beep beep!", MB_ICONEXCLAMATION | MB_SETFOREGROUND);
                // }
                if (manualVisible == false) {  
                    showCount();
                }
                previousCount = countNow;
            }
            if (i % 500 == 0) {
                writeToFile();
            }
        }
        if (lastCharHit != ERR) {
            if (manualVisible == true) {
                toggleInstructions();
            }
            else {
                if (mvinch(0, 0) != 'x') {
                    showTimeFactor();
                }
                if (lastCharHit == ' ') {
                    running = !running;
                    if (running == true) {
                        newStart();               
                    }
                    mvprintw(1, rightwardness,"");                    
                }
                else if (lastCharHit == 10) {
                    processCommand();
                }
                else {
                    ++rightwardness;
                    mvprintw(1, rightwardness - 1, "%c", lastCharHit);
                }
            }
        }             
        refresh();
        i = (i + 1) % 1000;
        usleep(10000);
    }
    endwin();
    return 0;
}
