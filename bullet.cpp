#include "bullet.h"
#include "gamemath.h"
#include <QRandomGenerator>
#include <QDebug>

Bullet::Bullet(BulletType type, const QPointF& startPos, double angle,
               int damage, QObject *parent)
    : QObject(parent), m_type(type), m_state(FLYING), m_position(startPos),
    m_angle(angle), m_damage(damage), m_active(true), m_markedForDeletion(false),
    m_lifeTime(0), m_rotationAngle(0), m_rotationSpeed(360),
    m_explosionRadius(0), m_explosionTimer(0), m_netAnimationTime(0)
{
    loadBulletData();

    // 计算速度向量
    m_velocity = QPointF(m_speed * qCos(angle), m_speed * qSin(angle));

    // 初始化网格点
    generateNetPoints();

    // 设置清理定时器
    m_cleanupTimer = new QTimer(this);
    m_cleanupTimer->setSingleShot(true);
    connect(m_cleanupTimer, &QTimer::timeout, this, &Bullet::onCleanupTimer);

    // 设置最大生命时间（防止子弹永远存在）
    m_maxLifeTime = 10.0; // 10秒后自动清理

    qDebug() << "Bullet created at" << m_position << "angle" << angle << "type" << m_type;
}

Bullet::~Bullet()
{
    if (m_cleanupTimer) {
        m_cleanupTimer->stop();
    }
    qDebug() << "Bullet destroyed";
}

void Bullet::loadBulletData()
{
    switch (m_type) {
    case NORMAL_NET:
        m_speed = 300;
        m_radius = 15;
        m_maxExplosionRadius = 80;
        m_explosionSpeed = 150;
        m_explosionDuration = 1.0;
        break;
    case SILVER_NET:
        m_speed = 350;
        m_radius = 20;
        m_maxExplosionRadius = 100;
        m_explosionSpeed = 180;
        m_explosionDuration = 1.2;
        break;
    case GOLD_NET:
        m_speed = 400;
        m_radius = 25;
        m_maxExplosionRadius = 120;
        m_explosionSpeed = 200;
        m_explosionDuration = 1.5;
        break;
    case LASER_BEAM:
        m_speed = 500;
        m_radius = 30;
        m_maxExplosionRadius = 150;
        m_explosionSpeed = 250;
        m_explosionDuration = 2.0;
        break;
    }

    QString imagePath = QString(":/images/bullets/bullet_%1.png").arg((int)m_type);
    if (!m_pixmap.load(imagePath)) {
        createDefaultPixmap();
    }
}

void Bullet::generateNetPoints()
{
    m_netPoints.clear();

    int gridSize = 8;
    double spacing = m_maxExplosionRadius * 2 / gridSize;

    for (int i = 0; i <= gridSize; ++i) {
        for (int j = 0; j <= gridSize; ++j) {
            double x = -m_maxExplosionRadius + i * spacing;
            double y = -m_maxExplosionRadius + j * spacing;

            if (x*x + y*y <= m_maxExplosionRadius * m_maxExplosionRadius) {
                m_netPoints.append(QPointF(x, y));
            }
        }
    }
}

