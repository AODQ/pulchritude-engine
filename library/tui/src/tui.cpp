#include <pulchritude-tui/tui.h>

#include <ncurses.h>

extern "C" {

PuleTuiWindow puleTuiInitialize() {
  /*ncurse_*/WINDOW * scrWindow = /*ncurse_*/initscr();
  (void)scrWindow;
  /*ncurse_*/timeout(3000);
  ///*ncurse_*/keypad(stdscr, TRUE);
  ///*ncurse_*/cbreak();
  ///*ncurse_*/raw();
  ///*ncurse_*/noecho();
  /*ncurse_*/start_color();

  PuleTuiWindow const window = {
    .id = (
      reinterpret_cast<uint64_t>(/*ncurse_*/newwin(0, 0, 0, 0)) // full screen
    ),
  };

  return window;
}

void puleTuiDestroy(PuleTuiWindow const window) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  /*ncurse_*/delwin(ncurseWindow);
  /*ncurse_*/endwin();

}

void puleTuiClear(PuleTuiWindow const window) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  /*ncurse_*/wclear(ncurseWindow);
}

void puleTuiRefresh(PuleTuiWindow const window) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  /*ncurse_*/wrefresh(ncurseWindow);
}

void puleTuiRenderString(
  PuleTuiWindow const window,
  PuleStringView const content,
  PuleF32v3 const rgb
) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  /*ncurse_*/init_color(
    101,
    (int32_t)(rgb.x/255.0f),
    (int32_t)(rgb.y/255.0f),
    (int32_t)(rgb.z/255.0f)
  );
  /*ncurse_*/init_pair(8, 101, COLOR_BLACK);
  /*ncurse_*/wattron(ncurseWindow, /*NCURSE_*/COLOR_PAIR(8));
  for (size_t it = 0; it < content.len; ++ it) {
    /*ncurse_*/waddch(ncurseWindow, content.contents[it]);
  }
  /*ncurse_*/wattroff(ncurseWindow, /*NCURSE_*/COLOR_PAIR(8));
}

int32_t puleTuiReadInputCharacterBlocking(PuleTuiWindow const window) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  return /*ncurse_*/wgetch(ncurseWindow);
}

void puleTuiMoveCursor(PuleTuiWindow const window, PuleI32v2 const position) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  /*ncurse_*/wmove(ncurseWindow, position.y, position.x); // flip xy
}

PuleI32v2 puleTuiWindowDim(PuleTuiWindow const window) {
  auto ncurseWindow = reinterpret_cast</*ncurse_*/WINDOW *>(window.id);
  return PuleI32v2 {
    /*ncurse_*/getmaxx(ncurseWindow),
    /*ncurse_*/getmaxy(ncurseWindow),
  };
}

} // C
