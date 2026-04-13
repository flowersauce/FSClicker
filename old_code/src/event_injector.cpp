#include "event_injector.h"
#include <QElapsedTimer>

EventInjector::EventInjector(QObject *parent) : QObject(parent), eventInjector_flag(false), inputEvent{{0}}
{
}

bool EventInjector::isRunning() const
{
    return eventInjector_flag.load();
}

void EventInjector::stop()
{
    eventInjector_flag = false;
}

void EventInjector::startTimer(int inputKey, int inputActionMode, int cursorMoveMode, DWORD diyKey, int x, int y, double eventCycle)
{
    if (eventInjector_flag.exchange(true))
    {
        return;
    }

    emit started();

    // 根据参数配置INPUT结构体
    int eventNum = 2;
    if (inputActionMode == PRESS)
    {
        eventNum = 1;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    x = (x * 65535) / screenWidth;
    y = (y * 65535) / screenHeight;

    inputEvent[0] = {0};
    inputEvent[1] = {0};
    switch (inputKey)
    {
        case MOUSELEFTKEY:
        {
            inputEvent[0].type = INPUT_MOUSE;
            inputEvent[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            inputEvent[1].type = INPUT_MOUSE;
            inputEvent[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
            if (cursorMoveMode == LOCK)
            {
                inputEvent[0].mi.dwFlags |= MOUSEEVENTF_MOVE;
                inputEvent[0].mi.dwFlags |= MOUSEEVENTF_ABSOLUTE;
                inputEvent[0].mi.dx = x;
                inputEvent[0].mi.dy = y;
            }
            break;
        }
        case MOUSEMIDDLEKEY:
        {
            inputEvent[0].type = INPUT_MOUSE;
            inputEvent[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            inputEvent[1].type = INPUT_MOUSE;
            inputEvent[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            if (cursorMoveMode == LOCK)
            {
                inputEvent[0].mi.dwFlags |= MOUSEEVENTF_MOVE;
                inputEvent[0].mi.dwFlags |= MOUSEEVENTF_ABSOLUTE;
                inputEvent[0].mi.dx = x;
                inputEvent[0].mi.dy = y;
            }
            break;
        }
        case MOUSERIGHTKEY:
        {
            inputEvent[0].type = INPUT_MOUSE;
            inputEvent[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            inputEvent[1].type = INPUT_MOUSE;
            inputEvent[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            if (cursorMoveMode == LOCK)
            {
                inputEvent[0].mi.dwFlags |= MOUSEEVENTF_MOVE;
                inputEvent[0].mi.dwFlags |= MOUSEEVENTF_ABSOLUTE;
                inputEvent[0].mi.dx = x;
                inputEvent[0].mi.dy = y;
            }
            break;
        }
        case DIYKEY:
        {
            inputEvent[0].type = INPUT_KEYBOARD;
            inputEvent[0].ki.wVk = diyKey;
            inputEvent[0].ki.dwFlags = 0;

            inputEvent[1].type = INPUT_KEYBOARD;
            inputEvent[1].ki.wVk = diyKey;
            inputEvent[1].ki.dwFlags = KEYEVENTF_KEYUP;
            break;
        }
        default:
            break;
    }

    qint64 executionTimes = 0;                // 用于记录整个循环生命周期内的执行次数
    double cumulativeError = 0.0;             // 整个循环生命周期内的累积误差
    double singleCycle = eventCycle * 1000.0; // 单次循环周期 (ms)
    // 长按模式下设置固定的单次循环周期
    if (inputActionMode == PRESS)
    {
        singleCycle = 50;
    }

    QElapsedTimer errorTimer;
    errorTimer.start();

    while (eventInjector_flag)
    {
        // 判断是否需要跳过延时以降低累计误差
        if (static_cast<double>(errorTimer.elapsed()) + cumulativeError < static_cast<double>(executionTimes) * singleCycle)
        {
            auto sleepCount = static_cast<unsigned long>(std::round(singleCycle));
            while (sleepCount > 0 && eventInjector_flag)
            {
                QThread::msleep(1);
                sleepCount--;
            }
        }
        // 误差累积
        cumulativeError += static_cast<double>(errorTimer.elapsed()) - static_cast<double>(executionTimes) * singleCycle;
        // 动作执行
        if (!eventInjector_flag)
        {
            break;
        }
        SendInput(eventNum, inputEvent, sizeof(INPUT));
        // 执行次数累加
        executionTimes++;
    }

    // 长按模式按键状态复原
	if (inputActionMode == PRESS)
	{
		SendInput(eventNum, &inputEvent[1], sizeof(INPUT));
	}

    eventInjector_flag = false;
    emit stopped();
}
