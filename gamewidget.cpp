#include "gamewidget.h"
#include "gamemath.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QApplication>
#include <QRandomGenerator>
#include <QDateTime>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), m_gameRunning(false), m_gamePaused(false), m_initialized(false),
    m_cannon(nullptr), m_fishManager(nullptr),
    m_coins(1000), m_score(0), m_fishCaught(0),
    m_mousePressed(false), m_frameCount(0), m_currentFPS(0),
    m_showEventNotification(false), m_eventNotificationAlpha(0),
    m_gameTimer(nullptr), m_eventNotificationTimer(nullptr)
{
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    qDebug() << "GameWidget: Initializing... User:" << "sdkadjad" << "Time:" << QDateTime::currentDateTime().toString();

    try {
        // 创建定时器
        m_gameTimer = new QTimer(this);
        if (!m_gameTimer) {
            throw std::runtime_error("Failed to create game timer");
        }

        m_eventNotificationTimer = new QTimer(this);
        if (!m_eventNotificationTimer) {
            throw std::runtime_error("Failed to create notification timer");
        }
        m_eventNotificationTimer->setSingleShot(true);

        // 连接定时器信号
        connect(m_gameTimer, &QTimer::timeout, this, &GameWidget::updateGame);
        connect(m_eventNotificationTimer, &QTimer::timeout, this, &GameWidget::hideEventNotification);

        // 启动游戏定时器
        m_gameTimer->start(16); // 约60 FPS

        // 启动计时器
        m_elapsedTimer.start();
        m_fpsTimer.start();

        // 延迟初始化游戏对象以避免构造函数中的复杂操作
        QTimer::singleShot(50, this, &GameWidget::delayedInitialization);

        qDebug() << "GameWidget: Constructor complete";

    } catch (const std::exception& e) {
        qDebug() << "GameWidget constructor failed:" << e.what();
        throw;
    }
}

GameWidget::~GameWidget()
{
    qDebug() << "GameWidget: Destroying...";

    try {
        // 首先停止所有定时器
        if (m_gameTimer) {
            m_gameTimer->stop();
            m_gameTimer->disconnect();
        }

        if (m_eventNotificationTimer) {
            m_eventNotificationTimer->stop();
            m_eventNotificationTimer->disconnect();
        }

        // 标记为正在销毁，防止后续操作
        m_gameRunning = false;
        m_gamePaused = true;
        m_initialized = false;

        // 断开所有信号连接
        disconnect();

        // 安全清理游戏对象
        cleanupGame();

        qDebug() << "GameWidget: Destroyed safely";

    } catch (const std::exception& e) {
        qDebug() << "Error during GameWidget destruction:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error during GameWidget destruction";
    }
}

void GameWidget::delayedInitialization()
{
    qDebug() << "GameWidget: Starting delayed initialization...";

    try {
        initializeGame();
        m_initialized = true;
        qDebug() << "GameWidget: Delayed initialization complete";
    } catch (const std::exception& e) {
        qDebug() << "GameWidget: Delayed initialization failed:" << e.what();
        m_initialized = false;
    }
}

void GameWidget::initializeGame()
{
    qDebug() << "GameWidget: Initializing game...";

    try {
        // 只在重新初始化时才清理（避免首次初始化时的空指针访问）
        if (m_initialized && (m_cannon || m_fishManager)) {
            cleanupGame();
        }

        // 创建炮塔
        m_cannon = new Cannon(Cannon::BASIC_CANNON, this);
        if (!m_cannon) {
            throw std::runtime_error("Failed to create cannon!");
        }

        // 设置炮塔位置（底部中央）
        QPointF cannonPos(GAME_WIDTH / 2, GAME_HEIGHT - CANNON_Y_OFFSET);
        m_cannon->setPosition(cannonPos);

        // 连接炮塔信号
        connect(m_cannon, &Cannon::fired, this, &GameWidget::onCannonFired, Qt::QueuedConnection);

        // 创建鱼群管理器
        m_fishManager = new FishManager(this);
        if (!m_fishManager) {
            throw std::runtime_error("Failed to create fish manager!");
        }

        // 设置游戏区域
        m_fishManager->setGameBounds(rect());

        // 创建背景
        createBackground();

        // 设置UI按钮位置
        setupUI();

        // 重置游戏状态
        m_coins = 1000;
        m_score = 0;
        m_fishCaught = 0;

        emit coinsChanged(m_coins);
        emit scoreChanged(m_score);
        emit fishCaughtChanged(m_fishCaught);
        emit cannonTypeChanged(m_cannon->getType());

        qDebug() << "GameWidget: Game initialized successfully";
    } catch (const std::exception& e) {
        qDebug() << "GameWidget: Exception during initialization:" << e.what();
        throw;
    }
}

