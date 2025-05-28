#include "fish.h"
#include "gamemath.h"
#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>

Fish::Fish(FishType type, const QPointF& startPos, QObject *parent)
    : QObject(parent), m_type(type), m_state(NORMAL), m_position(startPos),
    m_currentPathIndex(0), m_pathProgress(0), m_animationTime(0),
    m_flipX(false), m_frozenTimer(0), m_electrifiedTimer(0),
    m_shieldTimer(0), m_invisibleTimer(0), m_visible(true),
    m_hasShield(false), m_abilityTimer(0), m_regenerateTimer(0),
    m_scoreMultiplier(1), m_glowIntensity(0), m_canPhase(false),
    m_phaseTimer(0), m_bombTimer(0)
{
    loadFishData();
}

void Fish::loadFishData()
{
    switch (m_type) {
    case SMALL_FISH:
        m_maxHP = 10;
        m_speed = 50;
        m_radius = 15;
        m_baseCoinReward = 1;
        m_baseScore = 10;
        m_animationSpeed = 2.0;
        m_abilityCooldown = 0;
        break;

    case MEDIUM_FISH:
        m_maxHP = 25;
        m_speed = 40;
        m_radius = 25;
        m_baseCoinReward = 3;
        m_baseScore = 30;
        m_animationSpeed = 1.5;
        m_abilityCooldown = 0;
        break;

    case LARGE_FISH:
        m_maxHP = 50;
        m_speed = 30;
        m_radius = 35;
        m_baseCoinReward = 8;
        m_baseScore = 80;
        m_animationSpeed = 1.0;
        m_abilityCooldown = 0;
        break;

    case BOSS_FISH:
        m_maxHP = 200;
        m_speed = 20;
        m_radius = 50;
        m_baseCoinReward = 50;
        m_baseScore = 500;
        m_animationSpeed = 0.8;
        m_abilityCooldown = 5.0;
        break;

    case SPECIAL_FISH:
        m_maxHP = 30;
        m_speed = 60;
        m_radius = 20;
        m_baseCoinReward = 15;
        m_baseScore = 150;
        m_animationSpeed = 3.0;
        m_abilityCooldown = 3.0;
        break;

    case ELECTRIC_FISH:
        m_maxHP = 40;
        m_speed = 35;
        m_radius = 30;
        m_baseCoinReward = 12;
        m_baseScore = 120;
        m_animationSpeed = 2.0;
        m_abilityCooldown = 4.0;
        break;

    case SHIELD_FISH:
        m_maxHP = 60;
        m_speed = 25;
        m_radius = 32;
        m_baseCoinReward = 20;
        m_baseScore = 200;
        m_animationSpeed = 1.2;
        m_abilityCooldown = 0;
        m_hasShield = true;
        m_shieldTimer = 999; // 永久护盾直到被破坏
        break;

    case SPEED_FISH:
        m_maxHP = 15;
        m_speed = 120;
        m_radius = 18;
        m_baseCoinReward = 25;
        m_baseScore = 250;
        m_animationSpeed = 4.0;
        m_abilityCooldown = 2.0;
        break;

    case BOMB_FISH:
        m_maxHP = 35;
        m_speed = 40;
        m_radius = 28;
        m_baseCoinReward = 18;
        m_baseScore = 180;
        m_animationSpeed = 1.8;
        m_abilityCooldown = 0;
        m_bombTimer = 10.0; // 10秒后自爆
        break;

    case GOLD_FISH:
        m_maxHP = 80;
        m_speed = 45;
        m_radius = 25;
        m_baseCoinReward = 100;
        m_baseScore = 1000;
        m_animationSpeed = 1.5;
        m_abilityCooldown = 6.0;
        break;

    case ICE_FISH:
        m_maxHP = 45;
        m_speed = 30;
        m_radius = 30;
        m_baseCoinReward = 15;
        m_baseScore = 150;
        m_animationSpeed = 1.0;
        m_abilityCooldown = 0;
        break;

    case MULTIPLIER_FISH:
        m_maxHP = 25;
        m_speed = 50;
        m_radius = 22;
        m_baseCoinReward = 5;
        m_baseScore = 50;
        m_animationSpeed = 2.5;
        m_abilityCooldown = 0;
        m_scoreMultiplier = QRandomGenerator::global()->bounded(2, 6); // 2-5倍
        break;

    case PHANTOM_FISH:
        m_maxHP = 20;
        m_speed = 55;
        m_radius = 20;
        m_baseCoinReward = 30;
        m_baseScore = 300;
        m_animationSpeed = 2.2;
        m_abilityCooldown = 3.0;
        m_canPhase = true;
        break;

    case REGENERATE_FISH:
        m_maxHP = 100;
        m_speed = 20;
        m_radius = 40;
        m_baseCoinReward = 35;
        m_baseScore = 350;
        m_animationSpeed = 0.8;
        m_abilityCooldown = 0;
        m_regenerateTimer = 1.0; // 每秒回血
        break;
    }

    m_currentHP = m_maxHP;
    m_baseSpeed = m_speed;

    // 加载图片
    QString imagePath = QString(":/images/fishes/fish_%1.png").arg((int)m_type);
    if (!m_pixmap.load(imagePath)) {
        createDefaultPixmap();
    }
}

