#ifndef EVENT_INJECTOR_H
#define EVENT_INJECTOR_H

#include <atomic>
#include <QObject>
#include <QThread>
#include <windows.h>
#include "global_definitions.h"

class EventInjector : public QObject
{
Q_OBJECT

public:
	explicit EventInjector(QObject *parent = nullptr);
	~EventInjector() override = default;
	bool isRunning() const;

private:
	std::atomic<bool> eventInjector_flag;
	INPUT inputEvent[2];

public slots:
	void startTimer(int inputKey,
	                int inputActionMode,
	                int cursorMoveMode,
	                DWORD diyKey,
	                int x,
	                int y,
	                double eventCycle
	);
	void stop();

signals:
	void started();
	void stopped();
};

#endif //EVENT_INJECTOR_H
