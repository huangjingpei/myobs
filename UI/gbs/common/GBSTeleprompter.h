#ifndef __GBSTELEPROMPTER__
#define __GBSTELEPROMPTER__
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QScrollBar>
#include <QSlider>
#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QDebug>

class GBSTeleprompter : public QWidget {
	Q_OBJECT

public:
	GBSTeleprompter(QWidget *parent = nullptr) : QWidget(parent)
	{
		setWindowTitle("提词器");

		// 创建多行文本框
		textEdit = new QTextEdit(this);
		// textEdit->setPlainText("这里是提词器的文本内容。\n"
		//                        "你可以在这里输入多行文本。\n"
		//                        "当文本内容超过文本框高度时，会自动显示滚动条。\n"
		//                        "通过下方的滑块可以调整滚动速度。\n"
		//                        "点击“开始”按钮后，文本会自动滚动。\n"
		//                        "滚动到底部后，会从头开始循环播放。");

		// 记录原始字体颜色
		originalColor = textEdit->textColor(); // 保存默认颜色

		        textEdit->setStyleSheet("QTextEdit {"
					"   background-color: #F9F9F9;" // 文本框背景色
					"}"
					"QScrollBar:vertical {" // 垂直滚动条
					"   border: none;"
					"   background: #DEDEDE;" // 滚动条背景色
					"   width: 14px;"           // 滚动条宽度 (包含箭头)
					"   margin: 14px 0 14px 0;" // 上下箭头区域高度 (正方形边长)
					"}"
					"QScrollBar::handle:vertical {" // 垂直滚动条滑块
					"   background: #00C566;"       // 滑块颜色（绿色）
					"   min-height: 20px;"          // 滑块最小高度
					"}"
					"QScrollBar::add-line:vertical {" // 垂直滚动条下箭头 (隐藏)
					"   border: none;"
					"   background: none;"            // 隐藏
					"   height: 14px;"                // 箭头高度
					"   subcontrol-position: bottom;" // 箭头位于底部
					"   subcontrol-origin: margin;"
					"}"
					"QScrollBar::sub-line:vertical {" // 垂直滚动条上箭头 (隐藏)
					"   border: none;"
					"   background: none;"         // 隐藏
					"   height: 14px;"             // 箭头高度
					"   subcontrol-position: top;" // 箭头位于顶部
					"   subcontrol-origin: margin;"
					"}"
					"QScrollBar::up-arrow:vertical {" // 上端绿色正方形和白色三角形
					"   border: none;"
					"   background: #00C566;" // 绿色正方形
					"   width: 14px;"         // 正方形边长
					"   height: 14px;"        // 正方形边长
					"   subcontrol-position: top;"
					"   subcontrol-origin: content;"
					"   image: url(:/gbs/images/gbs/biz/gbs-green-scrollbar-uparrow.png);" // 设置图片
					"}"
					"QScrollBar::down-arrow:vertical {" // 下端绿色正方形和白色三角形
					"   border: none;"
					"   background: #00C566;" // 绿色正方形
					"   width: 14px;"         // 正方形边长
					"   height: 14px;"        // 正方形边长
					"   subcontrol-position: bottom;"
					"   subcontrol-origin: content;"
					"    image: url(:/gbs/images/gbs/biz/gbs-green-scrollbar-downarrow.png);" // 设置图片
					"}"
					"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {" // 垂直滚动条页
					"   background: none;" // 上下翻页时区域的颜色，这里设置成透明
					"}");

		// 创建滚动速度设置滑块
		// QLabel *speedLabel = new QLabel("滚动速度 (ms/步):", this);
		// speedSlider = new QSlider(Qt::Horizontal, this);
		// speedSlider->setRange(50, 500); // 50ms to 500ms per step
		// speedSlider->setValue(100);     // Default 100ms per step

		// 创建开始/停止按钮
		// QPushButton *startButton = new QPushButton("开始", this);
		// QPushButton *stopButton = new QPushButton("停止", this);

		// 创建布局
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->addWidget(textEdit);
		// layout->addWidget(speedLabel);
		// layout->addWidget(speedSlider);
		// layout->addWidget(startButton);
		// layout->addWidget(stopButton);
		setLayout(layout);

		// 创建定时器
		timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &GBSTeleprompter::scrollText);

		// 连接按钮信号
		// connect(startButton, &QPushButton::clicked, this, &GBSTeleprompter::startScrolling);
		// connect(stopButton, &QPushButton::clicked, this, &GBSTeleprompter::stopScrolling);

		// 连接滑块信号
		// connect(speedSlider, &QSlider::valueChanged, this, &GBSTeleprompter::updateTimerInterval);

		timerInterval = 500; // 初始化定时器间隔 ,单位ms
		currentLine = 0;     // 初始化当前行
		readLines.clear();   // 初始化已读行列表
	}

