#ifndef LiveMngrWidget_H
#define LiveMngrWidget_H

#include <QWidget>
#include <QTableView>
#include <QComboBox> // ComboBox
#include <QStyledItemDelegate> // Delegate
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include "LiveMngrModel.h"
#include "ComboBoxDelegate.h"
#include "SliderDelegate.h" // 包含 SliderDelegate
#include "LiveMngrOperationDelegate.h" // 包含 PasswordDelegate
#include "LiveMngrView.h" // 引入自定义 MyTableView
#include "LiveMngrRemoteAuthDelegate.h"

#include <QStringList>

class LiveMngrWidget : public QWidget
{
    Q_OBJECT

public:
    LiveMngrWidget(QWidget *parent = nullptr);
    ~LiveMngrWidget() override;

public:
    void addRow(const QStringList &rowData);
    void removeRow(int row);
    void clearRows();

    void setOperationMask(int mask);

private slots:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onTableMousePressEvent(QMouseEvent *event);
    void onTableClicked(const QModelIndex &index);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
	void deleteLiveClient(int row, QString id);
	void disconnectLiveClient(QString id);
	void connectRemoteMachine(QString username, QString password);


  
    

private:
    LiveMngrView *m_tableView; 
    LiveMngrModel *m_tableModel;
    LiveMngrRemoteAuthDelegate *m_passwordDelegate; // PasswordDelegate
    LiveMngrOperationDelegate *m_operation2Delegate;

    QPushButton *m_addRowButton;

};

#endif // LiveMngrWidget_H
