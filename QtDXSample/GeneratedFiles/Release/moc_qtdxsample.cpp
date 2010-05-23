/****************************************************************************
** Meta object code from reading C++ file 'qtdxsample.h'
**
** Created: Mon May 24 00:47:37 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qtdxsample.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtdxsample.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtDXSample[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      36,   28,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtDXSample[] = {
    "QtDXSample\0\0repaintCanvas()\0pressed\0"
    "toggleAnimation(bool)\0"
};

const QMetaObject QtDXSample::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QtDXSample,
      qt_meta_data_QtDXSample, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtDXSample::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtDXSample::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtDXSample::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtDXSample))
        return static_cast<void*>(const_cast< QtDXSample*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QtDXSample::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: repaintCanvas(); break;
        case 1: toggleAnimation((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
