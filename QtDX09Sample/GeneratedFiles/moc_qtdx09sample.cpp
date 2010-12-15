/****************************************************************************
** Meta object code from reading C++ file 'qtdx09sample.h'
**
** Created: Thu Dec 16 00:33:43 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qtdx09sample.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtdx09sample.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtDX09Sample[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtDX09Sample[] = {
    "QtDX09Sample\0\0idle()\0"
};

const QMetaObject QtDX09Sample::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QtDX09Sample,
      qt_meta_data_QtDX09Sample, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtDX09Sample::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtDX09Sample::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtDX09Sample::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtDX09Sample))
        return static_cast<void*>(const_cast< QtDX09Sample*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QtDX09Sample::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: idle(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
