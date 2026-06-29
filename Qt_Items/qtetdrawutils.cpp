#include "qtetdrawutils.h"

#include <QFontMetrics>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QWheelEvent>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <QEasingCurve>

QColor QtETDrawUtils::blendColors(const QColor &from, const QColor &to, qreal opacity)
{
    return QColor::fromRgbF(
        from.redF() * (1.0 - opacity) + to.redF() * opacity,
        from.greenF() * (1.0 - opacity) + to.greenF() * opacity,
        from.blueF() * (1.0 - opacity) + to.blueF() * opacity,
        from.alphaF() * (1.0 - opacity) + to.alphaF() * opacity
    );
}

void QtETDrawUtils::drawRoundedRect(QPainter *painter, const QRect &rect, int radius,
                                     const QColor &fillColor, const QColor &borderColor,
                                     int borderWidth)
{
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    painter->fillPath(path, fillColor);
    painter->setPen(QPen(borderColor, borderWidth));
    painter->drawPath(path);
}

void QtETDrawUtils::drawRoundedFill(QPainter *painter, const QRect &rect, int radius,
                                     const QColor &fillColor)
{
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    painter->fillPath(path, fillColor);
}

void QtETDrawUtils::drawBottomIndicator(QPainter *painter, qreal x, qreal y,
                                         qreal width, qreal height, const QColor &color)
{
    QPainterPath path;
    path.addRoundedRect(QRectF(x, y, width, height), height / 2.0, height / 2.0);
    painter->fillPath(path, color);
}

void QtETDrawUtils::drawLabel(QPainter *painter, int x, int y,
                               const QString &text, const QColor &bgColor,
                               const QColor &textColor, int fontSize,
                               int hPadding, int vPadding, int radius)
{
    QFont oldFont = painter->font();
    QFont labelFont = oldFont;
    labelFont.setPointSize(fontSize);
    labelFont.setWeight(QFont::Bold);
    painter->setFont(labelFont);

    QFontMetrics fm(labelFont);
    int textWidth = fm.horizontalAdvance(text);
    int labelWidth = textWidth + 2 * hPadding;
    int labelHeight = fm.height() + 2 * vPadding;

    QPainterPath path;
    path.addRoundedRect(x, y, labelWidth, labelHeight, radius, radius);
    painter->fillPath(path, bgColor);

    painter->setPen(textColor);
    int textX = x + (labelWidth - textWidth) / 2;
    int textY = y + vPadding + fm.ascent();
    painter->drawText(textX, textY, text);

    painter->setFont(oldFont);
}

QSize QtETDrawUtils::labelSize(const QString &text, int fontSize,
                                int hPadding, int vPadding)
{
    QFont font;
    font.setPointSize(fontSize);
    font.setWeight(QFont::Bold);
    QFontMetrics fm(font);
    return QSize(fm.horizontalAdvance(text) + 2 * hPadding, fm.height() + 2 * vPadding);
}

void QtETDrawUtils::safeEndPainter(QPainter *painter)
{
    if (painter->isActive()) {
        painter->end();
    }
}

QtETSmoothScroll::QtETSmoothScroll(QAbstractScrollArea *scrollArea, QObject *parent)
    : QObject(parent)
    , m_scrollArea(scrollArea)
{
}

QtETSmoothScroll* QtETSmoothScroll::install(QAbstractScrollArea *scrollArea,
                                             int duration, int step)
{
    auto *filter = new QtETSmoothScroll(scrollArea, scrollArea);
    filter->setDuration(duration);
    filter->setScrollStep(step);
    scrollArea->viewport()->installEventFilter(filter);
    return filter;
}

void QtETSmoothScroll::setDuration(int ms)
{
    m_duration = ms;
}

void QtETSmoothScroll::setScrollStep(int step)
{
    m_scrollStep = step;
}

bool QtETSmoothScroll::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel && m_scrollArea) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        QScrollBar *scrollBar = m_scrollArea->verticalScrollBar();
        if (!scrollBar || !scrollBar->isVisible()) {
            return QObject::eventFilter(watched, event);
        }

        // 触控板（含 macOS）会发出高频、连续的像素级增量事件；鼠标滚轮发出
        // 离散的角度增量（angleDelta，无 pixelDelta）。对触控板套用“固定步长 +
        // 属性动画”会让每个小增量都触发一次大跳并反复打断动画，表现为滚动乱跳/
        // 轨迹失控。因此：触控板按 pixelDelta 做 1:1 直接滚动（交给系统惯性），
        // 仅鼠标滚轮才使用平滑动画。
        const QPoint pixelDelta = wheelEvent->pixelDelta();
        if (!pixelDelta.isNull()) {
            if (m_animation) {
                m_animation->stop();
            }
            scrollBar->setValue(scrollBar->value() - pixelDelta.y());
            return true;
        }

        int delta = -wheelEvent->angleDelta().y();
        if (delta == 0) {
            return QObject::eventFilter(watched, event);
        }
        int currentValue = scrollBar->value();
        // 动画进行中以目标值为基准累加，保证连续滚动叠加而非互相打断。
        int base = (m_animation && m_animation->state() == QAbstractAnimation::Running)
                       ? m_targetValue
                       : currentValue;
        m_targetValue = base + (delta > 0 ? m_scrollStep : -m_scrollStep);
        m_targetValue = qBound(scrollBar->minimum(), m_targetValue, scrollBar->maximum());

        if (!m_animation) {
            m_animation = new QPropertyAnimation(scrollBar, "value", this);
            m_animation->setDuration(m_duration);
            m_animation->setEasingCurve(QEasingCurve::OutQuad);
        }

        m_animation->stop();
        m_animation->setStartValue(currentValue);
        m_animation->setEndValue(m_targetValue);
        m_animation->start();

        return true;
    }

    return QObject::eventFilter(watched, event);
}
