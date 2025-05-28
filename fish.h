#ifndef FISH_H
#define FISH_H

#include <QObject>
#include <QPointF>
#include <QPixmap>
#include <QPainter>
#include <QVector>
#include <QTimer>

class Fish : public QObject
{
    Q_OBJECT

public:
    enum FishType {
        SMALL_FISH,         // 小鱼
        MEDIUM_FISH,        // 中鱼
        LARGE_FISH,         // 大鱼
        BOSS_FISH,          // BOSS鱼
        SPECIAL_FISH,       // 特殊鱼
        ELECTRIC_FISH,      // 电鱼（能电击周围的鱼）
        SHIELD_FISH,        // 护盾鱼（有护盾）
        SPEED_FISH,         // 极速鱼（速度极快）
        BOMB_FISH,          // 炸弹鱼（死亡时爆炸伤害周围）
        GOLD_FISH,          // 金鱼（高价值，难捕获）
        ICE_FISH,           // 冰鱼（死亡时冰冻周围）
        MULTIPLIER_FISH,    // 倍数鱼（击杀获得倍数奖励）
        PHANTOM_FISH,       // 幽灵鱼（间歇性隐身）
        REGENERATE_FISH     // 再生鱼（缓慢恢复血量）
    };

    enum FishState {
        NORMAL,
        FROZEN,
        ELECTRIFIED,
        INVISIBLE,
        SHIELDED
    };

    explicit Fish(FishType type, const QPointF& startPos, QObject *parent = nullptr);

    void update(double deltaTime);
    void draw(QPainter& painter);

    // 战斗相关
    void takeDamage(int damage);
    bool isAlive() const { return m_currentHP > 0; }
    void freeze(double duration);
    void electrify(double duration);
    void shield(double duration);
    void makeInvisible(double duration);

    // 获取属性
    QPointF getPosition() const { return m_position; }
    double getRadius() const { return m_radius; }
    FishType getType() const { return m_type; }
    FishState getState() const { return m_state; }
    int getCurrentHP() const { return m_currentHP; }
    int getMaxHP() const { return m_maxHP; }
    int getCoinReward() const;
    int getScore() const;
    bool isVisible() const { return m_visible; }

    // 路径相关
    void setPath(const QVector<QPointF>& path);
    bool isOutOfBounds(const QRect& bounds) const;

    // 特殊能力
    void triggerSpecialAbility();

signals:
    void died(Fish* fish);
    void specialAbilityTriggered(Fish* fish, const QString& abilityType);

private:
    void loadFishData();
    void createDefaultPixmap();
    void updateMovement(double deltaTime);
    void updateAnimation(double deltaTime);
    void updateSpecialEffects(double deltaTime);
    void updateAbilities(double deltaTime);

    void drawFish(QPainter& painter);
    void drawEffects(QPainter& painter);
    void drawHealthBar(QPainter& painter);
    void drawShield(QPainter& painter);

    FishType m_type;
    FishState m_state;
    QPointF m_position;
    QVector<QPointF> m_path;
    int m_currentPathIndex;
    double m_pathProgress;

    // 基础属性
    int m_currentHP;
    int m_maxHP;
    int m_baseCoinReward;
    int m_baseScore;
    double m_speed;
    double m_baseSpeed;
    double m_radius;

    // 状态效果
    double m_frozenTimer;
    double m_electrifiedTimer;
    double m_shieldTimer;
    double m_invisibleTimer;
    bool m_visible;
    bool m_hasShield;

    // 特殊能力
    double m_abilityTimer;
    double m_abilityCooldown;
    double m_regenerateTimer;
    int m_scoreMultiplier;

    // 视觉效果
    QPixmap m_pixmap;
    double m_animationTime;
    double m_animationSpeed;
    bool m_flipX;
    QColor m_effectColor;
    double m_glowIntensity;

    // 特殊行为
    bool m_canPhase; // 幽灵鱼相位能力
    double m_phaseTimer;
    double m_bombTimer; // 炸弹鱼倒计时
};

#endif // FISH_H
