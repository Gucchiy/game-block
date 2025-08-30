#include <locale.h>
#include <ncursesw/curses.h>
#include <unistd.h>

int main(void){
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) { start_color(); init_pair(1, COLOR_CYAN, COLOR_BLACK); }

    int y, x; getmaxyx(stdscr, y, x);
    int px = x/2, py = y/2;  // プレイヤー
    int ch;

    while(1){
        ch = getch();
        if (ch == 'q' || ch == 27) break;
        else if (ch == KEY_UP)    py--;
        else if (ch == KEY_DOWN)  py++;
        else if (ch == KEY_LEFT)  px--;
        else if (ch == KEY_RIGHT) px++;

        // 画面外に出ない
        if (px < 0) px = 0; if (px >= x) px = x-1;
        if (py < 0) py = 0; if (py >= y) py = y-1;

        clear();
        attron(COLOR_PAIR(1) | A_BOLD);
        mvaddstr(0, 2, "Arrow: Move  q: Quit");
        attroff(COLOR_PAIR(1) | A_BOLD);
        mvaddch(py, px, '@');
        refresh();
        usleep(16000); // ~60fps
    }

    endwin();
    return 0;
}