void Fish::createDefaultPixmap()
{
    int size = m_radius * 2;
    m_pixmap = QPixmap(size, size);
    m_pixmap.fill(Qt::transparent);

    QPainter painter(&m_pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor fishColor;
    switch (m_type) {
    case SMALL_FISH: fishColor = QColor(255, 200, 0); break;
    case MEDIUM_FISH: fishColor = QColor(0, 200, 255); break;
    case LARGE_FISH: fishColor = QColor(255, 100, 100); break;
    case BOSS_FISH: fishColor = QColor(128, 0, 128); break;
    case SPECIAL_FISH: fishColor = QColor(255, 100, 255); break;
    case ELECTRIC_FISH: fishColor = QColor(255, 255, 0); break;
    case SHIELD_FISH: fishColor = QColor(0, 255, 255); break;
    case SPEED_FISH: fishColor = QColor(255, 0, 0); break;
    case BOMB_FISH: fishColor = QColor(255, 165, 0); break;
    case GOLD_FISH: fishColor = QColor(255, 215, 0); break;
    case ICE_FISH: fishColor = QColor(173, 216, 230); break;
    case MULTIPLIER_FISH: fishColor = QColor(148, 0, 211); break;
    case PHANTOM_FISH: fishColor = QColor(128, 128, 128); break;
    case REGENERATE_FISH: fishColor = QColor(0, 255, 0); break;
    }

    // 绘制鱼身体
    painter.setBrush(QBrush(fishColor));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(5, m_radius/2, size-10, m_radius);

    // 绘制特殊标记
    switch (m_type) {
    case ELECTRIC_FISH:
        // 绘制闪电标记
        painter.setBrush(QBrush(Qt::yellow));
        painter.drawEllipse(size-12, m_radius/2 + 2, 4, 4);
        break;
    case BOMB_FISH:
        // 绘制引信
        painter.setPen(QPen(Qt::red, 3));
        painter.drawLine(size-8, m_radius/2, size-4, m_radius/2 - 4);
        break;
    case GOLD_FISH:
        // 绘制金币标记
        painter.setBrush(QBrush(Qt::yellow));
        painter.drawEllipse(size-10, m_radius/2, 6, 6);
        break;
    default:
        break;
    }

    // 绘制鱼尾巴
    QPolygon tail;
    tail << QPoint(5, m_radius/2)
         << QPoint(5, m_radius + m_radius/2)
         << QPoint(0, m_radius);
    painter.setBrush(QBrush(fishColor.darker()));
    painter.drawPolygon(tail);

    // 绘制鱼眼睛
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(size-15, m_radius/2 + 2, 6, 6);
    painter.setBrush(QBrush(Qt::black));
    painter.drawEllipse(size-13, m_radius/2 + 4, 2, 2);
}

void Fish::update(double deltaTime)
{
    if (!isAlive()) return;

    updateMovement(deltaTime);
    updateAnimation(deltaTime);
    updateSpecialEffects(deltaTime);
    updateAbilities(deltaTime);
}

void Fish::updateSpecialEffects(double deltaTime)
{
    // 更新状态效果计时器
    if (m_frozenTimer > 0) {
        m_frozenTimer -= deltaTime;
        if (m_frozenTimer <= 0) {
            m_state = NORMAL;
            m_speed = m_baseSpeed;
        }
    }

    if (m_electrifiedTimer > 0) {
        m_electrifiedTimer -= deltaTime;
        if (m_electrifiedTimer <= 0) {
            m_state = NORMAL;
        }
    }

    if (m_shieldTimer > 0) {
        m_shieldTimer -= deltaTime;
        if (m_shieldTimer <= 0) {
            m_hasShield = false;
            m_state = NORMAL;
        }
    }

    if (m_invisibleTimer > 0) {
        m_invisibleTimer -= deltaTime;
        if (m_invisibleTimer <= 0) {
            m_visible = true;
            m_state = NORMAL;
        }
    }

    // 更新发光效果
    if (m_state == ELECTRIFIED) {
        m_glowIntensity = 0.5 + 0.5 * qSin(m_animationTime * 10);
    } else {
        m_glowIntensity = 0;
    }
}

void Fish::updateAbilities(double deltaTime)
{
    m_abilityTimer += deltaTime;

    switch (m_type) {
    case BOMB_FISH:
        m_bombTimer -= deltaTime;
        if (m_bombTimer <= 0) {
            // 自爆
            emit specialAbilityTriggered(this, "bomb_explode");
            m_currentHP = 0;
        }
        break;

    case REGENERATE_FISH:
        m_regenerateTimer -= deltaTime;
        if (m_regenerateTimer <= 0 && m_currentHP < m_maxHP) {
            m_currentHP = qMin(m_maxHP, m_currentHP + 2); // 每秒回复2点血量
            m_regenerateTimer = 1.0;
        }
        break;

    case PHANTOM_FISH:
        if (m_canPhase) {
            m_phaseTimer -= deltaTime;
            if (m_phaseTimer <= 0) {
                makeInvisible(2.0); // 隐身2秒
                m_phaseTimer = 5.0; // 5秒后再次隐身
            }
        }
        break;

    default:
        // 触发主动技能
        if (m_abilityTimer >= m_abilityCooldown && m_abilityCooldown > 0) {
            triggerSpecialAbility();
            m_abilityTimer = 0;
        }
        break;
    }
}

void Fish::updateMovement(double deltaTime)
{
    if (m_state == FROZEN || m_path.isEmpty()) return;

    if (m_currentPathIndex >= m_path.size() - 1) return;

    QPointF start = m_path[m_currentPathIndex];
    QPointF end = m_path[m_currentPathIndex + 1];

    double segmentLength = GameMath::distance(start, end);
    double moveDistance = m_speed * deltaTime;
    double progressIncrease = moveDistance / segmentLength;

    m_pathProgress += progressIncrease;

    if (m_pathProgress >= 1.0) {
        m_currentPathIndex++;
        m_pathProgress = 0.0;

        if (m_currentPathIndex < m_path.size() - 1) {
            start = m_path[m_currentPathIndex];
            end = m_path[m_currentPathIndex + 1];
            m_flipX = (end.x() < start.x());
        }
    }

    if (m_currentPathIndex < m_path.size() - 1) {
        start = m_path[m_currentPathIndex];
        end = m_path[m_currentPathIndex + 1];
        m_position = GameMath::lerp(start, end, m_pathProgress);
    }
}

void Fish::updateAnimation(double deltaTime)
{
    m_animationTime += deltaTime * m_animationSpeed;
    if (m_animationTime >= 2 * M_PI) {
        m_animationTime -= 2 * M_PI;
    }
}

void Fish::draw(QPainter& painter)
{
    if (!isAlive() || !m_visible) return;

    drawFish(painter);
    drawEffects(painter);
    drawHealthBar(painter);

    if (m_hasShield) {
        drawShield(painter);
    }
}

void Fish::drawFish(QPainter& painter)
{
    painter.save();

    // 幽灵鱼透明度效果
    if (m_type == PHANTOM_FISH && m_visible) {
        painter.setOpacity(0.7);
    }

    painter.translate(m_position);

    if (m_flipX) {
        painter.scale(-1, 1);
    }

    // 摆动动画
    double swayAngle = qSin(m_animationTime) * 5;
    painter.rotate(swayAngle);

    // 发光效果
    if (m_glowIntensity > 0) {
        painter.setOpacity(m_glowIntensity);
        painter.setBrush(QBrush(Qt::yellow));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(-m_radius-5, -m_radius-5, (m_radius+5)*2, (m_radius+5)*2);
        painter.setOpacity(1.0);
    }

    QPointF drawPos(-m_pixmap.width()/2, -m_pixmap.height()/2);
    painter.drawPixmap(drawPos, m_pixmap);

    painter.restore();
}

void Fish::drawEffects(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);

    switch (m_state) {
    case FROZEN:
        // 冰冻效果
        painter.setBrush(QBrush(QColor(173, 216, 230, 100)));
        painter.setPen(QPen(QColor(135, 206, 250), 2));
        painter.drawEllipse(-m_radius-3, -m_radius-3, (m_radius+3)*2, (m_radius+3)*2);
        break;

    case ELECTRIFIED:
        // 电击效果
        painter.setPen(QPen(Qt::yellow, 3));
        for (int i = 0; i < 5; ++i) {
            double angle = (i * 72 + m_animationTime * 180) * M_PI / 180;
            QPointF start = GameMath::pointFromAngle(QPointF(0, 0), angle, m_radius);
            QPointF end = GameMath::pointFromAngle(QPointF(0, 0), angle, m_radius + 10);
            painter.drawLine(start, end);
        }
        break;

    default:
        break;
    }

    // 特殊鱼类的额外效果
    switch (m_type) {
    case BOMB_FISH:
        // 闪烁警告（时间不多时）
        if (m_bombTimer < 3.0) {
            double flashIntensity = qSin(m_animationTime * 15);
            if (flashIntensity > 0) {
                painter.setBrush(QBrush(QColor(255, 0, 0, 100)));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(-m_radius, -m_radius, m_radius*2, m_radius*2);
            }
        }
        break;

    case GOLD_FISH:
    {
        // 金光效果
        painter.setPen(QPen(QColor(255, 215, 0, 150), 2));
        painter.setBrush(Qt::NoBrush);
        double goldRadius = m_radius + 5 + 3 * qSin(m_animationTime * 3);
        painter.drawEllipse(-goldRadius, -goldRadius, goldRadius*2, goldRadius*2);
    }
    break;

    case MULTIPLIER_FISH:
        // 显示倍数
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(-10, -m_radius-10, QString("x%1").arg(m_scoreMultiplier));
        break;

    default:
        break;
    }

    painter.restore();
}

