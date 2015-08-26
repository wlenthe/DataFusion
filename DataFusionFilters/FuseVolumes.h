/*
 * Your License or Copyright can go here
 */

#ifndef _FuseVolumes_H_
#define _FuseVolumes_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DLib/FilterParameters/DynamicTableData.h"

/**
 * @brief The FuseVolumes class. See [Filter documentation](@ref fusevolumes) for details.
 */
class FuseVolumes : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(FuseVolumes)
    DREAM3D_STATIC_NEW_MACRO(FuseVolumes)
    DREAM3D_TYPE_MACRO_SUPER(FuseVolumes, AbstractFilter)

    virtual ~FuseVolumes();

    //user paramters
    DREAM3D_FILTER_PARAMETER(QString, Prefix)
    Q_PROPERTY(QString Prefix READ getPrefix WRITE setPrefix)

    DREAM3D_FILTER_PARAMETER(int, TransformationType)
    Q_PROPERTY(int TransformationType READ getTransformationType WRITE setTransformationType)

    DREAM3D_FILTER_PARAMETER(DynamicTableData, ManualTransformation)
    Q_PROPERTY(DynamicTableData ManualTransformation READ getManualTransformation WRITE setManualTransformation)

    //input array paths
    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceVolume)
    Q_PROPERTY(DataArrayPath ReferenceVolume READ getReferenceVolume WRITE setReferenceVolume)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingVolume)
    Q_PROPERTY(DataArrayPath MovingVolume READ getMovingVolume WRITE setMovingVolume)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, TransformationArrayPath)
    Q_PROPERTY(DataArrayPath TransformationArrayPath READ getTransformationArrayPath WRITE setTransformationArrayPath)


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
    FuseVolumes();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    DEFINE_DATAARRAY_VARIABLE(float, Transformation)

    FuseVolumes(const FuseVolumes&); // Copy Constructor Not Implemented
    void operator=(const FuseVolumes&); // Operator '=' Not Implemented
};

#endif /* _FuseVolumes_H_ */
