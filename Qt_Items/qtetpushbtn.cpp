#include "qtetpushbtn.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyleHints>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QFontMetrics>

QtETPushBtn::QtETPushBtn(QWidget *parent)
    : QPushButton(parent)
    , m_borderOpacity(0.0)
    , m_isPressed(false)
{
    init();
}

QtETPushBtn::QtETPushBtn(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , m_borderOpacity(0.0)
    , m_isPressed(false)
{
    init();
}

QtETPushBtn::~QtETPushBtn()
{
    if (m_borderAnimation) {
        m_borderAnimation->stop();
    }
}

void QtETPushBtn::init()
{
    // 初始化边框高亮动画
    m_borderAnimation = new QPropertyAnimation(this, "borderOpacity", this);
    m_borderAnimation->setDuration(BORDER_ANIMATION_DURATION);
    m_borderAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 初始化颜色
    updateColorScheme();

    // 监听系统主题变化
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        updateColorScheme();
        update();
    });
}

void QtETPushBtn::updateColorScheme()
{
    const bool isDark = (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);

    if (isDark) {
        // 深色模式
        m_backgroundColor = QColor(45, 45, 45);
        m_normalBorderColor = QColor(70, 70, 70);
        m_highlightBorderColor = QColor("#66ccff");
        m_pressedBorderColor = QColor("#3399dd");
    } else {
        // 浅色模式
        m_backgroundColor = QColor(240, 240, 240);
        m_normalBorderColor = QColor(180, 180, 180);
        m_highlightBorderColor = QColor("#66ccff");
        m_pressedBorderColor = QColor("#3399dd");
    }

    update();
}

qreal QtETPushBtn::borderOpacity() const
{
    return m_borderOpacity;
}

void QtETPushBtn::setBorderOpacity(qreal opacity)
{
    if (!qFuzzyCompare(m_borderOpacity, opacity)) {
        m_borderOpacity = qBound(0.0, opacity, 1.0);
        update();
    }
}

QSize QtETPushBtn::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    int width = textWidth;
    int height = textHeight;

    // 如果有图标，增加图标宽度和间距
    if (!icon().isNull()) {
        QSize iconSize = this->iconSize();
        width += iconSize.width();
        if (!text().isEmpty()) {
            width += 6; // 图标和文字之间的间距
        }
        height = qMax(height, iconSize.height());
    }

    // 加上边距
    width += CONTENT_MARGIN * 2;
    height += CONTENT_MARGIN * 2;

    return QSize(width, height);
}

QSize QtETPushBtn::minimumSizeHint() const
{
    return sizeHint();
}

void QtETPushBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制控件边框和背景
    QRect borderRect = rect().adjusted(1, 1, -1, -1);

    // 混合边框颜色：普通 -> 高亮 或 按下
    QColor borderColor;
    if (m_isPressed) {
        // 按下状态使用按下颜色
        borderColor = m_pressedBorderColor;
    } else {
        // 悬停状态混合普通和高亮颜色
        borderColor = QColor::fromRgbF(
            m_normalBorderColor.redF() * (1 - m_borderOpacity) + m_highlightBorderColor.redF() * m_borderOpacity,
            m_normalBorderColor.greenF() * (1 - m_borderOpacity) + m_highlightBorderColor.greenF() * m_borderOpacity,
            m_normalBorderColor.blueF() * (1 - m_borderOpacity) + m_highlightBorderColor.blueF() * m_borderOpacity,
            m_normalBorderColor.alphaF() * (1 - m_borderOpacity) + m_highlightBorderColor.alphaF() * m_borderOpacity
        );
    }

    // 绘制背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(borderRect, BORDER_RADIUS, BORDER_RADIUS);
    painter.fillPath(bgPath, m_backgroundColor);

    // 按下状态时绘制30%透明度的淡蓝色叠加层
    if (m_isPressed) {
        QColor pressedOverlay("#66ccff");
        pressedOverlay.setAlphaF(0.3);
        painter.fillPath(bgPath, pressedOverlay);
    }

    // 绘制边框
    painter.setPen(QPen(borderColor, BORDER_WIDTH));
    painter.drawPath(bgPath);

    // 绘制图标和文字
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    bool hasIcon = !icon().isNull();
    QSize drawIconSize = hasIcon ? iconSize() : QSize(0, 0);

    // 绘制图标（固定在左侧，垂直居中）
    if (hasIcon) {
        int iconX = CONTENT_MARGIN;
        int iconY = (height() - drawIconSize.height()) / 2;
        QPixmap pix = icon().pixmap(drawIconSize);
        painter.drawPixmap(iconX, iconY, pix);
    }

    // 绘制文字（永远居中，但确保不与图标重叠）
    if (!text().isEmpty()) {
        // 文字理想位置（完全居中）
        int idealTextX = (width() - textWidth) / 2;

        // 如果有图标，计算最小左边距（图标右边 + 间距）
        int minTextX = CONTENT_MARGIN;
        if (hasIcon) {
            minTextX = CONTENT_MARGIN + drawIconSize.width() + 6;
        }

        // 确保文字不与图标重叠
        int textX = qMax(idealTextX, minTextX);
        int textY = (height() - textHeight) / 2 + fm.ascent();
        painter.setFont(font());
        painter.setPen(palette().color(QPalette::Text));
        painter.drawText(textX, textY, text());
    }
}

void QtETPushBtn::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);

    // 启动边框高亮动画（渐显）
    m_borderAnimation->stop();
    m_borderAnimation->setStartValue(m_borderOpacity);
    m_borderAnimation->setEndValue(1.0);
    m_borderAnimation->start();
}

void QtETPushBtn::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);

    // 启动边框淡出动画（渐隐）
    m_borderAnimation->stop();
    m_borderAnimation->setStartValue(m_borderOpacity);
    m_borderAnimation->setEndValue(0.0);
    m_borderAnimation->start();
}

void QtETPushBtn::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        update();
    }

    QPushButton::mousePressEvent(event);
}

void QtETPushBtn::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        update();
    }

    QPushButton::mouseReleaseEvent(event);
}