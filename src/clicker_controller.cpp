/**
 * @file clicker_controller.cpp
 * @brief 连点器状态、配置和热键处理。
 */

#include "clicker_controller.h"
#include <QFile>
#include <algorithm>
#include <mmsystem.h>
#include "global_definitions.h"

ClickerController::ClickerController(QObject *parent)
	: QObject(parent)
	, eventInjector(new EventInjector(nullptr))
	, keyboardHook(this)
	, runningFlag(false)
	, canStartFlag(true)
	, capturingGlobalSwitchKeyFlag(false)
	, capturingDiyKeyFlag(false)
	, capturingCoordinateFlag(false)
	, statusTextValue(QStringLiteral("按下全局开关启动"))
	, globalSwitchKey(VK_F8)
	, diyKey(0x00)
	, inputKeyValue(MOUSE_LEFT_KEY)
	, inputActionModeValue(CLICKS)
	, cursorMoveModeValue(FREE)
	, cycleSecondsValue(0.01)
	, timingJitterPercentValue(0.0)
	, lockedCoordinate(0, 0)
{
	qRegisterMetaType<DWORD>("DWORD");

	QFile openSound(QStringLiteral(":/resources/open.wav"));
	if (openSound.open(QIODevice::ReadOnly))
	{
		openSoundData = openSound.readAll();
	}
	QFile closeSound(QStringLiteral(":/resources/close.wav"));
	if (closeSound.open(QIODevice::ReadOnly))
	{
		closeSoundData = closeSound.readAll();
	}

	eventInjectorThread.start();
	eventInjector->moveToThread(&eventInjectorThread);

	connect(this, &ClickerController::startEventInjector, eventInjector, &EventInjector::startTimer);
	connect(this, &ClickerController::stopEventInjector, eventInjector, &EventInjector::stop, Qt::DirectConnection);
	connect(eventInjector, &EventInjector::started, this, &ClickerController::setInjectorStarted);
	connect(eventInjector, &EventInjector::stopped, this, &ClickerController::setInjectorStopped);
	connect(&keyboardHook, &KeyboardHook::keyPressed, this, &ClickerController::handleKeyPressed, Qt::QueuedConnection);

	updateCanStart();
}

ClickerController::~ClickerController()
{
	PlaySound(nullptr, nullptr, 0);
	eventInjector->stop();
	eventInjectorThread.quit();
	eventInjectorThread.wait();
	delete eventInjector;
}

bool ClickerController::running() const
{
	return runningFlag;
}

bool ClickerController::canStart() const
{
	return canStartFlag;
}

bool ClickerController::hookInstalled() const
{
	return keyboardHook.isInstalled();
}

bool ClickerController::capturingGlobalSwitchKey() const
{
	return capturingGlobalSwitchKeyFlag;
}

bool ClickerController::capturingDiyKey() const
{
	return capturingDiyKeyFlag;
}

bool ClickerController::capturingCoordinate() const
{
	return capturingCoordinateFlag;
}

QString ClickerController::statusText() const
{
	return statusTextValue;
}

QString ClickerController::globalSwitchKeyName() const
{
	return keyName(globalSwitchKey);
}

QString ClickerController::diyKeyName() const
{
	return diyKey == 0x00 ? QStringLiteral("未设置") : keyName(diyKey);
}

QString ClickerController::coordinateText() const
{
	return QStringLiteral("%1,%2").arg(lockedCoordinate.x()).arg(lockedCoordinate.y());
}

int ClickerController::inputKey() const
{
	return inputKeyValue;
}

int ClickerController::inputActionMode() const
{
	return inputActionModeValue;
}

int ClickerController::cursorMoveMode() const
{
	return cursorMoveModeValue;
}

double ClickerController::cycleSeconds() const
{
	return cycleSecondsValue;
}

double ClickerController::timingJitterPercent() const
{
	return timingJitterPercentValue;
}

QJsonObject ClickerController::configJson() const
{
	return QJsonObject{
		{QStringLiteral("globalSwitchKey"), static_cast<int>(globalSwitchKey)},
		{QStringLiteral("diyKey"), static_cast<int>(diyKey)},
		{QStringLiteral("inputKey"), inputKeyValue},
		{QStringLiteral("inputActionMode"), inputActionModeValue},
		{QStringLiteral("cursorMoveMode"), cursorMoveModeValue},
		{QStringLiteral("cycleSeconds"), cycleSecondsValue},
		{QStringLiteral("timingJitterPercent"), timingJitterPercentValue},
		{QStringLiteral("lockedX"), lockedCoordinate.x()},
		{QStringLiteral("lockedY"), lockedCoordinate.y()},
	};
}

