#include "lancetBackendBase.h"

#include <itkOutputWindow.h>
#include <mitkLog.h>

struct lancet::BackendBase::BackendBasePrivateImp
{
	std::string format;
	std::stringstream stream;
};

lancet::BackendBase::BackendBase()
	: imp(std::make_shared<BackendBasePrivateImp>())
{
	this->imp->format = "${message}";
}

void lancet::BackendBase::ProcessMessage(const mbilog::LogMessage& logMessage)
{
	std::string type_color = "";
	switch (logMessage.level)
	{
	case mbilog::Info:
		type_color = "\033[32m";
		break;

	case mbilog::Warn:
		type_color = "\033[33m";
		break;

	case mbilog::Error:
		type_color = "\033[31m";
		break;

	case mbilog::Fatal:
		type_color = "\033[35m";
		break;

	case mbilog::Debug:
		type_color = "\033[37m";
		break;
	}

	this->FormatFull(this->GetStdStringStream(), this->GetFormat(), logMessage);
	std::cout << type_color << this->GetStdStringStream().str() << "\033[0m";
	////itk::OutputWindow::GetInstance()->DisplayText(this->GetStdStringStream().str().c_str());
	this->GetStdStringStream().str("");
}

void lancet::BackendBase::SetFormat(const std::string& format)
{
	this->imp->format = format;
}

std::string lancet::BackendBase::GetFormat() const
{
	return this->imp->format;
}

std::stringstream& lancet::BackendBase::GetStdStringStream() const
{
	return this->imp->stream;
}