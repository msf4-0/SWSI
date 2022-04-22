// #ifndef EPRINT_H
// #define EPRINT_H
//
// #define ENABLE_GxEPD2_GFX 1
//
// #include <GxEPD2_BW.h>  // GxEPD2 library for black and white displays
// #include <Fonts/FreeMono9pt7b.h>
// #include <Fonts/FreeMono12pt7b.h>
// #include <Fonts/FreeMonoBold9pt7b.h>
// #include <Fonts/FreeMonoBold12pt7b.h>
//
// //#include "WeighModule.h"
//
// class Eprint {
//   private:
//     byte cs;
//     byte dc;
//     byte rst;
//     byte busy;
//     int mainIndex;
//     // GxEPD2_GFX* disp;
//
//   public:
//     // Constructor
//     Eprint(byte cs, byte dc, byte rst, byte busy);
//
//     // void updateDisplay(WeighModule *module, int page, int state);
//
//     void updateDisplay(GxEPD2_GFX& disp, WeighModule *module, int state);
//     void clear(GxEPD2_GFX& disp);
//
//     int getMenuIndex();
// };
// #endif