void GameWidget::cleanupGame()
{
    qDebug() << "GameWidget: Cleaning up game...";

    // 停止定时器 - 添加空指针检查
    if (m_gameTimer && m_gameTimer->isActive()) {
        m_gameTimer->stop();
    }
    if (m_eventNotificationTimer && m_eventNotificationTimer->isActive()) {
        m_eventNotificationTimer->stop();
    }

    // 清理子弹 - 只有在不为空时才清理
    if (!m_bullets.isEmpty()) {
        cleanupBullets();
    }

    // 清理鱼群管理器 - 添加空指针检查
    if (m_fishManager) {
        m_fishManager->disconnect();
        m_fishManager->deleteLater();
        m_fishManager = nullptr;
    }

    // 清理炮塔 - 添加空指针检查
    if (m_cannon) {
        m_cannon->disconnect();
        m_cannon->deleteLater();
        m_cannon = nullptr;
    }

    qDebug() << "GameWidget: Cleanup complete";
}

void GameWidget::cleanupBullets()
{
    if (m_bullets.isEmpty()) return;

    qDebug() << "GameWidget: Cleaning up" << m_bullets.size() << "bullets";

    // 使用反向迭代以避免索引问题
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        Bullet* bullet = m_bullets[i];
        if (bullet) {
            bullet->disconnect();
            bullet->deleteLater();
        }
    }
    m_bullets.clear();
}

void GameWidget::createBackground()
{
    qDebug() << "GameWidget: Creating background...";

    m_backgroundPixmap = QPixmap(GAME_WIDTH, GAME_HEIGHT);

    QPainter painter(&m_backgroundPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制渐变海洋背景
    QLinearGradient gradient(0, 0, 0, GAME_HEIGHT);
    gradient.setColorAt(0, QColor(135, 206, 250));  // 浅蓝色
    gradient.setColorAt(0.5, QColor(70, 130, 180)); // 中蓝色
    gradient.setColorAt(1, QColor(25, 25, 112));    // 深蓝色

    painter.fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT, gradient);

    // 绘制一些装饰性的泡泡
    painter.setBrush(QBrush(QColor(255, 255, 255, 50)));
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < 30; ++i) {
        int x = QRandomGenerator::global()->bounded(GAME_WIDTH);
        int y = QRandomGenerator::global()->bounded(GAME_HEIGHT);
        int size = 5 + QRandomGenerator::global()->bounded(15);
        painter.drawEllipse(x, y, size, size);
    }

    // 绘制海底装饰
    painter.setBrush(QBrush(QColor(34, 139, 34, 100)));
    for (int i = 0; i < 20; ++i) {
        int x = QRandomGenerator::global()->bounded(GAME_WIDTH);
        int y = GAME_HEIGHT - 100 + QRandomGenerator::global()->bounded(50);
        int width = 10 + QRandomGenerator::global()->bounded(20);
        int height = 20 + QRandomGenerator::global()->bounded(40);
        painter.drawEllipse(x, y, width, height);
    }

    qDebug() << "GameWidget: Background created";
}

void GameWidget::setupUI()
{
    // 设置UI按钮位置
    int buttonSize = 40;
    int cannonX = GAME_WIDTH / 2;
    int cannonY = GAME_HEIGHT - CANNON_Y_OFFSET;

    m_upgradeButtonRect = QRect(cannonX + 80, cannonY - buttonSize/2, buttonSize, buttonSize);
    m_downgradeButtonRect = QRect(cannonX - 80 - buttonSize, cannonY - buttonSize/2, buttonSize, buttonSize);

    // 炮塔信息显示区域
    m_cannonInfoRect = QRect(cannonX - 100, cannonY + 40, 200, 60);

    qDebug() << "GameWidget: UI setup complete";
}

