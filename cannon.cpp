#include "cannon.h"
#include "gamemath.h"
#include <QDebug>

Cannon::Cannon(CannonType type, QObject *parent)
    : QObject(parent), m_type(type), m_position(0, 0),
    m_currentAngle(0), m_targetAngle(0), m_rotationSpeed(180), // 180度/秒
    m_canFire(true)
{
    loadCannonData();

    m_fireTimer = new QTimer(this);
    m_fireTimer->setSingleShot(true);
    connect(m_fireTimer, &QTimer::timeout, [this]() {
        m_canFire = true;
    });
}

void Cannon::setPosition(const QPointF& position)
{
    m_position = position;
}

void Cannon::setTargetAngle(double angle)
{
    m_targetAngle = angle;
}

void Cannon::loadCannonData()
{
    switch (m_type) {
    case BASIC_CANNON:
        m_damage = 5;
        m_fireRate = 500;   // 0.5秒
        m_range = 200;
        m_upgradeCost = 100;
        m_baseSize = 40;
        m_barrelLength = 50;
        break;
    case SILVER_CANNON:
        m_damage = 25;
        m_fireRate = 400;
        m_range = 350;
        m_upgradeCost = 500;
        m_baseSize = 45;
        m_barrelLength = 55;
        break;
    case GOLD_CANNON:
        m_damage = 50;
        m_fireRate = 300;
        m_range = 400;
        m_upgradeCost = 1000;
        m_baseSize = 50;
        m_barrelLength = 60;
        break;
    case LASER_CANNON:
        m_damage = 100;
        m_fireRate = 200;
        m_range = 500;
        m_upgradeCost = 0;  // 最高级，不能再升级
        m_baseSize = 55;
        m_barrelLength = 65;
        break;
    }

    // 尝试加载图片，如果失败则创建默认图片
    QString basePath = QString(":/images/cannons/cannon_%1_base.png").arg((int)m_type);
    QString barrelPath = QString(":/images/cannons/cannon_%1_barrel.png").arg((int)m_type);

    if (!m_basePixmap.load(basePath)) {
        // 创建默认底座
        m_basePixmap = QPixmap(m_baseSize, m_baseSize);
        m_basePixmap.fill(Qt::transparent);
        QPainter painter(&m_basePixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        QColor baseColor;
        switch (m_type) {
        case BASIC_CANNON: baseColor = QColor(139, 69, 19); break;  // 棕色
        case SILVER_CANNON: baseColor = QColor(192, 192, 192); break; // 银色
        case GOLD_CANNON: baseColor = QColor(255, 215, 0); break;   // 金色
        case LASER_CANNON: baseColor = QColor(138, 43, 226); break; // 紫色
        }

        painter.setBrush(QBrush(baseColor));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(0, 0, m_baseSize, m_baseSize);
    }

    if (!m_barrelPixmap.load(barrelPath)) {
        // 创建默认炮管
        m_barrelPixmap = QPixmap(m_barrelLength, 20);
        m_barrelPixmap.fill(Qt::transparent);
        QPainter painter(&m_barrelPixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        QColor barrelColor;
        switch (m_type) {
        case BASIC_CANNON: barrelColor = QColor(105, 105, 105); break; // 暗灰色
        case SILVER_CANNON: barrelColor = QColor(169, 169, 169); break; // 淡灰色
        case GOLD_CANNON: barrelColor = QColor(218, 165, 32); break;   // 深金色
        case LASER_CANNON: barrelColor = QColor(75, 0, 130); break;    // 靛青色
        }

        painter.setBrush(QBrush(barrelColor));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRoundedRect(0, 5, m_barrelLength, 10, 5, 5);
    }
}

bool Cannon::canFire() const
{
    return m_canFire;
}

void Cannon::fire()
{
    if (!canFire()) return;

    m_canFire = false;
    m_fireTimer->start(m_fireRate);

    QPointF startPos = getBarrelTip();
    emit fired(startPos, m_currentAngle, m_damage);
}

void Cannon::upgrade()
{
    if (!canUpgrade()) return;

    CannonType newType = (CannonType)((int)m_type + 1);
    m_type = newType;
    loadCannonData();
}

bool Cannon::canUpgrade() const
{
    return m_type != LASER_CANNON;
}

int Cannon::getUpgradeCost() const
{
    return m_upgradeCost;
}

void Cannon::update(double deltaTime)
{
    updateRotation(deltaTime);
}

void Cannon::updateRotation(double deltaTime)
{
    double angleDiff = m_targetAngle - m_currentAngle;

    // 处理角度跨越180度的情况
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;

    if (qAbs(angleDiff) > 0.01) {
        double rotationStep = GameMath::toRadians(m_rotationSpeed) * deltaTime;
        if (qAbs(angleDiff) <= rotationStep) {
            m_currentAngle = m_targetAngle;
        } else {
            m_currentAngle += (angleDiff > 0 ? rotationStep : -rotationStep);
        }
    }
}

void Cannon::draw(QPainter& painter)
{
    painter.save();

    // 绘制底座
    QPointF baseCenter = m_position - QPointF(m_baseSize/2, m_baseSize/2);
    painter.drawPixmap(baseCenter, m_basePixmap);

    // 绘制炮管（需要旋转）
    painter.translate(m_position);
    painter.rotate(GameMath::toDegrees(m_currentAngle));

    QPointF barrelPos(-5, -m_barrelPixmap.height()/2);
    painter.drawPixmap(barrelPos, m_barrelPixmap);

    painter.restore();
}

QPointF Cannon::getBarrelTip() const
{
    return GameMath::pointFromAngle(m_position, m_currentAngle, m_barrelLength - 5);
}