void ClickerController::applyConfigJson(const QJsonObject &config)
{
	const auto readKey = [](const QJsonObject &object, const QString &name, DWORD fallback) -> DWORD
	{
		const int	value = object.value(name).toInt(static_cast<int>(fallback));
		const DWORD key	  = static_cast<DWORD>(value);
		return keyMap.contains(key) ? key : fallback;
	};

	const auto readRange = [](const QJsonObject &object, const QString &name, int fallback, int min, int max) -> int
	{ return std::clamp(object.value(name).toInt(fallback), min, max); };

	const auto readDoubleRange = [](const QJsonObject &object, const QString &name, double fallback, double min, double max) -> double
	{ return std::clamp(object.value(name).toDouble(fallback), min, max); };

	globalSwitchKey			 = readKey(config, QStringLiteral("globalSwitchKey"), VK_F8);
	diyKey					 = readKey(config, QStringLiteral("diyKey"), 0x00);
	inputKeyValue			 = readRange(config, QStringLiteral("inputKey"), MOUSE_LEFT_KEY, MOUSE_LEFT_KEY, DIY_KEY);
	inputActionModeValue	 = readRange(config, QStringLiteral("inputActionMode"), CLICKS, CLICKS, PRESS);
	cursorMoveModeValue		 = readRange(config, QStringLiteral("cursorMoveMode"), FREE, FREE, LOCK);
	cycleSecondsValue		 = std::max(config.value(QStringLiteral("cycleSeconds")).toDouble(0.01), 0.001);
	timingJitterPercentValue = readDoubleRange(config, QStringLiteral("timingJitterPercent"), 0.0, 0.0, 20.0);
	lockedCoordinate		 = QPoint(config.value(QStringLiteral("lockedX")).toInt(0), config.value(QStringLiteral("lockedY")).toInt(0));

	if (globalSwitchKey == diyKey)
	{
		diyKey = 0x00;
		if (inputKeyValue == DIY_KEY)
		{
			inputKeyValue = MOUSE_LEFT_KEY;
		}
	}
	if ((diyKey == FS_WHEEL_UP || diyKey == FS_WHEEL_DOWN) && inputActionModeValue == PRESS)
	{
		inputActionModeValue = CLICKS;
	}

	emit globalSwitchKeyChanged();
	emit diyKeyChanged();
	emit inputKeyChanged();
	emit inputActionModeChanged();
	emit cursorMoveModeChanged();
	emit cycleSecondsChanged();
	emit timingJitterPercentChanged();
	emit coordinateChanged();
	updateCanStart();
}

void ClickerController::setInputKey(int value)
{
	const int normalized = std::clamp(value, static_cast<int>(MOUSE_LEFT_KEY), static_cast<int>(DIY_KEY));
	if (inputKeyValue == normalized)
	{
		return;
	}

	inputKeyValue = normalized;
	emit inputKeyChanged();
	updateCanStart();
}

void ClickerController::setInputActionMode(int value)
{
	const int normalized = std::clamp(value, static_cast<int>(CLICKS), static_cast<int>(PRESS));
	if (inputActionModeValue == normalized)
	{
		return;
	}

	inputActionModeValue = normalized;
	emit inputActionModeChanged();
	updateCanStart();
}

void ClickerController::setCursorMoveMode(int value)
{
	const int normalized = std::clamp(value, static_cast<int>(FREE), static_cast<int>(LOCK));
	if (cursorMoveModeValue == normalized)
	{
		return;
	}

	cursorMoveModeValue = normalized;
	emit cursorMoveModeChanged();
}

void ClickerController::setCycleSeconds(double value)
{
	const double normalized = std::max(value, 0.001);
	if (qFuzzyCompare(cycleSecondsValue, normalized))
	{
		return;
	}

	cycleSecondsValue = normalized;
	emit cycleSecondsChanged();
	updateCanStart();
}

void ClickerController::setTimingJitterPercent(double value)
{
	const double normalized = std::clamp(value, 0.0, 20.0);
	if (qFuzzyCompare(timingJitterPercentValue, normalized))
	{
		return;
	}

	timingJitterPercentValue = normalized;
	emit timingJitterPercentChanged();
}

void ClickerController::beginGlobalSwitchKeyCapture()
{
	if (runningFlag)
	{
		return;
	}

	capturingGlobalSwitchKeyFlag = true;
	capturingDiyKeyFlag			 = false;
	capturingCoordinateFlag		 = false;
	setStatusText(QStringLiteral("按下新的全局开关"));
	emit captureStateChanged();
	updateCanStart();
}

void ClickerController::beginDiyKeyCapture()
{
	if (runningFlag)
	{
		return;
	}

	capturingDiyKeyFlag			 = true;
	capturingGlobalSwitchKeyFlag = false;
	capturingCoordinateFlag		 = false;
	inputKeyValue				 = DIY_KEY;
	emit inputKeyChanged();
	setStatusText(QStringLiteral("按下要连点的键"));
	emit captureStateChanged();
	updateCanStart();
}

void ClickerController::beginCoordinateCapture()
{
	if (runningFlag)
	{
		return;
	}

	capturingCoordinateFlag		 = true;
	capturingGlobalSwitchKeyFlag = false;
	capturingDiyKeyFlag			 = false;
	cursorMoveModeValue			 = LOCK;
	emit cursorMoveModeChanged();
	emit captureStateChanged();
	setStatusText(QStringLiteral("点击屏幕捕获坐标"));
	updateCanStart();
}

