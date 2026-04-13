#include "main_window.h"
#include "ui_main_window.h"
#include <QApplication>
#include <QDoubleValidator>
#include <QFontDatabase>
#include <QMetaType>

// =============================================================================================== Class
MainWindow::MainWindow(QWidget *parent) :
		CustomWidget(parent),
		primaryWidgetColor("#21252b"),
		secondaryWidgetColor("#282c34"),
		ui(new Ui::MainWindow),
		soundEffectPlayer(new QMediaPlayer(this)),
		soundEffectAudioOutput(new QAudioOutput(this)),
		eventInjectorThread(new QThread(this)),
		eventInjector(new EventInjector(nullptr)),
		keyboardHook(new KeyboardHook(this)),
		pageButtonGroup(new QButtonGroup(this)),
		inputKeyButtonGroup(new QButtonGroup(this)),
		inputActionButtonGroup(new QButtonGroup(this)),
		cursorMoveButtonGroup(new QButtonGroup(this)),
		mainWindowPin(false),
		mousePressed(false),
		eventInjectorRunning(false),
		getGlobalSwitchKeyHook_flag(false),
		getDIYKeyHook_flag(false),
		startEventInjectorAllowed_flag(true),
		pageNum(IOCONFIGPAGE),
		inputKey(MOUSELEFTKEY),
		inputActionMode(CLICKS),
		cursorMoveMode(FREE),
		globalSwitchKey(VK_F8),
		diyKey(0x00),
		coordinateXY({0, 0})
{
	ui->setupUi(this);

	initializeEventInjector();
	initializeAudio();
	initializeKeyboardHook();
	initializeButtonGroups();
	initializeAppearance();
	initializeInputValidator();
	initializeFont();
	initializeAboutPage();
	connectSignals();
	initializePageState();
}

MainWindow::~MainWindow()
{
	eventInjector->stop();

	eventInjectorThread->quit();
	eventInjectorThread->wait();
	delete eventInjector;
	eventInjector = nullptr;
	eventInjectorThread->deleteLater();
	eventInjectorThread = nullptr;

	delete ui;
}

void MainWindow::initializeEventInjector()
{
	eventInjectorThread->start();
	eventInjector->moveToThread(eventInjectorThread);
}

void MainWindow::initializeKeyboardHook()
{
	qRegisterMetaType<DWORD>("DWORD");
	connect(keyboardHook, &KeyboardHook::keyPressed, this, &MainWindow::obtainedKey, Qt::QueuedConnection);
}

void MainWindow::initializeAudio()
{
	soundEffectPlayer->setAudioOutput(soundEffectAudioOutput);
	soundEffectAudioOutput->setVolume(1.0);
}

void MainWindow::initializeButtonGroups()
{
	pageButtonGroup->addButton(ui->IOConfig_widget_button);
	pageButtonGroup->addButton(ui->about_widget_button);

	inputKeyButtonGroup->addButton(ui->mouseLeftButton);
	inputKeyButtonGroup->addButton(ui->mouseMiddleButton);
	inputKeyButtonGroup->addButton(ui->mouseRightButton);
	inputKeyButtonGroup->addButton(ui->DIYKeyButton);

	inputActionButtonGroup->addButton(ui->clicksButton);
	inputActionButtonGroup->addButton(ui->pressButton);

	cursorMoveButtonGroup->addButton(ui->cursorFreeButton);
	cursorMoveButtonGroup->addButton(ui->cursorLockButton);
}

