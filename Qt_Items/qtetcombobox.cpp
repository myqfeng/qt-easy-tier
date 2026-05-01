#include "qtetcombobox.h"
#include "qtettheme.h"
#include "qtetdrawutils.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyleHints>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QListView>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QFrame>
#include <QVBoxLayout>
#include <QScreen>

// ==================== 下拉列表委托 ====================

class QtETComboDelegate : public QStyledItemDelegate
{
public:
    explicit QtETComboDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        auto *theme = QtETTheme::instance();
        QRect itemRect = option.rect;

        if (option.state & QStyle::State_Selected) {
            QColor selBg = theme->selectedBackgroundColor();
            QPainterPath path;
            path.addRoundedRect(itemRect.adjusted(1, 1, -1, -1), 4, 4);
            painter->fillPath(path, selBg);
        } else if (option.state & QStyle::State_MouseOver) {
            QColor hoverBg = theme->hoverBackgroundColor();
            QPainterPath path;
            path.addRoundedRect(itemRect.adjusted(1, 1, -1, -1), 4, 4);
            painter->fillPath(path, hoverBg);
        }

        painter->setPen(theme->textColor());
        QFont font = option.font;
        painter->setFont(font);
        QString text = index.data(Qt::DisplayRole).toString();
        QRect textRect = itemRect.adjusted(8, 0, -8, 0);
        QFontMetrics fm(font);
        QString elidedText = fm.elidedText(text, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elidedText);

        painter->restore();
    }

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(qMax(size.height(), 28));
        return size;
    }
};

// ==================== 圆角弹出窗口 ====================

class QtETComboPopup : public QFrame
{
    Q_OBJECT

public:
    explicit QtETComboPopup(QWidget *parent = nullptr)
        : QFrame(parent, Qt::Popup | Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(POPUP_MARGIN, POPUP_MARGIN, POPUP_MARGIN, POPUP_MARGIN);
        layout->setSpacing(0);

        m_view = new QListView(this);
        m_view->setFrameShape(QFrame::NoFrame);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_view->setMouseTracking(true);
        m_view->setItemDelegate(new QtETComboDelegate(m_view));
        layout->addWidget(m_view);

        connect(m_view, &QAbstractItemView::clicked, this, &QtETComboPopup::itemClicked);
        connect(m_view, &QAbstractItemView::activated, this, &QtETComboPopup::itemClicked);
    }

    void setModel(QAbstractItemModel *model)
    {
        m_view->setModel(model);
    }

    void setCurrentIndex(int index)
    {
        if (index >= 0 && index < m_view->model()->rowCount()) {
            m_view->setCurrentIndex(m_view->model()->index(index, 0));
        }
    }

    QListView* view() const { return m_view; }

    void updateTheme()
    {
        auto *theme = QtETTheme::instance();
        QPalette pal = m_view->palette();
        pal.setColor(QPalette::Base, theme->widgetBackgroundColor());
        pal.setColor(QPalette::Text, theme->textColor());
        pal.setColor(QPalette::Highlight, Qt::transparent);
        pal.setColor(QPalette::HighlightedText, theme->textColor());
        m_view->setPalette(pal);
        m_borderColor = theme->normalBorderColor();
        m_bgColor = theme->widgetBackgroundColor();
    }

    void calcAndSetSize(int minWidth)
    {
        int rowCount = m_view->model() ? m_view->model()->rowCount() : 0;
        if (rowCount == 0) {
            return;
        }

        int itemHeight = m_view->sizeHintForRow(0);
        int maxVisibleItems = 7;
        int visibleRows = qMin(rowCount, maxVisibleItems);

        // 计算内容最大文字宽度
        QFontMetrics fm(m_view->font());
        int maxTextWidth = 0;
        for (int i = 0; i < rowCount; ++i) {
            QString text = m_view->model()->data(m_view->model()->index(i, 0), Qt::DisplayRole).toString();
            maxTextWidth = qMax(maxTextWidth, fm.horizontalAdvance(text));
        }

        // 内容宽度 = 文字宽度 + 左右内边距 + 滚动条预留
        int contentWidth = maxTextWidth + 32;
        if (visibleRows < rowCount) {
            contentWidth += m_view->verticalScrollBar()->sizeHint().width();
        }
        int popupWidth = qMax(minWidth, contentWidth) + POPUP_MARGIN * 2;

        int popupHeight = visibleRows * itemHeight + POPUP_MARGIN * 2;

        setFixedSize(popupWidth, popupHeight);

        // 设置窗口遮罩实现圆角
        QRegion region(0, 0, popupWidth, popupHeight, QRegion::Rectangle);
        QPainterPath maskPath;
        maskPath.addRoundedRect(rect(), POPUP_RADIUS, POPUP_RADIUS);
        setMask(maskPath.toFillPolygon().toPolygon());
    }

signals:
    void itemClicked(const QModelIndex &index);

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRect drawRect = rect().adjusted(1, 1, -1, -1);
        QtETDrawUtils::drawRoundedRect(&painter, drawRect, POPUP_RADIUS, m_bgColor, m_borderColor, 1);
    }

    void hideEvent(QHideEvent *event) override
    {
        QFrame::hideEvent(event);
        emit popupHidden();
    }

