#include "coordinate_capture_window.h"

CoordinateCaptureWindow::CoordinateCaptureWindow(QWidget *parent)
		: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowState(Qt::WindowFullScreen);
	// 捕捉鼠标事件
	setMouseTracking(true);
}

void CoordinateCaptureWindow::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	// 获取鼠标位置（逻辑坐标）
	QPoint cursorPos = QCursor::pos();
	// 获取屏幕的缩放因子
	QScreen *screen = QGuiApplication::primaryScreen();
	qreal scaleFactor = screen->devicePixelRatio();
	// 转换为实际屏幕像素坐标
	int actualX = static_cast<int>(cursorPos.x() * scaleFactor);
	int actualY = static_cast<int>(cursorPos.y() * scaleFactor);
	int width = this->width();
	int height = this->height();
	// 画半透明背景
	painter.fillRect(rect(), QColor(0, 0, 0, 100));
	// 画交叉线
	painter.setPen(QPen(Qt::white, 2));
	painter.drawLine(cursorPos.x(), 0, cursorPos.x(), height); // 竖线
	painter.drawLine(0, cursorPos.y(), width, cursorPos.y());  // 横线
	// 准备显示的实际坐标文本
	QString coordinates = QString("X: %1, Y: %2").arg(actualX).arg(actualY);
	// 获取文本的宽度和高度
	int textWidth = painter.fontMetrics().horizontalAdvance(coordinates);
	int textHeight = painter.fontMetrics().height();
	// 动态调整坐标显示位置
	QPoint textOffset;
	const int margin = 10;
	if (cursorPos.x() + textWidth + margin > width)
	{
		// 当文本宽度超出屏幕右侧边缘时，文本显示在左侧
		textOffset.setX(-textWidth - margin);
	}
	else
	{
		// 否则，文本显示在右侧
		textOffset.setX(margin);
	}

	if (cursorPos.y() + textHeight + margin > height)
	{
		// 当文本高度超出屏幕下方边缘时，文本显示在上方
		textOffset.setY(-margin);
	}
	else
	{
		// 否则，文本显示在下方
		textOffset.setY(textHeight + margin);
	}
	// 显示实际坐标
	painter.drawText(cursorPos + textOffset, coordinates);
}

void CoordinateCaptureWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		// 捕捉鼠标左键点击时的坐标
		QPoint cursorPos = QCursor::pos();

		// 获取屏幕的缩放因子
		QScreen *screen = QGuiApplication::primaryScreen();
		qreal scaleFactor = screen->devicePixelRatio();

		// 转换为实际屏幕像素坐标
		int actualX = static_cast<int>(cursorPos.x() * scaleFactor);
		int actualY = static_cast<int>(cursorPos.y() * scaleFactor);

		// 发出信号，传递实际屏幕坐标
		emit coordinatesCaptured(actualX, actualY);

		// 关闭窗口
		close();
	}
}

void CoordinateCaptureWindow::mouseMoveEvent(QMouseEvent *event)
{
	// 强制重新绘制以更新交叉线位置
	update();
}
