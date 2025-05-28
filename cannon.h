#ifndef CANNON_H
#define CANNON_H

#include <QPointF>
#include <QPixmap>
#include <QPainter>
#include <QTimer>

class Cannon : public QObject
{
    Q_OBJECT

public:
    enum CannonType {
        BASIC_CANNON,
        SILVER_CANNON,
        GOLD_CANNON,
        LASER_CANNON
    };

    explicit Cannon(CannonType type = BASIC_CANNON, QObject *parent = nullptr);

    // 设置炮塔位置
    void setPosition(const QPointF& position);
    QPointF getPosition() const { return m_position; }

    // 设置目标角度
    void setTargetAngle(double angle);
    double getCurrentAngle() const { return m_currentAngle; }

    // 升级炮塔
    void upgrade();
    bool canUpgrade() const;
    int getUpgradeCost() const;

    // 射击
    bool canFire() const;
    void fire();

    // 获取炮塔属性
    int getDamage() const { return m_damage; }
    int getFireRate() const { return m_fireRate; }
    double getRange() const { return m_range; }
    CannonType getType() const { return m_type; }

    // 更新和绘制
    void update(double deltaTime);
    void draw(QPainter& painter);

    // 获取炮口位置
    QPointF getBarrelTip() const;

signals:
    void fired(const QPointF& startPos, double angle, int damage);

private:
    void loadCannonData();
    void updateRotation(double deltaTime);

    CannonType m_type;
    QPointF m_position;
    double m_currentAngle;      // 当前角度
    double m_targetAngle;       // 目标角度
    double m_rotationSpeed;     // 旋转速度

    // 炮塔属性
    int m_damage;
    int m_fireRate;             // 射击间隔(毫秒)
    double m_range;
    int m_upgradeCost;

    // 射击控制
    QTimer* m_fireTimer;
    bool m_canFire;

    // 视觉效果
    QPixmap m_basePixmap;       // 炮塔底座
    QPixmap m_barrelPixmap;     // 炮管
    double m_baseSize;
    double m_barrelLength;
};

#endif // CANNON_H
