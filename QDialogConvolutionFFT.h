/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */


#ifndef QDIALOGCONVOLUTIONFFT_H
#define QDIALOGCONVOLUTIONFFT_H

#include "DesktopServices.h"

#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>

#include "WidgetInputSelection.h"
#include "RasterUtilities.h"
#include "ObjectResource.h"

class DatasetPage;
class FusionAlgorithmInputsPage;
class FusionLayersSelectPage;
class FusionPage;
class GcpPoint;
class PlugIn;
class ProgressTracker;
class RasterLayer;
class RasterElement;
class SpatialDataView;
class SpatialDataWindow;
class TiePointPage;

namespace boost { class any; }

class FFTConvolutionDlg : public QDialog { Q_OBJECT
public:
   FFTConvolutionDlg(PlugIn* pPlugIn, ProgressTracker& progressTracker, QWidget* pParent = NULL);
   virtual ~FFTConvolutionDlg();

   void windowDeleted(Subject& subject, const std::string& signal, const boost::any& v);

   SpatialDataView* displayResult();
   RasterElement* resultRasterElement;

protected:
   void showPage(QWidget* pPage);
   //virtual void closeEvent(QCloseEvent* pEvent);

protected slots:
   void accept();
   void reject();
   void convolve();

private:
   SpatialDataView* pView;

   Service<DesktopServices> mpDesktop;

   QLabel* mpDescriptionLabel;
   QPushButton* mpOKButton;

   PlugIn* mpPlugIn;
   ProgressTracker& mProgressTracker;

   WidgetInputSelection* widgetInputSelection;
};

#endif // QDIALOGCONVOLUTIONFFT_H
