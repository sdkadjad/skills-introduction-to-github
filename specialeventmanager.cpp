#include "specialeventmanager.h"
#include <QRandomGenerator>
#include <QDebug>

SpecialEventManager::SpecialEventManager(QObject *parent)
    : QObject(parent), m_randomEventInterval(30.0), m_currentScore(0), m_eventsEnabled(true)
{
    initializeEvents();

    m_randomEventTimer = new QTimer(this);
    connect(m_randomEventTimer, &QTimer::timeout, this, &SpecialEventManager::onRandomEventTimer);
    m_randomEventTimer->start(30000); // 每30秒检查一次随机事件

    m_lastEventTime.start();
}

void SpecialEventManager::initializeEvents()
{
    m_events.clear();

    // 初始化所有特殊事件
    m_events.append(SpecialEvent(FISH_FRENZY, "鱼群狂潮",
                                 "大量鱼类涌入！捕鱼效率提升！", 15.0, 0.3, 200));

    m_events.append(SpecialEvent(GOLDEN_HOUR, "黄金时刻",
                                 "所有鱼类变成金鱼！超高价值！", 10.0, 0.15, 500));

    m_events.append(SpecialEvent(ELECTRIC_STORM, "电击风暴",
                                 "电鱼数量大增！连锁电击伤害！", 12.0, 0.2, 300));

    m_events.append(SpecialEvent(ICE_AGE, "冰河时代",
                                 "所有鱼类移动缓慢！", 20.0, 0.25, 150));

    m_events.append(SpecialEvent(BONUS_ROUND, "奖励回合",
                                 "分数和金币获得双倍奖励！", 8.0, 0.2, 400));

    m_events.append(SpecialEvent(MEGA_BOSS, "超级BOSS",
                                 "巨型BOSS鱼出现！击败获得巨额奖励！", 30.0, 0.05, 1000));

    m_events.append(SpecialEvent(DOUBLE_COINS, "双倍金币",
                                 "金币获得翻倍！", 15.0, 0.3, 100));

    m_events.append(SpecialEvent(SLOW_MOTION, "慢动作",
                                 "时间变慢，瞄准更容易！", 10.0, 0.2, 250));

    m_events.append(SpecialEvent(SHIELD_BREAK, "破盾时刻",
                                 "所有护盾鱼失去护盾！", 8.0, 0.15, 350));

    m_events.append(SpecialEvent(PHANTOM_WAVE, "幽灵波",
                                 "幽灵鱼大量出现！", 12.0, 0.1, 600));
}

void SpecialEventManager::update(double deltaTime)
{
    if (!m_eventsEnabled) return;

    updateActiveEvents(deltaTime);
}

void SpecialEventManager::updateActiveEvents(double deltaTime)
{
    for (SpecialEvent& event : m_events) {
        if (event.isActive) {
            event.remainingTime -= deltaTime;

            emit eventUpdate(event.type, event.remainingTime);

            if (event.remainingTime <= 0) {
                endEvent(event.type);
            }
        }
    }
}

void SpecialEventManager::checkEventTriggers(int currentScore, int fishCount)
{
    m_currentScore = currentScore;

    // 基于分数的自动触发
    for (SpecialEvent& event : m_events) {
        if (!event.isActive && currentScore >= event.minScore) {
            // 检查是否应该触发
            if (currentScore % event.minScore < 50) { // 在达到分数后的50分内有机会触发
                double random = QRandomGenerator::global()->generateDouble();
                if (random < event.probability * 0.1) { // 降低自动触发概率
                    triggerEvent(event.type);
                }
            }
        }
    }

    // 特殊条件触发
    if (fishCount > 15 && !isEventActive(FISH_FRENZY)) {
        double random = QRandomGenerator::global()->generateDouble();
        if (random < 0.1) {
            triggerEvent(FISH_FRENZY);
        }
    }
}

void SpecialEventManager::onRandomEventTimer()
{
    if (!m_eventsEnabled) return;

    // 随机选择一个未激活的事件
    QVector<EventType> availableEvents;
    for (const SpecialEvent& event : m_events) {
        if (!event.isActive && m_currentScore >= event.minScore) {
            availableEvents.append(event.type);
        }
    }

    if (!availableEvents.isEmpty()) {
        double random = QRandomGenerator::global()->generateDouble();
        if (random < 0.3) { // 30%概率触发随机事件
            int index = QRandomGenerator::global()->bounded(availableEvents.size());
            triggerEvent(availableEvents[index]);
        }
    }
}

void SpecialEventManager::triggerEvent(EventType type)
{
    for (SpecialEvent& event : m_events) {
        if (event.type == type && !event.isActive) {
            event.isActive = true;
            event.remainingTime = event.duration;

            emit eventStarted(type, event.name, event.description);

            qDebug() << "Special Event Started:" << event.name;
            break;
        }
    }
}

void SpecialEventManager::endEvent(EventType type)
{
    for (SpecialEvent& event : m_events) {
        if (event.type == type && event.isActive) {
            event.isActive = false;
            event.remainingTime = 0;

            emit eventEnded(type, event.name);

            qDebug() << "Special Event Ended:" << event.name;
            break;
        }
    }
}

bool SpecialEventManager::isEventActive(EventType type) const
{
    for (const SpecialEvent& event : m_events) {
        if (event.type == type) {
            return event.isActive;
        }
    }
    return false;
}

QString SpecialEventManager::getCurrentEventDescription() const
{
    QStringList activeEvents;
    for (const SpecialEvent& event : m_events) {
        if (event.isActive) {
            activeEvents.append(QString("%1 (%.1fs)").arg(event.name).arg(event.remainingTime));
        }
    }

    return activeEvents.isEmpty() ? "无特殊事件" : activeEvents.join(" | ");
}

QVector<SpecialEventManager::SpecialEvent> SpecialEventManager::getActiveEvents() const
{
    QVector<SpecialEvent> activeEvents;
    for (const SpecialEvent& event : m_events) {
        if (event.isActive) {
            activeEvents.append(event);
        }
    }
    return activeEvents;
}
