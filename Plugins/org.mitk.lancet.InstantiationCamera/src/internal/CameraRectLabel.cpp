#include "CameraRectLabel.h"

CameraRectLabel::CameraRectLabel(QLabel* aLabel)
{
    m_Label = aLabel;
}

void CameraRectLabel::StartDraw()
{
    m_IsSelecting = true;
}

void CameraRectLabel::mousePressEvent(QMouseEvent* event)
{
    if (m_IsSelecting == true)
    {
        if (event->button() == Qt::LeftButton) {

            m_StartPoint = event->pos();
            m_EndPoint = m_StartPoint;  // 初始时结束点与起点相同
            update();  // 更新以触发重绘
        }
    }
}

void CameraRectLabel::mouseMoveEvent(QMouseEvent* event)
{
    if (m_IsSelecting) {
        m_EndPoint = event->pos();  // 更新结束点为当前鼠标位置
        update();  // 更新以触发重绘
    }
}

void CameraRectLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_IsSelecting = false;  // 结束选择
        update();  // 最后一次重绘
    }
}

void CameraRectLabel::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);

    if (m_IsSelecting) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));  // 红色虚线框
        QRect rect = QRect(m_StartPoint, m_EndPoint).normalized();  // 归一化矩形
        painter.drawRect(rect);  // 绘制矩形
    }
}
