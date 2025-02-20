#ifndef PICROSS_MENU_H_
#define PICROSS_MENU_H_

#include "mode_picross.h"

extern swadgeMode modePicross;

void setPicrossMainMenu(bool resetPos);
void returnToPicrossMenu(void);
void returnToLevelSelect(void);
void selectPicrossLevel(picrossLevelDef_t* selectedLevel);
void exitTutorial(void);
void picrossSetSaveFlag(int pos, bool on);
bool picrossGetSaveFlag(int pos);

#endif