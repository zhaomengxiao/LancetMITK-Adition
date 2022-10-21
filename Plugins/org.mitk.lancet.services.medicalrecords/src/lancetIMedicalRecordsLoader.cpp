#include "lancetIMedicalRecordsLoader.h"

namespace lancet
{
	struct IMedicalRecordsLoader::IMedicalRecordsLoaderPrivateImp
	{
		QString fileName;
	};

	IMedicalRecordsLoader::IMedicalRecordsLoader()
		: imp(std::make_shared<IMedicalRecordsLoaderPrivateImp>())
	{
	}
	QString IMedicalRecordsLoader::GetFileName() const
	{
		return this->imp->fileName;
	}
	void IMedicalRecordsLoader::SetFileName(const QString& fp)
	{
		this->imp->fileName = fp;
	}
}