/**
 * @file event_injector.cpp
 * @brief 基于 SendInput 的输入事件调度。
 */

#include "event_injector.h"
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QThread>
#include <algorithm>
#include "global_definitions.h"

namespace
{
	class ScopedThreadPriority
	{
	public:
		explicit ScopedThreadPriority(int priority)
			: thread(GetCurrentThread())
			, previousPriority(GetThreadPriority(thread))
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

		ScopedThreadPriority(const ScopedThreadPriority &)			  = delete;
		ScopedThreadPriority &operator=(const ScopedThreadPriority &) = delete;

	private:
		HANDLE thread;
		int	   previousPriority;
	};

	/**
	 * @brief 等待到目标时间，同时保持停止响应。
	 */
	void waitUntil(QElapsedTimer &timer, qint64 deadlineNs, const std::atomic<bool> &runningFlag)
	{
		constexpr qint64 sleepThresholdNs = 10'000'000;

		while (runningFlag.load(std::memory_order_relaxed))
		{
			const qint64 remainingNs = deadlineNs - timer.nsecsElapsed();
			if (remainingNs <= 0)
			{
				return;
			}

			if (remainingNs > sleepThresholdNs)
			{
				QThread::msleep(1);
				continue;
			}

			YieldProcessor();
		}
	}

	/**
	 * @brief 根据配置生成单次间隔抖动。
	 */
	qint64 timingJitterNs(qint64 cycleNs, double timingJitterPercent)
	{
		const double jitterRatio = std::clamp(timingJitterPercent, 0.0, 20.0) / 100.0;
		if (jitterRatio <= 0.0)
		{
			return 0;
		}

		const auto	 jitterRangeNs = static_cast<qint64>(static_cast<double>(cycleNs) * jitterRatio);
		const double randomFactor  = QRandomGenerator::global()->generateDouble() * 2.0 - 1.0;
		return static_cast<qint64>(randomFactor * static_cast<double>(jitterRangeNs));
	}

	/**
	 * @brief 将虚拟屏幕坐标转换为 SendInput 的绝对坐标。
	 */
	int absoluteCoordinate(int value, int origin, int size)
	{
		if (size <= 1)
		{
			return 0;
		}

		return std::clamp(MulDiv(value - origin, 65535, size - 1), 0, 65535);
	}
} // namespace

EventInjector::EventInjector(QObject *parent)
	: QObject(parent)
	, eventInjectorFlag(false)
	, inputEvent{{0}}
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

void EventInjector::startTimer(int inputKey, int inputActionMode, int cursorMoveMode, DWORD diyKey, int x, int y, double eventCycle, double timingJitterPercent)
{
	if (eventInjectorFlag.exchange(true, std::memory_order_acq_rel))
	{
		return;
	}

	emit started();

	const ScopedThreadPriority threadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	int						   eventNum = inputActionMode == PRESS ? 1 : 2;

	const int virtualScreenX	  = GetSystemMetrics(SM_XVIRTUALSCREEN);
	const int virtualScreenY	  = GetSystemMetrics(SM_YVIRTUALSCREEN);
	const int virtualScreenWidth  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	const int virtualScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	const int absoluteX			  = absoluteCoordinate(x, virtualScreenX, virtualScreenWidth);
	const int absoluteY			  = absoluteCoordinate(y, virtualScreenY, virtualScreenHeight);

	inputEvent[0] = {0};
	inputEvent[1] = {0};

	switch (inputKey)
	{
		case MOUSE_LEFT_KEY:
			inputEvent[0].type		 = INPUT_MOUSE;
			inputEvent[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			inputEvent[1].type		 = INPUT_MOUSE;
			inputEvent[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
			break;
		case MOUSE_MIDDLE_KEY:
			inputEvent[0].type		 = INPUT_MOUSE;
			inputEvent[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			inputEvent[1].type		 = INPUT_MOUSE;
			inputEvent[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			break;
		case MOUSE_RIGHT_KEY:
			inputEvent[0].type		 = INPUT_MOUSE;
			inputEvent[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			inputEvent[1].type		 = INPUT_MOUSE;
			inputEvent[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			break;
		case DIY_KEY:
			if (diyKey == FS_WHEEL_UP || diyKey == FS_WHEEL_DOWN)
			{
				eventNum				   = 1;
				inputEvent[0].type		   = INPUT_MOUSE;
				inputEvent[0].mi.dwFlags   = MOUSEEVENTF_WHEEL;
				inputEvent[0].mi.mouseData = diyKey == FS_WHEEL_UP ? WHEEL_DELTA : -WHEEL_DELTA;
			}
			else
			{
				inputEvent[0].type		 = INPUT_KEYBOARD;
				inputEvent[0].ki.wVk	 = static_cast<WORD>(diyKey);
				inputEvent[1].type		 = INPUT_KEYBOARD;
				inputEvent[1].ki.wVk	 = static_cast<WORD>(diyKey);
				inputEvent[1].ki.dwFlags = KEYEVENTF_KEYUP;
			}
			break;
		default:
			eventInjectorFlag.store(false, std::memory_order_release);
			emit stopped();
			return;
	}

	if (cursorMoveMode == LOCK && inputKey != DIY_KEY)
	{
		inputEvent[0].mi.dwFlags |= MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
		inputEvent[0].mi.dx = absoluteX;
		inputEvent[0].mi.dy = absoluteY;
	}

	if (inputActionMode == PRESS && !(inputKey == DIY_KEY && (diyKey == FS_WHEEL_UP || diyKey == FS_WHEEL_DOWN)))
	{
		SendInput(eventNum, inputEvent, sizeof(INPUT));
		while (eventInjectorFlag.load(std::memory_order_relaxed))
		{
			QThread::msleep(1);
		}

		SendInput(1, &inputEvent[1], sizeof(INPUT));
		eventInjectorFlag.store(false, std::memory_order_release);
		emit stopped();
		return;
	}

	const qint64  cycleNs = std::max<qint64>(1, static_cast<qint64>(eventCycle * 1'000'000'000.0));
	QElapsedTimer timer;
	timer.start();
	qint64 deadlineNs = 0;

	while (eventInjectorFlag.load(std::memory_order_relaxed))
	{
		waitUntil(timer, deadlineNs, eventInjectorFlag);

		if (!eventInjectorFlag.load(std::memory_order_relaxed))
		{
			break;
		}

		SendInput(eventNum, inputEvent, sizeof(INPUT));

		const qint64 nowNs = timer.nsecsElapsed();
		if (deadlineNs < nowNs - cycleNs)
		{
			deadlineNs = nowNs;
		}
		deadlineNs += std::max<qint64>(1, cycleNs + timingJitterNs(cycleNs, timingJitterPercent));
	}

	eventInjectorFlag.store(false, std::memory_order_release);
	emit stopped();
}
