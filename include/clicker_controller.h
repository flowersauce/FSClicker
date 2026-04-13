#ifndef CLICKER_CONTROLLER_H
#define CLICKER_CONTROLLER_H

#include <windows.h>
#include <QAudioOutput>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QObject>
#include <QPoint>
#include <QThread>
#include "event_injector.h"
#include "keyboard_hook.h"

class ClickerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool canStart READ canStart NOTIFY canStartChanged)
    Q_PROPERTY(bool hookInstalled READ hookInstalled CONSTANT)
    Q_PROPERTY(bool capturingGlobalSwitchKey READ capturingGlobalSwitchKey NOTIFY captureStateChanged)
    Q_PROPERTY(bool capturingDiyKey READ capturingDiyKey NOTIFY captureStateChanged)
    Q_PROPERTY(bool capturingCoordinate READ capturingCoordinate NOTIFY captureStateChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString globalSwitchKeyName READ globalSwitchKeyName NOTIFY globalSwitchKeyChanged)
    Q_PROPERTY(QString diyKeyName READ diyKeyName NOTIFY diyKeyChanged)
    Q_PROPERTY(QString coordinateText READ coordinateText NOTIFY coordinateChanged)
    Q_PROPERTY(int inputKey READ inputKey WRITE setInputKey NOTIFY inputKeyChanged)
    Q_PROPERTY(int inputActionMode READ inputActionMode WRITE setInputActionMode NOTIFY inputActionModeChanged)
    Q_PROPERTY(int cursorMoveMode READ cursorMoveMode WRITE setCursorMoveMode NOTIFY cursorMoveModeChanged)
    Q_PROPERTY(double cycleSeconds READ cycleSeconds WRITE setCycleSeconds NOTIFY cycleSecondsChanged)

public:
    explicit ClickerController(QObject *parent = nullptr);
    ~ClickerController() override;

    bool running() const;
    bool canStart() const;
    bool hookInstalled() const;
    bool capturingGlobalSwitchKey() const;
    bool capturingDiyKey() const;
    bool capturingCoordinate() const;
    QString statusText() const;
    QString globalSwitchKeyName() const;
    QString diyKeyName() const;
    QString coordinateText() const;
    int inputKey() const;
    int inputActionMode() const;
    int cursorMoveMode() const;
    double cycleSeconds() const;
    QJsonObject configJson() const;
    void applyConfigJson(const QJsonObject &config);

    void setInputKey(int value);
    void setInputActionMode(int value);
    void setCursorMoveMode(int value);
    void setCycleSeconds(double value);

    Q_INVOKABLE void beginGlobalSwitchKeyCapture();
    Q_INVOKABLE void beginDiyKeyCapture();
    Q_INVOKABLE void beginCoordinateCapture();
    Q_INVOKABLE void cancelCoordinateCapture();
    Q_INVOKABLE void finishCoordinateCapture(int x, int y);
    Q_INVOKABLE void setLockedCoordinate(int x, int y);
    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void stop();

signals:
    void runningChanged();
    void canStartChanged();
    void captureStateChanged();
    void statusTextChanged();
    void globalSwitchKeyChanged();
    void diyKeyChanged();
    void coordinateChanged();
    void inputKeyChanged();
    void inputActionModeChanged();
    void cursorMoveModeChanged();
    void cycleSecondsChanged();
    void startEventInjector(int inputKey,
                            int inputActionMode,
                            int cursorMoveMode,
                            DWORD diyKey,
                            int x,
                            int y,
                            double eventCycle);
    void stopEventInjector();

private slots:
    void handleKeyPressed(DWORD keyCode);
    void setInjectorStarted();
    void setInjectorStopped();

private:
    void start();
    void updateCanStart();
    void setStatusText(const QString &text);
    static QString keyName(DWORD keyCode);

    QThread eventInjectorThread;
    EventInjector *eventInjector;
    KeyboardHook keyboardHook;
    QMediaPlayer soundPlayer;
    QAudioOutput audioOutput;
    bool runningFlag;
    bool canStartFlag;
    bool capturingGlobalSwitchKeyFlag;
    bool capturingDiyKeyFlag;
    bool capturingCoordinateFlag;
    QString statusTextValue;
    DWORD globalSwitchKey;
    DWORD diyKey;
    int inputKeyValue;
    int inputActionModeValue;
    int cursorMoveModeValue;
    double cycleSecondsValue;
    QPoint lockedCoordinate;
};

#endif // CLICKER_CONTROLLER_H
