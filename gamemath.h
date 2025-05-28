#ifndef GAMEMATH_H
#define GAMEMATH_H

#include <QPoint>
#include <QPointF>
#include <QtMath>

class GameMath
{
public:
    // 计算两点间距离
    static double distance(const QPointF& p1, const QPointF& p2);

    // 计算两点间角度（弧度）
    static double angle(const QPointF& from, const QPointF& to);

    // 角度转弧度
    static double toRadians(double degrees);

    // 弧度转角度
    static double toDegrees(double radians);

    // 根据角度和距离计算目标点
    static QPointF pointFromAngle(const QPointF& origin, double angle, double distance);

    // 判断点是否在圆形区域内
    static bool isPointInCircle(const QPointF& point, const QPointF& center, double radius);

    // 向量归一化
    static QPointF normalize(const QPointF& vector);

    // 线性插值
    static double lerp(double a, double b, double t);
    static QPointF lerp(const QPointF& a, const QPointF& b, double t);
};

#endif // GAMEMATH_H
