#include "ltoolattitudemessage.h"

struct LToolAttitudeMessage::Imp
{
    QString m_toolname;
    bool m_valid;
    int m_errorIdentify {0};
    double m_point[3] {0.0};
    double m_orientation[4] {0.0};
    double m_matrix4x4[16] {0.0};

    QString m_toolSide;
    QString m_toolType;
    QString m_sensorValue;
    QString m_pressureValue;
    double m_a;
    double m_b;
    double m_c;

    QVector<double> extendedDataField;
};

LToolAttitudeMessage::LToolAttitudeMessage()
    : LAbstractMessage()
    , m_imp(std::make_shared<Imp>())
{
    this->m_classname = "LToolAttitudeMessage";
}

LToolAttitudeMessage::~LToolAttitudeMessage()
{

}

void LToolAttitudeMessage::operator=(const LToolAttitudeMessage& o)
{
    // *this->m_imp = *o.m_imp;
    this->m_imp->m_toolname = o.m_imp->m_toolname;
    this->m_imp->m_valid = o.m_imp->m_valid;
    this->m_imp->m_errorIdentify = o.m_imp->m_errorIdentify;
    this->m_imp->m_toolSide = o.m_imp->m_toolSide;
    this->m_imp->m_toolType = o.m_imp->m_toolType;
    this->m_imp->m_sensorValue = o.m_imp->m_sensorValue;
    this->m_imp->m_pressureValue = o.m_imp->m_pressureValue;
    this->m_imp->m_a = o.m_imp->m_a;
    this->m_imp->m_b = o.m_imp->m_b;
    this->m_imp->m_c = o.m_imp->m_c;
    memcpy_s(this->m_imp->m_point, sizeof(double) * 3, o.m_imp->m_point, sizeof(double) * 3);
    memcpy_s(this->m_imp->m_orientation, sizeof(double) * 4, o.m_imp->m_orientation, sizeof(double) * 4);
    memcpy_s(this->m_imp->m_matrix4x4, sizeof(double) * 16, o.m_imp->m_matrix4x4, sizeof(double) * 16);
}

bool LToolAttitudeMessage::valid() const
{
    return this->m_imp->m_valid;
}

bool LToolAttitudeMessage::isValid() const
{
    return this->valid();
}

bool LToolAttitudeMessage::isValidData() const
{
    return this->distanceTo(LToolAttitudeMessage()) >= 1.0;
}

void LToolAttitudeMessage::setValid(bool valid)
{
    this->m_imp->m_valid = valid;
}

bool LToolAttitudeMessage::isLToolAttitudeMessage() const
{
    return true;
}

QString LToolAttitudeMessage::toolname() const
{
    return this->m_imp->m_toolname;
}

void LToolAttitudeMessage::setToolName(const QString& name)
{
    this->m_imp->m_toolname = name;
}

double* LToolAttitudeMessage::pointDate()
{
    return this->m_imp->m_point;
}

QVector<double> LToolAttitudeMessage::vectorPointDate() const
{

    return {this->m_imp->m_point[0], this->m_imp->m_point[1], this->m_imp->m_point[2]};
}

const double* LToolAttitudeMessage::constPointDate() const
{
    return this->m_imp->m_point;
}

double* LToolAttitudeMessage::orientationDate()
{
    return this->m_imp->m_orientation;
}

QVector<double> LToolAttitudeMessage::vectorOrientationDate() const
{
    return {this->m_imp->m_orientation[0], this->m_imp->m_orientation[1],
            this->m_imp->m_orientation[2], this->m_imp->m_orientation[3]};
}

const double* LToolAttitudeMessage::constOrientationDate() const
{
    return this->m_imp->m_orientation;
}

void LToolAttitudeMessage::setPointDate(const double* point, int size)
{
    for (auto index = 0; index < size && index < 4; ++index)
    {
        this->m_imp->m_orientation[index] = point[index];
    }
}

void LToolAttitudeMessage::setPointDate(double x, double y, double z)
{
    this->m_imp->m_point[0] = x;
    this->m_imp->m_point[1] = y;
    this->m_imp->m_point[2] = z;
}

void LToolAttitudeMessage::setPointDate(const QVector<double>& point)
{
    for (auto index = 0; index < point.size() && index < 3; ++index)
    {
        this->m_imp->m_point[index] = point.at(index);
    }
}

void LToolAttitudeMessage::setOrientationDate(const double* orientation, int size)
{
    for (auto index = 0; index < size && index < 4; ++index)
    {
        this->m_imp->m_orientation[index] = orientation[index];
    }
}

void LToolAttitudeMessage::setOrientationDate(double x, double y, double z, double r)
{
    this->m_imp->m_orientation[0] = x;
    this->m_imp->m_orientation[1] = y;
    this->m_imp->m_orientation[2] = z;
    this->m_imp->m_orientation[3] = r;
}

void LToolAttitudeMessage::setOrientationDate(const QVector<double>& orientation)
{
    for (auto index = 0; index < orientation.size() && index < 4; ++index)
    {
        this->m_imp->m_orientation[index] = orientation.at(index);
    }
}

