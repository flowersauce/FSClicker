/**
 * @file main.cpp
 * @brief 程序入口和 QML 上下文初始化。
 */

#include <QFontDatabase>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringList>
#include <QTimer>
#include "app_config.h"
#include "clicker_controller.h"

namespace
{
	enum class VelopackHook
	{
		None,
		Install,
		Obsolete,
		Updated,
		Uninstall,
	};

	VelopackHook velopackHookFromArgument(const char *argument)
	{
		const QString value = QString::fromLocal8Bit(argument);
		if (value == QStringLiteral("--veloapp-install"))
		{
			return VelopackHook::Install;
		}
		if (value == QStringLiteral("--veloapp-obsolete"))
		{
			return VelopackHook::Obsolete;
		}
		if (value == QStringLiteral("--veloapp-updated"))
		{
			return VelopackHook::Updated;
		}
		if (value == QStringLiteral("--veloapp-uninstall"))
		{
			return VelopackHook::Uninstall;
		}

		return VelopackHook::None;
	}

	VelopackHook currentVelopackHook(int argc, char *argv[])
	{
		for (int index = 1; index < argc; ++index)
		{
			const VelopackHook hook = velopackHookFromArgument(argv[index]);
			if (hook != VelopackHook::None)
			{
				return hook;
			}
		}

		return VelopackHook::None;
	}

	void disableDiskCaches()
	{
		QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);
		qputenv("QML_DISABLE_DISK_CACHE", "1");
	}

	void loadApplicationFont(const QString &resourcePath)
	{
		QFontDatabase::addApplicationFont(resourcePath);
	}
} // namespace

int main(int argc, char *argv[])
{
	const VelopackHook velopackHook = currentVelopackHook(argc, argv);
	if (velopackHook != VelopackHook::None)
	{
		return 0;
	}

	disableDiskCaches();

	const QGuiApplication app(argc, argv);
	QGuiApplication::setApplicationName(QStringLiteral(FS_CLICKER_APP_NAME));
	QGuiApplication::setApplicationVersion(QStringLiteral(FS_CLICKER_APP_VERSION));

	loadApplicationFont(QStringLiteral(":/resources/Jura-Medium.ttf"));
	loadApplicationFont(QStringLiteral(":/resources/SarasaUiSC-Regular.ttf"));

	QFont appFont;
	appFont.setFamilies(QStringList{
		QStringLiteral("Jura"),
		QStringLiteral("Sarasa UI SC"),
		QStringLiteral("Sarasa UI"),
		QStringLiteral("Microsoft YaHei UI"),
		QStringLiteral("Microsoft YaHei"),
		QStringLiteral("SimHei"),
		QStringLiteral("sans-serif"),
	});
	appFont.setPointSize(10);
	QGuiApplication::setFont(appFont);

	ClickerController controller;
	AppConfig		  appConfig(&controller);

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty(QStringLiteral("clicker"), &controller);
	engine.rootContext()->setContextProperty(QStringLiteral("appConfig"), &appConfig);
	engine.rootContext()->setContextProperty(QStringLiteral("appVersion"), QGuiApplication::applicationVersion());
	engine.rootContext()->setContextProperty(QStringLiteral("appAuthor"), QStringLiteral(FS_CLICKER_AUTHOR_NAME));
	engine.rootContext()->setContextProperty(QStringLiteral("appRepositoryUrl"), QStringLiteral(FS_CLICKER_REPOSITORY_URL));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

	engine.loadFromModule(QStringLiteral("FSClicker"), QStringLiteral("Main"));

	QTimer::singleShot(0, &controller, &ClickerController::installHooks);

	return QGuiApplication::exec();
}