signals:
    void popupHidden();

private:
    QListView *m_view = nullptr;
    QColor m_bgColor;
    QColor m_borderColor;

    static constexpr int POPUP_RADIUS = 6;
    static constexpr int POPUP_MARGIN = 1;
};

// ==================== QtETComboBox 实现 ====================

QtETComboBox::QtETComboBox(QWidget *parent)
    : QComboBox(parent)
    , m_isPressed(false)
{
    init();
}

QtETComboBox::~QtETComboBox()
{
    if (m_borderAnimation) {
        m_borderAnimation->stop();
    }
    delete m_popup;
}

void QtETComboBox::init()
{
    m_borderAnimation = new QPropertyAnimation(this, "borderOpacity", this);
    m_borderAnimation->setDuration(BORDER_ANIMATION_DURATION);
    m_borderAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 禁用 QComboBox 内置弹出，由我们自定义
    setEditable(false);

    updateColorScheme();

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        updateColorScheme();
        if (m_popup) {
            m_popup->updateTheme();
            m_popup->update();
        }
        update();
    });
}

void QtETComboBox::updateColorScheme()
{
    auto *theme = QtETTheme::instance();
    m_backgroundColor = theme->widgetBackgroundColor();
    m_normalBorderColor = theme->normalBorderColor();
    m_highlightBorderColor = theme->highlightBorderColor();
    m_pressedBorderColor = theme->pressedBorderColor();
    m_disabledTextColor = theme->textInactiveColor();
    update();
}

qreal QtETComboBox::borderOpacity() const
{
    return m_borderOpacity;
}

void QtETComboBox::setBorderOpacity(qreal opacity)
{
    if (!qFuzzyCompare(m_borderOpacity, opacity)) {
        m_borderOpacity = qBound(0.0, opacity, 1.0);
        update();
    }
}

void QtETComboBox::startBorderAnimation(qreal targetOpacity)
{
    if (m_borderAnimation->state() == QAbstractAnimation::Running) {
        m_borderAnimation->stop();
    }
    m_borderAnimation->setStartValue(m_borderOpacity);
    m_borderAnimation->setEndValue(targetOpacity);
    m_borderAnimation->start();
}

QSize QtETComboBox::sizeHint() const
{
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(currentText());
    int textHeight = fm.height();

    int width = textWidth + ARROW_SIZE + CONTENT_MARGIN * 2 + 12;
    int height = textHeight + CONTENT_MARGIN * 2;

    return QSize(width, height);
}

QSize QtETComboBox::minimumSizeHint() const
{
    return QSize(80, sizeHint().height());
}

void QtETComboBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect borderRect = rect().adjusted(1, 1, -1, -1);

    QColor borderColor;
    QColor bgColor = m_backgroundColor;

    if (!isEnabled()) {
        borderColor = m_normalBorderColor;
        bgColor = QtETDrawUtils::blendColors(bgColor, QColor(128, 128, 128), 0.1);
    } else if (m_isPressed || (m_popup && m_popup->isVisible())) {
        borderColor = m_pressedBorderColor;
    } else {
        borderColor = QtETDrawUtils::blendColors(m_normalBorderColor, m_highlightBorderColor, m_borderOpacity);
    }

    QtETDrawUtils::drawRoundedRect(&painter, borderRect, BORDER_RADIUS, bgColor, borderColor, BORDER_WIDTH);

    if ((m_isPressed || (m_popup && m_popup->isVisible())) && isEnabled()) {
        QColor pressedOverlay = QtETTheme::AccentColor;
        pressedOverlay.setAlphaF(0.3);
        QPainterPath overlayPath;
        overlayPath.addRoundedRect(borderRect, BORDER_RADIUS, BORDER_RADIUS);
        painter.fillPath(overlayPath, pressedOverlay);
    }

    // 绘制选中文字
    if (!currentText().isEmpty()) {
        QFontMetrics fm(font());
        QString elidedText = fm.elidedText(currentText(), Qt::ElideRight,
                                           width() - ARROW_SIZE - CONTENT_MARGIN * 2 - 12);
        QRect textRect = rect().adjusted(CONTENT_MARGIN + 2, 0,
                                         -(ARROW_SIZE + CONTENT_MARGIN + 6), 0);
        painter.setFont(font());
        painter.setPen(isEnabled() ? palette().color(QPalette::Text) : m_disabledTextColor);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elidedText);
    }

    // 绘制下拉箭头三角形
    int arrowX = width() - ARROW_SIZE - CONTENT_MARGIN - 4;
    int arrowY = (height() - ARROW_HEIGHT) / 2;
    QPainterPath arrowPath;
    arrowPath.moveTo(arrowX, arrowY);
    arrowPath.lineTo(arrowX + ARROW_SIZE, arrowY);
    arrowPath.lineTo(arrowX + ARROW_SIZE / 2, arrowY + ARROW_HEIGHT);
    arrowPath.closeSubpath();
    painter.fillPath(arrowPath, isEnabled() ? palette().color(QPalette::Text) : m_disabledTextColor);
}

void QtETComboBox::enterEvent(QEnterEvent *event)
{
    QComboBox::enterEvent(event);
    if (isEnabled()) {
        startBorderAnimation(1.0);
    }
}

void QtETComboBox::leaveEvent(QEvent *event)
{
    QComboBox::leaveEvent(event);
    if (isEnabled()) {
        startBorderAnimation(0.0);
    }
}

void QtETComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;
        update();
    }
    QComboBox::mousePressEvent(event);
}

void QtETComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        update();
    }
    QComboBox::mouseReleaseEvent(event);
}

void QtETComboBox::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        update();
    }
    QComboBox::changeEvent(event);
}

void QtETComboBox::showPopup()
{
    if (!isEnabled() || count() == 0) {
        return;
    }

    // 如果弹出已显示，切换为关闭
    if (m_popup && m_popup->isVisible()) {
        hidePopup();
        return;
    }

    // 创建或复用弹出窗口
    if (!m_popup) {
        m_popup = new QtETComboPopup(nullptr);
        m_popup->updateTheme();

        connect(m_popup, &QtETComboPopup::itemClicked, this, &QtETComboBox::onPopupItemClicked);
        connect(m_popup, &QtETComboPopup::popupHidden, this, [this]() {
            m_isPressed = false;
            update();
        });
    }

    // 将 model 传递给弹出窗口
    m_popup->setModel(model());
    m_popup->setCurrentIndex(currentIndex());

    // 计算弹出窗口大小与位置
    m_popup->calcAndSetSize(width());

    // 定位到控件下方
    QPoint globalPos = mapToGlobal(QPoint(0, height() + 1));
    QScreen *screen = QApplication::screenAt(globalPos);
    if (!screen) {
        screen = QApplication::primaryScreen();
    }
    if (screen) {
        QRect screenGeom = screen->availableGeometry();
        if (globalPos.y() + m_popup->height() > screenGeom.bottom()) {
            globalPos.setY(mapToGlobal(QPoint(0, 0)).y() - m_popup->height() - 1);
        }
        if (globalPos.x() + m_popup->width() > screenGeom.right()) {
            globalPos.setX(screenGeom.right() - m_popup->width());
        }
    }

    m_popup->move(globalPos);
    m_popup->show();
    m_popup->view()->setFocus();

    update();
}

void QtETComboBox::hidePopup()
{
    if (m_popup && m_popup->isVisible()) {
        m_popup->hide();
    }
}

void QtETComboBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_F4) {
        if (m_popup && m_popup->isVisible()) {
            hidePopup();
        } else {
            showPopup();
        }
        return;
    }
    QComboBox::keyPressEvent(event);
}

void QtETComboBox::onPopupItemClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        setCurrentIndex(index.row());
    }
    hidePopup();
}

#include "qtetcombobox.moc"
