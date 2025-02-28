#include "LiveMngrOperationDelegate.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QTextEdit>
#include <QClipboard>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>

LiveMngrOperationDelegate::LiveMngrOperationDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void LiveMngrOperationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton buttonOption;

    // 获取单元格的矩形区域
    QRect cellRect = option.rect;

    // 计算按钮宽度
    int buttonWidth = cellRect.width() / 3;

    // 计算按钮的矩形区域
    QRect button1Rect = QRect(cellRect.x() + 0 * buttonWidth, cellRect.y(), buttonWidth, cellRect.height());
    QRect button2Rect = QRect(cellRect.x() + 1 * buttonWidth, cellRect.y(), buttonWidth, cellRect.height());
    QRect button3Rect = QRect(cellRect.x() + 2 * buttonWidth, cellRect.y(), buttonWidth, cellRect.height());

    int delEnable = m_operationMask & 4;
    int recordEnable = m_operationMask & 2;
    int shareEnable = m_operationMask & 1;
    // 加载图片资源
    QPixmap delPixmap(":/gbs/images/gbs/biz/gbs-trash-20px.png");    //替换为自己的图片

    // 绘制按钮1
    if (!delPixmap.isNull()) {
	    painter->drawPixmap(button1Rect.center() - QPoint(delPixmap.width() / 2, delPixmap.height() / 2),
				delPixmap);
    }

    // 绘制按钮2
    if (!recordEnable) {
	    QPixmap recordPixmap(":/gbs/images/gbs/biz/gbs-grey-disable-video.png"); //替换为自己的图片
	    if (!recordPixmap.isNull()) {
		    painter->drawPixmap(button2Rect.center() -
						QPoint(recordPixmap.width() / 2, recordPixmap.height() / 2),
					recordPixmap);
	    }
    } else {
	    QPixmap recordPixmap(":/gbs/images/gbs/biz/gbs-disable-video.png"); //替换为自己的图片
	    if (!recordPixmap.isNull()) {
		    painter->drawPixmap(button2Rect.center() -
						QPoint(recordPixmap.width() / 2, recordPixmap.height() / 2),
					recordPixmap);
	    }

    }

    // 绘制按钮3
    if (!shareEnable) {
	    QPixmap sharePixmap(":/gbs/images/gbs/biz/gbs-grey-remote-share.png"); //替换为自己的图片
	    if (!sharePixmap.isNull()) {
		    painter->drawPixmap(button3Rect.center() -
						QPoint(sharePixmap.width() / 2, sharePixmap.height() / 2),
					sharePixmap);
	    }
    } else {
	    QPixmap sharePixmap(":/gbs/images/gbs/biz/gbs-remote-share.png"); //替换为自己的图片
	    if (!sharePixmap.isNull()) {
		    painter->drawPixmap(button3Rect.center() -
						QPoint(sharePixmap.width() / 2, sharePixmap.height() / 2),
					sharePixmap);
	    }
    }

}


QRect LiveMngrOperationDelegate::buttonRect(const QRect &cellRect, int buttonIndex) const
{
    int buttonWidth = cellRect.width() / 3;
    int buttonX = cellRect.x() + buttonWidth * buttonIndex;
    return QRect(buttonX, cellRect.y(), buttonWidth, cellRect.height());
}

void LiveMngrOperationDelegate::setOperationMask(int mask)
{
    m_operationMask = mask;
}
