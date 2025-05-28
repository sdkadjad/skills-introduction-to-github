/****************************************************************************
** Meta object code from reading C++ file 'gamewindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../gamewindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gamewindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSGameWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSGameWindowENDCLASS = QtMocHelpers::stringData(
    "GameWindow",
    "onGameStartPause",
    "",
    "onGameReset",
    "onCoinsChanged",
    "coins",
    "onScoreChanged",
    "score",
    "onCannonTypeChanged",
    "Cannon::CannonType",
    "type",
    "onGameSettingsChanged",
    "onMouseMoved",
    "position",
    "showAbout",
    "showHelp",
    "onGameTimerUpdate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSGameWindowENDCLASS_t {
    uint offsetsAndSizes[34];
    char stringdata0[11];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[15];
    char stringdata5[6];
    char stringdata6[15];
    char stringdata7[6];
    char stringdata8[20];
    char stringdata9[19];
    char stringdata10[5];
    char stringdata11[22];
    char stringdata12[13];
    char stringdata13[9];
    char stringdata14[10];
    char stringdata15[9];
    char stringdata16[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSGameWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSGameWindowENDCLASS_t qt_meta_stringdata_CLASSGameWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "GameWindow"
        QT_MOC_LITERAL(11, 16),  // "onGameStartPause"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 11),  // "onGameReset"
        QT_MOC_LITERAL(41, 14),  // "onCoinsChanged"
        QT_MOC_LITERAL(56, 5),  // "coins"
        QT_MOC_LITERAL(62, 14),  // "onScoreChanged"
        QT_MOC_LITERAL(77, 5),  // "score"
        QT_MOC_LITERAL(83, 19),  // "onCannonTypeChanged"
        QT_MOC_LITERAL(103, 18),  // "Cannon::CannonType"
        QT_MOC_LITERAL(122, 4),  // "type"
        QT_MOC_LITERAL(127, 21),  // "onGameSettingsChanged"
        QT_MOC_LITERAL(149, 12),  // "onMouseMoved"
        QT_MOC_LITERAL(162, 8),  // "position"
        QT_MOC_LITERAL(171, 9),  // "showAbout"
        QT_MOC_LITERAL(181, 8),  // "showHelp"
        QT_MOC_LITERAL(190, 17)   // "onGameTimerUpdate"
    },
    "GameWindow",
    "onGameStartPause",
    "",
    "onGameReset",
    "onCoinsChanged",
    "coins",
    "onScoreChanged",
    "score",
    "onCannonTypeChanged",
    "Cannon::CannonType",
    "type",
    "onGameSettingsChanged",
    "onMouseMoved",
    "position",
    "showAbout",
    "showHelp",
    "onGameTimerUpdate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGameWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x08,    1 /* Private */,
       3,    0,   75,    2, 0x08,    2 /* Private */,
       4,    1,   76,    2, 0x08,    3 /* Private */,
       6,    1,   79,    2, 0x08,    5 /* Private */,
       8,    1,   82,    2, 0x08,    7 /* Private */,
      11,    0,   85,    2, 0x08,    9 /* Private */,
      12,    1,   86,    2, 0x08,   10 /* Private */,
      14,    0,   89,    2, 0x08,   12 /* Private */,
      15,    0,   90,    2, 0x08,   13 /* Private */,
      16,    0,   91,    2, 0x08,   14 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject GameWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSGameWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGameWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGameWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GameWindow, std::true_type>,
        // method 'onGameStartPause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGameReset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCoinsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onScoreChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onCannonTypeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Cannon::CannonType, std::false_type>,
        // method 'onGameSettingsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMouseMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'showAbout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showHelp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGameTimerUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void GameWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GameWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onGameStartPause(); break;
        case 1: _t->onGameReset(); break;
        case 2: _t->onCoinsChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onScoreChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onCannonTypeChanged((*reinterpret_cast< std::add_pointer_t<Cannon::CannonType>>(_a[1]))); break;
        case 5: _t->onGameSettingsChanged(); break;
        case 6: _t->onMouseMoved((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 7: _t->showAbout(); break;
        case 8: _t->showHelp(); break;
        case 9: _t->onGameTimerUpdate(); break;
        default: ;
        }
    }
}

const QMetaObject *GameWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGameWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int GameWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
