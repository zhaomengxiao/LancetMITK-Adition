#ifndef LancetLogginBackend_H
#define LancetLogginBackend_H

#include "lancetBackendBase.h"

namespace lancet
{
class LogginBackend : public BackendBase
{
public:
	LogginBackend();

  /**
   * @return The type of this backend.
   */
  virtual mbilog::OutputType GetOutputType() const override;

  /** \brief registers lancet logging backend at mbilog
   */
  static void Register();

  /** \brief Unregisters lancet logging backend at mbilog
   */
  static void Unregister();
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
    const mbilog::LogMessage& logMessage) const override;

  /**
   * @return Type converted string for log messages.
   */
  std::string GetMessageType(const mbilog::LogMessage& logMessage) const;

  /**
   * @return Time format string of Qt.
   * 
   * \param format Target Format.
   */
  std::string GetQtTimeFromat(const std::string& format) const;
};
}
#endif //!LancetLogginBackend_H
