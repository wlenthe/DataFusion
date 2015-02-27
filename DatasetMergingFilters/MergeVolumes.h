/*
 * Your License or Copyright Information can go here
 */

#ifndef _MergeVolumes_H_
#define _MergeVolumes_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class MergeVolumes MergeVolumes.h DatasetMerging/DatasetMergingFilters/MergeVolumes.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class MergeVolumes : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    DREAM3D_SHARED_POINTERS(MergeVolumes)
    DREAM3D_STATIC_NEW_MACRO(MergeVolumes)
    DREAM3D_TYPE_MACRO_SUPER(MergeVolumes, AbstractFilter)

    virtual ~MergeVolumes();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ReferenceCellAttributeMatrixArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceCellAttributeMatrixArrayPath READ getReferenceCellAttributeMatrixArrayPath WRITE setReferenceCellAttributeMatrixArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MovingCellAttributeMatrixArrayPath)
    Q_PROPERTY(DataArrayPath MovingCellAttributeMatrixArrayPath READ getMovingCellAttributeMatrixArrayPath WRITE setMovingCellAttributeMatrixArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, Prefix)
    Q_PROPERTY(QString Prefix READ getPrefix WRITE setPrefix)

    DREAM3D_FILTER_PARAMETER(FloatVec4_t, Row1)
    Q_PROPERTY(FloatVec4_t Row1 READ getRow1 WRITE setRow1)
     
    DREAM3D_FILTER_PARAMETER(FloatVec4_t, Row2)
    Q_PROPERTY(FloatVec4_t Row2 READ getRow2 WRITE setRow2)
     
    DREAM3D_FILTER_PARAMETER(FloatVec4_t, Row3)
    Q_PROPERTY(FloatVec4_t Row3 READ getRow3 WRITE setRow3)

    DREAM3D_FILTER_PARAMETER(FloatVec4_t, Row4)
    Q_PROPERTY(FloatVec4_t Row4 READ getRow4 WRITE setRow4)



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
    MergeVolumes();

    /**
    * @brief Checks for the appropriate parameter values and availability of arrays in the data container
    */
    void dataCheck();

  private:
    /* Your private class instance variables go here. You can use several preprocessor macros to help
     * make sure you have all the variables defined correctly. Those are "DEFINE_REQUIRED_DATAARRAY_VARIABLE()"
     * and  DEFINE_CREATED_DATAARRAY_VARIABLE() which are defined in DREAM3DGetSetMacros.h
     */

    MergeVolumes(const MergeVolumes&); // Copy Constructor Not Implemented
    void operator=(const MergeVolumes&); // Operator '=' Not Implemented
};

#endif /* _MergeVolumes_H_ */
