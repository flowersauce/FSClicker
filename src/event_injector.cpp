#include "event_injector.h"
#include <cmath>
#include <QElapsedTimer>
#include <QThread>
#include "global_definitions.h"

namespace
{
class ScopedThreadPriority
{
public:
    explicit ScopedThreadPriority(int priority) :
        thread(GetCurrentThread()),
        previousPriority(GetThreadPriority(thread))
    {
        SetThreadPriority(thread, priority);
    }

    ~ScopedThreadPriority()
    {
        if (previousPriority != THREAD_PRIORITY_ERROR_RETURN)
        {
            SetThreadPriority(thread, previousPriority);
        }
    }

    ScopedThreadPriority(const ScopedThreadPriority &) = delete;
    ScopedThreadPriority &operator=(const ScopedThreadPriority &) = delete;

private:
    HANDLE thread;
    int previousPriority;
};
}

EventInjector::EventInjector(QObject *parent) :
    QObject(parent),
    eventInjectorFlag(false),
    inputEvent{{0}}
{
}

bool EventInjector::isRunning() const
{
    return eventInjectorFlag.load(std::memory_order_relaxed);
}

void EventInjector::stop()
{
    eventInjectorFlag.store(false, std::memory_order_release);
}

void EventInjector::startTimer(int inputKey, int inputActionMode, int cursorMoveMode, DWORD diyKey, int x, int y, double eventCycle)
{
    if (eventInjectorFlag.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }

    emit started();

    const ScopedThreadPriority threadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
    int eventNum = inputActionMode == PRESS ? 1 : 2;

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int absoluteX = screenWidth > 0 ? (x * 65535) / screenWidth : 0;
    const int absoluteY = screenHeight > 0 ? (y * 65535) / screenHeight : 0;

    inputEvent[0] = {0};
    inputEvent[1] = {0};

    switch (inputKey)
    {
        case MOUSE_LEFT_KEY:
            inputEvent[0].type = INPUT_MOUSE;
            inputEvent[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            inputEvent[1].type = INPUT_MOUSE;
            inputEvent[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case MOUSE_MIDDLE_KEY:
            inputEvent[0].type = INPUT_MOUSE;
            inputEvent[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            inputEvent[1].type = INPUT_MOUSE;
            inputEvent[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        case MOUSE_RIGHT_KEY:
            inputEvent[0].type = INPUT_MOUSE;
            inputEvent[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            inputEvent[1].type = INPUT_MOUSE;
            inputEvent[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case DIY_KEY:
            inputEvent[0].type = INPUT_KEYBOARD;
            inputEvent[0].ki.wVk = static_cast<WORD>(diyKey);
            inputEvent[1].type = INPUT_KEYBOARD;
            inputEvent[1].ki.wVk = static_cast<WORD>(diyKey);
            inputEvent[1].ki.dwFlags = KEYEVENTF_KEYUP;
            break;
        default:
            eventInjectorFlag.store(false, std::memory_order_release);
            emit stopped();
            return;
    }

    if (cursorMoveMode == LOCK && inputKey != DIY_KEY)
    {
        inputEvent[0].mi.dwFlags |= MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        inputEvent[0].mi.dx = absoluteX;
        inputEvent[0].mi.dy = absoluteY;
    }

    qint64 executionTimes = 0;
    double cumulativeError = 0.0;
    double singleCycle = inputActionMode == PRESS ? 50.0 : eventCycle * 1000.0;

    QElapsedTimer errorTimer;
    errorTimer.start();

    while (eventInjectorFlag.load(std::memory_order_relaxed))
    {
        if (static_cast<double>(errorTimer.elapsed()) + cumulativeError < static_cast<double>(executionTimes) * singleCycle)
        {
            auto sleepCount = static_cast<unsigned long>(std::round(singleCycle));
            while (sleepCount > 0 && eventInjectorFlag.load(std::memory_order_relaxed))
            {
                QThread::msleep(1);
                sleepCount--;
            }
        }

        cumulativeError += static_cast<double>(errorTimer.elapsed()) - static_cast<double>(executionTimes) * singleCycle;

        if (!eventInjectorFlag.load(std::memory_order_relaxed))
        {
            break;
        }

        SendInput(eventNum, inputEvent, sizeof(INPUT));
        executionTimes++;
    }

    if (inputActionMode == PRESS)
    {
        SendInput(1, &inputEvent[1], sizeof(INPUT));
    }

    eventInjectorFlag.store(false, std::memory_order_release);
    emit stopped();
}
