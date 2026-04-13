#include "keyboard_hook.h"

static KeyboardHook *activeKeyboardHook = nullptr;

KeyboardHook::KeyboardHook(QObject *parent) :
		QObject(parent),
		hook(nullptr)
{
	activeKeyboardHook = this;
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookCallback, nullptr, 0);
}

KeyboardHook::~KeyboardHook()
{
	if (hook)
	{
		UnhookWindowsHookEx(hook);
		hook = nullptr;
	}

	if (activeKeyboardHook == this)
	{
		activeKeyboardHook = nullptr;
	}
}

bool KeyboardHook::isInstalled() const
{
	return hook != nullptr;
}

LRESULT CALLBACK KeyboardHook::hookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && wParam == WM_KEYDOWN && activeKeyboardHook)
	{
		auto *keyboardEvent = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
		emit activeKeyboardHook->keyPressed(keyboardEvent->vkCode);
	}

	const auto hookHandle = activeKeyboardHook ? activeKeyboardHook->hook : nullptr;
	return CallNextHookEx(hookHandle, nCode, wParam, lParam);
}
