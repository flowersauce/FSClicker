#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <windows.h>
#include <array>
#include <QButtonGroup>
#include <QThread>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "global_definitions.h"
#include "custom_widget.h"
#include "coordinate_capture_window.h"
#include "event_injector.h"
#include "keyboard_hook.h"


QT_BEGIN_NAMESPACE

namespace Ui
{
	class MainWindow;
}

QT_END_NAMESPACE


class MainWindow : public CustomWidget
{
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

	QString primaryWidgetColor;
	QString secondaryWidgetColor;

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private:
	Ui::MainWindow *ui;
	QMediaPlayer *soundEffectPlayer;
	QAudioOutput *soundEffectAudioOutput;
	QThread *eventInjectorThread;
	EventInjector *eventInjector;
	KeyboardHook *keyboardHook;
	QButtonGroup *pageButtonGroup;
	QButtonGroup *inputKeyButtonGroup;
	QButtonGroup *inputActionButtonGroup;
	QButtonGroup *cursorMoveButtonGroup;
	bool mainWindowPin;
	bool mousePressed;
	bool eventInjectorRunning;
	bool getGlobalSwitchKeyHook_flag;                               // 获取全局开关键钩子工作状态
	bool getDIYKeyHook_flag;                                        // 获取自定义按键钩子工作状态
	bool startEventInjectorAllowed_flag;							// 事件注入器允许使用标志
	QPoint mouseStartPoint;                                         // 记录鼠标按下时的全局位置
	QPoint windowStartPoint;                                        // 记录窗口初始位置
	int pageNum;                                                    // 页码
	int inputKey;                                                   // 输入按键
	int inputActionMode;                                            // 输入行为模式
	int cursorMoveMode;                                             // 光标位置模式
	DWORD globalSwitchKey;                                          // 全局开关键
	DWORD diyKey;                                                   // 自定义按键
	std::array<int, 2> coordinateXY;                                // 坐标

	void initializeEventInjector();
	void initializeKeyboardHook();
	void initializeAudio();
	void initializeButtonGroups();
	void initializeAppearance();
	void initializeInputValidator();
	void initializeFont();
	void initializeAboutPage();
	void initializePageState();
	void connectSignals();
	void startInjection();
	void stopInjection();
	void setStartLabelIdle();
	void setStartLabelDisabled();
	void setStartLabelRunning();

private slots:
	static void applicationExit();									// 程序退出
	void applicationMinimize();										// 程序最小化
	void applicationPin();											// 程序置顶
	void getGlobalSwitchKey(bool status);							// 获取全局开关键
	void getDIYKey();												// 获取自定义按键
	void obtainedKey(DWORD keyCode);								// 得到按键
	void startCoordinateCapture(bool status);						// 开始坐标捕获
	void getCursorCoordinate(int x, int y);							// 获取光标坐标
	void setEventInjectorIdle();										// 设置事件注入器空闲状态

signals:
	void startEventInjector(int inputKey,
	                        int inputActionMode,
	                        int cursorMoveMode,
	                        DWORD diyKey,
	                        int x,
	                        int y,
	                        double eventCycle);						// 启动事件注入器
	void stopEventInjector();										// 停止事件注入器
};

#endif // MAIN_WINDOW_H
