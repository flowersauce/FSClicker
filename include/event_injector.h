#ifndef EVENT_INJECTOR_H
#define EVENT_INJECTOR_H

#include <atomic>
#include <windows.h>
#include <QObject>

class EventInjector : public QObject
{
    Q_OBJECT

public:
    explicit EventInjector(QObject *parent = nullptr);
    ~EventInjector() override = default;

    bool isRunning() const;

public slots:
    void startTimer(int inputKey,
                    int inputActionMode,
                    int cursorMoveMode,
                    DWORD diyKey,
                    int x,
                    int y,
                    double eventCycle);
    void stop();

signals:
    void started();
    void stopped();

private:
    std::atomic<bool> eventInjectorFlag;
    INPUT inputEvent[2];
};

#endif // EVENT_INJECTOR_H
