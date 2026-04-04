#include "qtetcheckbtn.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyleHints>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QEvent>

QtETCheckBtn::QtETCheckBtn(QWidget *parent)
    : QCheckBox(parent)
    , m_tipFontSize(9)
    , m_sliderPosition(0.0)
    , m_pressedOnSwitch(false)
{
    init();
}

QtETCheckBtn::QtETCheckBtn(const QString &text, QWidget *parent)
    : QCheckBox(text, parent)
    , m_tipFontSize(9)
    , m_sliderPosition(0.0)
    , m_pressedOnSwitch(false)
{
    init();
}

QtETCheckBtn::~QtETCheckBtn()
{
    if (m_animation) {
        m_animation->stop();
    }
}

void QtETCheckBtn::init()
{
    // 初始化动画
    m_animation = new QPropertyAnimation(this, "sliderPosition", this);
    m_animation->setDuration(ANIMATION_DURATION);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // 初始化颜色
    updateColorScheme();

    // 监听系统主题变化
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        updateColorScheme();
        update();
    });

    // 监听选中状态变化
    connect(this, &QCheckBox::toggled, this, [this](bool checked) {
        m_animation->stop();
        m_animation->setStartValue(m_sliderPosition);
        m_animation->setEndValue(checked ? 1.0 : 0.0);
        m_animation->start();
    });

    // 初始化滑块位置
    m_sliderPosition = isChecked() ? 1.0 : 0.0;
}

void QtETCheckBtn::updateColorScheme()
{
    const bool isDark = (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);

    if (isDark) {
        // 深色模式
        m_activeColor = QColor("#66ccff");      // 蓝色激活
        m_inactiveColor = QColor(80, 80, 80);      // 深灰非激活
        m_sliderColor = QColor(255, 255, 255);     // 白色滑块
        m_tipHighlightColor = QColor("#66ccff"); // 蓝色高亮
    } else {
        // 浅色模式
        m_activeColor = QColor("#66ccff");      // 蓝色激活
        m_inactiveColor = QColor(200, 200, 200);   // 浅灰非激活
        m_sliderColor = QColor(255, 255, 255);     // 白色滑块
        m_tipHighlightColor = QColor("#66ccff"); // 高亮蓝
    }

    update();
}

QString QtETCheckBtn::tipText() const
{
    return m_tipText;
}

void QtETCheckBtn::setTipText(const QString &text)
{
    if (m_tipText != text) {
        m_tipText = text;
        updateGeometry();
        update();
    }
}

int QtETCheckBtn::tipFontSize() const
{
    return m_tipFontSize;
}

void QtETCheckBtn::setTipFontSize(int size)
{
    if (size > 0 && m_tipFontSize != size) {
        m_tipFontSize = size;
        updateGeometry();
        update();
    }
}

QColor QtETCheckBtn::activeColor() const
{
    return m_activeColor;
}

void QtETCheckBtn::setActiveColor(const QColor &color)
{
    if (m_activeColor != color) {
        m_activeColor = color;
        update();
    }
}

QColor QtETCheckBtn::inactiveColor() const
{
    return m_inactiveColor;
}

void QtETCheckBtn::setInactiveColor(const QColor &color)
{
    if (m_inactiveColor != color) {
        m_inactiveColor = color;
        update();
    }
}

qreal QtETCheckBtn::sliderPosition() const
{
    return m_sliderPosition;
}

void QtETCheckBtn::setSliderPosition(qreal pos)
{
    if (!qFuzzyCompare(m_sliderPosition, pos)) {
        m_sliderPosition = qBound(0.0, pos, 1.0);
        update();
    }
}

QSize QtETCheckBtn::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(text());
    int textHeight = fm.height();

    // 内容区域宽度：左边距 5 + 文字 + 间距 + 开关 + 右边距 5
    int width = textWidth + TEXT_SWITCH_SPACING + SWITCH_WIDTH + 10;
    int height = qMax(textHeight, SWITCH_HEIGHT);

    // 如果有tooltip，在下方高亮显示，增加高度
    if (!toolTip().isEmpty()) {
        QFont tipFont = font();
        tipFont.setPointSize(m_tipFontSize);
        QFontMetrics tipFm(tipFont);
        height += TIP_TEXT_SPACING + tipFm.height();
    }

    // 添加上下边距（各 5px）
    return QSize(width, height + 10);
}

QSize QtETCheckBtn::minimumSizeHint() const
{
    return sizeHint();
}

QRect QtETCheckBtn::calculateSwitchRect() const
{
    // 开关在右侧，与边框保持 4px 间距（边框 1px + 间距 4px = 5px）
    int switchX = width() - SWITCH_WIDTH - 5;
    int switchY = 5;  // 与上边框保持 4px 间距

    // 如果有tooltip，开关需要在上方区域
    if (toolTip().isEmpty()) {
        // 无 tooltip 时垂直居中
        switchY = (height() - SWITCH_HEIGHT) / 2;
    }

    return QRect(switchX, switchY, SWITCH_WIDTH, SWITCH_HEIGHT);
}

