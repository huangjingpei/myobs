#include "LiveMngrWidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QClipboard>
#include <QMouseEvent>
#include <QTextOption> //需要包含

LiveMngrWidget::LiveMngrWidget(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	m_tableView = new LiveMngrView(this);
	m_tableModel = new LiveMngrModel(this);

	//m_languageDelegate = new ComboBoxDelegate(this);
	//m_toneDelegate = new ComboBoxDelegate(this);
	//m_sliderDelegate = new SliderDelegate(this); 
	m_passwordDelegate = new LiveMngrRemoteAuthDelegate(this); 
	m_operation2Delegate = new LiveMngrOperationDelegate(this);

	m_tableView->setModel(m_tableModel);

	// 设置 ComboBox 代理
	//m_tableView->setItemDelegateForColumn(LiveMngrModel::Language, m_languageDelegate);
	//m_tableView->setItemDelegateForColumn(LiveMngrModel::Tone, m_toneDelegate);

	// 设置 Slider 代理
	//m_tableView->setItemDelegateForColumn(LiveMngrModel::Operation, m_sliderDelegate); // 设置操作列为 SliderDelegate

	// 设置 Password 代理
	m_tableView->setItemDelegateForColumn(LiveMngrModel::RemoteAuthInfo,
					      m_passwordDelegate); // 设置 "密码" 列为 PasswordDelegate

	// 设置 "操作2" 代理
	m_tableView->setItemDelegateForColumn(LiveMngrModel::LiveOperation, m_operation2Delegate);

	// 设置选择模式和行为：
	m_tableView->setSelectionMode(QAbstractItemView::NoSelection); // 单行选择
	// m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行

	// 设置自动换行
	m_tableView->wordWrap();

	// 设置表格样式 (移除网格线，并设置背景色)
	m_tableView->setStyleSheet("QTableView {"
				   "   background-color: #F7FAFF;"   /* 表格背景色 */
				   "   border: none;"                /* 移除边框 */
				   "   gridline-color: transparent;" /* 移除网格线 */
				   "}"
				   "QHeaderView::section {"
				   "   background-color: #E3ECFF;" /* 表头背景色 */
				   "   font-weight: bold;"
				   "   border: none;"                     /* 移除表头边框 */
				   "   qproperty-alignment: AlignCenter;" /*表头居中*/
				   "}"
				   "QTableView::item {"
				   "   border: none;" /* 移除单元格边框 */
				   "}"

	);

	// 可选: 设置水平滚动条策略
	m_tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	// 可选: 设置行和列自适应内容
	m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);        // 各列平均分配
	m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //各行自适应高度

	// 连接信号和槽
	connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
		&LiveMngrWidget::onSelectionChanged);

	layout->addWidget(m_tableView);
	setLayout(layout);

	connect(m_tableView, &LiveMngrView::mousePressEventOccurred, this,
		&LiveMngrWidget::onTableMousePressEvent); // 注意这里
	m_tableView->setMouseTracking(true);              // 开启鼠标追踪
}

LiveMngrWidget::~LiveMngrWidget()
{
	delete m_tableModel;
	//delete m_languageDelegate;
	//delete m_toneDelegate;
	//delete m_sliderDelegate;   // SliderDelegate
	delete m_passwordDelegate; // PasswordDelegate
	delete m_operation2Delegate;
}

void LiveMngrWidget::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	Q_UNUSED(deselected);

	if (!selected.indexes().isEmpty()) {
		int row = selected.indexes().first().row(); // 获取选中的行号

		// 获取该行的数据
		QVector<QVariant> rowData = m_tableModel->getRowData(row);

		// 使用获取到的数据...
		qDebug() << "选中行:" << row;
		qDebug() << "数据:" << rowData;
	}
}

void LiveMngrWidget::mousePressEvent(QMouseEvent *event) {}