void Bullet::createDefaultPixmap()
{
    int size = qMax(20, (int)(m_radius * 2));
    m_pixmap = QPixmap(size, size);
    m_pixmap.fill(Qt::transparent);

    QPainter painter(&m_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bulletColor;
    switch (m_type) {
    case NORMAL_NET:
        bulletColor = QColor(100, 100, 255, 200);
        break;
    case SILVER_NET:
        bulletColor = QColor(192, 192, 192, 200);
        break;
    case GOLD_NET:
        bulletColor = QColor(255, 215, 0, 200);
        break;
    case LASER_BEAM:
        bulletColor = QColor(255, 0, 255, 220);
        break;
    }

    // 绘制子弹核心
    painter.setBrush(QBrush(bulletColor));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(2, 2, size-4, size-4);

    // 添加内部光芒效果
    painter.setBrush(QBrush(bulletColor.lighter(150)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(size/4, size/4, size/2, size/2);
}

void Bullet::update(double deltaTime)
{
    if (!m_active || m_markedForDeletion) return;

    m_lifeTime += deltaTime;

    // 检查是否超过最大生命时间
    if (m_lifeTime >= m_maxLifeTime) {
        qDebug() << "Bullet exceeded max lifetime, marking for deletion";
        setActive(false);
        return;
    }

    switch (m_state) {
    case FLYING:
        updateFlying(deltaTime);
        break;
    case EXPANDING:
        updateExpanding(deltaTime);
        break;
    case EXPLODING:
        updateExploding(deltaTime);
        break;
    }

    m_netAnimationTime += deltaTime;
}

void Bullet::updateFlying(double deltaTime)
{
    // 更新位置
    m_position += m_velocity * deltaTime;

    // 更新旋转动画
    m_rotationAngle += m_rotationSpeed * deltaTime;
    if (m_rotationAngle >= 360) {
        m_rotationAngle -= 360;
    }

    // 记录位置变化用于调试
    // qDebug() << "Bullet flying at" << m_position;
}

void Bullet::updateExpanding(double deltaTime)
{
    m_explosionTimer += deltaTime;

    if (m_explosionRadius < m_maxExplosionRadius) {
        m_explosionRadius += m_explosionSpeed * deltaTime;
        if (m_explosionRadius >= m_maxExplosionRadius) {
            m_explosionRadius = m_maxExplosionRadius;
            m_state = EXPLODING;
            m_explosionTimer = 0;
            qDebug() << "Bullet entering exploding state";
        }
    }
}

void Bullet::updateExploding(double deltaTime)
{
    m_explosionTimer += deltaTime;

    if (m_explosionTimer >= m_explosionDuration) {
        qDebug() << "Bullet explosion finished, marking for deletion";
        setActive(false);
    }
}

void Bullet::draw(QPainter& painter)
{
    if (!m_active || m_markedForDeletion) return;

    switch (m_state) {
    case FLYING:
        drawFlying(painter);
        break;
    case EXPANDING:
        drawExpanding(painter);
        break;
    case EXPLODING:
        drawExploding(painter);
        break;
    }
}

void Bullet::drawFlying(QPainter& painter)
{
    painter.save();

    // 移动到子弹位置并旋转
    painter.translate(m_position);
    painter.rotate(m_rotationAngle);

    // 绘制子弹
    QPointF drawPos(-m_pixmap.width()/2.0, -m_pixmap.height()/2.0);
    painter.drawPixmap(drawPos, m_pixmap);

    // 绘制拖尾效果
    painter.setOpacity(0.5);
    QPointF normalizedVelocity = GameMath::normalize(m_velocity);

    for (int i = 1; i <= 3; ++i) {
        QPointF trailPos = -normalizedVelocity * (i * 8);
        double trailRadius = m_radius - i * 2;
        if (trailRadius > 0) {
            painter.drawEllipse(trailPos, trailRadius, trailRadius);
        }
    }

    painter.restore();
}

void Bullet::drawExpanding(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);

    QColor netColor;
    switch (m_type) {
    case NORMAL_NET: netColor = QColor(100, 100, 255, 180); break;
    case SILVER_NET: netColor = QColor(192, 192, 192, 180); break;
    case GOLD_NET: netColor = QColor(255, 215, 0, 180); break;
    case LASER_BEAM: netColor = QColor(255, 0, 255, 200); break;
    }

    // 绘制网络圆圈
    painter.setBrush(QBrush(QColor(netColor.red(), netColor.green(), netColor.blue(), 50)));
    painter.setPen(QPen(netColor, 3));
    painter.drawEllipse(-m_explosionRadius, -m_explosionRadius,
                        m_explosionRadius * 2, m_explosionRadius * 2);

    // 绘制网格线
    painter.setPen(QPen(netColor, 2));
    double gridSpacing = m_explosionRadius / 4;

    // 垂直线
    for (int i = -4; i <= 4; ++i) {
        double x = i * gridSpacing;
        if (qAbs(x) <= m_explosionRadius) {
            double y1 = -qSqrt(qMax(0.0, m_explosionRadius*m_explosionRadius - x*x));
            double y2 = qSqrt(qMax(0.0, m_explosionRadius*m_explosionRadius - x*x));
            painter.drawLine(QPointF(x, y1), QPointF(x, y2));
        }
    }

    // 水平线
    for (int i = -4; i <= 4; ++i) {
        double y = i * gridSpacing;
        if (qAbs(y) <= m_explosionRadius) {
            double x1 = -qSqrt(qMax(0.0, m_explosionRadius*m_explosionRadius - y*y));
            double x2 = qSqrt(qMax(0.0, m_explosionRadius*m_explosionRadius - y*y));
            painter.drawLine(QPointF(x1, y), QPointF(x2, y));
        }
    }

    // 绘制闪烁的网格点
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::NoPen);

    double pulseAlpha = 0.5 + 0.5 * qSin(m_netAnimationTime * 8);
    painter.setOpacity(pulseAlpha);

    for (const QPointF& point : m_netPoints) {
        if (GameMath::distance(QPointF(0, 0), point) <= m_explosionRadius) {
            painter.drawEllipse(point, 3, 3);
        }
    }

    painter.restore();
}

void Bullet::drawExploding(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);

    double fadeRatio = 1.0 - (m_explosionTimer / m_explosionDuration);

    QColor netColor;
    switch (m_type) {
    case NORMAL_NET: netColor = QColor(100, 100, 255, 100 * fadeRatio); break;
    case SILVER_NET: netColor = QColor(192, 192, 192, 100 * fadeRatio); break;
    case GOLD_NET: netColor = QColor(255, 215, 0, 100 * fadeRatio); break;
    case LASER_BEAM: netColor = QColor(255, 0, 255, 120 * fadeRatio); break;
    }

    painter.setPen(QPen(netColor, 2));
    painter.setBrush(Qt::NoBrush);

    // 绘制消散的网络圆圈
    for (int i = 0; i < 3; ++i) {
        double radius = m_maxExplosionRadius * (1 + i * 0.2) * fadeRatio;
        painter.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    }

    painter.restore();
}

