#ifndef _MODE_PICROSSLEVELSELECT_H_
#define _MODE_PICROSSLEVELSELECT_H_

#include "swadgeMode.h"
#include "aabb_utils.h"

typedef struct {
    uint8_t index;
    wsg_t levelWSG;
    wsg_t completedWSG;
    bool completed;
} picrossLevelDef_t;

typedef struct
{
    display_t* disp;
    font_t* game_font;
    uint8_t levelCount;
    picrossLevelDef_t levels[8];   
    picrossLevelDef_t* chosenLevel;
    uint8_t gridScale;
    int8_t hoverLevelIndex;
    int8_t hoverX;
    int8_t hoverY;
    uint8_t cols;
    uint16_t prevBtnState;
    uint16_t btnState;
    uint8_t paddingTop;
    uint8_t paddingLeft;
    uint8_t gap;
    wsg_t unknownPuzzle;
} picrossLevelSelect_t;



void picrossStartLevelSelect(display_t* disp, font_t* mmFont, picrossLevelDef_t levels[]);
void picrossLevelSelectLoop(int64_t elapsedUs);
void picrossLevelSelectButtonCb(buttonEvt_t* evt);
void picrossExitLevelSelect(void);

#endif