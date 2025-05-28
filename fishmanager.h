#ifndef FISHMANAGER_H
#define FISHMANAGER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPainter>
#include "fish.h"

class FishManager : public QObject
{
    Q_OBJECT

public:
    explicit FishManager(QObject *parent = nullptr);
    ~FishManager();

    void update(double deltaTime);
    void draw(QPainter& painter);
    void reset();

    // 碰撞检测
    QVector<Fish*> getCollisionFishes(const QPointF& point, double radius);

    // 设置游戏区域
    void setGameBounds(const QRect& bounds) { m_gameBounds = bounds; }

private slots:
    void spawnFish();
    void onFishDied(Fish* fish);

private:
    void createRandomPath(Fish::FishType type, QVector<QPointF>& path);
    QPointF getRandomSpawnPoint();
    Fish::FishType getRandomFishType();

    QVector<Fish*> m_fishes;
    QTimer* m_spawnTimer;
    QRect m_gameBounds;

    // 生成控制
    int m_spawnInterval;    // 生成间隔(毫秒)
    int m_maxFishCount;     // 最大鱼数量
};

#endif // FISHMANAGER_H