void MainWindow::initializeAppearance()
{
	this->setWidgetCornerRadius(28);
	this->setWidgetColor(QColor(primaryWidgetColor));
	this->setWidgetBorderLightness(200);

	ui->titleBar->setWidgetCornerRadius(16);
	ui->titleBar->setWidgetColor(QColor(secondaryWidgetColor));
	ui->titleBar->setWidgetBorderLightness(100);

	ui->function_widget->setWidgetCornerRadius(16);
	ui->function_widget->setWidgetColor(QColor(secondaryWidgetColor));
	ui->function_widget->setWidgetBorderLightness(100);

	auto setupConfigBar = [=](CustomWidget *widget)
	{
		widget->setWidgetCornerRadius(8);
		widget->setWidgetColor(QColor(secondaryWidgetColor).lighter(180));
		widget->setWidgetBorderLightness(100);
	};

	setupConfigBar(ui->IOConfig_bar_1);
	setupConfigBar(ui->IOConfig_bar_2);
	setupConfigBar(ui->IOConfig_bar_3);
	setupConfigBar(ui->IOConfig_bar_4);
	setupConfigBar(ui->IOConfig_bar_5);
	setupConfigBar(ui->IOConfig_bar_6);
}

void MainWindow::initializeInputValidator()
{
	auto *doubleValidator = new QDoubleValidator(0.000, 99999.999, 3, ui->PeriodValueInputLineEdit);
	doubleValidator->setNotation(QDoubleValidator::StandardNotation);
	ui->PeriodValueInputLineEdit->setValidator(doubleValidator);
}

void MainWindow::initializeFont()
{
	const int fontId = QFontDatabase::addApplicationFont(":/resources/CascadiaMono.ttf");
	const auto fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
	if (fontFamilies.isEmpty())
	{
		return;
	}

	QFont font(fontFamilies.at(0));
	font.setPointSize(10);
	ui->titleLabel->setFont(font);
	ui->appName_label_A->setFont(font);
	ui->appName_label_B->setFont(font);
}

void MainWindow::initializeAboutPage()
{
	ui->titleLabel->setText(FS_CLICKER_APP_NAME);
	ui->appName_label_A->setText("FS");
	ui->appName_label_B->setText("Clicker");
	ui->about_label_1->setText(QString("版本信息 : %1").arg(FS_CLICKER_APP_VERSION));
	ui->about_label_4->setText(
			"<a style='color: #ffada9; text-decoration: none;' href=\"https://github.com/flowersauce/FlowersauceClicker\">点击跳转到仓库</a>");
	ui->about_label_4->setOpenExternalLinks(true);
}

void MainWindow::initializePageState()
{
	ui->about_widget->setVisible(false);
	ui->IOConfig_bar_2->setEnabled(false);
}

void MainWindow::connectSignals()
{
	connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::applicationExit);
	connect(ui->minimizeButton, &QPushButton::clicked, this, &MainWindow::applicationMinimize);
	connect(ui->pinButton, &QPushButton::clicked, this, &MainWindow::applicationPin);

	connect(ui->IOConfig_widget_button, &QPushButton::clicked, this, [=]()
	{
		pageNum = IOCONFIGPAGE;
		ui->IOConfig_widget->setVisible(true);
		ui->about_widget->setVisible(false);
	});
	connect(ui->about_widget_button, &QPushButton::clicked, this, [=]()
	{
		pageNum = ABOUTPAGE;
		ui->about_widget->setVisible(true);
		ui->IOConfig_widget->setVisible(false);
	});

	connect(ui->globalSwitchCaptureButton, &QPushButton::toggled, this, &MainWindow::getGlobalSwitchKey);
	connect(ui->cursorCoordinateCaptureButton, &QPushButton::toggled, this, &MainWindow::startCoordinateCapture);

	connect(ui->mouseLeftButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (status)
		{
			inputKey = MOUSELEFTKEY;
		}
	});
	connect(ui->mouseMiddleButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (status)
		{
			inputKey = MOUSEMIDDLEKEY;
		}
	});
	connect(ui->mouseRightButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (status)
		{
			inputKey = MOUSERIGHTKEY;
		}
	});
	connect(ui->DIYKeyButton, &QPushButton::clicked, this, &MainWindow::getDIYKey);
	connect(ui->DIYKeyButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (!status)
		{
			ui->IOConfig_bar_2->setEnabled(true);
			ui->IOConfig_bar_5->setEnabled(true);
		}
	});

	connect(ui->clicksButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (status)
		{
			inputActionMode = CLICKS;
		}
	});
	connect(ui->pressButton, &QPushButton::toggled, this, [=](bool status)
	{
		inputActionMode = status ? PRESS : inputActionMode;
		ui->IOConfig_bar_6->setEnabled(!status);
	});

	connect(ui->cursorFreeButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (status)
		{
			cursorMoveMode = FREE;
			ui->IOConfig_bar_2->setEnabled(false);
		}
	});
	connect(ui->cursorLockButton, &QPushButton::toggled, this, [=](bool status)
	{
		if (status)
		{
			cursorMoveMode = LOCK;
			ui->IOConfig_bar_2->setEnabled(true);
		}
	});

	connect(ui->PeriodValueInputLineEdit, &QLineEdit::textChanged, this, [=](const QString &text)
	{
		startEventInjectorAllowed_flag = !text.isEmpty() && text.toDouble() != 0;
		if (startEventInjectorAllowed_flag)
		{
			setStartLabelIdle();
		}
		else
		{
			setStartLabelDisabled();
		}
	});

	connect(this, &MainWindow::startEventInjector, eventInjector, &EventInjector::startTimer);
	connect(this, &MainWindow::stopEventInjector, eventInjector, &EventInjector::stop, Qt::DirectConnection);
	connect(eventInjector, &EventInjector::started, this, [=]()
	{
		eventInjectorRunning = true;
	});
	connect(eventInjector, &EventInjector::stopped, this, &MainWindow::setEventInjectorIdle);
}

