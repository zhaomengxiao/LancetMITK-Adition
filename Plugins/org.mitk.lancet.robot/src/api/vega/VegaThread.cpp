/**
 * @file VegaThread.cpp
 * @author WUZHUOBIN jiejin2022.163.com
 * @since Mar.29.2020
 * @version 1.0
 * @copyright WUZHUOBIN All rights reserved.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the LICENSE for more detail.
 Copyright (c) WUZHUOBIN. All rights reserved.
 See COPYRIGHT for more detail.
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 Internal usage only, without the permission of the author, please DO
 NOT publish and distribute without the author's permission.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
#include "VegaThread.h"
 // vega
#include "CombinedApi.h"
// qt
#include <QDebug>
#include <QTimerEvent>
#include <QReadWriteLock>
#include <QCoreApplication>
#include <qsettings.h>
#include <qdir.h>
// vtk
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>
#include <vtkQuaternion.h>
// std
#include <sstream>

class Vega : public QObject
{
	Q_OBJECT;
public:
	typedef QHash<uint16_t, PortHandleInfo> PortHandleInfoMap;
	typedef QHash<uint16_t, ToolData> ToolDataMap;
	typedef QHash<QString, uint16_t> HandleMap;
	explicit Vega(unsigned int fps);
	virtual ~Vega() override;
public:
	Q_INVOKABLE void initializeImp();
	/**
	 * @brief Determines whether an NDI device supports the BX2 command by looking at the API revision
	 */
	void determineApiSupportForBX2();
	/**
	 * @brief Loads a tool from a tool definition file (.rom)
	 */
	void loadTool(const std::string &toolDefinitionFilePath) const;
	/**
	 * @brief Initialize and enable loaded tools. This is the same regardless of tool type.
	 */
	void initializeAndEnableTools();
	bool isConnectedImp() const;
	Q_INVOKABLE void startTrackingImp();
	Q_INVOKABLE void stopTrackingImp();

	bool warningOrError(int code, const QString &message = QString()) const;

	void tracking();

	Q_INVOKABLE QString getApiRevision() const;

	friend void VegaSpace::writeSettingFile();

	// ToolData getToolData(uint16_t i);

	// PortHandleInfo getPortHandleInfo(uint16_t i);

Q_SIGNALS:
	void error(const QString &message) const;
	void warning(const QString &message) const;
	void debug(const QString &message) const;
	void trackerUpdatedImp() const;

public:
	QReadWriteLock portHandleInfoMapLock;
	PortHandleInfoMap portHandleInfoMap;
	HandleMap handleMap;
	QReadWriteLock toolDataMapLock;
	ToolDataMap toolDataMap;
	static const QStringList SERIAL_NUMBER;

protected:
	virtual void timerEvent(QTimerEvent *event) override;
private:
	static const char VEGA_HOSTNAME[];
	static const char *MARKER_PATHS[];
	unsigned int fps;
	static constexpr const int WARNING_CODE_OFFSET = 1000;
	CombinedApi capi;
	bool capiConnected;
	bool apiSupportsBX2;
	int trackingTimerId;
};

const QStringList Vega::SERIAL_NUMBER{
	QStringLiteral("3C5DA803"), // PELVIS
	QStringLiteral("3C912C00"), // PROBE
	QStringLiteral("3C5DA800"), // ROBOT
	QStringLiteral("3C90F400")	// Tool0303
};//new

const char Vega::VEGA_HOSTNAME[] = "192.168.1.10";
//const char Vega::VEGA_HOSTNAME[] = "COM3";
const char *Vega::MARKER_PATHS[] = {
  "sroms/Pelvis.rom",
  "sroms/Probe.rom",
  "sroms/Robot.rom",
  "sroms/Tool.rom"
};

#include "VegaThread.moc"

void VegaSpace::writeSettingFile()
{
	QString appDir = QCoreApplication::applicationDirPath();
	QString backupDir = appDir+"/Backups";
	QDir dir(backupDir);
	if (!dir.exists())
	{
		dir.mkdir(backupDir);
	}
	QString fileName = backupDir + "/VegaSettings.ini";
	if (QFile::exists(fileName))
		return;
	QSettings vegaSetting(fileName,QSettings::IniFormat);
	vegaSetting.setValue("SERIAL_NUMBER.PELVIS", Vega::SERIAL_NUMBER[0]);
	vegaSetting.setValue("SERIAL_NUMBER.PROBE", Vega::SERIAL_NUMBER[1]);
	vegaSetting.setValue("SERIAL_NUMBER.ROBOT", Vega::SERIAL_NUMBER[2]);
	vegaSetting.setValue("SERIAL_NUMBER.TOOL", Vega::SERIAL_NUMBER[3]);
	vegaSetting.setValue("VEGA_HOSTNAME", Vega::VEGA_HOSTNAME);
	unsigned int pathNum=sizeof(Vega::MARKER_PATHS)/sizeof(char*);
	QString path;
	for (int i = 0; i < pathNum; i++)
	{
		path.append(Vega::MARKER_PATHS[i]);
		if (i != pathNum - 1)
			path.append(";");
	}
	vegaSetting.setValue("MARKER_PATHS", path);
	vegaSetting.setValue("debug", 0);
	vegaSetting.setValue("isFemoral", 0);
}

