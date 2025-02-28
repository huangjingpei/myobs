#ifndef LiveMngrOperationDelegate_H
#define LiveMngrOperationDelegate_H


#include <QStyledItemDelegate>
#include <QPushButton>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QHBoxLayout>
#include <QWidget>
#include <QPainter>

class LiveMngrOperationDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    LiveMngrOperationDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setOperationMask(int mask);

signals:
    void buttonClicked(int row, int buttonIndex);

private:
    QRect buttonRect(const QRect &cellRect, int buttonIndex) const;
	std::atomic<int> m_operationMask{7};
};

#endif // LiveMngrOperationDelegate_H
