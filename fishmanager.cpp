#include "fishmanager.h"
#include "gamemath.h"
#include <QRandomGenerator>
#include <QDebug>

FishManager::FishManager(QObject *parent)
    : QObject(parent), m_spawnInterval(3000), m_maxFishCount(10)
{
    m_gameBounds = QRect(0, 0, 1280, 720);

    // 设置生成定时器
    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &FishManager::spawnFish);
    m_spawnTimer->start(m_spawnInterval);

    // 立即生成一些鱼
    for (int i = 0; i < 3; ++i) {
        spawnFish();
    }
}

FishManager::~FishManager()
{
    qDeleteAll(m_fishes);
    m_fishes.clear();
}

void FishManager::update(double deltaTime)
{
    // 更新所有鱼
    for (int i = m_fishes.size() - 1; i >= 0; --i) {
        Fish* fish = m_fishes[i];
        if (!fish) continue;

        fish->update(deltaTime);

        // 移除死亡或超出边界的鱼
        if (!fish->isAlive() || fish->isOutOfBounds(m_gameBounds)) {
            m_fishes.removeAt(i);
            fish->deleteLater();
        }
    }
}

void FishManager::draw(QPainter& painter)
{
    for (Fish* fish : m_fishes) {
        if (fish && fish->isAlive()) {
            fish->draw(painter);
        }
    }
}

void FishManager::reset()
{
    qDeleteAll(m_fishes);
    m_fishes.clear();
}

QVector<Fish*> FishManager::getCollisionFishes(const QPointF& point, double radius)
{
    QVector<Fish*> result;

    for (Fish* fish : m_fishes) {
        if (!fish || !fish->isAlive()) continue;

        double distance = GameMath::distance(point, fish->getPosition());
        if (distance <= radius + fish->getRadius()) {
            result.append(fish);
        }
    }

    return result;
}

void FishManager::spawnFish()
{
    if (m_fishes.size() >= m_maxFishCount) return;

    Fish::FishType type = getRandomFishType();
    QPointF spawnPos = getRandomSpawnPoint();

    Fish* fish = new Fish(type, spawnPos, this);
    connect(fish, &Fish::died, this, &FishManager::onFishDied);

    // 创建随机路径
    QVector<QPointF> path;
    createRandomPath(type, path);
    fish->setPath(path);

    m_fishes.append(fish);
}

void FishManager::onFishDied(Fish* fish)
{
    Q_UNUSED(fish)
    // 死亡处理可以在这里添加特效等
}

void FishManager::createRandomPath(Fish::FishType type, QVector<QPointF>& path)
{
    Q_UNUSED(type)

    // 随机选择路径类型
    int pathType = QRandomGenerator::global()->bounded(3);

    switch (pathType) {
    case 0: // 直线路径
    {
        QPointF start = getRandomSpawnPoint();
        QPointF end;

        // 从左到右或从右到左
        if (start.x() < m_gameBounds.width() / 2) {
            end = QPointF(m_gameBounds.width() + 50,
                          start.y() + QRandomGenerator::global()->bounded(-100, 101));
        } else {
            end = QPointF(-50,
                          start.y() + QRandomGenerator::global()->bounded(-100, 101));
        }

        path << start << end;
    }
    break;

    case 1: // 弯曲路径
    {
        QPointF start = getRandomSpawnPoint();
        QPointF middle = QPointF(m_gameBounds.width() / 2 + QRandomGenerator::global()->bounded(-200, 201),
                                 m_gameBounds.height() / 2 + QRandomGenerator::global()->bounded(-150, 151));
        QPointF end;

        if (start.x() < m_gameBounds.width() / 2) {
            end = QPointF(m_gameBounds.width() + 50,
                          QRandomGenerator::global()->bounded(100, m_gameBounds.height() - 200));
        } else {
            end = QPointF(-50,
                          QRandomGenerator::global()->bounded(100, m_gameBounds.height() - 200));
        }

        path << start << middle << end;
    }
    break;

    case 2: // 圆弧路径
    {
        QPointF center(m_gameBounds.width() / 2, m_gameBounds.height() / 2);
        double radius = 200 + QRandomGenerator::global()->bounded(-50, 51);
        double startAngle = QRandomGenerator::global()->bounded(0, 360) * M_PI / 180.0;

        for (int i = 0; i <= 10; ++i) {
            double angle = startAngle + (M_PI * i / 10.0);
            QPointF point = GameMath::pointFromAngle(center, angle, radius);
            path << point;
        }
    }
    break;
    }
}

QPointF FishManager::getRandomSpawnPoint()
{
    // 从屏幕边缘生成
    int side = QRandomGenerator::global()->bounded(4); // 0-3对应上下左右

    switch (side) {
    case 0: // 上边
        return QPointF(QRandomGenerator::global()->bounded(m_gameBounds.width()),
                       -50);
    case 1: // 下边
        return QPointF(QRandomGenerator::global()->bounded(m_gameBounds.width()),
                       m_gameBounds.height() + 50);
    case 2: // 左边
        return QPointF(-50,
                       QRandomGenerator::global()->bounded(100, m_gameBounds.height() - 200));
    case 3: // 右边
        return QPointF(m_gameBounds.width() + 50,
                       QRandomGenerator::global()->bounded(100, m_gameBounds.height() - 200));
    default:
        return QPointF(0, 0);
    }
}

Fish::FishType FishManager::getRandomFishType()
{
    int roll = QRandomGenerator::global()->bounded(100);

    if (roll < 50) return Fish::SMALL_FISH;      // 50%
    else if (roll < 75) return Fish::MEDIUM_FISH; // 25%
    else if (roll < 90) return Fish::LARGE_FISH;  // 15%
    else if (roll < 98) return Fish::SPECIAL_FISH; // 8%
    else return Fish::BOSS_FISH;                   // 2%
}
