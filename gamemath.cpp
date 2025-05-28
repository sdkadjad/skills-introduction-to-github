#include "gamemath.h"

double GameMath::distance(const QPointF& p1, const QPointF& p2)
{
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    return qSqrt(dx * dx + dy * dy);
}

double GameMath::angle(const QPointF& from, const QPointF& to)
{
    double dx = to.x() - from.x();
    double dy = to.y() - from.y();
    return qAtan2(dy, dx);
}

double GameMath::toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double GameMath::toDegrees(double radians)
{
    return radians * 180.0 / M_PI;
}

QPointF GameMath::pointFromAngle(const QPointF& origin, double angle, double distance)
{
    double x = origin.x() + distance * qCos(angle);
    double y = origin.y() + distance * qSin(angle);
    return QPointF(x, y);
}

bool GameMath::isPointInCircle(const QPointF& point, const QPointF& center, double radius)
{
    return distance(point, center) <= radius;
}

QPointF GameMath::normalize(const QPointF& vector)
{
    double length = qSqrt(vector.x() * vector.x() + vector.y() * vector.y());
    if (length == 0) return QPointF(0, 0);
    return QPointF(vector.x() / length, vector.y() / length);
}

double GameMath::lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

QPointF GameMath::lerp(const QPointF& a, const QPointF& b, double t)
{
    return QPointF(lerp(a.x(), b.x(), t), lerp(a.y(), b.y(), t));
}
