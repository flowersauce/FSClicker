#ifndef COORDINATE_CAPTURE_WINDOW_H
#define COORDINATE_CAPTURE_WINDOW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include <QPoint>
#include <QString>

class CoordinateCaptureWindow : public QWidget
{
Q_OBJECT

public:
	explicit CoordinateCaptureWindow(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

signals:
	void coordinatesCaptured(int x, int y);
};

#endif //COORDINATE_CAPTURE_WINDOW_H
