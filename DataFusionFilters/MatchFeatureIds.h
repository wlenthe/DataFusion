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
 
#ifndef _MatchFeatureIds_H_
#define _MatchFeatureIds_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"

/**
 * @brief The MatchFeatureIds class. See [Filter documentation](@ref MatchFeatureIds) for details.
 */
class MatchFeatureIds : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(MatchFeatureIds)
    SIMPL_STATIC_NEW_MACRO(MatchFeatureIds)
    SIMPL_TYPE_MACRO_SUPER(MatchFeatureIds, AbstractFilter)

    virtual ~MatchFeatureIds();

    //input array paths
    SIMPL_FILTER_PARAMETER(DataArrayPath, ReferenceCellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath ReferenceCellFeatureAttributeMatrixPath READ getReferenceCellFeatureAttributeMatrixPath WRITE setReferenceCellFeatureAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MovingCellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath MovingCellFeatureAttributeMatrixPath READ getMovingCellFeatureAttributeMatrixPath WRITE setMovingCellFeatureAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, ReferenceFeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath ReferenceFeatureIdsArrayPath READ getReferenceFeatureIdsArrayPath WRITE setReferenceFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MovingFeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath MovingFeatureIdsArrayPath READ getMovingFeatureIdsArrayPath WRITE setMovingFeatureIdsArrayPath)

    //output array names
    SIMPL_FILTER_PARAMETER(QString, ReferenceUniqueArrayName)
    Q_PROPERTY(QString ReferenceUniqueArrayName READ getReferenceUniqueArrayName WRITE setReferenceUniqueArrayName)

    SIMPL_FILTER_PARAMETER(QString, MovingUniqueArrayName)
    Q_PROPERTY(QString MovingUniqueArrayName READ getMovingUniqueArrayName WRITE setMovingUniqueArrayName)

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
    DataArray<bool>::WeakPointer getReferenceUniquePtr() {return m_ReferenceUniquePtr;}
    DataArray<bool>::WeakPointer getMovingUniquePtr() {return m_MovingUniquePtr;}
    bool* getReferenceUnique() {return m_ReferenceUnique;}
    bool* getMovingUnique() {return m_MovingUnique;}

    DataArray<int32_t>::WeakPointer getReferenceFeatureIdsPtr() {return m_ReferenceFeatureIdsPtr;}
    DataArray<int32_t>::WeakPointer getMovingFeatureIdsPtr() {return m_MovingFeatureIdsPtr;}
    int32_t* getReferenceFeatureIds() {return m_ReferenceFeatureIds;}
    int32_t* getMovingFeatureIds() {return m_MovingFeatureIds;}

    virtual std::vector<size_t> match();
    DEFINE_DATAARRAY_VARIABLE(bool, ReferenceUnique)
    DEFINE_DATAARRAY_VARIABLE(bool, MovingUnique)
    DEFINE_DATAARRAY_VARIABLE(int32_t, ReferenceFeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, MovingFeatureIds)

  private:

    MatchFeatureIds(const MatchFeatureIds&); // Copy Constructor Not Implemented
    void operator=(const MatchFeatureIds&); // Operator '=' Not Implemented
};



#endif /* _MatchFeatureIds_H_ */
