/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "WidgetInputSelection.h"

#include <QtCore/QEvent>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>

#include "ApplicationServices.h"
#include "ConfigurationSettings.h"
#include "AppVerify.h"
#include "Slot.h"
#include "SpatialDataView.h"
#include "SpatialDataWindow.h"
#include "TypesFile.h"

#include <vector>
#include <string>
using namespace std;

WidgetInputSelection::WidgetInputSelection(QWidget* pParent) : QWidget(pParent) {
   QLabel* pPrimaryDatasetLabel = new QLabel("Data Set to Convolve", this);
   mpPrimaryList = new QListWidget(this);

   QLabel* pSecondaryDatasetLabel = new QLabel("Convolution Kernel", this);
   mpSecondaryList = new QListWidget(this);

   QPushButton* pBrowser = new QPushButton("Browse for Data", this);

   // Layout
   QHBoxLayout* pSecondaryLayout = new QHBoxLayout();
   pSecondaryLayout->setMargin(0);
   pSecondaryLayout->setSpacing(5);
   pSecondaryLayout->addWidget(pSecondaryDatasetLabel, Qt::AlignBottom);
   pSecondaryLayout->addStretch();
   pSecondaryLayout->addWidget(pBrowser);

   QVBoxLayout* pLayout = new QVBoxLayout(this);
   pLayout->setMargin(0);
   pLayout->setSpacing(5);
   pLayout->addWidget(pPrimaryDatasetLabel);
   pLayout->addWidget(mpPrimaryList, 10);
   pLayout->addLayout(pSecondaryLayout);
   pLayout->addWidget(mpSecondaryList, 10);

   // connections
   connect(pBrowser, SIGNAL(clicked()), this, SLOT(importData()));
   connect(mpPrimaryList, SIGNAL(itemSelectionChanged()), this, SIGNAL(modified()));
   connect(mpSecondaryList, SIGNAL(itemSelectionChanged()), this, SIGNAL(modified()));

   vector<Window*> windows;
   mpDesktop->getWindows(SPATIAL_DATA_WINDOW, windows);
   for (vector<Window*>::iterator it = windows.begin(); it != windows.end(); ++it) {
      Window* pWindow = *it;
      if (pWindow != NULL) {
         pWindow->attach(SIGNAL_NAME(Subject, Deleted), Slot(this, &WidgetInputSelection::windowDeleted));
         pWindow->attach(SIGNAL_NAME(Subject, Modified), Slot(this, &WidgetInputSelection::windowModified));
      }
   }

   mpDesktop->attach(SIGNAL_NAME(DesktopServices, WindowAdded), Slot(this, &WidgetInputSelection::windowAdded));
}

WidgetInputSelection::~WidgetInputSelection() {
   vector<Window*> windows;
   mpDesktop->getWindows(SPATIAL_DATA_WINDOW, windows);
   for (vector<Window*>::iterator it = windows.begin(); it != windows.end(); ++it) {
      Window* pWindow = *it;
      if (pWindow != NULL) {
         pWindow->detach(SIGNAL_NAME(Subject, Deleted), Slot(this, &WidgetInputSelection::windowDeleted));
         pWindow->detach(SIGNAL_NAME(Subject, Modified), Slot(this, &WidgetInputSelection::windowModified));
      }
   }

   mpDesktop->detach(SIGNAL_NAME(DesktopServices, WindowAdded), Slot(this, &WidgetInputSelection::windowAdded));
   setViews(NULL, NULL);
}

void WidgetInputSelection::setViews(SpatialDataView* pPrimary, SpatialDataView* pSecondary) {
   setView(pPrimary, *mpPrimaryList);
   setView(pSecondary, *mpSecondaryList);
}

SpatialDataView* WidgetInputSelection::getImageToConvolveView() const { return getView(mpPrimaryList); }
SpatialDataView* WidgetInputSelection::getConvolutionKernelView() const { return getView(mpSecondaryList); }
SpatialDataView* WidgetInputSelection::getPrimaryView() const { return getView(mpPrimaryList); }
SpatialDataView* WidgetInputSelection::getSecondaryView() const { return getView(mpSecondaryList); }

void WidgetInputSelection::importData() {
   VERIFYNRV(mpDesktop.get() != NULL);
   mpDesktop->importFile("Raster Element");
}

bool WidgetInputSelection::isValid() const {
   VERIFY(mpPrimaryList != NULL);
   VERIFY(mpSecondaryList != NULL);

   SpatialDataView* pPrimary = getPrimaryView();
   SpatialDataView* pSecondary = getSecondaryView();

   return pPrimary != NULL && pSecondary != NULL;
}

void WidgetInputSelection::attached(Subject& subject, const string& signal, const Slot& slot) {
   windowAttached(subject, signal, boost::any());
}

void WidgetInputSelection::detached(Subject& subject, const string& signal, const Slot& slot) {
   windowDeleted(subject, signal, boost::any());
}

void WidgetInputSelection::windowAdded(Subject& subject, const string& signal, const boost::any& value) {
   if (dynamic_cast<DesktopServices*>(&subject) == mpDesktop.get()) {
      Window* pWindow = boost::any_cast<Window*>(value);
      if (dynamic_cast<SpatialDataWindow*>(pWindow) != NULL) {
         pWindow->attach(SIGNAL_NAME(Subject, Modified), Slot(this, &WidgetInputSelection::windowModified));
         pWindow->attach(SIGNAL_NAME(Subject, Deleted), Slot(this, &WidgetInputSelection::windowDeleted));
      }
   }
}