public:
	// 开始滚动
	void startScrolling()
	{
		currentLine = 0;              // 从第一行开始
		readLines.clear();            // 清空已读行列表
		updateCurrentLineHighlight(); // 高亮第一行
		centerCurrentLine();          // 确保第一行居中
		timer->start(timerInterval);
	}

	// 停止滚动
	void stopScrolling() { timer->stop(); }

	void updateTimerInterval(int value)
	{
		timerInterval = value;
		if (timer->isActive()) {
			timer->start(timerInterval); // 用新间隔重启定时器
		}
	}

	// 滚动文本
	void scrollText()
	{
		QTextDocument *doc = textEdit->document();
		int totalLines = doc->blockCount();

		// 记录当前行为已读
		if (currentLine >= 0 && currentLine < totalLines) {
			readLines.insert(currentLine); // 标记当前行已读
		}

		// 移动到下一行
		currentLine++;
		if (currentLine >= totalLines) {
			currentLine = 0;  // 循环回到开头
			resetTextColor(); // 循环到头部时恢复所有文本颜色
		}

		// 更新高亮并居中
		updateCurrentLineHighlight();
		centerCurrentLine();
	}

	void setScrollSpeed(int ms) { timerInterval = ms; }

	void setText(QString text)
	{
		textEdit->setPlainText(text);
		readLines.clear(); // 清除已读行记录
		currentLine = 0;   // 重置当前行
	}
	void setFont(QFont font) { textEdit->setFont(font);
	}

	QString text() { return textEdit->toPlainText(); }

private:
	void updateCurrentLineHighlight()
	{
		// 清除之前的高亮
		removeUnderlineFromAllLines();

		// 高亮当前行并调整字号
		QTextDocument *doc = textEdit->document();
		if (currentLine >= 0 && currentLine < doc->blockCount()) {
			QTextBlock block = doc->findBlockByNumber(currentLine);
			QTextCursor cursor(block);

			// 获取当前文本的字体
			QFont currentFont = textEdit->font();
			int currentSize = currentFont.pointSize(); // 获取当前字号
			if (currentSize <= 0) {                    // 如果字号未设置，默认值处理
				currentSize = 10;                  // 假设默认 10 号
			}

			// 设置格式：下划线并加大字号
			QTextCharFormat fmt;
			fmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
			fmt.setFontPointSize(currentSize + 6); // 字号加大 6 号

			// 应用格式到当前行
			cursor.select(QTextCursor::BlockUnderCursor);
			cursor.mergeCharFormat(fmt);
		}

		// 更新已读行的颜色
		updateReadLinesColor();
	}

	void centerCurrentLine()
	{
		QTextDocument *doc = textEdit->document();
		if (currentLine >= 0 && currentLine < doc->blockCount()) {
			// 使用 QTextCursor 将光标移到当前行
			QTextBlock block = doc->findBlockByNumber(currentLine);
			QTextCursor cursor(block);
			textEdit->setTextCursor(cursor);

			// 获取视口高度和行高
			int viewportHeight = textEdit->viewport()->height();
			QFontMetrics fontMetrics(textEdit->font());
			int lineHeight = fontMetrics.lineSpacing();

			// 计算需要滚动的偏移量，使当前行居中
			QScrollBar *scrollBar = textEdit->verticalScrollBar();
			int cursorY = textEdit->cursorRect(cursor).top(); // 当前光标在视口中的 Y 坐标
			int targetScroll = scrollBar->value() + cursorY - (viewportHeight / 2);

			// 确保滚动值在有效范围内
			if (targetScroll < 0) {
				targetScroll = 0;
			} else if (targetScroll > scrollBar->maximum()) {
				targetScroll = scrollBar->maximum();
			}

			scrollBar->setValue(targetScroll);
		}
	}

	void removeUnderlineFromAllLines()
	{
		QTextCursor cursor = textEdit->textCursor();
		cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		QTextCharFormat fmt;
		fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
		fmt.setFontPointSize(textEdit->font().pointSize()); // 恢复默认字号
		cursor.mergeCharFormat(fmt);
	}

	void updateReadLinesColor()
	{
		QTextDocument *doc = textEdit->document();
		QTextCursor cursor(doc);

		// 遍历所有行，设置已读行颜色
		for (int line = 0; line < doc->blockCount(); ++line) {
			if (readLines.contains(line)) {
				// 设置已读行颜色为 #EC6083
				QTextBlock block = doc->findBlockByNumber(line);
				QTextCursor lineCursor(block);
				QTextCharFormat fmt = lineCursor.blockCharFormat();
				fmt.setForeground(QColor("#EC6083")); // 设置文字颜色为粉红色
				lineCursor.select(QTextCursor::BlockUnderCursor);
				lineCursor.mergeCharFormat(fmt);
			}
		}
	}

	void resetTextColor()
	{
		QTextCursor cursor = textEdit->textCursor();
		cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		QTextCharFormat fmt;
		fmt.setForeground(originalColor);                   // 恢复原始颜色
		fmt.setFontPointSize(textEdit->font().pointSize()); // 保持默认字号
		cursor.mergeCharFormat(fmt);
		readLines.clear(); // 清空已读行记录
	}

private:
	QTextEdit *textEdit;
	// QSlider *speedSlider;
	QTimer *timer;
	int timerInterval;    // 定时器间隔
	int currentLine;      // 当前高亮行
	QColor originalColor; // 原始文本颜色
	QSet<int> readLines;  // 记录已读行（使用 QSet 避免重复）
};

#endif //__GBSTELEPROMPTER__
