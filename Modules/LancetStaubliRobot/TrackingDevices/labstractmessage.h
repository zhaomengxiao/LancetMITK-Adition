#ifndef LABSTRACTMESSAGE_H
#define LABSTRACTMESSAGE_H

#include <QString>
#include <MitkLancetStaubliRobotExports.h>
#include "commonunit_global.h"
/**
 * \class LAbstractMessage
 *
 * \ingroup Interface
 *
 * \brief   Abstract message class provides basic message properties, and other
 *          functions are enhanced by subclasses.
 *
 * \author dabai
 *
 * \version 1.0.0
 *
 * \date 2021/03/29
 *
 * \see     LErrorMessage, LComponentAttributeMessage, LToolAttitudeMessage
 *
 * Contact: user@company.com
 *
 */
class MITKLANCETSTAUBLIROBOT_EXPORT LAbstractMessage
{
public:
    LAbstractMessage();
    ~LAbstractMessage();

    using LMessageType = lh_interface::error_types;
public:
    /**
      * \brief  Return message unique identifier.
      *
      * \return Unique identification code.
      */
    inline virtual QString identify() const;

    /**
      * \brief It is suggested that users should not use this interface, which
      *        is for message creators.
      * \param[in] Unique identification code.
      */
    inline virtual void setIdentify(const QString&);

    /**
      * \brief  Returns the class name of the current class. The type judgment
      *         is based on the field.
      * \return Class name string.
      */
    inline virtual QString classname() const;

    /**
      * \brief  The thread ID of the message birth. Users should pay attention
      *         to this information.
      * \return The thread identification string of the message birth, which is
      *         an address of type int.
      */
    inline virtual QString fromThreadIdentify() const;

    /**
      * \brief  Returns the message type property.
      * \see    LMessageType
      */
    inline virtual LMessageType type() const;

    /**
      * \brief  Set the message type property.
      * \param[in] message type
      *
      * \see    LMessageType, LMessageType type()
      */
    inline virtual void setType(const LMessageType&);

    /**
      * \brief  Get the module name of the trigger message.
      * \see    Module name, For details, refer to Doxygen development document.
      */
    inline virtual QString moduleName() const;

    /**
      * \brief  Set the module name of the trigger message.
      * \param[in] Module name, For details, refer to doxygen development
      *            document.
      */
    inline virtual void setModuleName(const QString&);

    /**
      * \brief  Get message creation time.
      * \see    QDateTime::currentDateTime("yyyy/MM/dd hh:mm:ss.zzz")
      *         void setTime(const QString&)
      */
    inline virtual QString time() const;

    /**
      * \brief  Set message creation time.
      * \see    QDateTime::currentDateTime("yyyy/MM/dd hh:mm:ss.zzz")
      *         QString time() const
      */
    inline virtual void setTime(const QString&);

    /**
      * \brief  Is it an error message object.
      * \attention This method will be implemented by the subclass, and the
      *            base class will return false by default.
      */
    inline virtual bool isLErrorMessage() const;

    /**
      * \brief  Is it a tool attitude message object.
      * \attention This method will be implemented by the subclass, and the
      *            base class will return false by default
      */
    inline virtual bool isLToolAttitudeMessage() const;

    /**
      * \brief  Is it a component property message object.
      * \attention This method will be implemented by the subclass, and the
      *            base class will return false by default
      */
    inline virtual bool isLComponentAttributeMessage() const;

    /**
      * \brief  Is it a message object to recover information.
      * \attention This method will be implemented by the subclass, and the
      *            base class will return false by default
      */
    inline virtual bool isLRecoveryMessage() const;
protected:
    /** Unique identification code. */
    QString m_identify;

    QString m_classname;
    QString m_modulename;

    /**
     * Thread address string, which is converted from QThread::currentThread().
     */
    QString m_fromThreadIdentify;

    /** Message level type. */
    LMessageType m_type;

    /** Create time. */
    QString m_time;
};

#endif // LABSTRACTMESSAGE_H