void GameWidget::startGame()
{
    if (!m_initialized) {
        qDebug() << "GameWidget: Cannot start game - not initialized";
        return;
    }

    qDebug() << "GameWidget: Starting game...";
    m_gameRunning = true;
    m_gamePaused = false;
    m_elapsedTimer.restart();

    if (m_gameTimer) {
        m_gameTimer->start(16);
    }
}

void GameWidget::pauseGame()
{
    qDebug() << "GameWidget: Toggling pause...";
    m_gamePaused = !m_gamePaused;
}

void GameWidget::resetGame()
{
    qDebug() << "GameWidget: Resetting game...";

    try {
        // 停止游戏
        m_gameRunning = false;
        m_gamePaused = false;

        // 清理子弹
        cleanupBullets();

        // 重置鱼群
        if (m_fishManager) {
            m_fishManager->reset();
        }

        // 重置游戏状态
        m_coins = 1000;
        m_score = 0;
        m_fishCaught = 0;

        // 安全地重置炮塔
        if (m_cannon) {
            m_cannon->disconnect();
            QPointF cannonPos = m_cannon->getPosition(); // 保存位置
            delete m_cannon;

            m_cannon = new Cannon(Cannon::BASIC_CANNON, this);
            if (m_cannon) {
                m_cannon->setPosition(cannonPos);
                connect(m_cannon, &Cannon::fired, this, &GameWidget::onCannonFired, Qt::QueuedConnection);
            }
        }

        emit coinsChanged(m_coins);
        emit scoreChanged(m_score);
        emit fishCaughtChanged(m_fishCaught);
        emit cannonTypeChanged(m_cannon ? m_cannon->getType() : Cannon::BASIC_CANNON);

        // 重新开始游戏
        startGame();

    } catch (const std::exception& e) {
        qDebug() << "Error in resetGame:" << e.what();
    }
}

Cannon::CannonType GameWidget::getCurrentCannonType() const
{
    return m_cannon ? m_cannon->getType() : Cannon::BASIC_CANNON;
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    try {
        // 绘制背景
        drawBackground(painter);

        if (!m_initialized) {
            // 显示加载画面
            painter.fillRect(rect(), QColor(0, 0, 0, 150));
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 24, QFont::Bold));
            painter.drawText(rect(), Qt::AlignCenter, "正在加载游戏...\n请稍候");
            return;
        }

        if (!m_gameRunning) {
            // 绘制暂停或游戏结束画面
            painter.fillRect(rect(), QColor(0, 0, 0, 100));
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 24, QFont::Bold));
            painter.drawText(rect(), Qt::AlignCenter, "游戏暂停\n按空格键开始/暂停");
            return;
        }

        // 绘制鱼群
        if (m_fishManager) {
            m_fishManager->draw(painter);
        }

        // 绘制子弹
        painter.save();
        for (Bullet* bullet : m_bullets) {
            if (bullet && bullet->isActive()) {
                try {
                    bullet->draw(painter);
                } catch (...) {
                    qDebug() << "Error drawing bullet";
                }
            }
        }
        painter.restore();

        // 绘制炮塔
        if (m_cannon) {
            painter.save();
            m_cannon->draw(painter);
            painter.restore();
        }

        // 绘制UI
        drawUI(painter);
        drawCannonUI(painter);
        drawEventNotification(painter);

    } catch (...) {
        qDebug() << "Error in paintEvent";
    }
}

void GameWidget::drawBackground(QPainter& painter)
{
    if (!m_backgroundPixmap.isNull()) {
        painter.drawPixmap(0, 0, m_backgroundPixmap);
    } else {
        // 如果背景未创建，绘制简单背景
        painter.fillRect(rect(), QColor(25, 25, 112));
    }
}

