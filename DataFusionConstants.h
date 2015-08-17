/*
 * Your License should go here
 */
#ifndef _DataFusionConstants_H_
#define _DataFusionConstants_H_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace DataFusionConstants
{
  const QString DataFusionPluginFile("DataFusionPlugin");
  const QString DataFusionPluginDisplayName("DataFusionPlugin");
  const QString DataFusionBaseName("DataFusionPlugin");

  const QString UniqueFeatures("UniqueFeatures");
  const QString Transformation("Transformation");
  const QString SimilarityCoefficient("SimilarityCoefficient");

  namespace FilterGroups
  {
  	const QString DataFusionFilters("DataFusion");
  }
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
#endif
