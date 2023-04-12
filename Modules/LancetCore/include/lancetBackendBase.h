#ifndef LancetBackendBase_H
#define LancetBackendBase_H

#include <sstream>
#include <iostream>
#include <mbilogBackendBase.h>

namespace lancet
{
class BackendBase : public mbilog::BackendBase
{
public:
  BackendBase();
  /**
   * \brief This method is called by the mbi logging mechanism if the object is registered in
   *         the mbi logging mechanism and a logging message is emitted.
   *
   * \param logMessage Logging message which was emitted.
   *
   */
  virtual void ProcessMessage(const mbilog::LogMessage& logMessage) override;

  /**
   * @return The type of this backend.
   */
  virtual mbilog::OutputType GetOutputType() const = 0;

  /**
	 * \brief Set the output format of log messages.
	 *
	 * \note FormatParameter
	 * ${type}	>> Message Type.
	 * ${file}	>> File Path.
	 * ${line}	>> Number of log message contacts.
	 * ${function}	>> Log Message Trigger Method.
	 * ${time:[yyyy.MM.dd.HH.ss.z]} >> Log message writing time, [yyyy.MM.dd.HH.ss.z]
	 *        format is Qt time conversion format.
	 * ${threadid}	>> Log message trigger thread ID.
	 * ${message}	>> Message content.
   * ${modulename} >> module name string.
   * ${category} >> Category of the logging event, which was defined by the user.
   */
  virtual void SetFormat(const std::string&);
  virtual std::string GetFormat() const;
protected:
  /**
   * \brief Method formats the given LogMessage into full/long format and writes 
   *        it to std:: stringstream.
   *
   * \param out Standard output stream object for C++.
   * \param format Target Format.
   * \param logMessage Logging message which was emitted.
   *
   */
  virtual void FormatFull(std::stringstream& out, const std::string& format, 
    const mbilog::LogMessage& logMessage) const = 0;
  /**
   * @return Standard output stream object for C++.
   */
  std::stringstream& GetStdStringStream() const;
private:
  struct BackendBasePrivateImp;
  std::shared_ptr<BackendBasePrivateImp> imp;
};
}

#endif //!LancetBackendBase_H
