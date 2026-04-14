/**
 * @file main.cpp
 * @brief 程序入口和 QML 上下文初始化。
 */

#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStringList>
#include "app_config.h"
#include "clicker_controller.h"

namespace
{
	void loadApplicationFont(const QString &resourcePath)
	{
		QFontDatabase::addApplicationFont(resourcePath);
	}
} // namespace

int main(int argc, char *argv[])
{
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

	return QGuiApplication::exec();
}
