/**
 * @file event_injector.h
 * @brief 输入事件注入线程对象。
 */

#ifndef EVENT_INJECTOR_H
#define EVENT_INJECTOR_H

#include <QObject>
#include <atomic>
#include <windows.h>

/**
 * @brief 按当前配置发送鼠标或键盘输入。
 */
class EventInjector : public QObject
{
	Q_OBJECT

public:
	explicit EventInjector(QObject *parent = nullptr);
	~EventInjector() override = default;

	bool isRunning() const;

public slots:
	void startTimer(int inputKey, int inputActionMode, int cursorMoveMode, DWORD diyKey, int x, int y, double eventCycle, double timingJitterPercent);
	void stop();

signals:
	void started();
	void stopped();

private:
	std::atomic<bool> eventInjectorFlag;
	INPUT			  inputEvent[2];
};

#endif // EVENT_INJECTOR_H
