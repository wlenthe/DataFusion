/*
 * Your License or Copyright can go here
 */

#ifndef _RegisterOrientations_H_
#define _RegisterOrientations_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

/**
 * @brief The RegisterOrientations class. See [Filter documentation](@ref registerorientations) for details.
 */
class RegisterOrientations : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(RegisterOrientations)
    DREAM3D_STATIC_NEW_MACRO(RegisterOrientations)
    DREAM3D_TYPE_MACRO_SUPER(RegisterOrientations, AbstractFilter)

    virtual ~RegisterOrientations();

    //required arrays
    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceAvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceAvgQuatsArrayPath READ getReferenceAvgQuatsArrayPath WRITE setReferenceAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingAvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath MovingAvgQuatsArrayPath READ getMovingAvgQuatsArrayPath WRITE setMovingAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferencePhasesArrayPath)
    Q_PROPERTY(DataArrayPath ReferencePhasesArrayPath READ getReferencePhasesArrayPath WRITE setReferencePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingPhasesArrayPath)
    Q_PROPERTY(DataArrayPath MovingPhasesArrayPath READ getMovingPhasesArrayPath WRITE setMovingPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceCrystalStructuresArrayPath READ getReferenceCrystalStructuresArrayPath WRITE setReferenceCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath MovingCrystalStructuresArrayPath READ getMovingCrystalStructuresArrayPath WRITE setMovingCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceGoodFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceGoodFeaturesArrayPath READ getReferenceGoodFeaturesArrayPath WRITE setReferenceGoodFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingGoodFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath MovingGoodFeaturesArrayPath READ getMovingGoodFeaturesArrayPath WRITE setMovingGoodFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(double, MinMiso)
    Q_PROPERTY(double MinMiso READ getMinMiso WRITE setMinMiso)

    DREAM3D_FILTER_PARAMETER(bool, UseGoodFeatures)
    Q_PROPERTY(bool UseGoodFeatures READ getUseGoodFeatures WRITE setUseGoodFeatures)


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
    RegisterOrientations();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;
    DEFINE_DATAARRAY_VARIABLE(float, ReferenceAvgQuats)
    DEFINE_DATAARRAY_VARIABLE(float, MovingAvgQuats)
    DEFINE_DATAARRAY_VARIABLE(bool, ReferenceGoodFeatures)
    DEFINE_DATAARRAY_VARIABLE(bool, MovingGoodFeatures)
    DEFINE_DATAARRAY_VARIABLE(int32_t, ReferencePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, MovingPhases)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, ReferenceCrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, MovingCrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(float, Transform)

    RegisterOrientations(const RegisterOrientations&); // Copy Constructor Not Implemented
    void operator=(const RegisterOrientations&); // Operator '=' Not Implemented
};

#endif /* _RegisterOrientations_H_ */
