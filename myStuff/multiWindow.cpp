#include <curses.h>
#include <Windows.h>

WINDOW *leftW{};
WINDOW *rightW{};

int main() {
	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	leftW = newwin(0,0,0,0);
	rightW = newwin(5,5,5,5);
	refresh();
    printw("before");
	wprintw(leftW, "left");
	wprintw(rightW, "right");
	wrefresh(leftW);
	wrefresh(rightW);
	// wgetch(leftW);
	// wgetch(rightW);
	// refresh();			/* Print it on to the real screen */
    // getch();			/* Wait for user input */
	while (true) {
		Sleep(10);
	}
	endwin();			/* End curses mode		  */
	return 0;
}