void Fish::drawHealthBar(QPainter& painter)
{
    if (m_currentHP >= m_maxHP) return;

    double hpRatio = (double)m_currentHP / m_maxHP;
    int barWidth = m_radius * 2;
    int barHeight = 4;

    QPointF barPos = m_position - QPointF(barWidth/2, m_radius + 10);

    // 背景条
    painter.setBrush(QBrush(Qt::red));
    painter.setPen(Qt::NoPen);
    painter.drawRect(barPos.x(), barPos.y(), barWidth, barHeight);

    // 当前血量条
    QColor healthColor = Qt::green;
    if (hpRatio < 0.3) healthColor = Qt::red;
    else if (hpRatio < 0.6) healthColor = Qt::yellow;

    painter.setBrush(QBrush(healthColor));
    painter.drawRect(barPos.x(), barPos.y(), barWidth * hpRatio, barHeight);
}

void Fish::drawShield(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);

    double shieldRadius = m_radius + 8;

    // 护盾效果
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(0, 255, 255, 150), 3));

    // 旋转的护盾环
    painter.rotate(m_animationTime * 50);
    for (int i = 0; i < 6; ++i) {
        double angle = i * 60 * M_PI / 180;
        painter.drawArc(QRectF(-shieldRadius, -shieldRadius, shieldRadius*2, shieldRadius*2),
                        angle * 180 / M_PI * 16, 60 * 16);
    }

    painter.restore();
}

