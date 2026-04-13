/**
 * @file app_config.h
 * @brief 应用配置读写和 QML 配置入口。
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <QObject>
#include <QString>

class ClickerController;

/**
 * @brief 管理界面设置，并保存连点功能配置。
 */
class AppConfig : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
	Q_PROPERTY(int languageIndex READ languageIndex WRITE setLanguageIndex NOTIFY languageIndexChanged)
	Q_PROPERTY(double uiScale READ uiScale WRITE setUiScale NOTIFY uiScaleChanged)
	Q_PROPERTY(int uiScaleIndex READ uiScaleIndex WRITE setUiScaleIndex NOTIFY uiScaleIndexChanged)
	Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)
	Q_PROPERTY(int themeMode READ themeMode WRITE setThemeMode NOTIFY themeModeChanged)

public:
	explicit AppConfig(ClickerController *clicker, QObject *parent = nullptr);

	QString language() const;
	int		languageIndex() const;
	double	uiScale() const;
	int		uiScaleIndex() const;
	bool	darkTheme() const;
	int		themeMode() const;

	void setLanguage(const QString &value);
	void setLanguageIndex(int value);
	void setUiScale(double value);
	void setUiScaleIndex(int value);
	void setDarkTheme(bool value);
	void setThemeMode(int value);

	Q_INVOKABLE QString filePath() const;
	Q_INVOKABLE bool	save();

signals:
	void languageChanged();
	void languageIndexChanged();
	void uiScaleChanged();
	void uiScaleIndexChanged();
	void darkThemeChanged();
	void themeModeChanged();

private:
	enum ThemeMode
	{
		ThemeAuto  = 0,
		ThemeDark  = 1,
		ThemeLight = 2,
	};

	void load();
	void resetConfigFile();

	/** 根据主题模式刷新实际生效的深浅色。 */
	void refreshEffectiveDarkTheme();

	ClickerController *clicker;
	QString			   configPath;
	int				   languageIndexValue;
	int				   uiScaleIndexValue;
	int				   themeModeValue;
	bool			   effectiveDarkThemeValue;
};

#endif // APP_CONFIG_H
