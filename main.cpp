#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStyleFactory>
#include <csignal>
#include "gamewindow.h"

// 全局崩溃处理函数
void crashHandler(int signal)
{
    QString crashInfo = QString("Game crashed with signal: %1\nTime: %2\nUser: sdkadjad")
    .arg(signal)
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    qDebug() << "CRASH:" << crashInfo;

    // 保存崩溃日志
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logPath);

    QFile logFile(logPath + "/crash_log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&logFile);
        stream << crashInfo << Qt::endl;
        stream << "=== Stack Trace ===" << Qt::endl;
        stream << "Application was running in game mode" << Qt::endl;
        stream << "====================" << Qt::endl << Qt::endl;
    }

    // 显示崩溃信息
    QMessageBox::critical(nullptr, "游戏崩溃",
                          QString("🚨 游戏遇到了一个严重错误 (信号: %1)\n\n"
                                  "💾 崩溃日志已保存到:\n%2\n\n"
                                  "⚠️ 请重新启动游戏。\n"
                                  "如问题持续出现，请联系技术支持。\n\n"
                                  "👤 用户: sdkadjad\n"
                                  "🕐 时间: %3")
                              .arg(signal)
                              .arg(logPath + "/crash_log.txt")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    std::exit(signal);
}

// 消息输出处理
void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logPath);

    static QFile debugFile(logPath + "/debug.log");
    static bool fileOpened = false;

    if (!fileOpened) {
        fileOpened = debugFile.open(QIODevice::WriteOnly | QIODevice::Append);
        if (fileOpened) {
            QTextStream stream(&debugFile);
            stream << QString("\n=== 新会话开始 %1 ===\n")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            stream << QString("用户: sdkadjad\n");
            stream << QString("应用程序: 捕鱼大战 v1.0\n");
            stream << "================================\n";
            stream.flush();
        }
    }

    if (fileOpened) {
        QTextStream stream(&debugFile);

        QString typeStr;
        switch (type) {
        case QtDebugMsg:    typeStr = "Debug"; break;
        case QtWarningMsg:  typeStr = "Warning"; break;
        case QtCriticalMsg: typeStr = "Critical"; break;
        case QtFatalMsg:    typeStr = "Fatal"; break;
        case QtInfoMsg:     typeStr = "Info"; break;
        }

        stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
               << " [" << typeStr << "] " << msg << Qt::endl;
        stream.flush();
    }

    // 也输出到控制台
    fprintf(stderr, "[%s] %s\n",
            QDateTime::currentDateTime().toString("hh:mm:ss").toLocal8Bit().constData(),
            msg.toLocal8Bit().constData());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置崩溃处理
    std::signal(SIGSEGV, crashHandler);  // 段错误
    std::signal(SIGABRT, crashHandler);  // 异常终止
    std::signal(SIGFPE, crashHandler);   // 浮点异常
    std::signal(SIGILL, crashHandler);   // 非法指令

    // 设置调试消息处理
    qInstallMessageHandler(messageOutput);

    // 设置应用程序信息
    app.setApplicationName("FishWarGame");
    app.setApplicationDisplayName("捕鱼大战");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("GameDev Team");
    app.setOrganizationDomain("gamedev.example.com");

    // 设置应用程序图标
    app.setWindowIcon(QIcon(":/images/icon.png"));

    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));

    // 设置全局样式表
    app.setStyleSheet(
        "QApplication { font-family: 'Microsoft YaHei', 'SimHei', Arial; }"
        "QToolTip { background-color: #2b2b2b; color: white; border: 1px solid #555; "
        "border-radius: 4px; padding: 4px; }"
        );

    qDebug() << "=== 应用程序启动 ===";
    qDebug() << "应用程序名称:" << app.applicationName();
    qDebug() << "应用程序版本:" << app.applicationVersion();
    qDebug() << "用户:" << "sdkadjad";
    qDebug() << "启动时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug() << "Qt版本:" << QT_VERSION_STR;
    qDebug() << "平台:" << QSysInfo::prettyProductName();
    qDebug() << "===================";

    try {
        // 创建并显示主窗口
        GameWindow window;

        // 设置窗口属性
        window.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
        window.setAttribute(Qt::WA_DeleteOnClose);

        // 居中显示窗口
        QRect screenGeometry = app.primaryScreen()->geometry();
        int x = (screenGeometry.width() - window.width()) / 2;
        int y = (screenGeometry.height() - window.height()) / 2;
        window.move(x, y);

        window.show();

        qDebug() << "应用程序启动成功，窗口已显示";
        qDebug() << "窗口位置:" << window.pos();
        qDebug() << "窗口大小:" << window.size();
        qDebug() << "用户sdkadjad已登录，等待游戏操作...";

        int result = app.exec();

        qDebug() << "=== 应用程序退出 ===";
        qDebug() << "退出代码:" << result;
        qDebug() << "用户:" << "sdkadjad";
        qDebug() << "退出时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "感谢使用捕鱼大战！";
        qDebug() << "==================";

        return result;

    } catch (const std::exception& e) {
        QString error = QString("启动失败: %1").arg(e.what());
        qCritical() << error;

        QMessageBox::critical(nullptr, "启动错误",
                              QString("🚨 捕鱼大战启动失败\n\n"
                                      "错误信息: %1\n\n"
                                      "👤 用户: sdkadjad\n"
                                      "🕐 时间: %2\n\n"
                                      "请检查系统环境并重试。")
                                  .arg(error)
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        return -1;

    } catch (...) {
        qCritical() << "未知错误导致程序无法启动";

        QMessageBox::critical(nullptr, "启动错误",
                              QString("🚨 捕鱼大战遇到未知错误\n\n"
                                      "👤 用户: sdkadjad\n"
                                      "🕐 时间: %1\n\n"
                                      "请重新启动应用程序。")
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        return -1;
    }
}