void Fish::takeDamage(int damage)
{
    if (m_hasShield) {
        // 护盾吸收伤害
        m_hasShield = false;
        m_shieldTimer = 0;
        return;
    }

    m_currentHP = qMax(0, m_currentHP - damage);

    if (!isAlive()) {
        // 死亡时触发特殊效果
        switch (m_type) {
        case ICE_FISH:
            emit specialAbilityTriggered(this, "ice_explosion");
            break;
        case BOMB_FISH:
            emit specialAbilityTriggered(this, "bomb_explode");
            break;
        case ELECTRIC_FISH:
            emit specialAbilityTriggered(this, "electric_burst");
            break;
        default:
            break;
        }

        emit died(this);
    }
}

void Fish::freeze(double duration)
{
    m_state = FROZEN;
    m_frozenTimer = duration;
    m_speed = 0;
}

void Fish::electrify(double duration)
{
    m_state = ELECTRIFIED;
    m_electrifiedTimer = duration;
}

void Fish::shield(double duration)
{
    m_hasShield = true;
    m_shieldTimer = duration;
    m_state = SHIELDED;
}

void Fish::makeInvisible(double duration)
{
    m_visible = false;
    m_invisibleTimer = duration;
    m_state = INVISIBLE;
}

void Fish::triggerSpecialAbility()
{
    switch (m_type) {
    case ELECTRIC_FISH:
        emit specialAbilityTriggered(this, "electric_pulse");
        break;
    case SPEED_FISH:
        m_speed = m_baseSpeed * 2; // 短时间加速
        QTimer::singleShot(2000, [this]() { m_speed = m_baseSpeed; });
        break;
    case GOLD_FISH:
        emit specialAbilityTriggered(this, "coin_shower");
        break;
    default:
        break;
    }
}

int Fish::getCoinReward() const
{
    return m_baseCoinReward * m_scoreMultiplier;
}

int Fish::getScore() const
{
    return m_baseScore * m_scoreMultiplier;
}

void Fish::setPath(const QVector<QPointF>& path)
{
    m_path = path;
    m_currentPathIndex = 0;
    m_pathProgress = 0;

    if (!path.isEmpty()) {
        m_position = path.first();
    }
}

bool Fish::isOutOfBounds(const QRect& bounds) const
{
    QRect expandedBounds = bounds.adjusted(-100, -100, 100, 100);
    return !expandedBounds.contains(m_position.toPoint());
}
