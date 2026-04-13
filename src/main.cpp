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
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
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
    app.setFont(appFont);

    ClickerController controller;
    AppConfig appConfig(&controller);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("clicker"), &controller);
    engine.rootContext()->setContextProperty(QStringLiteral("appConfig"), &appConfig);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []()
        {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.loadFromModule(QStringLiteral("FSClicker"), QStringLiteral("Main"));

    return app.exec();
}
