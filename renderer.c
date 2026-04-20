#include <assert.h>
#include <locale.h>
#include <math.h>
#include <ncurses.h>

void renderer_init(void) {
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    curs_set(0);
}

void renderer_refresh(void) {
    refresh();
}

void renderer_clear(void) {
    erase();
}

void renderer_shutdown(void) {
    endwin();
}

void render_bar(float height, int x_offset) {
    assert(height >= 0.0f && x_offset >= 0);

    static const char *blocks[] = {" ", "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    assert(x_offset < max_x);

    int full_rows = (int)height;
    float row_fraction = height - (float)full_rows;
    int row_partial = (int)roundf(row_fraction * 8.0f);

    if (row_partial == 8) {
        full_rows += 1;
        row_partial = 0;
    }

    int y = max_y - 1;

    for (int i = 0; i < full_rows; i++) {
        if (y < 0) {
            break;
        }

        mvaddstr(y, x_offset, blocks[8]);

        y--;
    }

    if (row_partial > 0 && y >= 0) {
        mvaddstr(y, x_offset, blocks[row_partial]);
    }
}
