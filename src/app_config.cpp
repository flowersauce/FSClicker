/**
 * @file app_config.cpp
 * @brief 负责 config.json 的读取、校验和退出保存。
 */

#include "app_config.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSaveFile>
#include <QStyleHints>
#include <algorithm>
#include <cmath>
#include "clicker_controller.h"

namespace
{
	/** 将语言索引转成 QML 使用的语言标识。 */
	QString languageFromIndex(int value)
	{
		return std::clamp(value, 0, 1) == 1 ? QStringLiteral("en") : QStringLiteral("zh");
	}

	/** 将语言标识转成配置索引。 */
	int indexFromLanguage(const QString &value)
	{
		return value == QStringLiteral("en") ? 1 : 0;
	}

	/** 将缩放索引转成实际缩放倍率。 */
	double scaleFromIndex(int value)
	{
		switch (std::clamp(value, 0, 2))
		{
			case 1:
				return 1.25;
			case 2:
				return 1.5;
			case 0:
			default:
				return 1.0;
		}
	}

	/** 将缩放倍率归到最近的索引。 */
	int indexFromScale(double value)
	{
		if (value >= 1.375)
		{
			return 2;
		}
		if (value >= 1.125)
		{
			return 1;
		}
		return 0;
	}

	/** 读取系统当前的深浅色偏好。 */
	bool systemPrefersDarkTheme()
	{
		if (auto *styleHints = QGuiApplication::styleHints())
		{
			return styleHints->colorScheme() == Qt::ColorScheme::Dark;
		}

		return true;
	}

	/**
	 * @brief 读取严格的整数配置项。
	 */
	bool readInteger(const QJsonObject &object, const QString &name, int *value)
	{
		const QJsonValue jsonValue = object.value(name);
		if (!jsonValue.isDouble())
		{
			return false;
		}

		const double rawValue = jsonValue.toDouble();
		if (std::trunc(rawValue) != rawValue)
		{
			return false;
		}

		*value = static_cast<int>(rawValue);
		return true;
	}

	QString applicationDirectoryPath()
	{
		return QCoreApplication::applicationDirPath();
	}

	QString appDirectoryConfigPath()
	{
		return QDir(applicationDirectoryPath()).filePath(QStringLiteral("config.json"));
	}

	bool isVelopackCurrentDirectory()
	{
		const QDir directory(applicationDirectoryPath());
		return QFileInfo(directory.absolutePath()).fileName().compare(QStringLiteral("current"), Qt::CaseInsensitive) == 0
			&& QFile::exists(directory.filePath(QStringLiteral("sq.version"))) && QFile::exists(directory.filePath(QStringLiteral("../Update.exe")));
	}

	QString configDirectoryPath()
	{
		if (isVelopackCurrentDirectory())
		{
			QDir installRoot(applicationDirectoryPath());
			installRoot.cdUp();
			return installRoot.filePath(QStringLiteral("config"));
		}

		return QDir(applicationDirectoryPath()).filePath(QStringLiteral("config"));
	}

	QString configFilePath()
	{
		return QDir(configDirectoryPath()).filePath(QStringLiteral("config.json"));
	}

	void migrateLegacyAppDirectoryConfig()
	{
		if (!QFile::exists(configFilePath()) && QFile::exists(appDirectoryConfigPath()))
		{
			QFile::copy(appDirectoryConfigPath(), configFilePath());
		}
	}

	QString resolveConfigPath()
	{
		QDir configDirectory(configDirectoryPath());
		configDirectory.mkpath(QStringLiteral("."));
		migrateLegacyAppDirectoryConfig();

		return configFilePath();
	}
} // namespace

AppConfig::AppConfig(ClickerController *clicker, QObject *parent)
	: QObject(parent)
	, clicker(clicker)
	, configPath(resolveConfigPath())
	, languageIndexValue(0)
	, uiScaleIndexValue(0)
	, themeModeValue(ThemeAuto)
	, effectiveDarkThemeValue(systemPrefersDarkTheme())
{
	load();
	if (auto *application = QCoreApplication::instance())
	{
		connect(application, &QCoreApplication::aboutToQuit, this, [this]() { save(); });
	}

	if (auto *styleHints = QGuiApplication::styleHints())
	{
		connect(styleHints, &QStyleHints::colorSchemeChanged, this, [this]() { refreshEffectiveDarkTheme(); });
	}
}

