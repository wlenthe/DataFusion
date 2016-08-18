/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                             *
 * Copyright (c) 2015 William Lenthe                                           *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
 *                                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#ifndef _MatchFeatureIdsGeometry_H_
#define _MatchFeatureIdsGeometry_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"

#include "MatchFeatureIds.h"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

/**
 * @brief The MatchFeatureIdsGeometry class. See [Filter documentation](@ref MatchFeatureIdsGeometry) for details.
 */
class MatchFeatureIdsGeometry : public MatchFeatureIds
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(MatchFeatureIdsGeometry)
    SIMPL_STATIC_NEW_MACRO(MatchFeatureIdsGeometry)
    SIMPL_TYPE_MACRO_SUPER(MatchFeatureIdsGeometry, AbstractFilter)

    virtual ~MatchFeatureIdsGeometry();

    //orientation matching
    SIMPL_FILTER_PARAMETER(DataArrayPath, ReferenceQuatsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceQuatsArrayPath READ getReferenceQuatsArrayPath WRITE setReferenceQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MovingQuatsArrayPath)
    Q_PROPERTY(DataArrayPath MovingQuatsArrayPath READ getMovingQuatsArrayPath WRITE setMovingQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, ReferencePhasesArrayPath)
    Q_PROPERTY(DataArrayPath ReferencePhasesArrayPath READ getReferencePhasesArrayPath WRITE setReferencePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MovingPhasesArrayPath)
    Q_PROPERTY(DataArrayPath MovingPhasesArrayPath READ getMovingPhasesArrayPath WRITE setMovingPhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, ReferenceCrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceCrystalStructuresArrayPath READ getReferenceCrystalStructuresArrayPath WRITE setReferenceCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MovingCrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath MovingCrystalStructuresArrayPath READ getMovingCrystalStructuresArrayPath WRITE setMovingCrystalStructuresArrayPath)

    //output array names
    SIMPL_FILTER_PARAMETER(QString, OverlapArrayName)
    Q_PROPERTY(QString OverlapArrayName READ getOverlapArrayName WRITE setOverlapArrayName)

    //user paramters
    SIMPL_FILTER_PARAMETER(int, Metric)
    Q_PROPERTY(int Metric READ getMetric WRITE setMetric)

    SIMPL_FILTER_PARAMETER(double, MetricThreshold)
    Q_PROPERTY(double MetricThreshold READ getMetricThreshold WRITE setMetricThreshold)

    SIMPL_FILTER_PARAMETER(bool, UseOrientations)
    Q_PROPERTY(bool UseOrientations READ getUseOrientations WRITE setUseOrientations)

    SIMPL_FILTER_PARAMETER(double, OrientationTolerance)
    Q_PROPERTY(double OrientationTolerance READ getOrientationTolerance WRITE setOrientationTolerance)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

   /**
    * @brief execute Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    MatchFeatureIdsGeometry();

    virtual std::vector<size_t> match();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;
    DEFINE_DATAARRAY_VARIABLE(float, Overlap)
    DEFINE_DATAARRAY_VARIABLE(float, ReferenceQuats)
    DEFINE_DATAARRAY_VARIABLE(float, MovingQuats)
    DEFINE_DATAARRAY_VARIABLE(int32_t, ReferencePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, MovingPhases)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, ReferenceCrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, MovingCrystalStructures)

    MatchFeatureIdsGeometry(const MatchFeatureIdsGeometry&); // Copy Constructor Not Implemented
    void operator=(const MatchFeatureIdsGeometry&); // Operator '=' Not Implemented
};



#endif /* _MatchFeatureIdsGeometry_H_ */
