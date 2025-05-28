#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QPointF>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QSet>

class Bullet : public QObject
{
    Q_OBJECT

public:
    enum BulletType {
        NORMAL_NET,
        SILVER_NET,
        GOLD_NET,
        LASER_BEAM
    };

    enum BulletState {
        FLYING,         // 飞行中
        EXPANDING,      // 网络扩散中
        EXPLODING       // 爆炸中
    };

    explicit Bullet(BulletType type, const QPointF& startPos, double angle,
                    int damage, QObject *parent = nullptr);
    ~Bullet();

    void update(double deltaTime);
    void draw(QPainter& painter);

    // 状态控制
    void explode();
    void hitTarget();

    // 获取属性
    QPointF getPosition() const { return m_position; }
    double getRadius() const;
    double getExplosionRadius() const { return m_explosionRadius; }
    int getDamage() const { return m_damage; }
    bool isActive() const { return m_active && !m_markedForDeletion; }
    BulletType getType() const { return m_type; }
    BulletState getState() const { return m_state; }

    // 伤害记录 - 防止重复伤害
    bool hasHitTarget(void* target) const;
    void recordHit(void* target);

    // 设置状态
    void setActive(bool active);
    void markForDeletion() { m_markedForDeletion = true; }

    // 检查是否超出边界
    bool isOutOfBounds(const QRect& bounds) const;

signals:
    void exploded(const QPointF& position, int damage, double radius);
    void finished(Bullet* bullet);

private slots:
    void onCleanupTimer();

private:
    void loadBulletData();
    void createDefaultPixmap();
    void generateNetPoints();
    void updateFlying(double deltaTime);
    void updateExpanding(double deltaTime);
    void updateExploding(double deltaTime);
    void drawFlying(QPainter& painter);
    void drawExpanding(QPainter& painter);
    void drawExploding(QPainter& painter);
    void startCleanupTimer();

    BulletType m_type;
    BulletState m_state;
    QPointF m_position;
    QPointF m_velocity;
    double m_angle;
    int m_damage;
    double m_speed;
    double m_radius;
    bool m_active;
    bool m_markedForDeletion;

    // 防止重复伤害的记录
    QSet<void*> m_hitTargets;

    // 生命周期管理
    double m_lifeTime;
    double m_maxLifeTime;
    QTimer* m_cleanupTimer;

    // 网络扩散效果
    double m_explosionRadius;
    double m_maxExplosionRadius;
    double m_explosionSpeed;
    double m_explosionTimer;
    double m_explosionDuration;

    // 视觉效果
    QPixmap m_pixmap;
    double m_rotationAngle;
    double m_rotationSpeed;

    // 网格动画
    QVector<QPointF> m_netPoints;
    double m_netAnimationTime;
};

#endif // BULLET_H
