#include "gamewindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>
#include <QScrollArea>
#include <QFrame>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>
#include <QDir>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent), m_gameWidget(nullptr),
      m_gameRunning(false), m_gamePaused(false),
      m_totalGameTime(0), m_sessionTime(0), m_currentCoins(1000),
      m_currentScore(0), m_currentFishCaught(0), m_totalFishCaught(0),
      m_shotsFired(0), m_highScore(0), m_gamesPlayed(0),
      m_frameCount(0), m_currentFPS(0),
      m_currentUser("sdkadjad"), m_sessionStartTime(QDateTime::currentDateTime())
{
    setWindowTitle("🐟 捕鱼大战 - Fishing War Game v1.0");
    setWindowIcon(QIcon(":/images/icon.png"));
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // 设置应用程序样式
    setStyleSheet(
        "QMainWindow { background-color: #2b2b2b; }"
        "QGroupBox { font-weight: bold; border: 2px solid #555; "
        "border-radius: 8px; margin: 6px; padding-top: 12px; "
        "background-color: #3b3b3b; color: white; } "
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; "
        "padding: 0 6px 0 6px; color: #FFD700; font-size: 12px; }"
        "QPushButton { background-color: #4a4a4a; color: white; "
        "border: 1px solid #666; border-radius: 4px; padding: 6px; "
        "font-weight: bold; } "
        "QPushButton:hover { background-color: #5a5a5a; } "
        "QPushButton:pressed { background-color: #3a3a3a; } "
        "QLabel { color: white; } "
        "QTextEdit { background-color: #1e1e1e; color: #ffffff; "
        "border: 1px solid #555; } "
        "QProgressBar { border: 1px solid #555; border-radius: 3px; "
        "background-color: #2b2b2b; } "
        "QProgressBar::chunk { background-color: #4CAF50; border-radius: 2px; }"
    );

    setupUI();
    setupMenuBar();
    setupStatusBar();
    connectSignals();
    loadSettings();

    // 游戏时间定时器
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameWindow::onGameTimerUpdate);
    m_gameTimer->start(1000); // 每秒更新

    // FPS定时器
    m_fpsTimer = new QTimer(this);
    connect(m_fpsTimer, &QTimer::timeout, this, &GameWindow::updateFPS);
    m_fpsTimer->start(1000); // 每秒更新FPS

    // 启动会话计时
    m_sessionTimeElapsed.start();

    // 初始化界面
    updateCannonInfo();
    updateGameStats();
    updateWindowTitle();

    // 欢迎信息
    m_gameLog->append("=== 🎮 欢迎来到捕鱼大战！===");
    m_gameLog->append(QString("🔧 游戏版本: v1.0 | 用户: %1").arg(m_currentUser));
    m_gameLog->append(QString("📅 当前时间: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    m_gameLog->append(QString("🚀 会话开始: %1").arg(m_sessionStartTime.toString("hh:mm:ss")));
    m_gameLog->append("=== 📖 操作说明 ===");
    m_gameLog->append("🖱 鼠标移动：瞄准目标");
    m_gameLog->append("🖱 鼠标左键：发射炮弹");
    m_gameLog->append("⌨ 空格键：开始/暂停游戏");
    m_gameLog->append("⌨ +/- 键：升级/降级炮塔");
    m_gameLog->append("⌨ R 键：重置游戏");
    m_gameLog->append("=== 🎯 游戏目标 ===");
    m_gameLog->append("🐟 射击各种鱼类获得金币和分数");
    m_gameLog->append("💰 使用金币升级炮塔增强威力");
    m_gameLog->append("🏆 挑战更高分数创造新记录");
    m_gameLog->append("🎊 触发特殊事件获得额外奖励");
    if (m_highScore > 0) {
        m_gameLog->append(QString("🥇 当前最高分：%1").arg(m_highScore));
    }
    m_gameLog->append("✨ 准备好了吗？按空格键开始！");
}

GameWindow::~GameWindow()
{
    saveSettings();
}

void GameWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(2);
    m_mainLayout->setContentsMargins(2, 2, 2, 2);

    setupGamePanel();
    setupControlPanel();

    m_mainLayout->addWidget(m_gameWidget, 1);
    m_mainLayout->addWidget(m_controlPanel, 0);

    // 确保控制面板不会被压缩
    m_controlPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_gameWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GameWindow::setupGamePanel()
{
    m_gameWidget = new GameWidget(this);
    m_gameWidget->setFocusPolicy(Qt::StrongFocus);

    // 为游戏区域添加边框
    m_gameWidget->setStyleSheet(
        "GameWidget { border: 2px solid #FFD700; border-radius: 5px; }"
    );
}

void GameWindow::setupControlPanel()
{
    m_controlPanel = new QWidget;
    m_controlPanel->setFixedWidth(CONTROL_PANEL_WIDTH);
    m_controlPanel->setMinimumHeight(WINDOW_HEIGHT);

    // 创建滚动区域
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFixedWidth(CONTROL_PANEL_WIDTH);

    // 创建滚动区域的内容容器
    QWidget* scrollContent = new QWidget;
    scrollContent->setMinimumWidth(CONTROL_PANEL_WIDTH - 20); // 减去滚动条宽度

    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(6);
    scrollLayout->setContentsMargins(5, 5, 5, 5);

    // 设置界面组件
    setupInfoPanel();

    // 添加所有组件到滚动布局
    scrollLayout->addWidget(m_gameControlGroup);
    scrollLayout->addWidget(m_cannonControlGroup);
    scrollLayout->addWidget(m_gameStatsGroup);
    scrollLayout->addWidget(m_gameInfoGroup);
    scrollLayout->addStretch(); // 添加弹性空间

    // 设置滚动区域内容
    m_scrollArea->setWidget(scrollContent);

    // 控制面板的主布局
    QVBoxLayout* controlLayout = new QVBoxLayout(m_controlPanel);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->addWidget(m_scrollArea);

    // 设置滚动条样式
    m_scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background-color: #2b2b2b; border: 1px solid #555; "
        "border-radius: 6px; width: 12px; }"
        "QScrollBar::handle:vertical { background-color: #4a4a4a; border-radius: 5px; "
        "min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background-color: #5a5a5a; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "background: none; border: none; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { "
        "background: none; }"
    );
}

void GameWindow::setupInfoPanel()
{
    // 游戏控制组
    m_gameControlGroup = new QGroupBox("🎮 游戏控制");
    m_gameControlGroup->setMaximumHeight(180);
    QVBoxLayout* gameControlLayout = new QVBoxLayout(m_gameControlGroup);
    gameControlLayout->setSpacing(4);

    m_startPauseButton = new QPushButton("▶ 开始游戏");
    m_startPauseButton->setFixedHeight(35);
    m_startPauseButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; "
        "font-weight: bold; padding: 8px; border-radius: 6px; font-size: 13px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }"
    );

    m_resetButton = new QPushButton("🔄 重置游戏");
    m_resetButton->setFixedHeight(28);
    m_resetButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; "
        "font-weight: bold; padding: 5px; border-radius: 4px; font-size: 11px; }"
        "QPushButton:hover { background-color: #da190b; }"
        "QPushButton:pressed { background-color: #b71c1c; }"
    );

    m_settingsButton = new QPushButton("⚙ 设置");
    m_settingsButton->setFixedHeight(28);
    m_settingsButton->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; "
        "font-weight: bold; padding: 5px; border-radius: 4px; font-size: 11px; }"
        "QPushButton:hover { background-color: #1976D2; }"
        "QPushButton:pressed { background-color: #0D47A1; }"
    );

    m_helpButton = new QPushButton("❓ 帮助");
    m_helpButton->setFixedHeight(28);
    m_helpButton->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: white; "
        "font-weight: bold; padding: 5px; border-radius: 4px; font-size: 11px; }"
        "QPushButton:hover { background-color: #F57C00; }"
        "QPushButton:pressed { background-color: #E65100; }"
    );

    m_exitButton = new QPushButton("❌ 退出");
    m_exitButton->setFixedHeight(28);
    m_exitButton->setStyleSheet(
        "QPushButton { background-color: #795548; color: white; "
        "font-weight: bold; padding: 5px; border-radius: 4px; font-size: 11px; }"
        "QPushButton:hover { background-color: #5D4037; }"
        "QPushButton:pressed { background-color: #3E2723; }"
    );

    gameControlLayout->addWidget(m_startPauseButton);
    gameControlLayout->addWidget(m_resetButton);
    gameControlLayout->addWidget(m_settingsButton);
    gameControlLayout->addWidget(m_helpButton);
    gameControlLayout->addWidget(m_exitButton);

    // 炮塔控制组
    m_cannonControlGroup = new QGroupBox("🚀 炮塔控制");
    m_cannonControlGroup->setMaximumHeight(160);
    QVBoxLayout* cannonControlLayout = new QVBoxLayout(m_cannonControlGroup);
    cannonControlLayout->setSpacing(3);

    // 炮塔信息
    m_cannonTypeLabel = new QLabel("炮塔类型: 基础炮塔");
    m_cannonTypeLabel->setStyleSheet("color: #FFD700; font-weight: bold; font-size: 11px;");

    m_cannonDamageLabel = new QLabel("攻击力: 10");
    m_cannonDamageLabel->setStyleSheet("font-size: 10px;");

    m_cannonFireRateLabel = new QLabel("射击速度: 500ms");
    m_cannonFireRateLabel->setStyleSheet("font-size: 10px;");

    m_upgradeCostLabel = new QLabel("升级费用: 100金币");
    m_upgradeCostLabel->setStyleSheet("font-size: 10px;");

    cannonControlLayout->addWidget(m_cannonTypeLabel);
    cannonControlLayout->addWidget(m_cannonDamageLabel);
    cannonControlLayout->addWidget(m_cannonFireRateLabel);
    cannonControlLayout->addWidget(m_upgradeCostLabel);

    // 升级/降级按钮
    QHBoxLayout* cannonButtonLayout = new QHBoxLayout;
    m_upgradeCannonButton = new QPushButton("⬆ 升级");
    m_upgradeCannonButton->setFixedHeight(28);
    m_upgradeCannonButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; "
        "font-weight: bold; padding: 5px; border-radius: 4px; font-size: 10px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #666; color: #999; }"
    );

    m_downgradeCannonButton = new QPushButton("⬇ 降级");
    m_downgradeCannonButton->setFixedHeight(28);
    m_downgradeCannonButton->setStyleSheet(
        "QPushButton { background-color: #FF5722; color: white; "
        "font-weight: bold; padding: 5px; border-radius: 4px; font-size: 10px; }"
        "QPushButton:hover { background-color: #E64A19; }"
        "QPushButton:disabled { background-color: #666; color: #999; }"
    );

    cannonButtonLayout->addWidget(m_upgradeCannonButton);
    cannonButtonLayout->addWidget(m_downgradeCannonButton);
    cannonControlLayout->addLayout(cannonButtonLayout);

    // 游戏统计组
    m_gameStatsGroup = new QGroupBox("📊 游戏统计");
    m_gameStatsGroup->setMaximumHeight(200);
    QVBoxLayout* statsLayout = new QVBoxLayout(m_gameStatsGroup);
    statsLayout->setSpacing(2);

    m_coinsLabel = new QLabel("💰 金币: 1000");
    m_coinsLabel->setStyleSheet("QLabel { color: #FFD700; font-weight: bold; font-size: 12px; }");

    m_scoreLabel = new QLabel("🏆 分数: 0");
    m_scoreLabel->setStyleSheet("QLabel { color: #00FF00; font-weight: bold; font-size: 12px; }");

    m_fishCountLabel = new QLabel("🐟 捕获: 0 条鱼");
    m_fishCountLabel->setStyleSheet("QLabel { color: #87CEEB; font-weight: bold; font-size: 11px; }");

    m_gameTimeLabel = new QLabel("⏰ 游戏时间: 00:00");
    m_gameTimeLabel->setStyleSheet("font-size: 10px;");

    m_highScoreLabel = new QLabel("🥇 最高分: 0");
    m_highScoreLabel->setStyleSheet("QLabel { color: #FF6B35; font-weight: bold; font-size: 10px; }");

    m_sessionStatsLabel = new QLabel("📈 本次会话: 0 局");
    m_sessionStatsLabel->setStyleSheet("font-size: 9px;");

    m_experienceBar = new QProgressBar();
    m_experienceBar->setRange(0, 1000);
    m_experienceBar->setValue(0);
    m_experienceBar->setFormat("等级进度: %v/%m");
    m_experienceBar->setFixedHeight(18);
    m_experienceBar->setStyleSheet(
        "QProgressBar { border: 1px solid #555; border-radius: 3px; "
        "text-align: center; background-color: #2b2b2b; font-size: 9px; }"
        "QProgressBar::chunk { background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, "
        "stop: 0 #4CAF50, stop: 1 #8BC34A); border-radius: 2px; }"
    );

    m_fpsLabel = new QLabel("📊 FPS: 60");
    m_fpsLabel->setStyleSheet("QLabel { color: #808080; font-size: 9px; }");

    statsLayout->addWidget(m_coinsLabel);
    statsLayout->addWidget(m_scoreLabel);
    statsLayout->addWidget(m_fishCountLabel);
    statsLayout->addWidget(m_gameTimeLabel);
    statsLayout->addWidget(m_highScoreLabel);
    statsLayout->addWidget(m_sessionStatsLabel);
    QLabel* expLabel = new QLabel("🎯 等级进度:");
    expLabel->setStyleSheet("font-size: 9px;");
    statsLayout->addWidget(expLabel);
    statsLayout->addWidget(m_experienceBar);
    statsLayout->addWidget(m_fpsLabel);

    // 游戏信息组
    m_gameInfoGroup = new QGroupBox("📋 游戏日志");
    m_gameInfoGroup->setMinimumHeight(280);
    QVBoxLayout* infoLayout = new QVBoxLayout(m_gameInfoGroup);
    infoLayout->setSpacing(4);

    m_gameLog = new QTextEdit();
    m_gameLog->setMinimumHeight(220);
    m_gameLog->setMaximumHeight(220);
    m_gameLog->setReadOnly(true);
    m_gameLog->setStyleSheet(
        "QTextEdit { background-color: #1e1e1e; color: #ffffff; "
        "font-family: 'Consolas', 'Monaco', monospace; font-size: 9px; "
        "border: 1px solid #555; border-radius: 4px; }"
    );

    // 用户信息标签
    m_userInfoLabel = new QLabel(QString("👤 用户: %1").arg(m_currentUser));
    m_userInfoLabel->setStyleSheet("QLabel { color: #87CEEB; font-size: 9px; font-weight: bold; }");

    m_versionLabel = new QLabel(QString("🔧 v1.0 | 构建: 2025-05-27 | 会话: %1")
                               .arg(m_sessionStartTime.toString("hh:mm")));
    m_versionLabel->setStyleSheet("QLabel { color: #888; font-size: 8px; }");

    infoLayout->addWidget(m_gameLog);
    infoLayout->addWidget(m_userInfoLabel);
    infoLayout->addWidget(m_versionLabel);
}

