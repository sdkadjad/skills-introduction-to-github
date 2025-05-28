#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QDebug>
#include <QLabel>
#include "cannon.h"
#include "bullet.h"
#include "fish.h"
#include "fishmanager.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

    // 游戏控制
    void startGame();
    void pauseGame();
    void resetGame();

    // 获取游戏状态
    int getCoins() const { return m_coins; }
    int getScore() const { return m_score; }
    int getFishCaught() const { return m_fishCaught; }
    Cannon::CannonType getCurrentCannonType() const;

    // 公开这些方法供主窗口调用
    void upgradeCannonPublic();
    void downgradeCannonPublic();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void coinsChanged(int coins);
    void scoreChanged(int score);
    void fishCaughtChanged(int fishCaught);
    void cannonTypeChanged(Cannon::CannonType type);
    void mouseMoved(const QPoint& position);
    void specialEventTriggered(const QString& eventName, const QString& description);

private slots:
    void updateGame();
    void onCannonFired(const QPointF& startPos, double angle, int damage);
    void onBulletFinished(Bullet* bullet);
    void hideEventNotification();
    void delayedInitialization();

private:
    void initializeGame();
    void updateBullets(double deltaTime);
    void updateFishes(double deltaTime);
    void checkCollisions();
    void drawBackground(QPainter& painter);
    void drawUI(QPainter& painter);
    void drawCannonUI(QPainter& painter);
    void drawEventNotification(QPainter& painter);
    void createBackground();
    void setupUI();
    void cleanupBullets();
    void cleanupGame();
    void showEventNotification(const QString& title, const QString& message);

    // 炮塔控制
    void updateCannonAiming();
    void upgradeCannon();
    void downgradeCannon();

    // 游戏状态
    bool m_gameRunning;
    bool m_gamePaused;
    bool m_initialized;

    // 游戏对象
    Cannon* m_cannon;
    QVector<Bullet*> m_bullets;
    FishManager* m_fishManager;

    // 游戏数据
    int m_coins;
    int m_score;
    int m_fishCaught;

    // 定时器
    QTimer* m_gameTimer;
    QElapsedTimer m_elapsedTimer;
    QTimer* m_eventNotificationTimer;

    // 输入状态
    QPointF m_mousePosition;
    bool m_mousePressed;

    // UI元素位置
    QRect m_upgradeButtonRect;
    QRect m_downgradeButtonRect;
    QRect m_cannonInfoRect;

    // 事件通知
    QString m_eventNotificationTitle;
    QString m_eventNotificationMessage;
    bool m_showEventNotification;
    double m_eventNotificationAlpha;

    // 背景
    QPixmap m_backgroundPixmap;

    // 调试和统计
    int m_frameCount;
    QElapsedTimer m_fpsTimer;
    int m_currentFPS;

    // 常量
    static const int GAME_WIDTH = 1280;
    static const int GAME_HEIGHT = 720;
    static const int CANNON_Y_OFFSET = 80;
};

#endif // GAMEWIDGET_H
