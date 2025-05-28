#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QTimer>
#include <QElapsedTimer>
#include <QMenuBar>
#include <QStatusBar>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QMessageBox>
#include <QDateTime>
#include <QSplitter>
#include <QFrame>
#include <QScrollArea>
#include "gamewidget.h"
#include "cannon.h"

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onGameStartPause();
    void onGameReset();
    void onCoinsChanged(int coins);
    void onScoreChanged(int score);
    void onFishCaughtChanged(int fishCaught);
    void onCannonTypeChanged(Cannon::CannonType type);
    void onGameSettingsChanged();
    void onMouseMoved(const QPoint& position);
    void onSpecialEventTriggered(const QString& eventName, const QString& description);
    void showAbout();
    void showHelp();
    void onGameTimerUpdate();
    void updateFPS();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupGamePanel();
    void setupControlPanel();
    void setupInfoPanel();
    void connectSignals();
    void updateCannonInfo();
    void updateGameStats();
    void saveSettings();
    void loadSettings();
    void updateWindowTitle();

    // 核心游戏组件
    GameWidget* m_gameWidget;

    // UI组件
    QWidget* m_centralWidget;
    QHBoxLayout* m_mainLayout;

    // 控制面板
    QWidget* m_controlPanel;
    QScrollArea* m_scrollArea;
    QGroupBox* m_gameControlGroup;
    QGroupBox* m_cannonControlGroup;
    QGroupBox* m_gameStatsGroup;
    QGroupBox* m_gameInfoGroup;

    // 游戏控制按钮
    QPushButton* m_startPauseButton;
    QPushButton* m_resetButton;
    QPushButton* m_settingsButton;
    QPushButton* m_helpButton;
    QPushButton* m_exitButton;

    // 炮塔控制
    QPushButton* m_upgradeCannonButton;
    QPushButton* m_downgradeCannonButton;
    QLabel* m_cannonTypeLabel;
    QLabel* m_cannonDamageLabel;
    QLabel* m_cannonFireRateLabel;
    QLabel* m_upgradeCostLabel;

    // 游戏统计
    QLabel* m_coinsLabel;
    QLabel* m_scoreLabel;
    QLabel* m_gameTimeLabel;
    QLabel* m_fishCountLabel;
    QLabel* m_highScoreLabel;
    QLabel* m_sessionStatsLabel;
    QProgressBar* m_experienceBar;

    // 游戏信息
    QTextEdit* m_gameLog;
    QLabel* m_fpsLabel;
    QLabel* m_versionLabel;
    QLabel* m_userInfoLabel;

    // 状态栏
    QLabel* m_statusLabel;
    QLabel* m_mousePosLabel;
    QLabel* m_timeLabel;

    // 游戏状态
    bool m_gameRunning;
    bool m_gamePaused;
    QTimer* m_gameTimer;
    QTimer* m_fpsTimer;
    QElapsedTimer m_gameTimeElapsed;
    QElapsedTimer m_sessionTimeElapsed;
    int m_totalGameTime;
    int m_sessionTime;

    // 游戏统计
    int m_currentCoins;
    int m_currentScore;
    int m_currentFishCaught;
    int m_totalFishCaught;
    int m_shotsFired;
    int m_highScore;
    int m_gamesPlayed;

    // FPS统计
    int m_frameCount;
    int m_currentFPS;

    // 用户信息
    QString m_currentUser;
    QDateTime m_sessionStartTime;

    // 常量
    static const int CONTROL_PANEL_WIDTH = 300;
    static const int WINDOW_WIDTH = 1280 + CONTROL_PANEL_WIDTH;
    static const int WINDOW_HEIGHT = 720;
};

#endif // GAMEWINDOW_H