/**
    @vtkcode
*/
template <typename T>
void ToMatrix3x3(T A[3][3], const T& w, const T& x, const T& y, const T& z)
{
    T ww = w * w;	//
    T wx = w * x;
    T wy = w * y;
    T wz = w * z;

    T xx = x * x;
    T yy = y * y;
    T zz = z * z;

    T xy = x * y;
    T xz = x * z;
    T yz = y * z;

    T rr = xx + yy + zz;
    // normalization factor, just in case quaternion was not normalized
    T f;
    if (ww + rr == 0.0) // means the quaternion is (0, 0, 0, 0)
    {
        A[0][0] = 0.0;
        A[1][0] = 0.0;
        A[2][0] = 0.0;
        A[0][1] = 0.0;
        A[1][1] = 0.0;
        A[2][1] = 0.0;
        A[0][2] = 0.0;
        A[1][2] = 0.0;
        A[2][2] = 0.0;
        return;
    }
    f = 1.0 / (ww + rr);

    T s = (ww - rr) * f;
    f *= 2.0;

    A[0][0] = xx * f + s;
    A[1][0] = (xy + wz) * f;
    A[2][0] = (xz - wy) * f;

    A[0][1] = (xy - wz) * f;
    A[1][1] = yy * f + s;
    A[2][1] = (yz + wx) * f;

    A[0][2] = (xz + wy) * f;
    A[1][2] = (yz - wx) * f;
    A[2][2] = zz * f + s;
}
/**
    @endvtkcode
*/


QVector<double> LToolAttitudeMessage::vectorMatrix4x4() const
{
    QVector<double> _retval(16, 0.0);
    memcpy_s(&_retval[0], sizeof(double) * _retval.size(), this->m_imp->m_matrix4x4, sizeof(double) * 16);
    return _retval;
}

void LToolAttitudeMessage::deepCopyMatrix4x4(double* matrix4x4) const
{
    if(matrix4x4)
    {
        auto _vecMatrix4x4 = this->vectorMatrix4x4();
        for(auto _index = 0; _index < 16; ++_index)
        {
            matrix4x4[_index] = _vecMatrix4x4[_index];
        }
    }
}

double* LToolAttitudeMessage::matrix4x4Date()
{
    return this->m_imp->m_matrix4x4;
}

void LToolAttitudeMessage::setMatrix4x4(const double* date, int size)
{
    memcpy_s(this->m_imp->m_matrix4x4, sizeof(double) * 16, date, sizeof(double) * size);
}

void LToolAttitudeMessage::setMatrix4x4(const QVector<double>& date)
{
    this->setMatrix4x4(date.data());
}

double LToolAttitudeMessage::distanceTo(const LToolAttitudeMessage& o) const
{
    auto origin = this->vectorMatrix4x4();
    auto target = o.vectorMatrix4x4();

    return sqrt((origin[3] - target[3]) * (origin[3] - target[3]) + (origin[7] - target[7]) * (origin[7] - target[7]) + (origin[11] - target[11]) * (origin[11] - target[11]));
}

void LToolAttitudeMessage::setErrorIdentify(int e)
{
    this->m_imp->m_errorIdentify = e;
}

int LToolAttitudeMessage::errorIdentify() const
{
    return this->m_imp->m_errorIdentify;
}

QString LToolAttitudeMessage::toolType() const
{
    return this->m_imp->m_toolType;
}

void LToolAttitudeMessage::setToolType(const QString& t)
{
    this->m_imp->m_toolType = t;
}

QString LToolAttitudeMessage::sensorValue() const
{
    return this->m_imp->m_sensorValue;
}

void LToolAttitudeMessage::setSensorValue(const QString& s)
{
    this->m_imp->m_sensorValue = s;
}

QString LToolAttitudeMessage::pressureValue() const
{
    return this->m_imp->m_pressureValue;
}

void LToolAttitudeMessage::setPressureValue(const QString& p)
{
    this->m_imp->m_pressureValue = p;
}

QString LToolAttitudeMessage::toolSide() const
{
    return this->m_imp->m_toolSide;
}

void LToolAttitudeMessage::setToolSide(const QString& s)
{
    this->m_imp->m_toolSide = s;
}

double LToolAttitudeMessage::aValue() const
{
    return this->m_imp->m_a;
}

void LToolAttitudeMessage::setaValue(const double a) const
{
    this->m_imp->m_a = a;
}

double LToolAttitudeMessage::bValue() const
{
    return this->m_imp->m_b;
}

void LToolAttitudeMessage::setbValue(const double b) const
{
    this->m_imp->m_b = b;
}

double LToolAttitudeMessage::cValue() const
{
    return this->m_imp->m_c;
}

void LToolAttitudeMessage::setcValue(const double c) const
{
    this->m_imp->m_c = c;
}

QVector<double> LToolAttitudeMessage::extendedDataField() const
{
    return this->m_imp->extendedDataField;
}

void LToolAttitudeMessage::setExtendedDataField(const QVector<double>& v)
{
    this->m_imp->extendedDataField = v;
}

QVector<double> LToolAttitudeMessage::translateOfVtkMatrix4x4(const QVector<double>& position, const QVector<double>& quatern)
{
    QVector<double> _matrix4x4(16, 0.0);

    _matrix4x4[15] = 1;
    _matrix4x4[0 * 4 + 3] = position[0];
    _matrix4x4[1 * 4 + 3] = position[1];
    _matrix4x4[2 * 4 + 3] = position[2];

    double data[3][3];
    ToMatrix3x3<double>(data, quatern[3], quatern[0], quatern[1], quatern[2]);
    memcpy(&_matrix4x4[0] + 0 * 4, data[0], sizeof(double[3]));
    memcpy(&_matrix4x4[0] + 1 * 4, data[1], sizeof(double[3]));
    memcpy(&_matrix4x4[0] + 2 * 4, data[2], sizeof(double[3]));

    return _matrix4x4;
}



