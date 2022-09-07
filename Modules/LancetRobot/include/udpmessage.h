#ifndef LROBOTUDPMESSAGE_H
#define LROBOTUDPMESSAGE_H

#include <memory>

#include <QByteArray>
#include "MitkLancetRobotExports.h"
class MITKLANCETROBOT_EXPORT RobotUDPMessage
{
    struct RobotUDPMessagePrivateImp;
public:
    /**
     * \enum ApplicationState
     *
     * \brief Current state of the default application
     *
     * \note The state of the application is represented by a string in the lower
     * computer, and the user can also convert the enumeration value into a string
     * eg. ApplicationState::RUNNING It is equal to the "RUNNING" of the lower co-
     * mputer.
     */
    enum ApplicationState
    {
        UNKNOWN,            ///< Unknown state
        IDLE,				///< The application is selected.
        RUNNING,			///< The application is executed.
        MOTIONPAUSED,		///< The application is paused.
        REPOSITIONING,		///< The robot is repositioned. The application is still paused because the robot has left the path.
        ERROR,				///< An error occurred while the application was running.
        STARTING,			///< The application is being initialized to switch to the RUNNING state.
        STOPPING			///< The application is being reset to the start of the program. The application is then in the IDLE state.
    };

    RobotUDPMessage();
    RobotUDPMessage(const QByteArray&);
    RobotUDPMessage(const RobotUDPMessage&);

    void operator=(const QByteArray&);
    void operator=(const RobotUDPMessage&);
public:
    /**
     * \brief Time stamp
     *
     * \note Type: Integer (long); unit: ms
     * The time stamp is the current system time of the robot controller when the sta-
     * tus message is sent. Corresponds to the time in milliseconds elapsed since mid-
     * night on 1.1.1970.
     */
    long time() const;
    void setTime(long);

    /**
     * \brief Data packet counter (packets sent to the client)
     *
     * \note When the robot controller sends a new status message, the counter is incre-
     * mented by 1. The client can use the counter to determine the order in which the
     * status messages were sent.
     */
    int frames() const;
    void setFrames(int);

    /**
     * \brief Data packet counter (packets sent to the client)
     *
     * \note When the robot controller sends a new status message, the counter is incre-
     * mented by 1. The client can use the counter to determine the order in which the
     * status messages were sent.
     * The client can request the counter for restoration of a cancelled connection and
     * then use the requested value+1 as the counter in its next controller message.

     */
    int validFrames() const;
    void setValidFrames(int);

    /**
     * \brief The ID signals to the client whether the received controller message was valid
     * or defective.
     */
    int errorCode() const;
    void setErrorCode(int);

    /**
     * \brief Current status of the output signal AutExt_Active
     *
     * \return
     * TRUE: AUT mode is active and the project on the robot controller can be
     * controlled externally via UDP.
     * FALSE: AUT mode is not active or the project on the robot controller cannot
     * be controlled externally via UDP
     */
    bool isConnectedUDP() const;
    void setConnectedUDP(bool);

    /**
     * \brief Current status of the output signal AutExt_AppReadyToStart
     *
     * \return
     * TRUE: The default application is ready to start.
     * FALSE: The default application cannot be started.
     */
    bool isApplicationReadyToStart() const;
    void setApplicationReadyToStart(bool);

    /**
     * \brief Current status of the output signal DefaultApp_Error
     *
     * \return
     * TRUE: An error occurred during execution of the default application.
     * FALSE: The default application has not signaled an error.
     */
    bool isApplicationError() const;
    void setApplicationError(bool);

    /**
     * \brief Current status of the output signal Station_Error
     *
     * \return
     * TRUE: The station has signaled an error.
     * FALSE: The station is running without errors.
     */
    bool isLowerMachineSignalError() const;
    void setLowerMachineSignalError(bool);

    /**
     * \brief Current state of the default application
     *
     * \see enum ApplicationState
     */
    ApplicationState applicationState() const;
    void setApplicationState(const QString&);
    void setApplicationState(const ApplicationState&);

    /**
     * \brief Current status of the input signal App_Start
     *
     * \return
     * TRUE: Request the current state of the signal to resume the cancelled connection.
     * FALSE: Do nothing.
     *
     * \note Status defined by the last valid controller message.
     * The client can request the current status of the signal for restoration of a can-
     * celled connection.
     */
    bool isApplicationStart() const;
    void setApplicationStart(bool);

    /**
     * \brief Current status of the input signal App_Enable
     *
     * \return
     * TRUE: Do nothing.
     * FALSE: If no controller message has been received in the last 100 ms.
     *
     * \note
     * Status defined by the last valid controller message;
     * The client can request the current status of the signal for restoration of a can-
     * celled connection.
     */
    bool isApplicationEnable() const;
    void setApplicationEnable(bool);
public:
    /**
     * \brief Translating manipulator UDP message objects.
     *
     * \param Message body character stream.
     * \param Message body character stream length.
     *
     * \return see \c LRobotUDPMessage
     */
    static RobotUDPMessage translateRobotUDPMessage(const char* stream, int lenght);

    static QByteArray toUDPStream(const RobotUDPMessage&);

    static ApplicationState applicationStateToKey(const QString&);
    static QString applicationStateToValue(const ApplicationState&);
protected:
    std::shared_ptr<RobotUDPMessagePrivateImp> m_imp;
};

#endif // !LROBOTUDPMESSAGE_H
