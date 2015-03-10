/*
 * Your License should go here
 */
#ifndef _DatasetMergingConstants_H_
#define _DatasetMergingConstants_H_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace DatasetMerging {
  const QString DatasetMergingPluginFile("DatasetMergingPlugin");
  const QString DatasetMergingPluginDisplayName("DatasetMerging");
  const QString DatasetMergingBaseName("DatasetMergingPlugin");
  const DREAM3D_STRING UniqueFeatures("UniqueFeatures");
  const DREAM3D_STRING SimilarityCoefficient("SimilarityCoefficient");
  const DREAM3D_STRING Transformation("Transformation");
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{

/*  const QString SomeCustomWidget("SomeCustomWidget"); */

}
#endif
