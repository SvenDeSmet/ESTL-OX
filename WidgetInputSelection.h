/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef WIDGETINPUTSELECTION_H
#define WIDGETINPUTSELECTION_H

#include "ConfigurationSettings.h"
#include "DesktopServices.h"
#include "Observer.h"
#include "PlugInManagerServices.h"

#include <QtCore/QMap>
#include <QtGui/QWidget>
#include <boost/any.hpp>

class QCheckBox;
class QGroupBox;
class QListWidget;
class QListWidgetItem;
class SpatialDataView;
class SpatialDataWindow;

namespace boost { class any; }

class WidgetInputSelection : public QWidget, public Observer { Q_OBJECT
public:
   WidgetInputSelection(QWidget* pParent);
   ~WidgetInputSelection();

   void setViews(SpatialDataView* pPrimary, SpatialDataView* pSecondary);
   SpatialDataView* getImageToConvolveView() const;
   SpatialDataView* getConvolutionKernelView() const;
   SpatialDataView* getPrimaryView() const;
   SpatialDataView* getSecondaryView() const;

   void windowAdded(Subject& subject, const std::string& signal, const boost::any& v);
   void windowDeleted(Subject& subject, const std::string& signal, const boost::any& v);
   void windowModified(Subject& subject, const std::string& signal, const boost::any& v);
   void windowAttached(Subject& subject, const std::string& signal, const boost::any& v);
   void attached(Subject& subject, const std::string& signal, const Slot& slot);
   void detached(Subject& subject, const std::string& signal, const Slot& slot);

   bool isValid() const;

protected slots:
   void importData();

signals:
   void modified();

private:
   // sets the current spatial data view and the appropriate ListBox
   // this method depends on the fact that a SpatialDataView and SpatialDataWindow have the same name
   void setView(SpatialDataView* pView, QListWidget& box);

   // gets the 'current' spatial data view of a list box
   SpatialDataView* getView(QListWidget* pBox) const;

   QMap<SpatialDataWindow*, QListWidgetItem*> mPrimaryMap, mSecondaryMap;
   QListWidget *mpSecondaryList, *mpPrimaryList;

   Service<DesktopServices> mpDesktop;
   Service<PlugInManagerServices> mpPlugMgr;
};


#endif // WIDGETINPUTSELECTION_H