Vega::Vega(unsigned int fps) :
	QObject(nullptr),
	fps(fps),
	capiConnected(false),
	apiSupportsBX2(false),
	trackingTimerId(0)
{
	
}

Vega::~Vega()
{
	this->stopTrackingImp();
}

void Vega::initializeImp()
{
	QSettings setting(QCoreApplication::applicationDirPath() + "/Backups" + "/VegaSettings.ini", QSettings::IniFormat);
	int errorCode;
	errorCode = this->capi.connect(setting.value("VEGA_HOSTNAME").toString().toStdString());
	if (this->warningOrError(errorCode, QStringLiteral("capi.connect()")))
	{
		return;
	}
	// Print the firmware version for debugging purposes
	capi.getUserParameter("Features.Firmware.Version");

	// Determine if the connected device supports the BX2 command
	this->determineApiSupportForBX2();

	// Initialize the system. This clears all previously loaded tools, unsaved settings etc...
	errorCode = this->capi.initialize();
	if (this->warningOrError(errorCode, QStringLiteral("capi.initialize()"))) {
		return;
	}

	QStringList markerPaths = setting.value("MARKER_PATHS").toString().split(";");

	for (std::size_t i = 0; i < markerPaths.size(); ++i)
	{
		qDebug() << markerPaths[i];
		this->loadTool((QCoreApplication::applicationDirPath() + '/' + markerPaths[i]).toStdString());
	}

	this->initializeAndEnableTools();
	this->capiConnected = true;
}

void Vega::determineApiSupportForBX2()
{
	// Lookup the API revision
	std::string response = capi.getApiRevision();

	// Refer to the API guide for how to interpret the APIREV response
	char deviceFamily = response[0];
	int majorVersion = capi.stringToInt(response.substr(2, 3));

	// As of early 2017, the only NDI device supporting BX2 is the Vega
	// Vega is a Polaris device with API major version 003
	if (deviceFamily == 'G' && majorVersion >= 3)
	{
		this->apiSupportsBX2 = true;
	}
}

void Vega::loadTool(const std::string &toolDefinitionFilePath) const
{
	// Request a port handle to load a passive tool into
	int portHandle = capi.portHandleRequest();
	if (this->warningOrError(portHandle, QStringLiteral("capi.portHandleRequest()")))
	{
		return;
	}
	// Load the .rom file using the previously obtained port handle
	capi.loadSromToPort(toolDefinitionFilePath, portHandle);
}

void Vega::initializeAndEnableTools()
{
	// Initialize and enable tools
	std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::NotInit);
	for (int i = 0; i < portHandles.size(); i++)
	{
		int errorCode = 0;
		errorCode = capi.portHandleInitialize(portHandles[i].getPortHandle());
		// if error is found, skip this tool
		if (this->warningOrError(errorCode, QStringLiteral("capi.portHandleInitialize()"))) {
			// break;
			continue;
		}
		errorCode = capi.portHandleEnable(portHandles[i].getPortHandle());
		if (this->warningOrError(errorCode, QStringLiteral("capi.portHandleEnable()"))) {
			// break;
			continue;
		}
	}
	// Print all enabled tools
	portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::Enabled);
	this->portHandleInfoMapLock.lockForWrite();
	for (PortHandleInfo portHandle : portHandles) {
		std::string handle = portHandle.getPortHandle();
		portHandle = capi.portHandleInfo(handle);
		this->handleMap.insert(
			QString::fromStdString(portHandle.getSerialNumber()),
			static_cast<uint16_t>(capi.stringToInt(handle))
		);
		this->portHandleInfoMap.insert(
			static_cast<uint16_t>(capi.stringToInt(handle)),
			capi.portHandleInfo(handle)
		);
	}
	this->portHandleInfoMapLock.unlock();
}

bool Vega::isConnectedImp() const
{
	return this->capiConnected;
}