void Bullet::explode()
{
    if (m_state == FLYING && m_active) {
        qDebug() << "Bullet exploding at" << m_position;
        m_state = EXPANDING;
        m_explosionTimer = 0;
        emit exploded(m_position, m_damage, m_maxExplosionRadius);
    }
}

void Bullet::hitTarget()
{
    if (m_state == FLYING && m_active) {
        qDebug() << "Bullet: Hit target, exploding";
        explode();
    }
}

void Bullet::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        if (!active) {
            qDebug() << "Bullet deactivated, starting cleanup";
            startCleanupTimer();
        }
    }
}

void Bullet::startCleanupTimer()
{
    if (m_cleanupTimer && !m_cleanupTimer->isActive()) {
        // 延迟一小段时间再发送finished信号，确保绘制完成
        m_cleanupTimer->start(100); // 100ms后清理
    }
}

void Bullet::onCleanupTimer()
{
    if (!m_markedForDeletion) {
        m_markedForDeletion = true;
        qDebug() << "Bullet cleanup timer triggered, emitting finished signal";
        emit finished(this);
    }
}

double Bullet::getRadius() const
{
    switch (m_state) {
    case FLYING:
        return m_radius;
    case EXPANDING:
    case EXPLODING:
        return m_explosionRadius;
    default:
        return m_radius;
    }
}

bool Bullet::isOutOfBounds(const QRect& bounds) const
{
    if (m_state != FLYING) return false;

    // 扩大边界检查范围，避免过早删除
    QRect expandedBounds = bounds.adjusted(-100, -100, 100, 100);
    return !expandedBounds.contains(m_position.toPoint());
}
bool Bullet::hasHitTarget(void* target) const
{
    return m_hitTargets.contains(target);
}

void Bullet::recordHit(void* target)
{
    m_hitTargets.insert(target);
    qDebug() << "Bullet: Recorded hit on target" << target;
}

// 在 hitTarget() 方法中确保只触发一次
