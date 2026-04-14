/**
 * @file keyboard_hook.h
 * @brief 全局键盘和鼠标钩子。
 */

#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

#include <QObject>
#include <windows.h>

/**
 * @brief 把全局键盘、鼠标输入统一转发为按键码。
 */
class KeyboardHook : public QObject
{
	Q_OBJECT

public:
	explicit KeyboardHook(QObject *parent = nullptr);
	~KeyboardHook() override;

	bool install();
	bool isInstalled() const;

signals:
	void keyPressed(DWORD keyCode);

private:
	static LRESULT CALLBACK keyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK mouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

	void uninstall();

	HHOOK keyboardHook;
	HHOOK mouseHook;
};

#endif // KEYBOARD_HOOK_H
