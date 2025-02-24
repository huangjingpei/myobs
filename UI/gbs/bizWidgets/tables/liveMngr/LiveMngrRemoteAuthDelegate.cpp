#include "LiveMngrRemoteAuthDelegate.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QTextEdit>
#include <QClipboard>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include "LiveMngrView.h"

LiveMngrRemoteAuthDelegate::LiveMngrRemoteAuthDelegate(QObject *parent) : QStyledItemDelegate(parent) {
	
	if (LiveMngrView *view = qobject_cast<LiveMngrView*>(parent)) {
        view->viewport()->installEventFilter(this);
    }

}
QWidget *LiveMngrRemoteAuthDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const
{
	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0); // 移除所有外边距
	layout->setSpacing(0);                  // 移除布局间距

	QTextEdit *textEdit = new QTextEdit(widget);
	textEdit->setReadOnly(true);
	textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 隐藏垂直滚动条
	textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条
	textEdit->setStyleSheet(
		"QTextEdit { border: none; padding: 0px; margin: 0px; background: transparent; color: black; }"); // 移除边框、内边距和背景，设置文本颜色
	textEdit->setAlignment(Qt::AlignCenter);      // 居中对齐
	textEdit->setFont(QApplication::font());      // 使用默认字体，与 paint 一致
	textEdit->setLineWrapMode(QTextEdit::NoWrap); // 禁止自动换行，确保单行显示

	// 精确设置行高为字体高度
	QFontMetrics fm(QApplication::font());
	int fontHeight = fm.height();
	textEdit->setFixedHeight(fontHeight);       // 设置固定高度为字体高度
	textEdit->document()->setDocumentMargin(0); // 移除文档内边距

	// 设置 QTextEdit 的最小和最大大小，确保布局比例
	textEdit->setMinimumWidth(1); // 允许 QHBoxLayout 按比例调整
	textEdit->setMaximumWidth(QWIDGETSIZE_MAX);
	textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 固定高度，按宽度扩展

	QPushButton *copyButton = new QPushButton("复制", widget);
	copyButton->setStyleSheet(
		"QPushButton { border: none; padding: 0px; margin: 0px; background: transparent; color: #00C566; }"); // 移除边框、内边距和背景，设置文本颜色
	copyButton->setMinimumWidth(1); // 允许 QHBoxLayout 按比例调整
	copyButton->setMaximumWidth(QWIDGETSIZE_MAX);
	copyButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 固定高度，按宽度扩展
	copyButton->setFixedHeight(fontHeight); // 设置固定高度为字体高度，与 QTextEdit 一致

	// 设置子控件的宽度比例（60% 文本 + 40% 按钮）
	layout->addWidget(textEdit, 6);   // 权重 6，占 60%
	layout->addWidget(copyButton, 4); // 权重 4，占 40%

	connect(copyButton, &QPushButton::clicked, [=]() {
		QClipboard *clipboard = QApplication::clipboard();
		QString password = textEdit->toPlainText();
		clipboard->setText(password);
	});

	return widget;
}
void LiveMngrRemoteAuthDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	return;
	//TODO WHY ???
	QWidget *widget = static_cast<QWidget *>(editor);
	QTextEdit *textEdit = widget->findChild<QTextEdit *>();
	QString password = index.model()->data(index, Qt::DisplayRole).toString();
	textEdit->setText(password);
	// 强制刷新 QTextEdit 的显示
	textEdit->update(); // 显式调用 update 刷新
}

void LiveMngrRemoteAuthDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {}

void LiveMngrRemoteAuthDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
					    const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

void LiveMngrRemoteAuthDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem myOption = option;
	QString text = index.model()->data(index, Qt::DisplayRole).toString();
	myOption.text = text;

	// 获取密码
	QString password = index.model()->data(index, Qt::DisplayRole).toString();

	// 计算各个控件的矩形
	QRect textRect = QRect(option.rect.x(), option.rect.y(), option.rect.width() * 0.6, option.rect.height());
	QRect buttonRect = QRect(option.rect.x() + option.rect.width() * 0.6, option.rect.y(),
				 option.rect.width() * 0.4, option.rect.height());

	// 绘制文本
	painter->drawText(textRect, Qt::AlignCenter, password);

	// 绘制按钮
	QStyleOptionButton buttonOption;
	buttonOption.rect = buttonRect;
	buttonOption.text = "复制";
	buttonOption.state |= QStyle::State_Enabled;
	
	// 设置按钮无边框
    buttonOption.features = QStyleOptionButton::Flat;

	// 检查鼠标悬停状态
    if (buttonRect.contains(QCursor::pos() - option.widget->mapToGlobal(QPoint(0, 0)))) {
        buttonOption.state |= QStyle::State_MouseOver;
    }
	// 设置按钮文本颜色
    QPalette palette = buttonOption.palette;
    palette.setColor(QPalette::ButtonText, QColor("#00C566")); // 将按钮文本颜色设置为红色
    buttonOption.palette = palette;

	QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);

	// 保存按钮矩形和索引以供事件处理
    const_cast<LiveMngrRemoteAuthDelegate*>(this)->buttonRects[index] = buttonRect;
}

bool LiveMngrRemoteAuthDelegate::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QTableView *view = qobject_cast<QTableView*>(parent());
        if (!view) return false;

        QPoint pos = mouseEvent->pos();
        QModelIndex index = view->indexAt(pos);

        if (buttonRects.contains(index)) {
            QRect buttonRect = buttonRects[index];
            if (buttonRect.contains(pos)) {
                // 点击了“复制”按钮
                QString password = index.model()->data(index, Qt::DisplayRole).toString();
                QApplication::clipboard()->setText(password);
                qDebug() << "Copied to clipboard:" << password;
                return true; // 事件已处理
            }
        }
    } 
    return QStyledItemDelegate::eventFilter(obj, event);
}
