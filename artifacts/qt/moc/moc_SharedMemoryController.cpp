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
        "aimbotEnabledChanged",
        "",
        "espEnabledChanged",
        "espShowNamesChanged",
        "espShowBoxChanged",
        "espShowBonesChanged",
        "espShowDistanceChanged",
        "espShowTracerChanged",
        "fovSizeChanged",
        "isConnectedChanged",
        "syncWithSharedMemory",
        "checkConnection",
        "aimbotEnabled",
        "espEnabled",
        "espShowNames",
        "espShowBox",
        "espShowBones",
        "espShowDistance",
        "espShowTracer",
        "fovSize",
        "isConnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'aimbotEnabledChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espEnabledChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowNamesChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowBoxChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowBonesChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowDistanceChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'espShowTracerChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fovSizeChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isConnectedChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'syncWithSharedMemory'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'checkConnection'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'aimbotEnabled'
        QtMocHelpers::PropertyData<bool>(13, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'espEnabled'
        QtMocHelpers::PropertyData<bool>(14, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'espShowNames'
        QtMocHelpers::PropertyData<bool>(15, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'espShowBox'
        QtMocHelpers::PropertyData<bool>(16, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'espShowBones'
        QtMocHelpers::PropertyData<bool>(17, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'espShowDistance'
        QtMocHelpers::PropertyData<bool>(18, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'espShowTracer'
        QtMocHelpers::PropertyData<bool>(19, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'fovSize'
        QtMocHelpers::PropertyData<float>(20, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(21, QMetaType::Bool, QMC::DefaultPropertyFlags, 8),
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
        case 0: _t->aimbotEnabledChanged(); break;
        case 1: _t->espEnabledChanged(); break;
        case 2: _t->espShowNamesChanged(); break;
        case 3: _t->espShowBoxChanged(); break;
        case 4: _t->espShowBonesChanged(); break;
        case 5: _t->espShowDistanceChanged(); break;
        case 6: _t->espShowTracerChanged(); break;
        case 7: _t->fovSizeChanged(); break;
        case 8: _t->isConnectedChanged(); break;
        case 9: _t->syncWithSharedMemory(); break;
        case 10: _t->checkConnection(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::aimbotEnabledChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espEnabledChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowNamesChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowBoxChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowBonesChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowDistanceChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::espShowTracerChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::fovSizeChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (SharedMemoryController::*)()>(_a, &SharedMemoryController::isConnectedChanged, 8))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->getAimbotEnabled(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->getEspEnabled(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->getEspShowNames(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->getEspShowBox(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->getEspShowBones(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->getEspShowDistance(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->getEspShowTracer(); break;
        case 7: *reinterpret_cast<float*>(_v) = _t->getFovSize(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->getIsConnected(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setAimbotEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 1: _t->setEspEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 2: _t->setEspShowNames(*reinterpret_cast<bool*>(_v)); break;
        case 3: _t->setEspShowBox(*reinterpret_cast<bool*>(_v)); break;
        case 4: _t->setEspShowBones(*reinterpret_cast<bool*>(_v)); break;
        case 5: _t->setEspShowDistance(*reinterpret_cast<bool*>(_v)); break;
        case 6: _t->setEspShowTracer(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setFovSize(*reinterpret_cast<float*>(_v)); break;
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
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SharedMemoryController::aimbotEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SharedMemoryController::espEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SharedMemoryController::espShowNamesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SharedMemoryController::espShowBoxChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SharedMemoryController::espShowBonesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SharedMemoryController::espShowDistanceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SharedMemoryController::espShowTracerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void SharedMemoryController::fovSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void SharedMemoryController::isConnectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
