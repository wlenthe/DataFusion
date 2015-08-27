/*
 *
 * Copyright (c) 2015 William Lenthe
 *
 * This file is part of This program.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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

  const QString DataFusionPluginDisplayName("DataFusion");
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
