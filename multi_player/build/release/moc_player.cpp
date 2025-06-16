/****************************************************************************
** Meta object code from reading C++ file 'player.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../player.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'player.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSPlayerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSPlayerENDCLASS = QtMocHelpers::stringData(
    "Player",
    "on_playButton_clicked",
    "",
    "on_stopButton_clicked",
    "on_forwardButton_clicked",
    "on_backwardButton_clicked",
    "openFile",
    "onPlaylistItemDoubleClicked",
    "QListWidgetItem*",
    "item",
    "onPlayRequested",
    "filePath",
    "onPositionChanged",
    "position",
    "onDurationChanged",
    "duration",
    "onPlaybackStateChanged",
    "QMediaPlayer::PlaybackState",
    "state",
    "onVolumeChanged",
    "volume",
    "onMutedChanged",
    "muted",
    "onFullScreenChanged",
    "fullScreen",
    "onWindowStateChanged",
    "Qt::WindowState",
    "newState",
    "toggleFullScreen",
    "videoFullScreen"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSPlayerENDCLASS_t {
    uint offsetsAndSizes[60];
    char stringdata0[7];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[25];
    char stringdata5[26];
    char stringdata6[9];
    char stringdata7[28];
    char stringdata8[17];
    char stringdata9[5];
    char stringdata10[16];
    char stringdata11[9];
    char stringdata12[18];
    char stringdata13[9];
    char stringdata14[18];
    char stringdata15[9];
    char stringdata16[23];
    char stringdata17[28];
    char stringdata18[6];
    char stringdata19[16];
    char stringdata20[7];
    char stringdata21[15];
    char stringdata22[6];
    char stringdata23[20];
    char stringdata24[11];
    char stringdata25[21];
    char stringdata26[16];
    char stringdata27[9];
    char stringdata28[17];
    char stringdata29[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSPlayerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSPlayerENDCLASS_t qt_meta_stringdata_CLASSPlayerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 6),  // "Player"
        QT_MOC_LITERAL(7, 21),  // "on_playButton_clicked"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 21),  // "on_stopButton_clicked"
        QT_MOC_LITERAL(52, 24),  // "on_forwardButton_clicked"
        QT_MOC_LITERAL(77, 25),  // "on_backwardButton_clicked"
        QT_MOC_LITERAL(103, 8),  // "openFile"
        QT_MOC_LITERAL(112, 27),  // "onPlaylistItemDoubleClicked"
        QT_MOC_LITERAL(140, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(157, 4),  // "item"
        QT_MOC_LITERAL(162, 15),  // "onPlayRequested"
        QT_MOC_LITERAL(178, 8),  // "filePath"
        QT_MOC_LITERAL(187, 17),  // "onPositionChanged"
        QT_MOC_LITERAL(205, 8),  // "position"
        QT_MOC_LITERAL(214, 17),  // "onDurationChanged"
        QT_MOC_LITERAL(232, 8),  // "duration"
        QT_MOC_LITERAL(241, 22),  // "onPlaybackStateChanged"
        QT_MOC_LITERAL(264, 27),  // "QMediaPlayer::PlaybackState"
        QT_MOC_LITERAL(292, 5),  // "state"
        QT_MOC_LITERAL(298, 15),  // "onVolumeChanged"
        QT_MOC_LITERAL(314, 6),  // "volume"
        QT_MOC_LITERAL(321, 14),  // "onMutedChanged"
        QT_MOC_LITERAL(336, 5),  // "muted"
        QT_MOC_LITERAL(342, 19),  // "onFullScreenChanged"
        QT_MOC_LITERAL(362, 10),  // "fullScreen"
        QT_MOC_LITERAL(373, 20),  // "onWindowStateChanged"
        QT_MOC_LITERAL(394, 15),  // "Qt::WindowState"
        QT_MOC_LITERAL(410, 8),  // "newState"
        QT_MOC_LITERAL(419, 16),  // "toggleFullScreen"
        QT_MOC_LITERAL(436, 15)   // "videoFullScreen"
    },
    "Player",
    "on_playButton_clicked",
    "",
    "on_stopButton_clicked",
    "on_forwardButton_clicked",
    "on_backwardButton_clicked",
    "openFile",
    "onPlaylistItemDoubleClicked",
    "QListWidgetItem*",
    "item",
    "onPlayRequested",
    "filePath",
    "onPositionChanged",
    "position",
    "onDurationChanged",
    "duration",
    "onPlaybackStateChanged",
    "QMediaPlayer::PlaybackState",
    "state",
    "onVolumeChanged",
    "volume",
    "onMutedChanged",
    "muted",
    "onFullScreenChanged",
    "fullScreen",
    "onWindowStateChanged",
    "Qt::WindowState",
    "newState",
    "toggleFullScreen",
    "videoFullScreen"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPlayerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  110,    2, 0x08,    1 /* Private */,
       3,    0,  111,    2, 0x08,    2 /* Private */,
       4,    0,  112,    2, 0x08,    3 /* Private */,
       5,    0,  113,    2, 0x08,    4 /* Private */,
       6,    0,  114,    2, 0x08,    5 /* Private */,
       7,    1,  115,    2, 0x08,    6 /* Private */,
      10,    1,  118,    2, 0x08,    8 /* Private */,
      12,    1,  121,    2, 0x08,   10 /* Private */,
      14,    1,  124,    2, 0x08,   12 /* Private */,
      16,    1,  127,    2, 0x08,   14 /* Private */,
      19,    1,  130,    2, 0x08,   16 /* Private */,
      21,    1,  133,    2, 0x08,   18 /* Private */,
      23,    1,  136,    2, 0x08,   20 /* Private */,
      25,    1,  139,    2, 0x08,   22 /* Private */,
      28,    0,  142,    2, 0x08,   24 /* Private */,
      29,    0,  143,    2, 0x08,   25 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::LongLong,   13,
    QMetaType::Void, QMetaType::LongLong,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Bool,   22,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Player::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSPlayerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPlayerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPlayerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Player, std::true_type>,
        // method 'on_playButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_stopButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_forwardButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_backwardButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPlaylistItemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onPlayRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onPositionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'onDurationChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'onPlaybackStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMediaPlayer::PlaybackState, std::false_type>,
        // method 'onVolumeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onMutedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onFullScreenChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onWindowStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::WindowState, std::false_type>,
        // method 'toggleFullScreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'videoFullScreen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Player::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Player *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_playButton_clicked(); break;
        case 1: _t->on_stopButton_clicked(); break;
        case 2: _t->on_forwardButton_clicked(); break;
        case 3: _t->on_backwardButton_clicked(); break;
        case 4: _t->openFile(); break;
        case 5: _t->onPlaylistItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 6: _t->onPlayRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onPositionChanged((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 8: _t->onDurationChanged((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 9: _t->onPlaybackStateChanged((*reinterpret_cast< std::add_pointer_t<QMediaPlayer::PlaybackState>>(_a[1]))); break;
        case 10: _t->onVolumeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->onMutedChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->onFullScreenChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->onWindowStateChanged((*reinterpret_cast< std::add_pointer_t<Qt::WindowState>>(_a[1]))); break;
        case 14: _t->toggleFullScreen(); break;
        case 15: _t->videoFullScreen(); break;
        default: ;
        }
    }
}

const QMetaObject *Player::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Player::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPlayerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Player::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
