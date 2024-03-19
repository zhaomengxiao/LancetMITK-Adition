#ifndef LTOOLATTITUDEMESSAGE_H
#define LTOOLATTITUDEMESSAGE_H

#include <memory>
#include <QVector>
#include <LAbstractMessage>
#include <MitkLancetTeethLhyExports.h>
/**
 * \inherits LAbstractMessage
 * \class LToolAttitudeMessage
 *
 * \ingroup Interface
 *
 * \brief   Tool attitude message object。
 *
 * \author dabai
 *
 * \version 1.0.0
 *
 * \date 2021/03/29
 *
 * Contact: user@company.com
 *
 */
class MITKLANCETTEETHLHY_EXPORT LToolAttitudeMessage : public LAbstractMessage
{
public:
    LToolAttitudeMessage();
    virtual ~LToolAttitudeMessage();

    void operator=(const LToolAttitudeMessage&);
public:
    virtual bool valid() const;
    virtual bool isValid() const;
    virtual bool isValidData() const;
    virtual void setValid(bool);
    virtual bool isLToolAttitudeMessage() const override;

    virtual QString toolname() const;

    virtual void setToolName(const QString&);

    virtual double* pointDate();

    virtual QVector<double> vectorPointDate() const;

    virtual double const* constPointDate() const;

    virtual double* orientationDate();

    virtual QVector<double> vectorOrientationDate() const;

    virtual double const* constOrientationDate() const;

    virtual void setPointDate(const double*, int = 3);
    virtual void setPointDate(double, double, double);
    virtual void setPointDate(const QVector<double>&);

    virtual void setOrientationDate(const double*, int = 4);
    virtual void setOrientationDate(double, double, double, double);
    virtual void setOrientationDate(const QVector<double>&);

    virtual void deepCopyMatrix4x4(double*) const;
    virtual double* matrix4x4Date();
    virtual QVector<double> vectorMatrix4x4() const;
    virtual void setMatrix4x4(const double*, int = 16);
    virtual void setMatrix4x4(const QVector<double>&);

    double distanceTo(const LToolAttitudeMessage&) const;

    void setErrorIdentify(int);
    int errorIdentify() const;
public:
    /**
      \bug Temporarily put the following interfaces here for later iterative
           optimization.
    */
    QString toolType() const;
    void setToolType(const QString&);

    QString sensorValue() const;
    void setSensorValue(const QString&);

    QString pressureValue() const;
    void setPressureValue(const QString&);

    QString toolSide() const;
    void setToolSide(const QString&);

    double aValue() const;
    void setaValue(const double) const;

    double bValue() const;
    void setbValue(const double) const;

    double cValue() const;
    void setcValue(const double) const;

public:
    QVector<double> extendedDataField() const;
    void setExtendedDataField(const QVector<double>&);
public:

    /**
     * \brief   Four elements + coordinate points converted to vtk4x4 matrix data.
     *
     * \param[in] Coordinate point.
     * \param[in] Four elements.
     */
    static QVector<double> translateOfVtkMatrix4x4(const QVector<double>&, const QVector<double>&);
protected:

    struct Imp;
    std::shared_ptr<Imp> m_imp;
};

#endif // LTOOLATTITUDEMESSAGE_H
