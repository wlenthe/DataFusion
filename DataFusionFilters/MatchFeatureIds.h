/*
 * Your License or Copyright can go here
 */

#ifndef _MatchFeatureIds_H_
#define _MatchFeatureIds_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

/**
 * @brief The MatchFeatureIds class. See [Filter documentation](@ref MatchFeatureIds) for details.
 */
class MatchFeatureIds : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(MatchFeatureIds)
    DREAM3D_STATIC_NEW_MACRO(MatchFeatureIds)
    DREAM3D_TYPE_MACRO_SUPER(MatchFeatureIds, AbstractFilter)

    virtual ~MatchFeatureIds();

    //input array paths
    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath ReferenceCellFeatureAttributeMatrixPath READ getReferenceCellFeatureAttributeMatrixPath WRITE setReferenceCellFeatureAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath MovingCellFeatureAttributeMatrixPath READ getMovingCellFeatureAttributeMatrixPath WRITE setMovingCellFeatureAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceFeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceFeatureIdsArrayPath READ getReferenceFeatureIdsArrayPath WRITE setReferenceFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingFeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath MovingFeatureIdsArrayPath READ getMovingFeatureIdsArrayPath WRITE setMovingFeatureIdsArrayPath)

        //orientation matching
        DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceQuatsArrayPath)
        Q_PROPERTY(DataArrayPath ReferenceQuatsArrayPath READ getReferenceQuatsArrayPath WRITE setReferenceQuatsArrayPath)

        DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingQuatsArrayPath)
        Q_PROPERTY(DataArrayPath MovingQuatsArrayPath READ getMovingQuatsArrayPath WRITE setMovingQuatsArrayPath)
        
        DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferencePhasesArrayPath)
        Q_PROPERTY(DataArrayPath ReferencePhasesArrayPath READ getReferencePhasesArrayPath WRITE setReferencePhasesArrayPath)

        DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingPhasesArrayPath)
        Q_PROPERTY(DataArrayPath MovingPhasesArrayPath READ getMovingPhasesArrayPath WRITE setMovingPhasesArrayPath)

        DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCrystalStructuresArrayPath)
        Q_PROPERTY(DataArrayPath ReferenceCrystalStructuresArrayPath READ getReferenceCrystalStructuresArrayPath WRITE setReferenceCrystalStructuresArrayPath)

        DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCrystalStructuresArrayPath)
        Q_PROPERTY(DataArrayPath MovingCrystalStructuresArrayPath READ getMovingCrystalStructuresArrayPath WRITE setMovingCrystalStructuresArrayPath)

    //output array names
    DREAM3D_FILTER_PARAMETER(QString, OverlapArrayName)
    Q_PROPERTY(QString OverlapArrayName READ getOverlapArrayName WRITE setOverlapArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ReferenceUniqueArrayName)
    Q_PROPERTY(QString ReferenceUniqueArrayName READ getReferenceUniqueArrayName WRITE setReferenceUniqueArrayName)

    DREAM3D_FILTER_PARAMETER(QString, MovingUniqueArrayName)
    Q_PROPERTY(QString MovingUniqueArrayName READ getMovingUniqueArrayName WRITE setMovingUniqueArrayName)

    //user paramters
    DREAM3D_FILTER_PARAMETER(int, Metric)
    Q_PROPERTY(int Metric READ getMetric WRITE setMetric)
    
    DREAM3D_FILTER_PARAMETER(double, MetricThreshold)
    Q_PROPERTY(double MetricThreshold READ getMetricThreshold WRITE setMetricThreshold)

    DREAM3D_FILTER_PARAMETER(bool, UseOrientations)
    Q_PROPERTY(bool UseOrientations READ getUseOrientations WRITE setUseOrientations)

    DREAM3D_FILTER_PARAMETER(double, OrientationTolerance)
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
    MatchFeatureIds();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;
    DEFINE_DATAARRAY_VARIABLE(float, Overlap)
    DEFINE_DATAARRAY_VARIABLE(bool, ReferenceUnique)
    DEFINE_DATAARRAY_VARIABLE(bool, MovingUnique)
    DEFINE_DATAARRAY_VARIABLE(int32_t, ReferenceFeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, MovingFeatureIds)
    DEFINE_DATAARRAY_VARIABLE(float, ReferenceQuats)
    DEFINE_DATAARRAY_VARIABLE(float, MovingQuats)
    DEFINE_DATAARRAY_VARIABLE(int32_t, ReferencePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, MovingPhases)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, ReferenceCrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, MovingCrystalStructures)

    MatchFeatureIds(const MatchFeatureIds&); // Copy Constructor Not Implemented
    void operator=(const MatchFeatureIds&); // Operator '=' Not Implemented
};

#endif /* _MatchFeatureIds_H_ */
