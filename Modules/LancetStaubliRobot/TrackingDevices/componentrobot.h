#ifndef COMPONENTROBOT_H
#define COMPONENTROBOT_H

#include <robotapi/include/RRTC.h>

//#include <RRCT.h>
#include <QTimer>
#include <QThread>
#include <QPointer>
#include <robotapi_Staubli.h>
#include <labstractdeviceinterface.h>
#include <QTime>
#include "Windows.h"
#include <LToolAttitudeMessage>

#include <MitkLancetStaubliRobotExports.h>

class MITKLANCETSTAUBLIROBOT_EXPORT ComponentRobot
    : public LAbstractDeviceInterface
{
    Q_OBJECT
public:
    ComponentRobot();
    virtual ~ComponentRobot();
    TcpInfo ui_tcp;
    typedef struct trgRealtimeDataPack
    {
        Joints joint;
        CartesianPose descartes;
    } RealtimeDataPack;
public:
    TcpInfo ui_tcpByCp;
    virtual void enabled () override;

    virtual void disable() override;
    virtual void enabledOnThread() override;
    virtual bool isEnabledOnThread() const override;

    virtual QString nameString() const override;

    virtual QString version() const override;

    /**
     * \brief   Returns whether the tool is enabled.
     *
     * \param[in] tool name.
     */
    virtual bool toolEnabled(const QString&) const override;

    /**
     * \brief   Returns whether the device loads the target tool.
     *
     * \param[in] tool name.
     */
    virtual bool hasTool(const QString&) const override;

    /**
     * \brief   Set whether the tool is enabled.
     *
     * \param[in] tool name.
     */
    virtual void setToolEnabled(const QString&) override;

    /**
     * \brief   Returns whether the tool is disabled.
     *
     * \param[in] tool name.
     */
    virtual bool toolDisable(const QString&) const override;

    /**
     * \brief   Set whether the tool is disabled.
     *
     * \param[in] tool name.
     */
    virtual void setToolDisable(const QString&) override;

    virtual bool isValidTool(const QString&);

    /**
     * \brief   Returns the option property value of the target component.
     *
     * \param[in] Component option name.
     *
     * \see     void setOptionValue(const QString&, const QVariant&)
     */
    virtual QVariant optionValue(const QString&) const override;

    /**
     * \brief   Sets the option property value of the target component.
     *
     * \param[in] Component option name.
     * \param[in] Component option value.
     *
     * \note	For a description of the option values, see labstractdeviceinterface class.
     *
     * \see     QVariant optionValue(const QString&)
     */
    virtual void setOptionValue(const QString&, const QVariant&) override;

    /**
     * \brief   Request component instance object processing target requirements.
     *
     * \param[in] Component function name.
     * \param[in] Component function param.
     *
     * \note	This method only pushes the request to the component instance object, and
     *			the final interpretation right belongs to the component instance object.
     *
     * \see     QVariant optionValue(const QString&)
     */
    virtual bool requestExecOperate(const QString&, const QStringList&) override;
public:
    /**
     * \brief   Return real time data of manipulator.
     * \see     struct RealtimeDataPack
     */
    virtual LToolAttitudeMessage realtimeData();

    /**
     * \brief   Get the operating mode of the manipulator.
     *
     * \note    Interface missing, not implemented
     *
     * \see     RobotApi::setworkmode(int)
     */
    __declspec(deprecated) virtual int workMode() const;

    /**
     * \brief   Set the operating mode of the manipulator.
     * \see     RobotApi::setworkmode(int)
     */
    virtual void setWorkMode(int);
public slots:
    /**
     * \brief   Robot heartbeat detection interface.
     */
    virtual void heartbeat();
protected slots:
    void on_robotApi_signal_api_isRobotConnected(bool);

    void on_updateToolPosition();
protected:
    static void workingOnHeartbeat(ComponentRobot*);
    QVector<double> translateOfVtkMatrix4x4(const RobotRealtimeData&);
protected:
    bool m_isConnect;
    RRTC m_rrtc;
    QTimer m_time;
    QPointer<QThread> m_thHeartbeat;
    RobotApi* robotApi;
};

#endif // COMPONENTROBOT_H
