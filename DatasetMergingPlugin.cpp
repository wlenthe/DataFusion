/*
 * Your License or Copyright Information can go here
 */


#include "DatasetMergingPlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "DatasetMerging/moc_DatasetMergingPlugin.cpp"

Q_EXPORT_PLUGIN2(DatasetMergingPlugin, DatasetMergingPlugin)

namespace Detail
{
  const QString DatasetMergingPluginFile("DatasetMergingPlugin");
  const QString DatasetMergingPluginDisplayName("DatasetMergingPlugin");
  const QString DatasetMergingPluginBaseName("DatasetMergingPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DatasetMergingPlugin::DatasetMergingPlugin()
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