void Vega::startTrackingImp()
{
	if (!this->capiConnected)
	{
		this->warning("Vega is not conneted. ");
		this->warning("Check whether Vega is connected to your port and invoke VegaThread#initialize(). ");
		return;
	}
	int errorCode = capi.startTracking();
	if (this->warningOrError(errorCode, QStringLiteral("capi.startTracking()")))
	{
		return;
	}
	if (this->trackingTimerId != 0)
	{
		this->killTimer(this->trackingTimerId);
	}
	this->trackingTimerId = this->startTimer(1000 / this->fps);
	if (this->trackingTimerId == 0)
	{
		emit this->error("Vega could not start timer.");
	}
}

void Vega::stopTrackingImp()
{
	int errorCode = capi.stopTracking();
	if (this->warningOrError(errorCode, QStringLiteral("capi.stopTracking()")))
	{
		return;
		// emit this->error(errorCode, "capi.stopTracking()");
	}
	if (this->trackingTimerId != 0)
	{
		this->killTimer(this->trackingTimerId);
	}
}

bool Vega::warningOrError(int code, const QString &message) const
{
	if (code >= 0) {
		return false;
	}
	std::string codeToString = CombinedApi::errorToString(code);
	code *= -1; // restore the errorCode to a positive value
	if (code > WARNING_CODE_OFFSET)
	{
		emit this->warning(QString::fromStdString(codeToString) + ' ' + message);
		return false;
	}
	else
	{
		emit this->error(QString::fromStdString(codeToString) + ' ' + message);
		return true;
	}
}

void Vega::tracking()
{
	// Demonstrate BX or BX2 command
	this->toolDataMapLock.lockForWrite();
	//std::vector<ToolData> toolDatas = apiSupportsBX2 ? this->capi.getTrackingDataBX2() : this->capi.getTrackingDataBX();
	// Force using BX command
	std::vector<ToolData> toolDatas = this->capi.getTrackingDataBX();
	for (std::size_t i = 0; i < toolDatas.size(); ++i)
	{
		this->toolDataMap.insert(toolDatas[i].transform.toolHandle, toolDatas[i]);
	}
	this->toolDataMapLock.unlock();
	emit this->trackerUpdatedImp();
}

QString Vega::getApiRevision() const
{
	return QString::fromStdString(this->capi.getApiRevision());
}

void Vega::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == this->trackingTimerId)
	{
		this->tracking();
	}
}

// ToolData Vega::getToolData(uint16_t i)
// {
//   this->toolDataMapLock.lockForRead();
//   ToolData data = this->toolDataMap.value(i);
//   this->toolDataMapLock.unlock();
//   return data;
// }

// PortHandleInfo Vega::getPortHandleInfo(uint16_t i)
// {
//   this->portHandleInfoMapLock.lockForRead();
//   PortHandleInfo data = this->portHandleInfoMap.value(i, PortHandleInfo(std::string()));
//   this->portHandleInfoMapLock.unlock();
//   return data;
// }

////////////////////////////////////////////////////////////////////////////////
/// VegaThread
////////////////////////////////////////////////////////////////////////////////
VegaThread::VegaThread(unsigned int fps, QObject *parent)
	:QThread(parent),
	vega(new Vega(fps))
{
	this->vega->moveToThread(this);
	connect(this, &QThread::finished, this->vega, &QObject::deleteLater);
	connect(this->vega, &Vega::error, [](const QString &message) {
		qCritical() << message;
	});
	connect(this->vega, &Vega::warning, [](const QString &message) {
		qWarning() << message;
	});
	connect(this->vega, &Vega::debug, [](const QString &message) {
		qDebug() << message;
	});
	connect(this->vega, &Vega::trackerUpdatedImp, this, &VegaThread::trackerUpdated);
	this->start();
}

VegaThread::VegaThread(QObject *parent) :
	VegaThread(20, parent)
{

}

VegaThread::~VegaThread()
{
	// this->quit();
	this->terminate();
	this->wait();
}

void VegaThread::initialize() const
{
	QMetaObject::invokeMethod(this->vega, "initializeImp", Qt::QueuedConnection);
}

void VegaThread::startTracking() const
{
	QMetaObject::invokeMethod(this->vega, "startTrackingImp", Qt::QueuedConnection);
}

void VegaThread::stopTracking() const
{
	QMetaObject::invokeMethod(this->vega, "stopTrackingImp", Qt::QueuedConnection);
}

QString VegaThread::getApiRevision() const
{
	QString ret;
	QMetaObject::invokeMethod(this->vega, "getApiRevision", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, ret));
	return ret;
}

