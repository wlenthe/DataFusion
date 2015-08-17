/*
 * Your License or Copyright can go here
 */

#ifndef _RegisterPointSets_H_
#define _RegisterPointSets_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @brief The RegisterPointSets class. See [Filter documentation](@ref registerpointsets) for details.
 */
class RegisterPointSets : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(RegisterPointSets)
    DREAM3D_STATIC_NEW_MACRO(RegisterPointSets)
    DREAM3D_TYPE_MACRO_SUPER(RegisterPointSets, AbstractFilter)

    virtual ~RegisterPointSets();

    //input array paths
    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCentroidsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceCentroidsArrayPath READ getReferenceCentroidsArrayPath WRITE setReferenceCentroidsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCentroidsArrayPath)
    Q_PROPERTY(DataArrayPath MovingCentroidsArrayPath READ getMovingCentroidsArrayPath WRITE setMovingCentroidsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceGoodFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceGoodFeaturesArrayPath READ getReferenceGoodFeaturesArrayPath WRITE setReferenceGoodFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingGoodFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath MovingGoodFeaturesArrayPath READ getMovingGoodFeaturesArrayPath WRITE setMovingGoodFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, WeightsArrayPath)
    Q_PROPERTY(DataArrayPath WeightsArrayPath READ getWeightsArrayPath WRITE setWeightsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, AttributeMatrixName)
    Q_PROPERTY(QString AttributeMatrixName READ getAttributeMatrixName WRITE setAttributeMatrixName)

    //parameters
    DREAM3D_INSTANCE_PROPERTY(bool, AllowTranslation)
    Q_PROPERTY(bool AllowTranslation READ getAllowTranslation WRITE setAllowTranslation)

    DREAM3D_INSTANCE_PROPERTY(bool, AllowRotation)
    Q_PROPERTY(bool AllowRotation READ getAllowRotation WRITE setAllowRotation)

    DREAM3D_INSTANCE_PROPERTY(bool, AllowScaling)
    Q_PROPERTY(bool AllowScaling READ getAllowScaling WRITE setAllowScaling)

    DREAM3D_INSTANCE_PROPERTY(int, ScalingType)
    Q_PROPERTY(int ScalingType READ getScalingType WRITE setScalingType)

    DREAM3D_INSTANCE_PROPERTY(bool, AllowShearing)
    Q_PROPERTY(bool AllowShearing READ getAllowShearing WRITE setAllowShearing)

    DREAM3D_INSTANCE_PROPERTY(bool, UseGoodPoints)
    Q_PROPERTY(bool UseGoodPoints READ getUseGoodPoints WRITE setUseGoodPoints)
    
    DREAM3D_INSTANCE_PROPERTY(bool, UseWeights)
    Q_PROPERTY(bool UseWeights READ getUseWeights WRITE setUseWeights)

    //created arrays
    DREAM3D_FILTER_PARAMETER(QString, TransformName)
    Q_PROPERTY(QString TransformName READ getTransformName WRITE setTransformName)


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
    RegisterPointSets();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    DEFINE_DATAARRAY_VARIABLE(float, ReferenceCentroids)
    DEFINE_DATAARRAY_VARIABLE(float, MovingCentroids)
    DEFINE_DATAARRAY_VARIABLE(bool, ReferenceGoodFeatures)
    DEFINE_DATAARRAY_VARIABLE(bool, MovingGoodFeatures)
    DEFINE_DATAARRAY_VARIABLE(float, Weights)
    DEFINE_DATAARRAY_VARIABLE(float, Transform)

    RegisterPointSets(const RegisterPointSets&); // Copy Constructor Not Implemented
    void operator=(const RegisterPointSets&); // Operator '=' Not Implemented
};

#endif /* _RegisterPointSets_H_ */
