/****************************************************************************
** Meta object code from reading C++ file 'SharedMemoryController.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../External/source/SharedMemoryController.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SharedMemoryController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
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
struct qt_meta_tag_ZN22SharedMemoryControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto SharedMemoryController::qt_create_metaobjectdata<qt_meta_tag_ZN22SharedMemoryControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SharedMemoryController",
        "triggerbotEnabledChanged",
        "",
        "targetSpeedChanged",
        "jumpPowerChanged",
        "aimbotEnabledChanged",
        "aimbotTypeChanged",
        "espEnabledChanged",
        "espShowNamesChanged",
        "espShowBoxChanged",
        "espShowBonesChanged",
        "espShowDistanceChanged",
        "espShowTracerChanged",
        "espCharmsChanged",
        "fovSizeChanged",
        "smoothChanged",
        "aimbotKeybindChanged",
        "useCornerBoxChanged",
        "rainbowChanged",
        "isConnectedChanged",
        "setTriggerbotEnabled",
        "value",
        "setTargetSpeed",
        "setJumpPower",
        "setAimbotEnabled",
        "setAimbotType",
        "setEspEnabled",
        "setEspShowNames",
        "setEspShowBox",
        "setEspShowBones",
        "setEspShowDistance",
        "setEspShowTracer",
        "setEspCharms",
        "setFovSize",
        "setSmooth",
        "setAimbotKeybind",
        "setUseCornerBox",
        "setRainbow",
        "onConnected",
        "onDisconnected",
        "onReadyRead",
        "onErrorOccurred",
        "QLocalSocket::LocalSocketError",
        "error",
        "checkConnection",
        "attemptReconnect",
        "aimbotEnabled",
        "aimbotType",
        "espEnabled",
        "espShowNames",
        "espShowBox",
        "espShowBones",
        "espShowDistance",
        "espShowTracer",
        "espCharms",
        "triggerbotEnabled",
        "targetSpeed",
        "jumpPower",
        "fovSize",
        "smooth",
        "aimbotKeybind",
        "useCornerBox",
        "rainbow",
        "isConnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'triggerbotEnabledChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'targetSpeedChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'jumpPowerChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'aimbotEnabledChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'aimbotTypeChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espEnabledChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowNamesChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowBoxChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowBonesChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowDistanceChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowTracerChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espCharmsChanged'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fovSizeChanged'
        QtMocHelpers::SignalData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'smoothChanged'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'aimbotKeybindChanged'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'useCornerBoxChanged'
        QtMocHelpers::SignalData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'rainbowChanged'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isConnectedChanged'
        QtMocHelpers::SignalData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setTriggerbotEnabled'
        QtMocHelpers::SlotData<void(bool)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setTargetSpeed'
        QtMocHelpers::SlotData<void(float)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 21 },
        }}),
        // Slot 'setJumpPower'
        QtMocHelpers::SlotData<void(float)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 21 },
        }}),
        // Slot 'setAimbotEnabled'
        QtMocHelpers::SlotData<void(bool)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setAimbotType'
        QtMocHelpers::SlotData<void(int)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Slot 'setEspEnabled'
        QtMocHelpers::SlotData<void(bool)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setEspShowNames'
        QtMocHelpers::SlotData<void(bool)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setEspShowBox'
        QtMocHelpers::SlotData<void(bool)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setEspShowBones'
        QtMocHelpers::SlotData<void(bool)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setEspShowDistance'
        QtMocHelpers::SlotData<void(bool)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setEspShowTracer'
        QtMocHelpers::SlotData<void(bool)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setEspCharms'
        QtMocHelpers::SlotData<void(bool)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setFovSize'
        QtMocHelpers::SlotData<void(float)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 21 },
        }}),
        // Slot 'setSmooth'
        QtMocHelpers::SlotData<void(float)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 21 },
        }}),
        // Slot 'setAimbotKeybind'
        QtMocHelpers::SlotData<void(const QString &)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 21 },
        }}),
        // Slot 'setUseCornerBox'
        QtMocHelpers::SlotData<void(bool)>(36, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'setRainbow'
        QtMocHelpers::SlotData<void(bool)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 21 },
        }}),
        // Slot 'onConnected'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onErrorOccurred'
        QtMocHelpers::SlotData<void(QLocalSocket::LocalSocketError)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 42, 43 },
        }}),
        // Slot 'checkConnection'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'attemptReconnect'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'aimbotEnabled'
        QtMocHelpers::PropertyData<bool>(46, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'aimbotType'
        QtMocHelpers::PropertyData<int>(47, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'espEnabled'
        QtMocHelpers::PropertyData<bool>(48, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'espShowNames'
        QtMocHelpers::PropertyData<bool>(49, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'espShowBox'
        QtMocHelpers::PropertyData<bool>(50, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'espShowBones'
        QtMocHelpers::PropertyData<bool>(51, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 8),
        // property 'espShowDistance'
        QtMocHelpers::PropertyData<bool>(52, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 9),
        // property 'espShowTracer'
        QtMocHelpers::PropertyData<bool>(53, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 10),
        // property 'espCharms'
        QtMocHelpers::PropertyData<bool>(54, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 11),
        // property 'triggerbotEnabled'
        QtMocHelpers::PropertyData<bool>(55, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'targetSpeed'
        QtMocHelpers::PropertyData<float>(56, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'jumpPower'
        QtMocHelpers::PropertyData<float>(57, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'fovSize'
        QtMocHelpers::PropertyData<float>(58, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 12),
        // property 'smooth'
        QtMocHelpers::PropertyData<float>(59, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 13),
        // property 'aimbotKeybind'
        QtMocHelpers::PropertyData<QString>(60, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 14),
        // property 'useCornerBox'
        QtMocHelpers::PropertyData<bool>(61, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 15),
        // property 'rainbow'
        QtMocHelpers::PropertyData<bool>(62, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 16),
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(63, QMetaType::Bool, QMC::DefaultPropertyFlags, 17),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SharedMemoryController, qt_meta_tag_ZN22SharedMemoryControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SharedMemoryController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22SharedMemoryControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22SharedMemoryControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22SharedMemoryControllerE_t>.metaTypes,
    nullptr
} };

void SharedMemoryController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SharedMemoryController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->triggerbotEnabledChanged(); break;
        case 1: _t->targetSpeedChanged(); break;
        case 2: _t->jumpPowerChanged(); break;
        case 3: _t->aimbotEnabledChanged(); break;
        case 4: _t->aimbotTypeChanged(); break;
        case 5: _t->espEnabledChanged(); break;
        case 6: _t->espShowNamesChanged(); break;
        case 7: _t->espShowBoxChanged(); break;
        case 8: _t->espShowBonesChanged(); break;
        case 9: _t->espShowDistanceChanged(); break;
        case 10: _t->espShowTracerChanged(); break;
        case 11: _t->espCharmsChanged(); break;
        case 12: _t->fovSizeChanged(); break;
        case 13: _t->smoothChanged(); break;
        case 14: _t->aimbotKeybindChanged(); break;
        case 15: _t->useCornerBoxChanged(); break;
        case 16: _t->rainbowChanged(); break;
        case 17: _t->isConnectedChanged(); break;
        case 18: _t->setTriggerbotEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->setTargetSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 20: _t->setJumpPower((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 21: _t->setAimbotEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 22: _t->setAimbotType((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->setEspEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 24: _t->setEspShowNames((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 25: _t->setEspShowBox((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->setEspShowBones((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 27: _t->setEspShowDistance((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 28: _t->setEspShowTracer((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 29: _t->setEspCharms((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 30: _t->setFovSize((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 31: _t->setSmooth((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 32: _t->setAimbotKeybind((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 33: _t->setUseCornerBox((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 34: _t->setRainbow((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 35: _t->onConnected(); break;
        case 36: _t->onDisconnected(); break;
        case 37: _t->onReadyRead(); break;
        case 38: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QLocalSocket::LocalSocketError>>(_a[1]))); break;
        case 39: _t->checkConnection(); break;
        case 40: _t->attemptReconnect(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::triggerbotEnabledChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::targetSpeedChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::jumpPowerChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::aimbotEnabledChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::aimbotTypeChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espEnabledChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowNamesChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowBoxChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowBonesChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowDistanceChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowTracerChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espCharmsChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::fovSizeChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::smoothChanged, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::aimbotKeybindChanged, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::useCornerBoxChanged, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::rainbowChanged, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::isConnectedChanged, 17))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->getAimbotEnabled(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->getAimbotType(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->getEspEnabled(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->getEspShowNames(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->getEspShowBox(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->getEspShowBones(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->getEspShowDistance(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->getEspShowTracer(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->getEspCharms(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->getTriggerbotEnabled(); break;
        case 10: *reinterpret_cast<float*>(_v) = _t->getTargetSpeed(); break;
        case 11: *reinterpret_cast<float*>(_v) = _t->getJumpPower(); break;
        case 12: *reinterpret_cast<float*>(_v) = _t->getFovSize(); break;
        case 13: *reinterpret_cast<float*>(_v) = _t->getSmooth(); break;
        case 14: *reinterpret_cast<QString*>(_v) = _t->getAimbotKeybind(); break;
        case 15: *reinterpret_cast<bool*>(_v) = _t->getUseCornerBox(); break;
        case 16: *reinterpret_cast<bool*>(_v) = _t->getRainbow(); break;
        case 17: *reinterpret_cast<bool*>(_v) = _t->getIsConnected(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setAimbotEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 1: _t->setAimbotType(*reinterpret_cast<int*>(_v)); break;
        case 2: _t->setEspEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 3: _t->setEspShowNames(*reinterpret_cast<bool*>(_v)); break;
        case 4: _t->setEspShowBox(*reinterpret_cast<bool*>(_v)); break;
        case 5: _t->setEspShowBones(*reinterpret_cast<bool*>(_v)); break;
        case 6: _t->setEspShowDistance(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setEspShowTracer(*reinterpret_cast<bool*>(_v)); break;
        case 8: _t->setEspCharms(*reinterpret_cast<bool*>(_v)); break;
        case 9: _t->setTriggerbotEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 10: _t->setTargetSpeed(*reinterpret_cast<float*>(_v)); break;
        case 11: _t->setJumpPower(*reinterpret_cast<float*>(_v)); break;
        case 12: _t->setFovSize(*reinterpret_cast<float*>(_v)); break;
        case 13: _t->setSmooth(*reinterpret_cast<float*>(_v)); break;
        case 14: _t->setAimbotKeybind(*reinterpret_cast<QString*>(_v)); break;
        case 15: _t->setUseCornerBox(*reinterpret_cast<bool*>(_v)); break;
        case 16: _t->setRainbow(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *SharedMemoryController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SharedMemoryController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22SharedMemoryControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SharedMemoryController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 41)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 41;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 41)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 41;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void SharedMemoryController::triggerbotEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SharedMemoryController::targetSpeedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SharedMemoryController::jumpPowerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SharedMemoryController::aimbotEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SharedMemoryController::aimbotTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SharedMemoryController::espEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SharedMemoryController::espShowNamesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void SharedMemoryController::espShowBoxChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void SharedMemoryController::espShowBonesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void SharedMemoryController::espShowDistanceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void SharedMemoryController::espShowTracerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void SharedMemoryController::espCharmsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void SharedMemoryController::fovSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void SharedMemoryController::smoothChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void SharedMemoryController::aimbotKeybindChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void SharedMemoryController::useCornerBoxChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void SharedMemoryController::rainbowChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void SharedMemoryController::isConnectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}
QT_WARNING_POP
