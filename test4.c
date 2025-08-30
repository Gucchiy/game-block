#include <ncurses.h>
#include <stdlib.h>

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mouseinterval(0);

    mmask_t old;
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &old);

    // 端末によっては REPORT_MOUSE_POSITION を明示要求すると安定
    // ncurses が通常はやってくれますが、効かない場合だけ試す:
    // printw("\033[?1003h"); refresh();

    printw("TERM=%s\n", getenv("TERM"));
    printw("Click or move mouse. Press 'q' to quit.\n");
    refresh();

    MEVENT me;
    int ch;
    while ((ch = getch()) != 'q') {
        if (ch == KEY_MOUSE) {
            if (getmouse(&me) == OK) {
                // ボタンや移動の種類を表示
                clear();
                mvprintw(0,0,"TERM=%s", getenv("TERM"));
                mvprintw(1,0,"Mouse: x=%d y=%d bstate=0x%lx", me.x, me.y, (unsigned long)me.bstate);
                mvprintw(3,0,"Press 'q' to quit.");
                refresh();
            }
        }
    }

    endwin();
    return 0;
}
