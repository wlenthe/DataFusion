/*
 * Your License or Copyright Information can go here
 */

#ifndef _MatchFeatureIds_H_
#define _MatchFeatureIds_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

 #include "OrientationLib/OrientationOps/OrientationOps.h"

/**
 * @class MatchFeatureIds MatchFeatureIds.h DatasetMerging/DatasetMergingFilters/MatchFeatureIds.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class MatchFeatureIds : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(MatchFeatureIds)
    DREAM3D_STATIC_NEW_MACRO(MatchFeatureIds)
    DREAM3D_TYPE_MACRO_SUPER(MatchFeatureIds, AbstractFilter)

    virtual ~MatchFeatureIds();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceFeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceFeatureIdsArrayPath READ getReferenceFeatureIdsArrayPath WRITE setReferenceFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingFeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath MovingFeatureIdsArrayPath READ getMovingFeatureIdsArrayPath WRITE setMovingFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath MovingCellFeatureAttributeMatrixPath READ getMovingCellFeatureAttributeMatrixPath WRITE setMovingCellFeatureAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath ReferenceCellFeatureAttributeMatrixPath READ getReferenceCellFeatureAttributeMatrixPath WRITE setReferenceCellFeatureAttributeMatrixPath)


    DREAM3D_FILTER_PARAMETER(int, Metric)
    Q_PROPERTY(int Metric READ getMetric WRITE setMetric)
    
    DREAM3D_FILTER_PARAMETER(float, MetricThreshold)
    Q_PROPERTY(float MetricThreshold READ getMetricThreshold WRITE setMetricThreshold)

    DREAM3D_FILTER_PARAMETER(QString, OverlapArrayName)
    Q_PROPERTY(QString OverlapArrayName READ getOverlapArrayName WRITE setOverlapArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ReferenceUniqueArrayName)
    Q_PROPERTY(QString ReferenceUniqueArrayName READ getReferenceUniqueArrayName WRITE setReferenceUniqueArrayName)

    DREAM3D_FILTER_PARAMETER(QString, MovingUniqueArrayName)
    Q_PROPERTY(QString MovingUniqueArrayName READ getMovingUniqueArrayName WRITE setMovingUniqueArrayName)


    DREAM3D_FILTER_PARAMETER(bool, UseOrientations)
    Q_PROPERTY(bool UseOrientations READ getUseOrientations WRITE setUseOrientations)

    DREAM3D_FILTER_PARAMETER(float, OrientationTolerance)
    Q_PROPERTY(float OrientationTolerance READ getOrientationTolerance WRITE setOrientationTolerance)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceQuatsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceQuatsArrayPath READ getReferenceQuatsArrayPath WRITE setReferenceQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferencePhasesArrayPath)
    Q_PROPERTY(DataArrayPath ReferencePhasesArrayPath READ getReferencePhasesArrayPath WRITE setReferencePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceCrystalStructuresArrayPath READ getReferenceCrystalStructuresArrayPath WRITE setReferenceCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingQuatsArrayPath)
    Q_PROPERTY(DataArrayPath MovingQuatsArrayPath READ getMovingQuatsArrayPath WRITE setMovingQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingPhasesArrayPath)
    Q_PROPERTY(DataArrayPath MovingPhasesArrayPath READ getMovingPhasesArrayPath WRITE setMovingPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath MovingCrystalStructuresArrayPath READ getMovingCrystalStructuresArrayPath WRITE setMovingCrystalStructuresArrayPath)




    /**
     * @brief getCompiledLibraryName Returns the name of the Library that this filter is a part of
     * @return
     */
    virtual const QString getCompiledLibraryName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const QString getSubGroupName();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    * @param index The index that the data should be written to
    * @return Returns the next index for writing
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    * @param index The index to read the information from
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

   /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

    /**
     * @brief newFilterInstance Returns a new instance of the filter optionally copying the filter parameters from the
     * current filter to the new instance.
     * @param copyFilterParameters
     * @return
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  signals:
    /**
     * @brief updateFilterParameters This is emitted when the filter requests all the latest Filter Parameters need to be
     * pushed from a user facing control such as the FilterParameter Widget
     * @param filter The filter to push the values into
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged This signal can be emitted when any of the filter parameters are changed internally.
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before the dataCheck() is called. This can change if needed.
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after the dataCheck() is called. Typically. This can change if needed.
     */
    void preflightExecuted();

  protected:
    MatchFeatureIds();

    /**
    * @brief Checks for the appropriate parameter values and availability of arrays in the data container
    */
    void dataCheck();

  private:
    std::vector<OrientationOps::Pointer> m_OrientationOps;
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Overlap)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, ReferenceUnique)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, MovingUnique)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, ReferenceFeatureIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, MovingFeatureIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, ReferenceQuats)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, MovingQuats)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, ReferencePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, MovingPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, ReferenceCrystalStructures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, MovingCrystalStructures)

    MatchFeatureIds(const MatchFeatureIds&); // Copy Constructor Not Implemented
    void operator=(const MatchFeatureIds&); // Operator '=' Not Implemented
};

#endif /* _MatchFeatureIds_H_ */
