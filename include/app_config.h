#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <QObject>
#include <QString>

class ClickerController;

class AppConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(double uiScale READ uiScale WRITE setUiScale NOTIFY uiScaleChanged)
    Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)

public:
    explicit AppConfig(ClickerController *clicker, QObject *parent = nullptr);

    QString language() const;
    double uiScale() const;
    bool darkTheme() const;

    void setLanguage(const QString &value);
    void setUiScale(double value);
    void setDarkTheme(bool value);

    Q_INVOKABLE QString filePath() const;
    Q_INVOKABLE bool save();

signals:
    void languageChanged();
    void uiScaleChanged();
    void darkThemeChanged();

private:
    void load();
    void connectClicker();
    void saveIfReady();

    ClickerController *clicker;
    QString configPath;
    QString languageValue;
    double uiScaleValue;
    bool darkThemeValue;
    bool loading;
};

#endif // APP_CONFIG_H
