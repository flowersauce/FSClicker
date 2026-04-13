/**
 * @file global_definitions.h
 * @brief 输入模式和按键名称表。
 */

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

/**
 * @brief 内部滚轮按键码，避开 Win32 虚拟键的 WORD 范围。
 */
constexpr DWORD FS_WHEEL_UP	  = 0x10001;
constexpr DWORD FS_WHEEL_DOWN = 0x10002;

#endif // GLOBAL_DEFINITIONS_H
