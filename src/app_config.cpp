#include "app_config.h"
#include <algorithm>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include "clicker_controller.h"

AppConfig::AppConfig(ClickerController *clicker, QObject *parent) :
    QObject(parent),
    clicker(clicker),
    configPath(QCoreApplication::applicationDirPath() + QStringLiteral("/config.json")),
    languageValue(QStringLiteral("zh")),
    uiScaleValue(1.0),
    darkThemeValue(true),
    loading(false)
{
    load();
    connectClicker();
}

QString AppConfig::language() const
{
    return languageValue;
}

double AppConfig::uiScale() const
{
    return uiScaleValue;
}

bool AppConfig::darkTheme() const
{
    return darkThemeValue;
}

void AppConfig::setLanguage(const QString &value)
{
    const QString normalized = value == QStringLiteral("en") ? QStringLiteral("en") : QStringLiteral("zh");
    if (languageValue == normalized)
    {
        return;
    }

    languageValue = normalized;
    emit languageChanged();
    saveIfReady();
}

void AppConfig::setUiScale(double value)
{
    const double normalized = std::clamp(value, 1.0, 1.5);
    if (qFuzzyCompare(uiScaleValue, normalized))
    {
        return;
    }

    uiScaleValue = normalized;
    emit uiScaleChanged();
    saveIfReady();
}

void AppConfig::setDarkTheme(bool value)
{
    if (darkThemeValue == value)
    {
        return;
    }

    darkThemeValue = value;
    emit darkThemeChanged();
    saveIfReady();
}

QString AppConfig::filePath() const
{
    return configPath;
}

bool AppConfig::save()
{
    QJsonObject root{
        {QStringLiteral("version"), 1},
        {QStringLiteral("language"), languageValue},
        {QStringLiteral("uiScale"), uiScaleValue},
        {QStringLiteral("darkTheme"), darkThemeValue},
    };

    if (clicker)
    {
        root.insert(QStringLiteral("clicker"), clicker->configJson());
    }

    QSaveFile file(configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return file.commit();
}

void AppConfig::load()
{
    QFile file(configPath);
    if (!file.exists())
    {
        save();
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject())
    {
        return;
    }

    loading = true;
    const QJsonObject root = document.object();
    setLanguage(root.value(QStringLiteral("language")).toString(languageValue));
    setUiScale(root.value(QStringLiteral("uiScale")).toDouble(uiScaleValue));
    setDarkTheme(root.value(QStringLiteral("darkTheme")).toBool(darkThemeValue));
    if (clicker)
    {
        clicker->applyConfigJson(root.value(QStringLiteral("clicker")).toObject());
    }
    loading = false;
}

void AppConfig::connectClicker()
{
    if (!clicker)
    {
        return;
    }

    connect(clicker, &ClickerController::globalSwitchKeyChanged, this, &AppConfig::saveIfReady);
    connect(clicker, &ClickerController::diyKeyChanged, this, &AppConfig::saveIfReady);
    connect(clicker, &ClickerController::coordinateChanged, this, &AppConfig::saveIfReady);
    connect(clicker, &ClickerController::inputKeyChanged, this, &AppConfig::saveIfReady);
    connect(clicker, &ClickerController::inputActionModeChanged, this, &AppConfig::saveIfReady);
    connect(clicker, &ClickerController::cursorMoveModeChanged, this, &AppConfig::saveIfReady);
    connect(clicker, &ClickerController::cycleSecondsChanged, this, &AppConfig::saveIfReady);
}

void AppConfig::saveIfReady()
{
    if (!loading)
    {
        save();
    }
}
