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
 
#ifndef _DataFusion_H_
#define _DataFusion_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"

/**
 * @brief The DataFusionPlugin class
 */
class DataFusionPlugin : public QObject, public ISIMPLibPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISIMPLibPlugin)
    Q_PLUGIN_METADATA(IID "com.your.domain.DataFusionPlugin")

  public:
    DataFusionPlugin();
    virtual ~DataFusionPlugin();
    
    /**
     * @brief getPluginName Returns the name of the plugin
     */
    virtual QString getPluginName();

    /**
     * @brief getVersion Returns the version
     */
    virtual QString getVersion();

     /**
     * @brief getCompatibilityVersion Returns the compatibility version
     */
    virtual QString getCompatibilityVersion();

     /**
     * @brief getVendor Returns the name of the vendor
     */
    virtual QString getVendor();

    /**
     * @brief getURL Returns the URL of the plugin
     */
    virtual QString getURL();

    /**
     * @brief getLocationReturns the location of the plugin
     */
    virtual QString getLocation();

    /**
     * @brief getDescription Returns the description of the plugin
     * PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
     * It is located at DataFusion/Resources/DataFusion/DataFusionDescription.txt
     */
    virtual QString getDescription();

    /**
     * @brief getCopyright Returns the copyright of the plugin
     */
    virtual QString getCopyright();

    /**
     * @brief getLicense Returns the license of the plugin
     * PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
     * It is located at DataFusion/Resources/DataFusion/DataFusionLicense.txt
     */
    virtual QString getLicense();

    /**
     * @brief getFilters Returns the filters of the plugin
     */
    virtual QList<QString> getFilters();

    /**
     * @brief getThirdPartyLicenses Returns the third party licenses of the plugin
     */
    virtual QMap<QString, QString> getThirdPartyLicenses();

    /**
     * @brief getDidLoad Returns the load status of the plugin
     */
    virtual bool getDidLoad();

    /**
     * @brief setDidLoad Sets the load status of the plugin
     * @param didLoad Boolean value to set status
     */
    virtual void setDidLoad(bool didLoad);

    /**
     * @brief setLocation Sets the location of the plugin on the file system.
     * This is required so that we can cache the file path information
     * as the plugin is loaded.
     * @param filePath File path
     */
    virtual void setLocation(QString filePath);

    /**
     * @brief registerFilterWidgets Register all the filters with the FilterWidgetFactory
     * @param fwm FilterWidgetManager instance pointer
     */
    virtual void registerFilterWidgets(FilterWidgetManager* fwm);

    /**
    * @brief registerFilters Registers the filters that this plugin implements with the Filter Manager that is passed in
    * @param fm FilterManager instance pointer
    */
    virtual void registerFilters(FilterManager* fm);


    /**
     * @brief writeSettings Writes the settings in the input gui to the Application's preference file
     * @param prefs QSettings reference variable
     */
    virtual void writeSettings(QSettings& prefs);

    /**
     * @brief readSettings Reads the settings from the Application's preference file and sets
     * the input GUI widgets accordingly.
     * @param prefs QSettings reference variable
     */
    virtual void readSettings(QSettings& prefs);

    /**
     * @brief Returns pluging wide citations a BibTex format
     */
    virtual const QString getCitations();

  private:
    QString             m_Version;
    QString             m_CompatibilityVersion;
    QString             m_Vendor;
    QString             m_URL;
    QString             m_Location;
    QString             m_Description;
    QString             m_Copyright;
    QList<QString>      m_Filters;
    bool                m_DidLoad;

    DataFusionPlugin(const DataFusionPlugin&); // Copy Constructor Not Implemented
    void operator=(const DataFusionPlugin&); // Operator '=' Not Implemented
};

#endif /* _DataFusion_H_ */
