/*
 * Your License or Copyright Information can go here
 */


#include "DatasetMergingPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "DatasetMerging/moc_DatasetMergingPlugin.cpp"

namespace Detail
{
  const QString DatasetMergingPluginFile("DatasetMergingPlugin");
  const QString DatasetMergingPluginDisplayName("DatasetMerging");
  const QString DatasetMergingPluginBaseName("DatasetMergingPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DatasetMergingPlugin::DatasetMergingPlugin() :
m_Version("Version Number"),                            // Initialize DatasetMerging's Version Number Here
m_CompatibilityVersion("Compatibility Version Number"), // Initialize DatasetMerging's Compatibility Version Number Here
m_Vendor("Vendor Name"),                                // Initialize DatasetMerging's Vendor Name Here
m_URL("URL"),                                           // Initialize Company URL Here
m_Location("Location"),                                 // Initialize Company Location Here
m_Description("Description"),                           // Initialize DatasetMerging's Description Here
m_Copyright("Copyright"),                               // Initialize DatasetMerging's Copyright Here
m_Filters(QList<QString>()),                        // Initialize DatasetMerging's List of Dependencies Here
m_DidLoad(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DatasetMergingPlugin::~DatasetMergingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getPluginName()
{
  return (Detail::DatasetMergingPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getDescription()
{
  /* PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
  It is located at DatasetMerging/Resources/DatasetMerging/DatasetMergingDescription.txt */

  QFile licenseFile(":/DatasetMerging/DatasetMergingDescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DatasetMergingPlugin::getLicense()
{
  /* PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
  It is located at DatasetMerging/Resources/DatasetMerging/DatasetMergingLicense.txt */

  QFile licenseFile(":/DatasetMerging/DatasetMergingLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> DatasetMergingPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");
  fileStrList.push_back(":/ThirdParty/Boost.txt");
  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for (QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if ( licenseFileInfo.exists() )
    {
      if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
      {
        QTextStream in(&file);
        licenseMap.insert(licenseFileInfo.baseName(), in.readAll());
      }
    }
  }

  return licenseMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DatasetMergingPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DatasetMergingPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DatasetMergingPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DatasetMergingPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DatasetMergingPlugin::readSettings(QSettings& prefs)
{

}

#include "DatasetMergingFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

