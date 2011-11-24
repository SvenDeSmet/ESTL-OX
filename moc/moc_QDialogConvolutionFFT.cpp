/****************************************************************************
** Meta object code from reading C++ file 'QDialogConvolutionFFT.h'
**
** Created: Fri 28. Oct 08:01:21 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "E:/conv2D_opticks-build-desktop/../conv2D_opticks/QDialogConvolutionFFT.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QDialogConvolutionFFT.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FFTConvolutionDlg[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      28,   18,   18,   18, 0x09,
      37,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_FFTConvolutionDlg[] = {
    "FFTConvolutionDlg\0\0accept()\0reject()\0"
    "convolve()\0"
};

const QMetaObject FFTConvolutionDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FFTConvolutionDlg,
      qt_meta_data_FFTConvolutionDlg, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FFTConvolutionDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FFTConvolutionDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FFTConvolutionDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FFTConvolutionDlg))
        return static_cast<void*>(const_cast< FFTConvolutionDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int FFTConvolutionDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: reject(); break;
        case 2: convolve(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
