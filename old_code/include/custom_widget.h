#ifndef CUSTOM_WIDGET_H
#define CUSTOM_WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class CustomWidget;
}
QT_END_NAMESPACE

class CustomWidget : public QWidget
{
Q_OBJECT

public:
	explicit CustomWidget(QWidget *parent = nullptr);
	~CustomWidget() override;

protected:
	void paintEvent(QPaintEvent *event) override;   // 重载paintEvent

private:
	Ui::CustomWidget *ui;
	int widgetCornerRadius;                         // 窗口圆角半径
	int widgetBorderWidth;                          // 窗口边框厚度
	int widgetBorderLightness;                      // 窗口边框亮度 (%)
	QColor widgetColor;                             // 窗口颜色

public slots:
	void setWidgetCornerRadius(int radius);         // 设置窗口圆角半径
	void setWidgetColor(const QColor &color);       // 设置窗口颜色
	void setWidgetBorderLightness(int lightness);   // 设置窗口边框亮度
};

#endif // CUSTOM_WIDGET_H
