/**
 * @file keyboard_hook.cpp
 * @brief 全局键盘和鼠标钩子实现。
 */

#include "keyboard_hook.h"
#include "global_definitions.h"

static KeyboardHook *activeKeyboardHook = nullptr;

KeyboardHook::KeyboardHook(QObject *parent)
	: QObject(parent)
	, keyboardHook(nullptr)
	, mouseHook(nullptr)
{
}

KeyboardHook::~KeyboardHook()
{
	uninstall();
}

bool KeyboardHook::install()
{
	if (isInstalled())
	{
		return false;
	}

	uninstall();
	activeKeyboardHook = this;
	keyboardHook	   = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookCallback, nullptr, 0);
	mouseHook		   = SetWindowsHookEx(WH_MOUSE_LL, mouseHookCallback, nullptr, 0);

	if (!isInstalled())
	{
		uninstall();
		return false;
	}

	return true;
}

bool KeyboardHook::isInstalled() const
{
	return keyboardHook != nullptr && mouseHook != nullptr;
}

void KeyboardHook::uninstall()
{
	if (keyboardHook)
	{
		UnhookWindowsHookEx(keyboardHook);
		keyboardHook = nullptr;
	}

	if (mouseHook)
	{
		UnhookWindowsHookEx(mouseHook);
		mouseHook = nullptr;
	}

	if (activeKeyboardHook == this)
	{
		activeKeyboardHook = nullptr;
	}
}

LRESULT CALLBACK KeyboardHook::keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && wParam == WM_KEYDOWN && activeKeyboardHook)
	{
		const auto *keyboardEvent = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
		emit		activeKeyboardHook->keyPressed(keyboardEvent->vkCode);
	}

	const auto hookHandle = activeKeyboardHook ? activeKeyboardHook->keyboardHook : nullptr;
	return CallNextHookEx(hookHandle, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHook::mouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && activeKeyboardHook)
	{
		const auto *mouseEvent = reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);
		switch (wParam)
		{
			case WM_LBUTTONDOWN:
				emit activeKeyboardHook->keyPressed(VK_LBUTTON);
				break;
			case WM_MBUTTONDOWN:
				emit activeKeyboardHook->keyPressed(VK_MBUTTON);
				break;
			case WM_RBUTTONDOWN:
				emit activeKeyboardHook->keyPressed(VK_RBUTTON);
				break;
			case WM_MOUSEWHEEL:
				emit activeKeyboardHook->keyPressed(GET_WHEEL_DELTA_WPARAM(mouseEvent->mouseData) > 0 ? FS_WHEEL_UP : FS_WHEEL_DOWN);
				break;
			default:
				break;
		}
	}

	const auto hookHandle = activeKeyboardHook ? activeKeyboardHook->mouseHook : nullptr;
	return CallNextHookEx(hookHandle, nCode, wParam, lParam);
}
