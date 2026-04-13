#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <unordered_map>
#include <string>
#include <windows.h>


enum pages              // 页面枚举
{
    IOCONFIGPAGE,
    ABOUTPAGE,
};

enum inputKeyType       // 输入按键类型枚举
{
    MOUSELEFTKEY,
    MOUSEMIDDLEKEY,
    MOUSERIGHTKEY,
    DIYKEY,
};

enum inputActionType    // 输入动作类型枚举
{
    CLICKS,
    PRESS,
};

enum cursorMoveType     // 光标移动类型枚举
{
    FREE,
    LOCK,
};


extern const std::unordered_map<DWORD, std::string> keyMap;     // 虚拟键值映射表

#endif //GLOBAL_DEFINITIONS_H