void GameWindow::setupMenuBar()
{
    QMenuBar* menuBar = this->menuBar();
    menuBar->setStyleSheet(
        "QMenuBar { background-color: #3b3b3b; color: white; border-bottom: 1px solid #555; }"
        "QMenuBar::item { padding: 6px 10px; }"
        "QMenuBar::item:selected { background-color: #4a4a4a; }"
        "QMenu { background-color: #3b3b3b; color: white; border: 1px solid #555; }"
        "QMenu::item { padding: 6px 20px; }"
        "QMenu::item:selected { background-color: #4a4a4a; }"
    );

    // 游戏菜单
    QMenu* gameMenu = menuBar->addMenu("🎮 游戏(&G)");

    QAction* startAction = gameMenu->addAction("▶ 开始/暂停 (&S)");
    startAction->setShortcut(QKeySequence(Qt::Key_Space));
    startAction->setStatusTip("开始或暂停游戏");
    connect(startAction, &QAction::triggered, this, &GameWindow::onGameStartPause);

    QAction* resetAction = gameMenu->addAction("🔄 重置游戏 (&R)");
    resetAction->setShortcut(QKeySequence(Qt::Key_R));
    resetAction->setStatusTip("重置当前游戏");
    connect(resetAction, &QAction::triggered, this, &GameWindow::onGameReset);

    gameMenu->addSeparator();

    QAction* exitAction = gameMenu->addAction("❌ 退出 (&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("退出游戏");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 炮塔菜单
    QMenu* cannonMenu = menuBar->addMenu("🚀 炮塔(&C)");

    QAction* upgradeAction = cannonMenu->addAction("⬆ 升级炮塔 (&U)");
    upgradeAction->setShortcut(QKeySequence(Qt::Key_Plus));
    upgradeAction->setStatusTip("升级炮塔");
    connect(upgradeAction, &QAction::triggered, [this]() {
        if (m_gameWidget) m_gameWidget->upgradeCannonPublic();
    });

    QAction* downgradeAction = cannonMenu->addAction("⬇ 降级炮塔 (&D)");
    downgradeAction->setShortcut(QKeySequence(Qt::Key_Minus));
    downgradeAction->setStatusTip("降级炮塔");
    connect(downgradeAction, &QAction::triggered, [this]() {
        if (m_gameWidget) m_gameWidget->downgradeCannonPublic();
    });

    // 设置菜单
    QMenu* settingsMenu = menuBar->addMenu("⚙ 设置(&S)");

    QAction* settingsAction = settingsMenu->addAction("🔧 游戏设置 (&P)");
    settingsAction->setStatusTip("打开游戏设置");
    connect(settingsAction, &QAction::triggered, this, &GameWindow::onGameSettingsChanged);

    settingsMenu->addSeparator();

    QAction* saveAction = settingsMenu->addAction("💾 保存设置");
    saveAction->setStatusTip("保存当前设置");
    connect(saveAction, &QAction::triggered, this, &GameWindow::saveSettings);

    // 帮助菜单
    QMenu* helpMenu = menuBar->addMenu("❓ 帮助(&H)");

    QAction* helpAction = helpMenu->addAction("📖 游戏帮助 (&H)");
    helpAction->setShortcut(QKeySequence::HelpContents);
    helpAction->setStatusTip("查看游戏帮助");
    connect(helpAction, &QAction::triggered, this, &GameWindow::showHelp);

    helpMenu->addSeparator();

    QAction* aboutAction = helpMenu->addAction("ℹ 关于游戏 (&A)");
    aboutAction->setStatusTip("关于捕鱼大战");
    connect(aboutAction, &QAction::triggered, this, &GameWindow::showAbout);
}

void GameWindow::setupStatusBar()
{
    QStatusBar* statusBar = this->statusBar();
    statusBar->setStyleSheet(
        "QStatusBar { background-color: #3b3b3b; color: white; border-top: 1px solid #555; }"
    );

    m_statusLabel = new QLabel("🟢 就绪");
    m_mousePosLabel = new QLabel("🖱 鼠标位置: (0, 0)");
    m_timeLabel = new QLabel();

    statusBar->addWidget(m_statusLabel, 1);
    statusBar->addPermanentWidget(m_mousePosLabel);
    statusBar->addPermanentWidget(m_timeLabel);

    // 更新时间显示
    QTimer* timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, [this]() {
        m_timeLabel->setText("🕐 " + QDateTime::currentDateTime().toString("hh:mm:ss"));
    });
    timeTimer->start(1000);
    m_timeLabel->setText("🕐 " + QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void GameWindow::connectSignals()
{
    // 游戏控制按钮
    connect(m_startPauseButton, &QPushButton::clicked, this, &GameWindow::onGameStartPause);
    connect(m_resetButton, &QPushButton::clicked, this, &GameWindow::onGameReset);
    connect(m_settingsButton, &QPushButton::clicked, this, &GameWindow::onGameSettingsChanged);
    connect(m_helpButton, &QPushButton::clicked, this, &GameWindow::showHelp);
    connect(m_exitButton, &QPushButton::clicked, this, &QWidget::close);

    // 炮塔控制按钮
    connect(m_upgradeCannonButton, &QPushButton::clicked, [this]() {
        if (m_gameWidget) {
            m_gameWidget->upgradeCannonPublic();
        }
    });

    connect(m_downgradeCannonButton, &QPushButton::clicked, [this]() {
        if (m_gameWidget) {
            m_gameWidget->downgradeCannonPublic();
        }
    });

    // 游戏数据更新
    if (m_gameWidget) {
        connect(m_gameWidget, &GameWidget::coinsChanged, this, &GameWindow::onCoinsChanged);
        connect(m_gameWidget, &GameWidget::scoreChanged, this, &GameWindow::onScoreChanged);
        connect(m_gameWidget, &GameWidget::fishCaughtChanged, this, &GameWindow::onFishCaughtChanged);
        connect(m_gameWidget, &GameWidget::cannonTypeChanged, this, &GameWindow::onCannonTypeChanged);
        connect(m_gameWidget, &GameWidget::mouseMoved, this, &GameWindow::onMouseMoved);
        connect(m_gameWidget, &GameWidget::specialEventTriggered, this, &GameWindow::onSpecialEventTriggered);
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    // 全局快捷键处理
    switch (event->key()) {
    case Qt::Key_Space:
        onGameStartPause();
        break;
    case Qt::Key_R:
        if (event->modifiers() & Qt::ControlModifier) {
            onGameReset();
        }
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        if (m_gameWidget) {
            m_gameWidget->upgradeCannonPublic();
        }
        break;
    case Qt::Key_Minus:
    case Qt::Key_Underscore:
        if (m_gameWidget) {
            m_gameWidget->downgradeCannonPublic();
        }
        break;
    case Qt::Key_F1:
        showHelp();
        break;
    case Qt::Key_Escape:
        if (m_gameRunning && !m_gamePaused) {
            onGameStartPause(); // 暂停游戏
        }
        break;
    default:
        // 将其他按键事件转发给游戏组件
        if (m_gameWidget && m_gameWidget->hasFocus()) {
            QApplication::sendEvent(m_gameWidget, event);
        }
        break;
    }

    QMainWindow::keyPressEvent(event);
}

void GameWindow::closeEvent(QCloseEvent *event)
{
    QString exitMessage = QString("确定要退出捕鱼大战吗？\n\n"
                                 "• 当前游戏进度将会丢失\n"
                                 "• 统计数据和设置会自动保存\n"
                                 "• 最高分记录会被保留\n\n"
                                 "用户: %1\n"
                                 "会话时长: %2 分钟")
                                .arg(m_currentUser)
                                .arg(m_sessionTimeElapsed.elapsed() / 60000);

    int ret = QMessageBox::question(this, "🚪 确认退出",
                                   exitMessage,
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // 保存游戏统计
        QSettings settings;
        settings.setValue("stats/totalFishCaught", m_totalFishCaught + m_currentFishCaught);
        settings.setValue("stats/gamesPlayed", m_gamesPlayed + 1);
        settings.setValue("stats/totalPlayTime", m_sessionTimeElapsed.elapsed() / 1000);
        settings.setValue("user/lastUser", m_currentUser);
        settings.setValue("user/lastSessionTime", m_sessionTimeElapsed.elapsed() / 1000);

        saveSettings();

        m_gameLog->append(QString("[%1] 👋 用户 %2 退出游戏，感谢游玩！")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(m_currentUser));

        event->accept();
    } else {
        event->ignore();
    }
}

void GameWindow::onGameStartPause()
{
    if (!m_gameRunning) {
        // 开始游戏
        if (m_gameWidget) {
            m_gameWidget->startGame();
        }
        m_gameRunning = true;
        m_gamePaused = false;
        m_startPauseButton->setText("⏸ 暂停游戏");
        m_startPauseButton->setStyleSheet(
            "QPushButton { background-color: #FF9800; color: white; "
            "font-weight: bold; padding: 8px; border-radius: 6px; font-size: 13px; }"
            "QPushButton:hover { background-color: #F57C00; }"
            "QPushButton:pressed { background-color: #E65100; }"
        );
        m_statusLabel->setText("🎮 游戏进行中");
        m_gameTimeElapsed.start();

        m_gameLog->append(QString("[%1] 🚀 用户 %2 开始游戏！祝您好运！")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(m_currentUser));
    } else {
        // 暂停/继续游戏
        if (m_gameWidget) {
            m_gameWidget->pauseGame();
        }
        m_gamePaused = !m_gamePaused;

        if (m_gamePaused) {
            m_startPauseButton->setText("▶ 继续游戏");
            m_startPauseButton->setStyleSheet(
                "QPushButton { background-color: #4CAF50; color: white; "
                "font-weight: bold; padding: 8px; border-radius: 6px; font-size: 13px; }"
                "QPushButton:hover { background-color: #45a049; }"
                "QPushButton:pressed { background-color: #3d8b40; }"
            );
            m_statusLabel->setText("⏸ 游戏暂停");
            m_gameLog->append(QString("[%1] ⏸ 游戏暂停")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        } else {
            m_startPauseButton->setText("⏸ 暂停游戏");
            m_startPauseButton->setStyleSheet(
                "QPushButton { background-color: #FF9800; color: white; "
                "font-weight: bold; padding: 8px; border-radius: 6px; font-size: 13px; }"
                "QPushButton:hover { background-color: #F57C00; }"
                "QPushButton:pressed { background-color: #E65100; }"
            );
            m_statusLabel->setText("🎮 游戏进行中");
            m_gameLog->append(QString("[%1] ▶ 游戏继续")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
        }
    }

    updateWindowTitle();
}

void GameWindow::onGameReset()
{
    QString progressInfo = QString("当前进度将会丢失：\n"
                                   "• 分数：%1\n"
                                   "• 金币：%2\n"
                                   "• 捕获鱼类：%3 条\n"
                                   "• 游戏时长：%4 秒")
                          .arg(m_currentScore)
                          .arg(m_currentCoins)
                          .arg(m_currentFishCaught)
                          .arg(m_totalGameTime);

    QString fullMessage = QString("确定要重置游戏吗？\n\n%1").arg(progressInfo);

    int ret = QMessageBox::question(this, "🔄 确认重置",
                                   fullMessage,
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (m_gameWidget) {
            m_gameWidget->resetGame();
        }

        m_gameRunning = false;
        m_gamePaused = false;
        m_totalGameTime = 0;
        m_gamesPlayed++;

        m_startPauseButton->setText("▶ 开始游戏");
        m_startPauseButton->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: white; "
            "font-weight: bold; padding: 8px; border-radius: 6px; font-size: 13px; }"
            "QPushButton:hover { background-color: #45a049; }"
            "QPushButton:pressed { background-color: #3d8b40; }"
        );
        m_statusLabel->setText("🟢 就绪");

        updateGameStats();
        updateCannonInfo();
        updateWindowTitle();

        m_gameLog->append("=== 🔄 游戏已重置 ===");
        m_gameLog->append(QString("用户 %1 重置了游戏，准备新的挑战！").arg(m_currentUser));
        m_gameLog->append("按空格键开始新游戏！");
    }
}

void GameWindow::onMouseMoved(const QPoint& position)
{
    m_mousePosLabel->setText(QString("🖱 鼠标位置: (%1, %2)")
                           .arg(position.x()).arg(position.y()));
}

void GameWindow::onCoinsChanged(int coins)
{
    m_currentCoins = coins;
    m_coinsLabel->setText(QString("💰 金币: %1").arg(coins));

    updateCannonInfo();

    // 低金币警告
    if (coins < 10 && coins > 0) {
        m_gameLog->append(QString("[%1] ⚠ 金币不足！当前仅剩 %2 枚金币")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(coins));
    }
}

void GameWindow::onScoreChanged(int score)
{
    m_currentScore = score;
    m_scoreLabel->setText(QString("🏆 分数: %1").arg(score));

    // 更新最高分
    if (score > m_highScore) {
        m_highScore = score;
        m_highScoreLabel->setText(QString("🥇 最高分: %1").arg(m_highScore));
        m_gameLog->append(QString("[%1] 🎉 用户 %2 创造新记录！最高分更新为 %3!")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(m_currentUser)
                         .arg(m_highScore));
    }

    // 更新经验条
    int experience = score % 1000;
    m_experienceBar->setValue(experience);

    // 检查里程碑成就
    static int lastMilestone = 0;
    int currentMilestone = score / 1000;
    if (currentMilestone > lastMilestone) {
        lastMilestone = currentMilestone;
        m_gameLog->append(QString("[%1] 🎖 里程碑达成！分数突破 %2 分！")
                         .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                         .arg(currentMilestone * 1000));
    }

    updateWindowTitle();
}

void GameWindow::onFishCaughtChanged(int fishCaught)
{
    m_currentFishCaught = fishCaught;
    m_fishCountLabel->setText(QString("🐟 捕获: %1 条鱼").arg(fishCaught));

    // 捕鱼成就检查
    if (fishCaught > 0) {
        if (fishCaught == 1) {
            m_gameLog->append(QString("[%1] 🎣 用户 %2 首次捕获！开始您的捕鱼之旅！")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(m_currentUser));
        } else if (fishCaught % 10 == 0) {
            m_gameLog->append(QString("[%1] 🏅 捕鱼能手！已捕获 %2 条鱼！")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(fishCaught));
        }
    }
}

void GameWindow::onCannonTypeChanged(Cannon::CannonType type)
{
    updateCannonInfo();

    QString typeName;
    QString emoji;
    switch (type) {
    case Cannon::BASIC_CANNON:
        typeName = "基础炮塔";
        emoji = "🔫";
        break;
    case Cannon::SILVER_CANNON:
        typeName = "银质炮塔";
        emoji = "🔫";
        break;
    case Cannon::GOLD_CANNON:
        typeName = "金质炮塔";
        emoji = "💫";
        break;
    case Cannon::LASER_CANNON:
        typeName = "激光炮塔";
        emoji = "⚡";
        break;
    }

    m_gameLog->append(QString("[%1] %2 炮塔升级！用户 %3 当前使用: %4")
                     .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                     .arg(emoji)
                     .arg(m_currentUser)
                     .arg(typeName));
}

void GameWindow::onGameSettingsChanged()
{
    // 创建设置对话框
    QMessageBox settingsBox(this);
    settingsBox.setWindowTitle("⚙ 游戏设置");
    settingsBox.setIcon(QMessageBox::Information);
    settingsBox.setText("🔧 游戏设置功能");
    settingsBox.setInformativeText(
        QString("当前版本的设置选项：\n\n"
                "• 🎵 音效设置（开发中）\n"
                "• 🎨 主题设置（开发中）\n"
                "• 🎮 控制设置（开发中）\n"
                "• 💾 数据备份（开发中）\n\n"
                "当前用户：%1\n"
                "会话时长：%2 分钟\n\n"
                "更多功能正在开发中，敬请期待下个版本！")
                .arg(m_currentUser)
                .arg(m_sessionTimeElapsed.elapsed() / 60000)
    );
    settingsBox.setStandardButtons(QMessageBox::Ok);
    settingsBox.setStyleSheet(
        "QMessageBox { background-color: #3b3b3b; color: white; }"
        "QMessageBox QLabel { color: white; }"
        "QPushButton { background-color: #4a4a4a; color: white; border: 1px solid #666; "
        "border-radius: 4px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #5a5a5a; }"
    );
    settingsBox.exec();
}

void GameWindow::onSpecialEventTriggered(const QString& eventName, const QString& description)
{
    m_gameLog->append(QString("[%1] 🎊 特殊事件：%2")
                     .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                     .arg(eventName));
    m_gameLog->append(QString("    📝 %1").arg(description));
    m_gameLog->append(QString("    👤 用户：%1").arg(m_currentUser));
}

void GameWindow::showAbout()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("ℹ 关于捕鱼大战");
    aboutBox.setIcon(QMessageBox::Information);
    aboutBox.setText("🐟 捕鱼大战 - Fishing War Game");
    aboutBox.setInformativeText(
        QString("<h3>🎮 捕鱼大战 v1.0</h3>"
                "<p>一款经典的捕鱼类街机游戏</p>"
                "<br>"
                "<p><b>🔧 开发信息：</b></p>"
                "<p>• 开发者：游戏开发团队</p>"
                "<p>• 技术栈：Qt6 C++</p>"
                "<p>• 版本：v1.0.0</p>"
                "<p>• 构建日期：2025-05-27</p>"
                "<br>"
                "<p><b>👤 当前会话：</b></p>"
                "<p>• 用户：%1</p>"
                "<p>• 开始时间：%2</p>"
                "<p>• 会话时长：%3 分钟</p>"
                "<br>"
                "<p><b>🎯 游戏特色：</b></p>"
                "<p>• 14种不同类型的鱼类</p>"
                "<p>• 4种可升级的炮塔系统</p>"
                "<p>• 10种特殊事件机制</p>"
                "<p>• 完整的成就系统</p>"
                "<p>• 实时统计和排行榜</p>"
                "<br>"
                "<p>🙏 感谢您的游玩！</p>")
                .arg(m_currentUser)
                .arg(m_sessionStartTime.toString("yyyy-MM-dd hh:mm:ss"))
                .arg(m_sessionTimeElapsed.elapsed() / 60000)
    );
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.setStyleSheet(
        "QMessageBox { background-color: #3b3b3b; color: white; }"
        "QMessageBox QLabel { color: white; }"
        "QPushButton { background-color: #4a4a4a; color: white; border: 1px solid #666; "
        "border-radius: 4px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #5a5a5a; }"
    );
    aboutBox.exec();
}

void GameWindow::showHelp()
{
    QMessageBox helpBox(this);
    helpBox.setWindowTitle("📖 游戏帮助");
    helpBox.setIcon(QMessageBox::Information);
    helpBox.setText("🎮 捕鱼大战 - 游戏指南");
    helpBox.setInformativeText(
        QString("<h3>🕹 基础操作：</h3>"
                "<p><b>🖱 鼠标移动：</b> 瞄准目标鱼类</p>"
                "<p><b>🖱 鼠标左键：</b> 发射炮弹</p>"
                "<p><b>⌨ 空格键：</b> 开始/暂停游戏</p>"
                "<p><b>⌨ R键：</b> 重置游戏</p>"
                "<p><b>⌨ +/-键：</b> 升级/降级炮塔</p>"
                "<p><b>⌨ F1：</b> 显示帮助</p>"
                "<p><b>⌨ ESC：</b> 暂停游戏</p>"
                "<br>"
                "<h3>🎯 游戏目标：</h3>"
                "<p>• 🎣 射击各种鱼类获得金币和分数</p>"
                "<p>• 💰 使用金币升级炮塔提升威力</p>"
                "<p>• 🏆 挑战更高分数创造记录</p>"
                "<p>• 🎊 触发特殊事件获得额外奖励</p>"
                "<br>"
                "<h3>🐟 鱼类介绍：</h3>"
                "<p><b>🟡 小鱼：</b> 血量低，奖励少，容易捕获</p>"
                "<p><b>🔵 中鱼：</b> 血量中等，奖励适中</p>"
                "<p><b>🔴 大鱼：</b> 血量高，奖励丰厚</p>"
                "<p><b>🟣 BOSS鱼：</b> 血量极高，超高奖励</p>"
                "<p><b>🟠 特殊鱼：</b> 拥有特殊能力，价值极高</p>"
                "<br>"
                "<h3>🚀 炮塔系统：</h3>"
                "<p><b>🔫 基础炮塔：</b> 伤害 10，射速 500ms，升级费用 100金币</p>"
                "<p><b>🔫 银质炮塔：</b> 伤害 25，射速 400ms，升级费用 500金币</p>"
                "<p><b>💫 金质炮塔：</b> 伤害 50，射速 300ms，升级费用 1000金币</p>"
                "<p><b>⚡ 激光炮塔：</b> 伤害 100，射速 200ms，已达最高级</p>"
                "<br>"
                "<p><b>👤 当前用户：</b> %1</p>"
                "<p><b>📊 当前最高分：</b> %2</p>")
                .arg(m_currentUser)
                .arg(m_highScore)
    );
    helpBox.setStandardButtons(QMessageBox::Ok);
    helpBox.setStyleSheet(
        "QMessageBox { background-color: #3b3b3b; color: white; }"
        "QMessageBox QLabel { color: white; }"
        "QPushButton { background-color: #4a4a4a; color: white; border: 1px solid #666; "
        "border-radius: 4px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #5a5a5a; }"
    );
    helpBox.exec();
}

void GameWindow::onGameTimerUpdate()
{
    if (m_gameRunning && !m_gamePaused) {
        m_totalGameTime++;
        updateGameStats();
    }

    // 更新会话时间
    m_sessionTime = m_sessionTimeElapsed.elapsed() / 1000;
}

void GameWindow::updateFPS()
{
    // 从游戏组件获取实际FPS或计算FPS
    if (m_fpsLabel) {
        m_fpsLabel->setText(QString("📊 FPS: %1").arg(m_currentFPS));
    }
}

void GameWindow::updateCannonInfo()
{
    if (!m_gameWidget) return;

    Cannon::CannonType type = m_gameWidget->getCurrentCannonType();
    QString typeName;
    QString emoji;
    int damage = 10;
    int fireRate = 500;
    int upgradeCost = 100;

    switch (type) {
    case Cannon::BASIC_CANNON:
        typeName = "基础炮塔";
        emoji = "🔫";
        damage = 10;
        fireRate = 500;
        upgradeCost = 100;
        break;
    case Cannon::SILVER_CANNON:
        typeName = "银质炮塔";
        emoji = "🔫";
        damage = 25;
        fireRate = 400;
        upgradeCost = 500;
        break;
    case Cannon::GOLD_CANNON:
        typeName = "金质炮塔";
        emoji = "💫";
        damage = 50;
        fireRate = 300;
        upgradeCost = 1000;
        break;
    case Cannon::LASER_CANNON:
        typeName = "激光炮塔";
        emoji = "⚡";
        damage = 100;
        fireRate = 200;
        upgradeCost = 0;
        break;
    }

    m_cannonTypeLabel->setText(QString("%1 炮塔类型: %2").arg(emoji).arg(typeName));
    m_cannonDamageLabel->setText(QString("💥 攻击力: %1").arg(damage));
    m_cannonFireRateLabel->setText(QString("⚡ 射击速度: %1ms").arg(fireRate));

    if (type == Cannon::LASER_CANNON) {
        m_upgradeCostLabel->setText("🏆 已达最高级");
        m_upgradeCostLabel->setStyleSheet("color: #FFD700; font-weight: bold; font-size: 10px;");
        m_upgradeCannonButton->setEnabled(false);
        m_upgradeCannonButton->setText("🏆 已满级");
    } else {
        m_upgradeCostLabel->setText(QString("💰 升级费用: %1金币").arg(upgradeCost));
        m_upgradeCostLabel->setStyleSheet("font-size: 10px;");
        m_upgradeCannonButton->setEnabled(m_currentCoins >= upgradeCost);
        m_upgradeCannonButton->setText("⬆ 升级");

        if (m_currentCoins < upgradeCost) {
            m_upgradeCostLabel->setStyleSheet("color: #ff6b6b; font-weight: bold; font-size: 10px;");
        }
    }

    m_downgradeCannonButton->setEnabled(type != Cannon::BASIC_CANNON);
}

void GameWindow::updateGameStats()
{
    // 更新游戏时间
    int hours = m_totalGameTime / 3600;
    int minutes = (m_totalGameTime % 3600) / 60;
    int seconds = m_totalGameTime % 60;

    if (hours > 0) {
        m_gameTimeLabel->setText(QString("⏰ 游戏时间: %1:%2:%3")
                                     .arg(hours, 2, 10, QChar('0'))
                                     .arg(minutes, 2, 10, QChar('0'))
                                     .arg(seconds, 2, 10, QChar('0')));
    } else {
        m_gameTimeLabel->setText(QString("⏰ 游戏时间: %1:%2")
                                     .arg(minutes, 2, 10, QChar('0'))
                                     .arg(seconds, 2, 10, QChar('0')));
    }

    // 更新会话统计
    int sessionMinutes = m_sessionTimeElapsed.elapsed() / 60000;
    m_sessionStatsLabel->setText(QString("📈 本次会话: %1 局 | %2 分钟")
                                     .arg(m_gamesPlayed)
                                     .arg(sessionMinutes));

    // 更新用户信息
    m_userInfoLabel->setText(QString("👤 用户: %1 | 在线: %2 分钟")
                                 .arg(m_currentUser)
                                 .arg(sessionMinutes));
}

void GameWindow::updateWindowTitle()
{
    QString title = QString("🐟 捕鱼大战 v1.0 - 用户: %1").arg(m_currentUser);

    if (m_gameRunning) {
        if (m_gamePaused) {
            title += " [⏸ 已暂停]";
        } else {
            title += QString(" [🎮 进行中] - 分数: %1 | 金币: %2")
                         .arg(m_currentScore)
                         .arg(m_currentCoins);
        }
    } else {
        title += " [🟢 就绪]";
    }

    if (m_highScore > 0) {
        title += QString(" | 最高分: %1").arg(m_highScore);
    }

    setWindowTitle(title);
}

void GameWindow::saveSettings()
{
    QSettings settings;

    // 保存窗口状态
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());

    // 保存游戏统计
    settings.setValue("stats/highScore", m_highScore);
    settings.setValue("stats/totalFishCaught", m_totalFishCaught + m_currentFishCaught);
    settings.setValue("stats/gamesPlayed", m_gamesPlayed);
    settings.setValue("stats/totalPlayTime", m_totalGameTime);
    settings.setValue("stats/shotsFired", m_shotsFired);

    // 保存用户信息
    settings.setValue("user/currentUser", m_currentUser);
    settings.setValue("user/lastLoginTime", QDateTime::currentDateTime());
    settings.setValue("user/sessionStartTime", m_sessionStartTime);
    settings.setValue("user/sessionDuration", m_sessionTimeElapsed.elapsed() / 1000);

    // 保存游戏设置
    settings.setValue("game/lastCoins", m_currentCoins);
    settings.setValue("game/lastScore", m_currentScore);
    settings.setValue("game/lastFishCaught", m_currentFishCaught);

    // 保存FPS和性能数据
    settings.setValue("performance/lastFPS", m_currentFPS);
    settings.setValue("performance/frameCount", m_frameCount);

    // 保存时间戳
    settings.setValue("system/lastSaveTime", QDateTime::currentDateTime());
    settings.setValue("system/version", "1.0.0");
    settings.setValue("system/buildDate", "2025-05-27");

    m_gameLog->append(QString("[%1] 💾 用户 %2 的设置已保存")
                          .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                          .arg(m_currentUser));
}

void GameWindow::loadSettings()
{
    QSettings settings;

    // 加载窗口状态
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());

    // 加载游戏统计
    m_highScore = settings.value("stats/highScore", 0).toInt();
    m_totalFishCaught = settings.value("stats/totalFishCaught", 0).toInt();
    m_gamesPlayed = settings.value("stats/gamesPlayed", 0).toInt();
    m_shotsFired = settings.value("stats/shotsFired", 0).toInt();

    // 加载用户信息
    QString savedUser = settings.value("user/currentUser", "").toString();
    if (!savedUser.isEmpty() && savedUser == m_currentUser) {
        QDateTime lastLogin = settings.value("user/lastLoginTime").toDateTime();
        if (lastLogin.isValid()) {
            m_gameLog->append(QString("👋 欢迎回来，%1！").arg(m_currentUser));
            m_gameLog->append(QString("📅 上次登录：%1").arg(lastLogin.toString("yyyy-MM-dd hh:mm:ss")));
        }
    }

    // 加载FPS数据
    m_currentFPS = settings.value("performance/lastFPS", 60).toInt();
    m_frameCount = settings.value("performance/frameCount", 0).toInt();

    // 显示版本和构建信息
    QString version = settings.value("system/version", "未知").toString();
    QString buildDate = settings.value("system/buildDate", "未知").toString();
    QDateTime lastSave = settings.value("system/lastSaveTime").toDateTime();

    if (lastSave.isValid()) {
        m_gameLog->append(QString("🔧 系统版本：%1 | 构建日期：%2")
                              .arg(version).arg(buildDate));
        m_gameLog->append(QString("💾 上次保存：%1")
                              .arg(lastSave.toString("yyyy-MM-dd hh:mm:ss")));
    }

    // 更新界面显示
    m_highScoreLabel->setText(QString("🥇 最高分: %1").arg(m_highScore));

    // 显示历史统计
    if (m_totalFishCaught > 0) {
        m_gameLog->append(QString("📊 历史统计：总共捕获 %1 条鱼，游玩 %2 局")
                              .arg(m_totalFishCaught)
                              .arg(m_gamesPlayed));
    }

    m_gameLog->append(QString("✅ 用户 %1 的设置加载完成").arg(m_currentUser));
}