void GameWidget::drawUI(QPainter& painter)
{
    painter.save();

    // 绘制分数和金币
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));

    QString scoreText = QString("分数: %1").arg(m_score);
    QString coinText = QString("金币: %1").arg(m_coins);
    QString fishText = QString("捕获: %1 条鱼").arg(m_fishCaught);

    painter.drawText(10, 30, scoreText);
    painter.drawText(10, 60, coinText);
    painter.drawText(10, 90, fishText);

    // 绘制FPS
    m_frameCount++;
    if (m_fpsTimer.elapsed() >= 1000) {
        m_currentFPS = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer.restart();
    }

    painter.drawText(GAME_WIDTH - 100, 30, QString("FPS: %1").arg(m_currentFPS));

    // 绘制子弹计数（调试用）
    painter.drawText(GAME_WIDTH - 150, 60, QString("子弹: %1").arg(m_bullets.size()));

    // 绘制当前时间
    QString timeText = QDateTime::currentDateTime().toString("hh:mm:ss");
    painter.drawText(GAME_WIDTH - 120, 90, timeText);

    // 绘制用户信息
    painter.setFont(QFont("Arial", 12));
    painter.drawText(GAME_WIDTH - 200, 120, QString("用户: sdkadjad"));

    painter.restore();
}

void GameWidget::drawCannonUI(QPainter& painter)
{
    if (!m_cannon) return;

    painter.save();

    // 绘制升级按钮
    painter.setBrush(QBrush(QColor(0, 255, 0, 180)));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(m_upgradeButtonRect);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(m_upgradeButtonRect, Qt::AlignCenter, "+");

    // 绘制降级按钮
    painter.setBrush(QBrush(QColor(255, 0, 0, 180)));
    painter.drawEllipse(m_downgradeButtonRect);
    painter.drawText(m_downgradeButtonRect, Qt::AlignCenter, "-");

    // 绘制炮塔信息
    painter.setBrush(QBrush(QColor(0, 0, 0, 150)));
    painter.setPen(QPen(Qt::white, 1));
    painter.drawRoundedRect(m_cannonInfoRect, 10, 10);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));

    QString cannonInfo;
    switch (m_cannon->getType()) {
    case Cannon::BASIC_CANNON:
        cannonInfo = "基础炮塔\n伤害: " + QString::number(m_cannon->getDamage());
        break;
    case Cannon::SILVER_CANNON:
        cannonInfo = "银质炮塔\n伤害: " + QString::number(m_cannon->getDamage());
        break;
    case Cannon::GOLD_CANNON:
        cannonInfo = "金质炮塔\n伤害: " + QString::number(m_cannon->getDamage());
        break;
    case Cannon::LASER_CANNON:
        cannonInfo = "激光炮塔\n伤害: " + QString::number(m_cannon->getDamage());
        break;
    }

    if (m_cannon->canUpgrade()) {
        cannonInfo += "\n升级费用: " + QString::number(m_cannon->getUpgradeCost());
    } else {
        cannonInfo += "\n已满级";
    }

    painter.drawText(m_cannonInfoRect, Qt::AlignCenter, cannonInfo);

    painter.restore();
}

void GameWidget::drawEventNotification(QPainter& painter)
{
    if (!m_showEventNotification || m_eventNotificationAlpha <= 0) return;

    painter.save();

    // 设置透明度
    painter.setOpacity(m_eventNotificationAlpha);

    // 计算通知框位置
    int boxWidth = 400;
    int boxHeight = 120;
    int x = (GAME_WIDTH - boxWidth) / 2;
    int y = 100;

    QRect notificationRect(x, y, boxWidth, boxHeight);

    // 绘制背景
    painter.setBrush(QBrush(QColor(0, 0, 0, 200)));
    painter.setPen(QPen(QColor(255, 215, 0), 3));
    painter.drawRoundedRect(notificationRect, 15, 15);

    // 绘制标题
    painter.setPen(QColor(255, 215, 0));
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    QRect titleRect = notificationRect.adjusted(10, 10, -10, -60);
    painter.drawText(titleRect, Qt::AlignCenter, m_eventNotificationTitle);

    // 绘制描述
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    QRect descRect = notificationRect.adjusted(10, 50, -10, -10);
    painter.drawText(descRect, Qt::AlignCenter | Qt::TextWordWrap, m_eventNotificationMessage);

    painter.restore();
}