bool VegaThread::isConnected() const
{
	return this->vega->isConnectedImp();
}

ToolData VegaThread::getToolData(unsigned short handle)
{
	this->vega->toolDataMapLock.lockForRead();
	ToolData data = this->vega->toolDataMap.value(handle);
	this->vega->toolDataMapLock.unlock();
	return data;
}

PortHandleInfo VegaThread::getPortHandleInfo(unsigned short handle)
{
	this->vega->portHandleInfoMapLock.lockForRead();
	PortHandleInfo data = this->vega->portHandleInfoMap.value(handle, PortHandleInfo(std::string()));
	this->vega->portHandleInfoMapLock.unlock();
	return data;
}

const QList<unsigned short> VegaThread::getHandles()
{
	this->vega->portHandleInfoMapLock.lockForRead();
	const QList<unsigned short> handles = this->vega->portHandleInfoMap.uniqueKeys();
	this->vega->portHandleInfoMapLock.unlock();
	return handles;
}

void VegaThread::getMatrix4x4(unsigned short handle, vtkMatrix4x4 *matrix)
{
	ToolData toolData = this->getToolData(static_cast<uint16_t>(handle));
	Transform transform = toolData.transform;
	double data_[16] = { 0 };
	data_[15] = 1;
	data_[0 * 4 + 3] = transform.tx;
	data_[1 * 4 + 3] = transform.ty;
	data_[2 * 4 + 3] = transform.tz;
	double data[3][3];
	vtkQuaterniond quaternion(transform.q0, transform.qx, transform.qy, transform.qz);
	quaternion.ToMatrix3x3(data);
	memcpy(data_ + 0 * 4, data[0], sizeof(double[3]));
	memcpy(data_ + 1 * 4, data[1], sizeof(double[3]));
	memcpy(data_ + 2 * 4, data[2], sizeof(double[3]));
	matrix->DeepCopy(data_);
}

void VegaThread::setReferenceHandle(unsigned short handle)
{
	this->referenceHandle = handle;
}

unsigned short VegaThread::getReferenceHandle() const
{
	return this->referenceHandle;
}

void VegaThread::getReferenceMatrix4x4(unsigned short handle, vtkMatrix4x4 *matrix)
{
	vtkSmartPointer<vtkMatrix4x4> refMatrix =
		vtkSmartPointer<vtkMatrix4x4>::New();
	this->getMatrix4x4(this->referenceHandle, refMatrix);
	refMatrix->Invert();
	vtkSmartPointer<vtkMatrix4x4> cacheMatrix =
		vtkSmartPointer<vtkMatrix4x4>::New();
	this->getMatrix4x4(handle, cacheMatrix);
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->SetMatrix(cacheMatrix);
	transform->Concatenate(refMatrix);
	transform->Update();
	transform->GetMatrix(matrix);
}

unsigned short VegaThread::partNumberToHandle(int partNumber)
{
	QString snType;
	switch (partNumber)
	{
	case 0:
		snType = "SERIAL_NUMBER.PELVIS";
		break;
	case 1:
		snType = "SERIAL_NUMBER.PROBE";
		break;
	case 2:
		snType = "SERIAL_NUMBER.ROBOT";
		break;
	case 3:
		snType = "SERIAL_NUMBER.TOOL";
		break;
	default:
		break;
	}
	QSettings setting(QCoreApplication::applicationDirPath() + "/Backups" + "/VegaSettings.ini", QSettings::IniFormat);
	unsigned short handle = this->vega->handleMap[setting.value(snType).toString()];
	return handle;
}

int VegaThread::handleToPartNumber(unsigned short handle)
{
	QString serialNumber = QString::fromStdString(this->getPortHandleInfo(handle).getSerialNumber());
	QSettings setting(QCoreApplication::applicationDirPath() + "/Backups" + "/VegaSettings.ini", QSettings::IniFormat);
	QStringList keys=setting.allKeys();
	for (int i = 0; i < keys.size(); i++)
	{
		if (setting.value(keys[i]).toString() == serialNumber)
		{
			if (keys[i] == "SERIAL_NUMBER.PELVIS")
				return 0;
			if (keys[i] == "SERIAL_NUMBER.PROBE")
				return 1;
			if (keys[i] == "SERIAL_NUMBER.ROBOT")
				return 2;
			if (keys[i] == "SERIAL_NUMBER.TOOL")
				return 3;
		}
	}
	return -1;
	//return Vega::SERIAL_NUMBER.indexOf(serialNumber);
}
