#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

static void enable_any_event_mouse(void) {
    // any-event(1003) + SGR(1006) mouse
    printf("\033[?1003h\033[?1006h");
    fflush(stdout);
}
static void disable_any_event_mouse(void) {
    printf("\033[?1003l\033[?1006l");
    fflush(stdout);
}

int main() {

    initscr();
    noecho(); //キーが入力されても表示しない
    curs_set(0);//カーソルを非表示
    keypad(stdscr, TRUE); // xtermでマウスイベントの取得に必
    mouseinterval(0);

    mmask_t old;
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &old  );//マウスイベントを取得

    // 端末によっては ncurses の自動設定だけだと 1002 止まりになることがあるため、
    // 念のため 1003/1006 を手動で有効化
    enable_any_event_mouse();

    nodelay(stdscr, FALSE);
    curs_set(0);

    MEVENT e;
    int px = 2;

    // getch の タイムアウトを最低値にする
    // 0 にもできるが、リフレッシュでちらついてしまう
    timeout(10);

    clock_t start_time = clock();
    clock_t end_time;

    int i = 0; // テストアニメーション用カウンタ

    while (true) {
        int ch = getch();
        if (ch == 'q') break;        
        if (ch == KEY_MOUSE) {
            if (getmouse(&e) == OK) {
                px = e.x;
                if (px < 2)px = 2;
                if (px > 77)px = 77;
            }
        }

        // アニメーション用に経過時間を計測する
        end_time = clock();
        double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // 経過時間が 0.004秒以上ならばテストアニメーションを更新する
        // なお、Timeout 時だけ処理す場合は
        // if( ch == ERR ){ などとする
        if (elapsed_time >= 0.004) {
            i++;
            if( i > 78 ) {
                i = 0;
            }
            start_time = clock();
        }

        clear();
        // mvprintw(0, i, "aaa\n");    // テストアニメーション
        mvprintw(23, px - 2, "-----");

        // かかった時間の出力を行うデバッグコード
        // mvprintw(15, 10, "%f \n", elapsed_time);
        refresh();

    }

    disable_any_event_mouse();
    endwin();
    return 0;
}