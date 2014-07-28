/*
 * Your License or Copyright Information can go here
 */

#ifndef _GenerateDatasetMisorientationColors_H_
#define _GenerateDatasetMisorientationColors_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class GenerateDatasetMisorientationColors GenerateDatasetMisorientationColors.h DatasetMerging/DatasetMergingFilters/GenerateDatasetMisorientationColors.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class GenerateDatasetMisorientationColors : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(GenerateDatasetMisorientationColors)
    DREAM3D_STATIC_NEW_MACRO(GenerateDatasetMisorientationColors)
    DREAM3D_TYPE_MACRO_SUPER(GenerateDatasetMisorientationColors, AbstractFilter)

    virtual ~GenerateDatasetMisorientationColors();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath1)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath1 READ getCellPhasesArrayPath1 WRITE setCellPhasesArrayPath1)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath1)
    Q_PROPERTY(DataArrayPath QuatsArrayPath1 READ getQuatsArrayPath1 WRITE setQuatsArrayPath1)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath1)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath1 READ getCrystalStructuresArrayPath1 WRITE setCrystalStructuresArrayPath1)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath2)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath2 READ getCellPhasesArrayPath2 WRITE setCellPhasesArrayPath2)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath2)
    Q_PROPERTY(DataArrayPath QuatsArrayPath2 READ getQuatsArrayPath2 WRITE setQuatsArrayPath2)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath2)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath2 READ getCrystalStructuresArrayPath2 WRITE setCrystalStructuresArrayPath2)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, MisorientationColorArrayName)
    Q_PROPERTY(QString MisorientationColorArrayName READ getMisorientationColorArrayName WRITE setMisorientationColorArrayName)

    DREAM3D_FILTER_PARAMETER(bool, UseGoodVoxels)
    Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)


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
    GenerateDatasetMisorientationColors();

    /**
    * @brief Checks for the appropriate parameter values and availability of arrays in the data container
    */
    void dataCheck();

  private:    
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases1)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases2)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Quats1)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Quats2)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures1)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures2)
    DEFINE_CREATED_DATAARRAY_VARIABLE(uint8_t, MisorientationColor)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, GoodVoxels)

    GenerateDatasetMisorientationColors(const GenerateDatasetMisorientationColors&); // Copy Constructor Not Implemented
    void operator=(const GenerateDatasetMisorientationColors&); // Operator '=' Not Implemented
};

#endif /* _GenerateDatasetMisorientationColors_H_ */