void LiveMngrWidget::onTableMousePressEvent(QMouseEvent *event)
{
	// 获取点击的单元格索引
	QModelIndex index = m_tableView->indexAt(event->pos());
	// 检查是否点击了 "密码" 列
	if (index.isValid() && index.column() == LiveMngrModel::RemoteAuthInfo) {
		// 获取 "密码" 列的按钮矩形区域
		QStyleOptionViewItem option;
		QRect buttonRect;

		// 获取点击的单元格在 viewport 中的位置
		QRect cellRect = m_tableView->visualRect(index);

		// 计算按钮的矩形区域
		buttonRect = QRect(cellRect.right() - 50, cellRect.top(), 50, cellRect.height());

		// 检查是否点击了按钮
		if (buttonRect.contains(event->pos())) {
			// 获取密码
			QString password = index.model()->data(index, Qt::DisplayRole).toString();

			// 复制到剪贴板
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(password);
			qDebug() << "点击了密码的复制列";
		}
	}

	// 检查是否点击了 "操作2" 列
	if (index.isValid() && index.column() == LiveMngrModel::LiveOperation) {
		QRect cellRect = m_tableView->visualRect(index);
		//计算3个按钮的rect
		int buttonWidth = cellRect.width() / 3; // 计算每个按钮的宽度
		QRect button1Rect =
			QRect(cellRect.x() + (0 * buttonWidth), cellRect.y(), buttonWidth, cellRect.height());
		QRect button2Rect =
			QRect(cellRect.x() + (1 * buttonWidth), cellRect.y(), buttonWidth, cellRect.height());
		QRect button3Rect =
			QRect(cellRect.x() + (2 * buttonWidth), cellRect.y(), buttonWidth, cellRect.height());

		// 检查是否点击了按钮
		if (button1Rect.contains(event->pos())) {
			// 删除当前行，使用 m_tableView->model() 确保非 const
			int row = index.row();
			if (row >= 0) {
				QVector<QVariant> rowData = m_tableModel->getRowData(row);
				int liveAccountColumn = LiveMngrModel::LiveAccount;
				if (liveAccountColumn < rowData.size()) {
					QString id = rowData[liveAccountColumn].toString();
					if (!id.isEmpty()) {
						bool success = m_tableModel->removeRow(row, QModelIndex());
						if (success) {
							qDebug() << "操作2：成功删除行" << row;
							// 刷新视图
							m_tableView->viewport()->update();
							emit deleteLiveClient(id);
						} else {
							qDebug() << "操作2：删除行失败，行号" << row;
						}
				
					}
				}

			}
		} else if (button2Rect.contains(event->pos())) {
			int row = index.row();
			if (row >= 0) {
				QVector<QVariant> rowData = m_tableModel->getRowData(row);
				int liveAccountColumn = LiveMngrModel::LiveAccount;
				if (liveAccountColumn < rowData.size()) {
					QString id = rowData[liveAccountColumn].toString();
					if (!id.isEmpty()) {
						emit disconnectLiveClient(id);
					}
				}
			}
		} else if (button3Rect.contains(event->pos())) {
			int row = index.row();
			if (row >= 0) {
				QVector<QVariant> rowData = m_tableModel->getRowData(row);
				int liveAccountColumn = LiveMngrModel::RemoteAuthInfo;
				if ( liveAccountColumn < rowData.size()) {
					QString id = rowData[liveAccountColumn].toString();
					QStringList parts = id.split("/");
					if (!parts.isEmpty() && parts.size() > 1) {
						QString username = parts.at(0);
						QString password = parts.at(1);
						if ((!username.isEmpty()) && (!password.isEmpty())) {
							connectRemoteMachine(username, password);
						}
					}
				}
			}
		}
	}
}
void LiveMngrWidget::onTableClicked(const QModelIndex &index) {}

void LiveMngrWidget::addRow(const QStringList &rowData) {
	if (rowData.size() != m_tableModel->columnCount()) {
		qWarning("Row data does not match column count.");
		return;
	}
	int rowCount = m_tableModel->rowCount();
	m_tableModel->insertRows(rowCount, 1, QModelIndex(), rowData);

}

void LiveMngrWidget::clearRows() {
	m_tableModel->clearRows();
}
