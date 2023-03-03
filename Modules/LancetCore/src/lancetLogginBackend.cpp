#include "lancetLogginBackend.h"

#include <cstdint>

#include <QRegExp>
#include <QThread>
#include <QString>
#include <QDateTime>
#include <mbilog.h>
#include <mbilogLoggingTypes.h>

static lancet::LogginBackend* g_log = nullptr;

lancet::LogginBackend::LogginBackend()
{
}

mbilog::OutputType lancet::LogginBackend::GetOutputType() const
{
	return mbilog::Console;
}

void lancet::LogginBackend::Register()
{
	if (nullptr == g_log)
	{
		g_log = new lancet::LogginBackend;
	}
	g_log->SetFormat("${function}-${line}-${time:yyyy.MM.dd.HH.ss.z}: ${message}");
	mbilog::RegisterBackend(g_log);
}

void lancet::LogginBackend::Unregister()
{
	mbilog::UnregisterBackend(g_log);
	if (g_log)
	{
		delete g_log;
		g_log = nullptr;
	}
}

void lancet::LogginBackend::FormatFull(std::stringstream& out, 
	const std::string& format, const mbilog::LogMessage& logMessage) const
{
	QString qformat(format.c_str());
	qformat = qformat.toLower();

	// ${type}
	QString type = this->GetMessageType(logMessage).c_str();
	type = type.toUpper();
	qformat.replace(QRegExp("\\$\\{type\\}"), type);

	// ${file}
	qformat.replace(QRegExp("\\$\\{file\\}"), logMessage.filePath);
	// ${function}
	qformat.replace(QRegExp("\\$\\{function\\}"), logMessage.functionName);
	// ${line}
	qformat.replace(QRegExp("\\$\\{line\\}"), QString::number(logMessage.lineNumber));
	// ${threadid}
	QString threadIdStr = "";
	Qt::HANDLE threadId = QThread::currentThreadId();
	int threadIdInt = (int)(intptr_t)threadId;
	threadIdStr = "0x" + QString::number(threadIdInt, 16);
	qformat.replace(QRegExp("\\$\\{threadid\\}"), threadIdStr);
	// ${message}
	qformat.replace(QRegExp("\\$\\{message\\}"), logMessage.message.c_str());
	// ${modulename}
	qformat.replace(QRegExp("\\$\\{modulename\\}"), logMessage.moduleName);
	// ${category}
	qformat.replace(QRegExp("\\$\\{category\\}"), logMessage.category.c_str());

	QString qTimeFormat = this->GetQtTimeFromat(this->GetFormat()).c_str();
	QString qTimeString = QDateTime::currentDateTime().toString(qTimeFormat);
	qformat.replace(QRegExp("\\$\\{time:[a-zA-Z\\.]+(\\})"), qTimeString);
	// return
	out << qformat.toStdString().c_str() << std::endl;
}

std::string lancet::LogginBackend::GetMessageType(const mbilog::LogMessage& logMessage) const
{
	switch (logMessage.level)
	{
	case mbilog::Info:
		return "INFO";
		break;

	case mbilog::Warn:
		return "WARN";
		break;

	case mbilog::Error:
		return "ERROR";
		break;

	case mbilog::Fatal:
		return "FATAL";
		break;

	case mbilog::Debug:
		return "DEBUG";
		break;
	}
	return std::string("UNKNOWN");
}

std::string lancet::LogginBackend::GetQtTimeFromat(const std::string& format) const
{
	std::string retval;
	QString qformat(format.c_str());

	int findIndexStart = qformat.indexOf(QRegExp("\\$\\{time:[a-zA-Z\\.]+(\\})"));

	if (findIndexStart != -1)
	{
		for (auto index = findIndexStart + sizeof("${time:") - 1; 
			index < qformat.size(); ++index)
		{
			if (qformat.at(index) == "}")
			{
				break;
			}
			retval += qformat.at(index).toLatin1();
		}
	}
	return retval;
}