void MainWindow::startInjection()
{
	eventInjectorRunning = true;
	soundEffectPlayer->setSource(QUrl("qrc:/resources/open.wav"));
	soundEffectPlayer->play();

	ui->IOConfigBars_widget->setEnabled(false);
	setStartLabelRunning();

	emit startEventInjector(inputKey,
	                        inputActionMode,
	                        cursorMoveMode,
	                        diyKey,
	                        coordinateXY.at(0),
	                        coordinateXY.at(1),
	                        ui->PeriodValueInputLineEdit->text().toDouble());
}

void MainWindow::stopInjection()
{
	soundEffectPlayer->setSource(QUrl("qrc:/resources/close.wav"));
	soundEffectPlayer->play();
	emit stopEventInjector();
}

void MainWindow::setStartLabelIdle()
{
	ui->startLabel->setStyleSheet("border: none; border-radius: 8px; background-color: #3686f2; color: white;");
	ui->startLabel->setText("按下 -全局开关- 启动");
}

void MainWindow::setStartLabelDisabled()
{
	ui->startLabel->setStyleSheet("border: none; border-radius: 8px; background-color: #1b397e; color: #7e7e7e;");
}

void MainWindow::setStartLabelRunning()
{
	ui->startLabel->setStyleSheet("border: none; border-radius: 8px; background-color: #ff5f56; color: white;");
	ui->startLabel->setText("按下 -全局开关- 终止");
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		// 检查鼠标下的控件是否是MainWindow本身
		if (ui->titleBar->rect().contains(event->pos()))
		{
			mousePressed = true;
			mouseStartPoint = event->globalPosition().toPoint();
			windowStartPoint = this->frameGeometry().topLeft();
		}
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (mousePressed)
	{
		QPoint movePoint = event->globalPosition().toPoint() - mouseStartPoint;
		this->move(windowStartPoint + movePoint);
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		mousePressed = false;
	}
}

void MainWindow::applicationExit()
{
	// 关闭程序
	QApplication::quit();
}

void MainWindow::applicationMinimize()
{
	// 最小化窗口
	this->showMinimized();
}

void MainWindow::applicationPin()
{
	if (!mainWindowPin)
	{
		this->setWidgetBorderLightness(500);
		setWindowFlag(Qt::WindowStaysOnTopHint, true);
		show();
		mainWindowPin = true;
	}
	else
	{
		this->setWidgetBorderLightness(200);
		setWindowFlag(Qt::WindowStaysOnTopHint, false);
		show();
		mainWindowPin = false;
	}
}

