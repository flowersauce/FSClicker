#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <string>
#include <unordered_map>
#include <windows.h>

enum InputKeyType
{
    MOUSE_LEFT_KEY,
    MOUSE_MIDDLE_KEY,
    MOUSE_RIGHT_KEY,
    DIY_KEY,
};

enum InputActionType
{
    CLICKS,
    PRESS,
};

enum CursorMoveType
{
    FREE,
    LOCK,
};

extern const std::unordered_map<DWORD, std::string> keyMap;

#endif // GLOBAL_DEFINITIONS_H
