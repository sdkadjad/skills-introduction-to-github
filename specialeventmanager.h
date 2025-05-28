#ifndef SPECIALEVENTMANAGER_H
#define SPECIALEVENTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QElapsedTimer>

class SpecialEventManager : public QObject
{
    Q_OBJECT

public:
    enum EventType {
        FISH_FRENZY,        // 鱼群狂潮
        GOLDEN_HOUR,        // 黄金时刻
        ELECTRIC_STORM,     // 电击风暴
        ICE_AGE,           // 冰河时代
        BONUS_ROUND,       // 奖励回合
        MEGA_BOSS,         // 超级BOSS
        DOUBLE_COINS,      // 双倍金币
        SLOW_MOTION,       // 慢动作
        SHIELD_BREAK,      // 破盾时刻
        PHANTOM_WAVE       // 幽灵波
    };

    struct SpecialEvent {
        EventType type;
        QString name;
        QString description;
        double duration;
        double probability;
        int minScore;           // 触发所需最低分数
        bool isActive;
        double remainingTime;

        SpecialEvent(EventType t, const QString& n, const QString& d,
                     double dur, double prob, int score)
            : type(t), name(n), description(d), duration(dur),
            probability(prob), minScore(score), isActive(false), remainingTime(0) {}
    };

    explicit SpecialEventManager(QObject *parent = nullptr);

    void update(double deltaTime);
    void checkEventTriggers(int currentScore, int fishCount);
    void triggerEvent(EventType type);
    void endEvent(EventType type);

    bool isEventActive(EventType type) const;
    QString getCurrentEventDescription() const;
    QVector<SpecialEvent> getActiveEvents() const;

signals:
    void eventStarted(EventType type, const QString& name, const QString& description);
    void eventEnded(EventType type, const QString& name);
    void eventUpdate(EventType type, double remainingTime);

private slots:
    void onRandomEventTimer();

private:
    void initializeEvents();
    void updateActiveEvents(double deltaTime);

    QVector<SpecialEvent> m_events;
    QTimer* m_randomEventTimer;
    QElapsedTimer m_lastEventTime;

    double m_randomEventInterval;   // 随机事件间隔
    int m_currentScore;
    bool m_eventsEnabled;
};

#endif // SPECIALEVENTMANAGER_H