void GameWidget::showEventNotification(const QString& title, const QString& message)
{
    m_eventNotificationTitle = title;
    m_eventNotificationMessage = message;
    m_showEventNotification = true;
    m_eventNotificationAlpha = 1.0;

    // 3秒后开始淡出
    if (m_eventNotificationTimer) {
        m_eventNotificationTimer->start(3000);
    }

    qDebug() << "GameWidget: Showing event notification:" << title;
}

void GameWidget::hideEventNotification()
{
    m_showEventNotification = false;
    m_eventNotificationAlpha = 0;
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_initialized || !m_gameRunning || m_gamePaused) return;

    m_mousePressed = true;
    m_mousePosition = event->pos();

    // 检查是否点击了UI按钮
    if (m_upgradeButtonRect.contains(event->pos())) {
        upgradeCannon();
        return;
    }

    if (m_downgradeButtonRect.contains(event->pos())) {
        downgradeCannon();
        return;
    }

    // 普通射击
    if (event->button() == Qt::LeftButton && m_cannon && m_cannon->canFire()) {
        qDebug() << "GameWidget: Mouse click, firing cannon";
        m_cannon->fire();
    }
}

void GameWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_initialized || !m_gameRunning || m_gamePaused) return;

    m_mousePosition = event->pos();
    updateCannonAiming();

    // 发射鼠标移动信号
    emit mouseMoved(event->pos());
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        if (!m_gameRunning) {
            startGame();
        } else {
            pauseGame();
        }
        break;
    case Qt::Key_R:
        resetGame();
        break;
    case Qt::Key_Equal:
    case Qt::Key_Plus:
        upgradeCannon();
        break;
    case Qt::Key_Minus:
        downgradeCannon();
        break;
    }
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // 如果需要支持窗口大小调整，在这里处理
}

void GameWidget::updateGame()
{
    // 基本有效性检查
    if (!m_initialized || !m_gameRunning || m_gamePaused) return;

    // 检查关键对象
    if (!m_cannon || !m_fishManager) {
        qDebug() << "GameWidget: Critical objects are null, stopping game";
        m_gameRunning = false;
        return;
    }

    double deltaTime = m_elapsedTimer.elapsed() / 1000.0;
    m_elapsedTimer.restart();

    // 限制deltaTime避免大跳跃
    deltaTime = qMin(deltaTime, 0.033); // 最大33ms (30 FPS)

    try {
        // 更新游戏对象
        if (m_cannon) {
            m_cannon->update(deltaTime);
        }

        updateBullets(deltaTime);
        updateFishes(deltaTime);
        checkCollisions();

        // 更新事件通知透明度
        if (m_showEventNotification && m_eventNotificationTimer && !m_eventNotificationTimer->isActive()) {
            m_eventNotificationAlpha -= deltaTime * 2; // 0.5秒淡出
            if (m_eventNotificationAlpha <= 0) {
                hideEventNotification();
            }
        }

        // 更新显示
        update();

    } catch (const std::exception& e) {
        qDebug() << "GameWidget: Exception in updateGame:" << e.what();
        m_gamePaused = true;
    } catch (...) {
        qDebug() << "GameWidget: Unknown exception in updateGame";
        m_gamePaused = true;
    }
}

void GameWidget::updateBullets(double deltaTime)
{
    if (m_bullets.isEmpty()) return;

    // 首先更新所有活跃的子弹
    for (Bullet* bullet : m_bullets) {
        if (bullet && bullet->isActive()) {
            try {
                bullet->update(deltaTime);
            } catch (...) {
                qDebug() << "Error updating bullet";
                bullet->setActive(false);
            }
        }
    }

    // 然后移除失效的子弹（使用反向迭代）
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (i >= m_bullets.size()) continue;

        Bullet* bullet = m_bullets[i];
        if (!bullet) {
            m_bullets.removeAt(i);
            continue;
        }

        // 检查子弹是否应该被移除
        bool shouldRemove = false;
        QString reason;

        if (!bullet->isActive()) {
            shouldRemove = true;
            reason = "inactive";
        } else if (bullet->isOutOfBounds(rect())) {
            shouldRemove = true;
            reason = "out of bounds";
        }

        if (shouldRemove) {
            qDebug() << "GameWidget: Removing bullet -" << reason;
            m_bullets.removeAt(i);
            bullet->deleteLater();
        }
    }
}

