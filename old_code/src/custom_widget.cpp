#include "custom_widget.h"
#include "ui_custom_widget.h"
#include <QPainter>

CustomWidget::CustomWidget(QWidget *parent)
		: QWidget(parent),
		  ui(new Ui::CustomWidget),
		  widgetCornerRadius(10),
		  widgetBorderWidth(2),
		  widgetBorderLightness(120),
		  widgetColor(Qt::white)
{
	ui->setupUi(this);

	// 只有当没有父窗口时，才设置为无边框和透明背景
	if (!parent)
	{
		setWindowFlags(Qt::FramelessWindowHint);  // 设置无边框窗口
	}
	setAttribute(Qt::WA_TranslucentBackground);  // 设置窗口背景透明
}

CustomWidget::~CustomWidget()
{
	delete ui;
}

void CustomWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿
	painter.setPen(Qt::transparent);  // 边框透明

	// 绘制外层
	QBrush borderBrush(widgetColor.lighter(widgetBorderLightness));
	painter.setBrush(borderBrush);
	QRect outerRect = this->rect();
	painter.drawRoundedRect(outerRect, widgetCornerRadius, widgetCornerRadius);
	// 绘制内层
	QBrush innerBrush(widgetColor);
	painter.setBrush(innerBrush);
	QRect innerRect = outerRect.adjusted(widgetBorderWidth, widgetBorderWidth, -widgetBorderWidth, -widgetBorderWidth);
	painter.drawRoundedRect(innerRect, widgetCornerRadius - widgetBorderWidth, widgetCornerRadius - widgetBorderWidth);

	QWidget::paintEvent(event);
}

void CustomWidget::setWidgetCornerRadius(int radius)
{
	widgetCornerRadius = radius;
	update();  // 重绘
}

void CustomWidget::setWidgetColor(const QColor &color)
{
	widgetColor = color;
	update();
}

void CustomWidget::setWidgetBorderLightness(int lightness)
{
	widgetBorderLightness = lightness;
	update();
}
