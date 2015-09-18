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
 
#ifndef _FuseVolumes_H_
#define _FuseVolumes_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"

#include "SIMPLib/FilterParameters/DynamicTableData.h"

/**
 * @brief The FuseVolumes class. See [Filter documentation](@ref fusevolumes) for details.
 */
class FuseVolumes : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(FuseVolumes)
    SIMPL_STATIC_NEW_MACRO(FuseVolumes)
    SIMPL_TYPE_MACRO_SUPER(FuseVolumes, AbstractFilter)

    virtual ~FuseVolumes();

    //user paramters
    SIMPL_FILTER_PARAMETER(QString, Prefix)
    Q_PROPERTY(QString Prefix READ getPrefix WRITE setPrefix)

    SIMPL_FILTER_PARAMETER(int, TransformationType)
    Q_PROPERTY(int TransformationType READ getTransformationType WRITE setTransformationType)

    SIMPL_FILTER_PARAMETER(DynamicTableData, ManualTransformation)
    Q_PROPERTY(DynamicTableData ManualTransformation READ getManualTransformation WRITE setManualTransformation)

    //input array paths
    SIMPL_FILTER_PARAMETER(DataArrayPath, ReferenceVolume)
    Q_PROPERTY(DataArrayPath ReferenceVolume READ getReferenceVolume WRITE setReferenceVolume)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MovingVolume)
    Q_PROPERTY(DataArrayPath MovingVolume READ getMovingVolume WRITE setMovingVolume)

    SIMPL_FILTER_PARAMETER(DataArrayPath, TransformationArrayPath)
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