void WidgetInputSelection::windowAttached(Subject& subject, const string& signal, const boost::any& v) {
   SpatialDataWindow* pWindow = dynamic_cast<SpatialDataWindow*>(&subject);
   if (pWindow != NULL) {
      string windowName = pWindow->getName();
      VERIFYNRV(!windowName.empty());

      QList<QListWidgetItem*> primaryItems = mpPrimaryList->findItems(QString::fromStdString(windowName),
         Qt::MatchExactly);
      QList<QListWidgetItem*> secondaryItems = mpSecondaryList->findItems(QString::fromStdString(windowName),
         Qt::MatchExactly);

      QListWidgetItem* pPrimaryItem = NULL;
      if (primaryItems.empty() == false)
      {
         pPrimaryItem = primaryItems.front();
      }

      QListWidgetItem* pSecondaryItem = NULL;
      if (secondaryItems.empty() == false)
      {
         pSecondaryItem = secondaryItems.front();
      }

      if (pPrimaryItem == NULL) // not found
      {
         mpPrimaryList->addItem(windowName.c_str()); // insert at end
         QListWidgetItem* pItem = mpPrimaryList->item(mpPrimaryList->count()-1); // get last item
         if (pItem != NULL)
         {
            mPrimaryMap.insert(pWindow, pItem);
         }
      }
      if (pSecondaryItem == NULL)
      {
         mpSecondaryList->addItem(windowName.c_str()); // insert at end
         QListWidgetItem* pItem = mpSecondaryList->item(mpPrimaryList->count()-1); // get last item
         if (pItem != NULL)
         {
            mSecondaryMap.insert(pWindow, pItem);
         }
      }
   }
}

void WidgetInputSelection::windowDeleted(Subject& subject, const string& signal, const boost::any& v)
{
   SpatialDataWindow* pWindow = dynamic_cast<SpatialDataWindow*>(&subject);
   if (pWindow != NULL)
   {
      string windowName = pWindow->getName();
      VERIFYNRV(!windowName.empty());

      QList<QListWidgetItem*> primaryItems = mpPrimaryList->findItems(QString::fromStdString(windowName),
         Qt::MatchExactly);
      QList<QListWidgetItem*> secondaryItems = mpSecondaryList->findItems(QString::fromStdString(windowName),
         Qt::MatchExactly);

      QListWidgetItem* pPrimaryItem = NULL;
      if (primaryItems.empty() == false) { pPrimaryItem = primaryItems.front(); }

      QListWidgetItem* pSecondaryItem = NULL;
      if (secondaryItems.empty() == false) { pSecondaryItem = secondaryItems.front(); }

      if (pPrimaryItem != NULL) // was found, so remove it
      {
         delete (mpPrimaryList->takeItem(mpPrimaryList->row(mPrimaryMap[pWindow])));
         mPrimaryMap.remove(pWindow);
      }
      if (pSecondaryItem != NULL) // was found, so remove it
      {
         delete (mpSecondaryList->takeItem(mpSecondaryList->row(mSecondaryMap[pWindow])));
         mSecondaryMap.remove(pWindow);
      }
      emit modified();
   }
}

void WidgetInputSelection::windowModified(Subject& subject, const string& signal, const boost::any& v)
{
   SpatialDataWindow* pWindow = dynamic_cast<SpatialDataWindow*>(&subject);
   if (pWindow != NULL)
   {
      string windowName = pWindow->getName();
      VERIFYNRV(!windowName.empty());

      QList<QListWidgetItem*> primaryItems = mpPrimaryList->findItems(QString::fromStdString(windowName),
         Qt::MatchExactly);
      QList<QListWidgetItem*> secondaryItems = mpSecondaryList->findItems(QString::fromStdString(windowName),
         Qt::MatchExactly);

      QListWidgetItem* pPrimaryItem = NULL;
      if (primaryItems.empty() == false)
      {
         pPrimaryItem = primaryItems.front();
      }

      QListWidgetItem* pSecondaryItem = NULL;
      if (secondaryItems.empty() == false)
      {
         pSecondaryItem = secondaryItems.front();
      }

      if (pPrimaryItem == NULL) // not found
      {
         mpPrimaryList->addItem(windowName.c_str()); // insert at end
         QListWidgetItem* pItem = mpPrimaryList->item(mpPrimaryList->count()-1); // get last item
         if (pItem != NULL)
         {
            mPrimaryMap.insert(pWindow, pItem);
         }
      }
      if (pSecondaryItem == NULL)
      {
         mpSecondaryList->addItem(windowName.c_str()); // insert at end
         QListWidgetItem* pItem = mpSecondaryList->item(mpPrimaryList->count()-1); // get last item
         if (pItem != NULL)
         {
            mSecondaryMap.insert(pWindow, pItem);
         }
      }
   }
}

void WidgetInputSelection::setView(SpatialDataView* pView, QListWidget& box)
{
   string viewName;
   if (pView != NULL)
   {
      viewName = pView->getName();
   }

   QList<QListWidgetItem*> items = box.findItems(viewName.c_str(), Qt::MatchExactly);
   if (items.empty() == false)
   {
      box.clearSelection();
      box.setItemSelected(items.front(), true);
   }
}

SpatialDataView* WidgetInputSelection::getView(QListWidget* pBox) const {
   if (pBox == NULL) return NULL;

   SpatialDataView* pView = NULL;

   QList<QListWidgetItem*> selectedItems = pBox->selectedItems();
   if (selectedItems.empty() == false)
   {
      QListWidgetItem* pItem = selectedItems.front();
      if (pItem != NULL)
      {
         QString windowName = pItem->text();
         if (!windowName.isEmpty())
         {
            SpatialDataWindow* pWindow =
               dynamic_cast<SpatialDataWindow*>(mpDesktop->getWindow(windowName.toStdString(), SPATIAL_DATA_WINDOW));
            if (pWindow != NULL)
            {
               pView = pWindow->getSpatialDataView();
            }
         }
      }
   }

   return pView;
}