void ClickerController::cancelCoordinateCapture()
{
	if (!capturingCoordinateFlag)
	{
		return;
	}

	capturingCoordinateFlag = false;
	emit captureStateChanged();
	updateCanStart();
}

void ClickerController::finishCoordinateCapture(int x, int y)
{
	if (!capturingCoordinateFlag)
	{
		return;
	}

	capturingCoordinateFlag = false;
	setLockedCoordinate(x, y);
	emit captureStateChanged();
	updateCanStart();
}

void ClickerController::setLockedCoordinate(int x, int y)
{
	const QPoint coordinate(x, y);
	if (lockedCoordinate == coordinate)
	{
		return;
	}

	lockedCoordinate = coordinate;
	emit coordinateChanged();
}

void ClickerController::toggleRunning()
{
	if (runningFlag)
	{
		stop();
		return;
	}

	if (canStartFlag)
	{
		start();
	}
}

void ClickerController::stop()
{
	if (!runningFlag)
	{
		return;
	}

	playSound(closeSoundData);
	emit stopEventInjector();
}

void ClickerController::handleKeyPressed(DWORD keyCode)
{
	const auto theKeyValue = keyMap.find(keyCode);
	if (capturingGlobalSwitchKeyFlag)
	{
		if (theKeyValue != keyMap.end() && keyCode != diyKey)
		{
			globalSwitchKey				 = keyCode;
			capturingGlobalSwitchKeyFlag = false;
			emit globalSwitchKeyChanged();
			emit captureStateChanged();
			setStatusText(QStringLiteral("按下全局开关启动"));
			updateCanStart();
		}
		else
		{
			setStatusText(QStringLiteral("按键不可用，请重试"));
		}
		return;
	}

	if (capturingDiyKeyFlag)
	{
		const bool mouseButtonPreset = keyCode == VK_LBUTTON || keyCode == VK_MBUTTON || keyCode == VK_RBUTTON;
		if (theKeyValue != keyMap.end() && keyCode != globalSwitchKey && !mouseButtonPreset)
		{
			diyKey				= keyCode;
			capturingDiyKeyFlag = false;
			if ((diyKey == FS_WHEEL_UP || diyKey == FS_WHEEL_DOWN) && inputActionModeValue == PRESS)
			{
				inputActionModeValue = CLICKS;
				emit inputActionModeChanged();
			}
			emit diyKeyChanged();
			emit captureStateChanged();
			setStatusText(QStringLiteral("按下全局开关启动"));
			updateCanStart();
		}
		else
		{
			setStatusText(QStringLiteral("按键不可用，请重试"));
		}
		return;
	}

	if (keyCode == globalSwitchKey && canStartFlag)
	{
		toggleRunning();
	}
}

void ClickerController::setInjectorStarted()
{
	if (runningFlag)
	{
		return;
	}

	runningFlag = true;
	emit runningChanged();
	setStatusText(QStringLiteral("按下全局开关终止"));
}

void ClickerController::setInjectorStopped()
{
	if (!runningFlag)
	{
		return;
	}

	runningFlag = false;
	emit runningChanged();
	setStatusText(QStringLiteral("按下全局开关启动"));
}

void ClickerController::start()
{
	runningFlag = true;
	emit runningChanged();
	setStatusText(QStringLiteral("按下全局开关终止"));

	playSound(openSoundData);

	emit startEventInjector(inputKeyValue,
							inputActionModeValue,
							cursorMoveModeValue,
							diyKey,
							lockedCoordinate.x(),
							lockedCoordinate.y(),
							cycleSecondsValue,
							timingJitterPercentValue);
}

void ClickerController::updateCanStart()
{
	const bool previousValue = canStartFlag;
	canStartFlag			 = !capturingGlobalSwitchKeyFlag && !capturingDiyKeyFlag && !capturingCoordinateFlag && cycleSecondsValue > 0.0
		&& (inputKeyValue != DIY_KEY || diyKey != 0x00);

	if (canStartFlag != previousValue)
	{
		emit canStartChanged();
	}

	if (!runningFlag && !capturingGlobalSwitchKeyFlag && !capturingDiyKeyFlag && !capturingCoordinateFlag)
	{
		setStatusText(canStartFlag ? QStringLiteral("按下全局开关启动") : QStringLiteral("等待配置完成"));
	}
}

void ClickerController::setStatusText(const QString &text)
{
	if (statusTextValue == text)
	{
		return;
	}

	statusTextValue = text;
	emit statusTextChanged();
}

QString ClickerController::keyName(DWORD keyCode)
{
	const auto theKeyValue = keyMap.find(keyCode);
	if (theKeyValue == keyMap.end())
	{
		return QStringLiteral("未知");
	}

	return QString::fromStdString(theKeyValue->second);
}

void ClickerController::playSound(const QByteArray &soundData) const
{
	if (soundData.isEmpty())
	{
		return;
	}

	PlaySoundA(soundData.constData(), nullptr, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
}
