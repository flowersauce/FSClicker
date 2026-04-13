#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

#include <windows.h>
#include <QObject>

class KeyboardHook : public QObject
{
Q_OBJECT

public:
	explicit KeyboardHook(QObject *parent = nullptr);
	~KeyboardHook() override;

	bool isInstalled() const;

signals:
	void keyPressed(DWORD keyCode);

private:
	static LRESULT CALLBACK hookCallback(int nCode, WPARAM wParam, LPARAM lParam);

	HHOOK hook;
};

#endif // KEYBOARD_HOOK_H
