/****************************************************************************
** Meta object code from reading C++ file 'qtd2dsample.h'
**
** Created: Thu Oct 21 20:22:30 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qtd2dsample.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtd2dsample.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtD2DSample[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_QtD2DSample[] = {
    "QtD2DSample\0"
};

const QMetaObject QtD2DSample::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QtD2DSample,
      qt_meta_data_QtD2DSample, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtD2DSample::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtD2DSample::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtD2DSample::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtD2DSample))
        return static_cast<void*>(const_cast< QtD2DSample*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QtD2DSample::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