QString AppConfig::language() const
{
	return languageFromIndex(languageIndexValue);
}

int AppConfig::languageIndex() const
{
	return languageIndexValue;
}

double AppConfig::uiScale() const
{
	return scaleFromIndex(uiScaleIndexValue);
}

int AppConfig::uiScaleIndex() const
{
	return uiScaleIndexValue;
}

bool AppConfig::darkTheme() const
{
	return effectiveDarkThemeValue;
}

int AppConfig::themeMode() const
{
	return themeModeValue;
}

void AppConfig::setLanguage(const QString &value)
{
	setLanguageIndex(indexFromLanguage(value));
}

void AppConfig::setLanguageIndex(int value)
{
	const int normalized = std::clamp(value, 0, 1);
	if (languageIndexValue == normalized)
	{
		return;
	}

	languageIndexValue = normalized;
	emit languageIndexChanged();
	emit languageChanged();
}

void AppConfig::setUiScale(double value)
{
	setUiScaleIndex(indexFromScale(value));
}

void AppConfig::setUiScaleIndex(int value)
{
	const int normalized = std::clamp(value, 0, 2);
	if (uiScaleIndexValue == normalized)
	{
		return;
	}

	uiScaleIndexValue = normalized;
	emit uiScaleIndexChanged();
	emit uiScaleChanged();
}

void AppConfig::setDarkTheme(bool value)
{
	setThemeMode(value ? ThemeDark : ThemeLight);
}

void AppConfig::setThemeMode(int value)
{
	const int normalized = std::clamp(value, static_cast<int>(ThemeAuto), static_cast<int>(ThemeLight));
	if (themeModeValue == normalized)
	{
		return;
	}

	themeModeValue = normalized;
	emit themeModeChanged();
	refreshEffectiveDarkTheme();
}

QString AppConfig::filePath() const
{
	return configPath;
}

bool AppConfig::save()
{
	QJsonObject settings{
		{QStringLiteral("languageIndex"), languageIndexValue},
		{QStringLiteral("uiScaleIndex"), uiScaleIndexValue},
		{QStringLiteral("themeMode"), themeModeValue},
	};

	QJsonObject root{
		{QStringLiteral("settings"), settings},
	};

	if (clicker)
	{
		root.insert(QStringLiteral("function"), clicker->configJson());
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
		file.close();
		resetConfigFile();
		return;
	}

	const QJsonObject root = document.object();

	const QJsonValue settingsValue = root.value(QStringLiteral("settings"));
	const QJsonValue functionValue = root.value(QStringLiteral("function"));
	if (!settingsValue.isObject() || (clicker && !functionValue.isObject()))
	{
		file.close();
		resetConfigFile();
		return;
	}

	const QJsonObject settings			  = settingsValue.toObject();
	int				  storedLanguageIndex = 0;
	int				  storedUiScaleIndex  = 0;
	int				  storedThemeMode	  = 0;
	if (!readInteger(settings, QStringLiteral("languageIndex"), &storedLanguageIndex)
		|| !readInteger(settings, QStringLiteral("uiScaleIndex"), &storedUiScaleIndex) || !readInteger(settings, QStringLiteral("themeMode"), &storedThemeMode))
	{
		file.close();
		resetConfigFile();
		return;
	}

	setLanguageIndex(storedLanguageIndex);
	setUiScaleIndex(storedUiScaleIndex);
	setThemeMode(storedThemeMode);

	if (clicker)
	{
		clicker->applyConfigJson(functionValue.toObject());
	}
}

void AppConfig::resetConfigFile()
{
	QFile::remove(configPath);
	save();
}

void AppConfig::refreshEffectiveDarkTheme()
{
	const bool previousValue = effectiveDarkThemeValue;
	switch (themeModeValue)
	{
		case ThemeDark:
			effectiveDarkThemeValue = true;
			break;
		case ThemeLight:
			effectiveDarkThemeValue = false;
			break;
		case ThemeAuto:
		default:
			effectiveDarkThemeValue = systemPrefersDarkTheme();
			break;
	}

	if (effectiveDarkThemeValue != previousValue)
	{
		emit darkThemeChanged();
	}
}