void GameWidget::updateFishes(double deltaTime)
{
    if (!m_fishManager) return;

    try {
        m_fishManager->update(deltaTime);
    } catch (...) {
        qDebug() << "Error updating fish manager";
    }
}

void GameWidget::checkCollisions()
{
    if (!m_fishManager || m_bullets.isEmpty()) return;

    try {
        // 创建活跃子弹的副本以避免在迭代过程中修改原列表
        QVector<Bullet*> activeBullets;
        for (Bullet* bullet : m_bullets) {
            if (bullet && bullet->isActive()) {
                activeBullets.append(bullet);
            }
        }

        for (Bullet* bullet : activeBullets) {
            if (!bullet || !bullet->isActive()) continue;
            if (!m_bullets.contains(bullet)) continue; // 确保子弹仍在主列表中

            QVector<Fish*> hitFishes = m_fishManager->getCollisionFishes(
                bullet->getPosition(), bullet->getRadius());

            bool bulletHit = false;
            for (Fish* fish : hitFishes) {
                if (!fish || !fish->isAlive()) continue;

                // 检查是否已经对这条鱼造成过伤害
                if (bullet->hasHitTarget(fish)) {
                    continue;
                }

                qDebug() << "GameWidget: Bullet hit fish!";

                // 记录击中，防止重复伤害
                bullet->recordHit(fish);

                // 对鱼造成伤害
                fish->takeDamage(bullet->getDamage());

                // 如果鱼死了，给予奖励
                if (!fish->isAlive()) {
                    m_coins += fish->getCoinReward();
                    m_score += fish->getScore();
                    m_fishCaught++; // 增加捕获计数

                    emit coinsChanged(m_coins);
                    emit scoreChanged(m_score);
                    emit fishCaughtChanged(m_fishCaught);

                    qDebug() << "GameWidget: Fish killed! Coins:" << m_coins << "Score:" << m_score << "Fish caught:" << m_fishCaught;

                    // 显示捕获通知
                    showEventNotification("🎣 捕获成功！",
                                          QString("获得 %1 金币，%2 分数！")
                                              .arg(fish->getCoinReward())
                                              .arg(fish->getScore()));
                }

                bulletHit = true;
                break; // 每颗子弹只能击中一条鱼
            }

            // 子弹击中目标后变成网
            if (bulletHit && m_bullets.contains(bullet)) {
                bullet->hitTarget();
            }
        }

    } catch (...) {
        qDebug() << "Error in collision detection";
    }
}

void GameWidget::updateCannonAiming()
{
    if (!m_cannon) return;

    QPointF cannonPos = m_cannon->getPosition();

    // 计算角度
    double angle = qAtan2(m_mousePosition.y() - cannonPos.y(),
                          m_mousePosition.x() - cannonPos.x());

    // 限制炮塔只能向上方射击
    // -π 到 0 弧度（即 -180° 到 0°，对应上半圆）
    if (angle > 0) {
        // 如果角度在下半圆，调整到最近的上半圆边界
        if (angle <= M_PI / 2) {
            angle = -0.1;  // 稍微向右上
        } else {
            angle = -M_PI + 0.1;  // 稍微向左上
        }
    }

    // 进一步限制角度范围
    double minAngle = -M_PI * 5.0 / 6.0;  // -150度
    double maxAngle = -M_PI / 6.0;        // -30度

    if (angle < minAngle) angle = minAngle;
    if (angle > maxAngle) angle = maxAngle;

    m_cannon->setTargetAngle(angle);
}

void GameWidget::upgradeCannon()
{
    if (!m_cannon || !m_cannon->canUpgrade()) return;

    int cost = m_cannon->getUpgradeCost();
    if (m_coins >= cost) {
        qDebug() << "GameWidget: Upgrading cannon, cost:" << cost;
        m_coins -= cost;
        m_cannon->upgrade();

        emit coinsChanged(m_coins);
        emit cannonTypeChanged(m_cannon->getType());

        // 显示升级通知
        showEventNotification("🚀 炮塔升级！",
                              QString("炮塔升级成功！\n花费 %1 金币").arg(cost));
    } else {
        qDebug() << "GameWidget: Not enough coins for upgrade. Need:" << cost << "Have:" << m_coins;
        showEventNotification("💰 金币不足",
                              QString("需要 %1 金币\n当前仅有 %2 金币").arg(cost).arg(m_coins));
    }
}