QRectF QtETCheckBtn::calculateSliderRect() const
{
    QRect switchRect = calculateSwitchRect();
    qreal sliderSize = switchRect.height() - 2 * SLIDER_MARGIN;

    // 计算滑块位置：从左边滑动到右边
    qreal sliderX = switchRect.x() + SLIDER_MARGIN + 
                    m_sliderPosition * (switchRect.width() - 2 * SLIDER_MARGIN - sliderSize);
    qreal sliderY = switchRect.y() + SLIDER_MARGIN;

    return QRectF(sliderX, sliderY, sliderSize, sliderSize);
}

void QtETCheckBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制控件边框（类似 QPushButton 样式）
    QRect borderRect = rect().adjusted(1, 1, -1, -1);
    constexpr int borderRadius = 5;
    
    // 获取边框颜色
    QColor borderColor = palette().color(QPalette::Mid);
    QColor backgroundColor = palette().color(QPalette::Button);
    
    // 只有鼠标悬停时高亮边框
    if (underMouse()) {
        borderColor = palette().color(QPalette::Highlight);
    }
    
    // 绘制背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(borderRect, borderRadius, borderRadius);
    painter.fillPath(bgPath, backgroundColor);
    
    // 绘制边框
    painter.setPen(QPen(borderColor, 1));
    painter.drawPath(bgPath);

    QRect switchRect = calculateSwitchRect();
    QRectF sliderRect = calculateSliderRect();

    // 绘制文字（左对齐，与边框保持 4px 间距）
    QFontMetrics fm(font());
    int textHeight = fm.height();
    int textY;
    
    // 文字左边距：边框 1px + 间距 4px = 5px
    constexpr int contentMargin = 5;
    
    if (!toolTip().isEmpty()) {
        // 有 tooltip 时，文字与上边框保持 4px 间距
        textY = contentMargin + fm.ascent();
    } else {
        textY = (height() - textHeight) / 2 + fm.ascent();
    }

    painter.setFont(font());
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(contentMargin, textY, text());

    // 绘制开关背景（带圆角）
    QPainterPath switchPath;
    switchPath.addRoundedRect(switchRect, BORDER_RADIUS, BORDER_RADIUS);
    
    // 背景颜色插值
    QColor bgColor = QColor::fromHsvF(
        m_inactiveColor.toHsv().hueF(),
        m_inactiveColor.toHsv().saturationF() * (1 - m_sliderPosition) + m_activeColor.toHsv().saturationF() * m_sliderPosition,
        m_inactiveColor.toHsv().valueF() * (1 - m_sliderPosition) + m_activeColor.toHsv().valueF() * m_sliderPosition
    );

    // 简化：直接根据滑块位置选择颜色
    if (m_sliderPosition > 0.5) {
        bgColor = m_activeColor;
    } else {
        bgColor = m_inactiveColor;
    }
    
    painter.fillPath(switchPath, bgColor);

    // 绘制滑块（圆形）
    QPainterPath sliderPath;
    sliderPath.addEllipse(sliderRect);
    painter.fillPath(sliderPath, m_sliderColor);

    // 绘制tooltip文字（高亮显示在下方）
    if (!toolTip().isEmpty()) {
        QFont tipFont = font();
        tipFont.setPointSize(m_tipFontSize);
        QFontMetrics tipFm(tipFont);

        painter.setFont(tipFont);
        painter.setPen(m_tipHighlightColor);

        // tooltip 与上方内容保持间距
        int tipY = textY - fm.ascent() + textHeight + TIP_TEXT_SPACING + tipFm.ascent();
        painter.drawText(contentMargin, tipY, toolTip());
    }
}

void QtETCheckBtn::resizeEvent(QResizeEvent *event)
{
    QCheckBox::resizeEvent(event);
    update();
}

void QtETCheckBtn::mousePressEvent(QMouseEvent *event)
{
    // 记录按下时是否在开关区域
    QRect switchRect = calculateSwitchRect();
    m_pressedOnSwitch = switchRect.contains(event->pos());
    
    if (m_pressedOnSwitch) {
        // 接受事件，阻止传播
        event->accept();
    } else {
        // 不在开关区域，忽略事件
        event->ignore();
    }
}

void QtETCheckBtn::mouseReleaseEvent(QMouseEvent *event)
{
    // 只有按下时在开关区域，且释放时也在开关区域才切换状态
    QRect switchRect = calculateSwitchRect();
    
    if (m_pressedOnSwitch && switchRect.contains(event->pos())) {
        // 切换状态
        setChecked(!isChecked());
        event->accept();
    } else {
        event->ignore();
    }
    
    m_pressedOnSwitch = false;
}

bool QtETCheckBtn::event(QEvent *event)
{
    // 拦截 ToolTip 事件，不在鼠标悬停时显示系统tooltip
    // 因为我们已经在下方绘制了tooltip文字
    if (event->type() == QEvent::ToolTip) {
        event->ignore();
        return true;
    }
    
    // 监听 tooltip 变化，更新布局
    if (event->type() == QEvent::ToolTipChange) {
        updateGeometry();
        update();
    }
    
    return QCheckBox::event(event);
}