void MainWindow::getGlobalSwitchKey(bool status)
{
	if (status)
	{
		setStartLabelDisabled();

		ui->IOConfig_bar_1->setEnabled(false);
		ui->IOConfig_bar_3->setEnabled(false);
		ui->globalSwitchCaptureButton->clearFocus();
		ui->globalSwitchCaptureButton->setText("捕获中");

		getGlobalSwitchKeyHook_flag = true;
	}
	else
	{
		setStartLabelIdle();

		ui->IOConfig_bar_1->setEnabled(true);
		ui->IOConfig_bar_3->setEnabled(true);
	}
}

void MainWindow::getDIYKey()
{
	ui->DIYKeyButton->clearFocus();
	ui->IOConfig_bar_1->setEnabled(false);
	ui->IOConfig_bar_2->setEnabled(false);
	ui->IOConfig_bar_3->setEnabled(false);
	ui->IOConfig_bar_5->setEnabled(false);
	setStartLabelDisabled();

	inputKey = DIYKEY;
	ui->DIYKeyButton->setText("捕获中");

	getDIYKeyHook_flag = true;
}

void MainWindow::obtainedKey(DWORD keyCode)
{
	auto theKeyValue = keyMap.find(keyCode);
	if (getGlobalSwitchKeyHook_flag)
	{
		if (theKeyValue != keyMap.end() && keyCode != diyKey)
		{
			ui->globalSwitchCaptureButton->setText(QString::fromStdString(theKeyValue->second));
			ui->globalSwitchCaptureButton->setChecked(false);
			globalSwitchKey = keyCode;
			getGlobalSwitchKeyHook_flag = false;
		}
		else
		{
			ui->globalSwitchCaptureButton->setText("请重试");
		}
	}
	else if (getDIYKeyHook_flag)
	{
		if (theKeyValue != keyMap.end() && keyCode != globalSwitchKey)
		{
			ui->DIYKeyButton->setText(QString::fromStdString(theKeyValue->second));
			diyKey = keyCode;
			getDIYKeyHook_flag = false;
			setStartLabelIdle();

			ui->IOConfig_bar_1->setEnabled(true);
			ui->IOConfig_bar_3->setEnabled(true);
		}
		else
		{
			ui->DIYKeyButton->setText("请重试");
		}
	}
	else if (startEventInjectorAllowed_flag)
	{
		if (keyCode == globalSwitchKey)
		{
			if (!eventInjectorRunning)
			{
				startInjection();
			}
			else
			{
				stopInjection();
			}
		}
	}
}

void MainWindow::setEventInjectorIdle()
{
	eventInjectorRunning = false;
	ui->IOConfigBars_widget->setEnabled(true);
	setStartLabelIdle();
}

void MainWindow::startCoordinateCapture(bool status)
{
	if (status)
	{
		// 禁止启动事件注入器
		startEventInjectorAllowed_flag = false;
		setStartLabelDisabled();
		// 控件设置
		ui->cursorCoordinateCaptureButton->clearFocus();
		ui->cursorCoordinateCaptureButton->setText("捕获中");
		// 动态创建窗口
		auto *captureWindow = new CoordinateCaptureWindow(this);
		// 自动删除窗口对象
		captureWindow->setAttribute(Qt::WA_DeleteOnClose);
		connect(captureWindow, &CoordinateCaptureWindow::coordinatesCaptured, this, &MainWindow::getCursorCoordinate);
		captureWindow->show();
	}
}

void MainWindow::getCursorCoordinate(int x, int y)
{
	coordinateXY.at(0) = x;
	coordinateXY.at(1) = y;
	auto coordinateXYStr = QString::number(x) + "," + QString::number(y);
	ui->cursorCoordinateCaptureButton->setText(coordinateXYStr);
	ui->cursorCoordinateCaptureButton->setChecked(false);
	// 允许启动事件注入器
	startEventInjectorAllowed_flag = true;
	setStartLabelIdle();
}