void GameWidget::downgradeCannon()
{
    if (!m_cannon) return;

    Cannon::CannonType currentType = m_cannon->getType();
    if (currentType == Cannon::BASIC_CANNON) return;

    qDebug() << "GameWidget: Downgrading cannon";

    // 计算返还金币（升级费用的70%）
    int refund = 0;
    switch (currentType) {
    case Cannon::SILVER_CANNON: refund = 70; break;
    case Cannon::GOLD_CANNON: refund = 350; break;
    case Cannon::LASER_CANNON: refund = 700; break;
    default: break;
    }

    // 降级炮塔
    Cannon::CannonType newType = (Cannon::CannonType)((int)currentType - 1);
    QPointF cannonPos = m_cannon->getPosition();

    m_cannon->disconnect();
    delete m_cannon;
    m_cannon = new Cannon(newType, this);
    m_cannon->setPosition(cannonPos);
    connect(m_cannon, &Cannon::fired, this, &GameWidget::onCannonFired, Qt::QueuedConnection);

    m_coins += refund;
    emit coinsChanged(m_coins);
    emit cannonTypeChanged(m_cannon->getType());

    // 显示降级通知
    showEventNotification("⬇️ 炮塔降级",
                          QString("炮塔降级完成\n返还 %1 金币").arg(refund));
}

void GameWidget::onCannonFired(const QPointF& startPos, double angle, int damage)
{
    if (!m_cannon) return;

    try {
        qDebug() << "GameWidget: Cannon fired at angle" << angle << "damage" << damage;

        // 创建对应类型的子弹
        Bullet::BulletType bulletType;
        switch (m_cannon->getType()) {
        case Cannon::BASIC_CANNON: bulletType = Bullet::NORMAL_NET; break;
        case Cannon::SILVER_CANNON: bulletType = Bullet::SILVER_NET; break;
        case Cannon::GOLD_CANNON: bulletType = Bullet::GOLD_NET; break;
        case Cannon::LASER_CANNON: bulletType = Bullet::LASER_BEAM; break;
        default: bulletType = Bullet::NORMAL_NET; break;
        }

        Bullet* bullet = new Bullet(bulletType, startPos, angle, damage, this);
        if (bullet) {
            connect(bullet, &Bullet::finished, this, &GameWidget::onBulletFinished, Qt::QueuedConnection);
            m_bullets.append(bullet);

            qDebug() << "GameWidget: Bullet created, total bullets:" << m_bullets.size();

            // 扣除射击费用
            int fireCost = 1;
            m_coins = qMax(0, m_coins - fireCost);
            emit coinsChanged(m_coins);
        } else {
            qDebug() << "GameWidget: Failed to create bullet!";
        }

    } catch (...) {
        qDebug() << "GameWidget: Error creating bullet";
    }
}

void GameWidget::onBulletFinished(Bullet* bullet)
{
    // 增加空指针和有效性检查
    if (!bullet || !this) return;

    qDebug() << "GameWidget: Bullet finished signal received";

    try {
        // 检查子弹是否仍在列表中
        bool found = false;
        for (int i = 0; i < m_bullets.size(); ++i) {
            if (m_bullets[i] == bullet) {
                qDebug() << "GameWidget: Removing finished bullet from list, remaining:" << (m_bullets.size() - 1);
                m_bullets.removeAt(i);

                // 延迟删除以避免立即销毁
                if (bullet) {
                    bullet->disconnect(); // 断开所有连接
                    bullet->deleteLater();
                }
                found = true;
                break;
            }
        }

        if (!found) {
            qDebug() << "GameWidget: Warning - Finished bullet not found in list";
            // 即使没找到也要安全删除
            if (bullet) {
                bullet->disconnect();
                bullet->deleteLater();
            }
        }

    } catch (const std::exception& e) {
        qDebug() << "Error in onBulletFinished:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error in onBulletFinished";
    }
}

// 公开方法
void GameWidget::upgradeCannonPublic()
{
    upgradeCannon();
}

void GameWidget::downgradeCannonPublic()
{
    downgradeCannon();
}
