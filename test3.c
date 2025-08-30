#include <locale.h>
#include <ncursesw/curses.h>
#include <stdio.h>

static void enable_mouse_tracking(void) {
    // 1006: SGR拡張、1003: 任意移動の逐次レポート（かなり多く飛びます）
    printf("\033[?1006h\033[?1003h");
    fflush(stdout);
}
static void disable_mouse_tracking(void) {
    printf("\033[?1003l\033[?1006l");
    fflush(stdout);
}

int main(void) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);   // 非ブロッキング
    curs_set(0);

    if (has_colors()) { start_color(); init_pair(1, COLOR_CYAN, COLOR_BLACK); }

    mouseinterval(0); // クリックを遅延まとめしない
    mmask_t oldmask;
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &oldmask);

    enable_mouse_tracking();

    int ch;
    MEVENT me;
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    while (1) {
        ch = getch();
        if (ch == 'q' || ch == 27) break;

        clear();
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(0, 2, "Mouse tracker  (move / click / wheel)   q or ESC: quit");
        attroff(COLOR_PAIR(1) | A_BOLD);

        // 端末サイズ
        getmaxyx(stdscr, maxy, maxx);
        mvprintw(1, 2, "TERM=%s  size=%dx%d", getenv("TERM") ? getenv("TERM") : "?", maxx, maxy);

        if (ch == KEY_MOUSE) {
            if (getmouse(&me) == OK) {
                // bstate はビットフラグ。例:
                // BUTTON1_PRESSED, BUTTON1_RELEASED, BUTTON1_CLICKED,
                // BUTTON4_PRESSED(ホイール↑), BUTTON5_PRESSED(ホイール↓),
                // REPORT_MOUSE_POSITION など
                mvprintw(3, 2, "Event: id=%d  x=%d y=%d z=%d  bstate=0x%08lx",
                         me.id, me.x, me.y, me.z, (unsigned long)me.bstate);

                // 状態をもう少し分かりやすく表示
                int row = 5;
                if (me.bstate & BUTTON1_PRESSED)        mvprintw(row++, 4, "BUTTON1_PRESSED");
                if (me.bstate & BUTTON1_RELEASED)       mvprintw(row++, 4, "BUTTON1_RELEASED");
                if (me.bstate & BUTTON1_CLICKED)        mvprintw(row++, 4, "BUTTON1_CLICKED");
                if (me.bstate & BUTTON1_DOUBLE_CLICKED) mvprintw(row++, 4, "BUTTON1_DOUBLE_CLICKED");
                if (me.bstate & BUTTON2_PRESSED)        mvprintw(row++, 4, "BUTTON2_PRESSED");
                if (me.bstate & BUTTON3_PRESSED)        mvprintw(row++, 4, "BUTTON3_PRESSED");
#ifdef BUTTON4_PRESSED
                if (me.bstate & BUTTON4_PRESSED)        mvprintw(row++, 4, "WHEEL_UP (BUTTON4_PRESSED)");
#endif
#ifdef BUTTON5_PRESSED
                if (me.bstate & BUTTON5_PRESSED)        mvprintw(row++, 4, "WHEEL_DOWN (BUTTON5_PRESSED)");
#endif
                if (me.bstate & REPORT_MOUSE_POSITION)  mvprintw(row++, 4, "REPORT_MOUSE_POSITION");
            }
        } else {
            mvprintw(3, 2, "Move the mouse inside the terminal...");
        }

        // 現在位置の目印
        // 直近の座標が分からないときは描かない（KEY_MOUSEが来たフレームのみ描画）
        if (ch == KEY_MOUSE && me.y >= 0 && me.y < maxy && me.x >= 0 && me.x < maxx) {
            mvaddch(me.y, me.x, '@' | A_BOLD);
        }

        refresh();
        napms(10); // CPU負荷を下げる
    }

    // 後片付け
    disable_mouse_tracking();
    mousemask(oldmask, NULL);
    endwin();
    return 0;
}
