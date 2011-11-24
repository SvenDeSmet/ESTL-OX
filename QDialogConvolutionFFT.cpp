/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "QDialogConvolutionFFT.h"

#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCloseEvent>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitter>
#include <QtGui/QTreeWidget>

#include "AoiElement.h"
#include "AoiLayer.h"
#include "AppVerify.h"
#include "DataAccessor.h"
#include "DataAccessorImpl.h"
#include "DataRequest.h"
#include "DockWindow.h"
#include "Executable.h"
#include "GcpList.h"
#include "GcpLayer.h"
//#include "GeoAlgorithms.h"
#include "GraphicLayer.h"
#include "GraphicObject.h"
#include "LatLonLayer.h"
#include "LayerList.h"
//#include "MouseMode.h"
#include "PlugIn.h"
#include "PlugInArg.h"
#include "PlugInArgList.h"
#include "PlugInResource.h"
#include "ProgressTracker.h"
#include "RasterLayer.h"
#include "RasterElement.h"
#include "RasterDataDescriptor.h"
#include "Slot.h"
#include "SpatialDataView.h"
#include "SpatialDataWindow.h"
#include "switchOnEncoding.h"
#include "TypesFile.h"
#include "Undo.h"
#include "UtilityServices.h"
#include "WorkspaceWindow.h"

#include <list>
#include <string>

#include "../ESTL/Convolution.h"

using namespace std;

FFTConvolutionDlg::FFTConvolutionDlg(PlugIn* pPlugIn, ProgressTracker& progressTracker, QWidget* pParent) :
   QDialog(pParent), mpDescriptionLabel(NULL), mpPlugIn(pPlugIn), mProgressTracker(progressTracker), pView(NULL) {
   setWindowTitle("FFT-based Convolution");
   setModal(false);
   
   mpDescriptionLabel = new QLabel(this);
   mpDescriptionLabel->setText("Select Data to Convolve and Convolution Kernel");

   QFrame* pDescriptionLine = new QFrame(this);
   pDescriptionLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);

   QFrame* pButtonLine = new QFrame(this);
   pButtonLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
   
   mpOKButton = new QPushButton("&OK", this);

   QPushButton* pCancelButton = new QPushButton("&Cancel", this);
   pCancelButton->setDefault(true);
  
   QHBoxLayout* pButtonLayout = new QHBoxLayout();
   pButtonLayout->setMargin(0);
   pButtonLayout->setSpacing(5);
   pButtonLayout->addStretch();
   pButtonLayout->addWidget(mpOKButton);
   pButtonLayout->addWidget(pCancelButton);

   QVBoxLayout* pLayout = new QVBoxLayout(this);
   pLayout->setMargin(10);
   pLayout->setSpacing(10);
   pLayout->addWidget(mpDescriptionLabel);
   pLayout->addWidget(pDescriptionLine);
   pLayout->addWidget(widgetInputSelection = new WidgetInputSelection(this), 10);
   pLayout->addWidget(pButtonLine);
   pLayout->addLayout(pButtonLayout);

   resize(minimumSizeHint()); // required so the dialog shows up the correct size
   
   // connections
   VERIFYNR(connect(mpOKButton, SIGNAL(clicked()), this, SLOT(accept())));
   VERIFYNR(connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject())));
}

FFTConvolutionDlg::~FFTConvolutionDlg() { mProgressTracker.report("Convolution transformation complete", 100, NORMAL, true); }

void FFTConvolutionDlg::windowDeleted(Subject& subject, const string& signal, const boost::any& v) { }

void FFTConvolutionDlg::accept() { convolve(); done(QDialog::Accepted); }
void FFTConvolutionDlg::reject() { done(QDialog::Rejected); }

class Image {
private:
public:
    SpatialDataView* pView;
    RasterElement* pRaster;
    const RasterDataDescriptor* pDataDescriptor;

    Image(SpatialDataView* iPView) : pView(iPView) {
        LayerList* pLayerList = pView->getLayerList(); VERIFYNRV(pLayerList != NULL);
        pRaster = dynamic_cast<RasterElement*>(pLayerList->getPrimaryRasterElement()); VERIFYNRV(pRaster != NULL);
        pDataDescriptor = dynamic_cast<const RasterDataDescriptor*>(pRaster->getDataDescriptor()); VERIFYNR(pDataDescriptor != NULL);
/*        Service<DesktopServices> pDesktop;
        VERIFYNRV(pDesktop.get() != NULL);
        SpatialDataWindow* pWindow = dynamic_cast<SpatialDataWindow*>(pDesktop->getWindow(pRaster->getName(), SPATIAL_DATA_WINDOW));
        VERIFYNRV(pWindow != NULL);
        VERIFYNRV(pDesktop->setCurrentWorkspaceWindow(pWindow));*/
    }

    Image(RasterElement* iPRaster) : pView(NULL), pRaster(iPRaster) {
        pDataDescriptor = dynamic_cast<const RasterDataDescriptor*>(pRaster->getDataDescriptor()); VERIFYNR(pDataDescriptor != NULL);
        /*      SpatialDataWindow* pPrimaryWindow = dynamic_cast<SpatialDataWindow*>(pDesktop->getWindow(pPrimaryRaster->getName(), SPATIAL_DATA_WINDOW));
              VERIFYNRV(pPrimaryWindow != NULL);
              VERIFYNRV(pDesktop->setCurrentWorkspaceWindow(pPrimaryWindow));*/
    }

    int getRowCount() { return pDataDescriptor->getRowCount(); }
    int getColumnCount() { return pDataDescriptor->getColumnCount(); }
    int getBandCount() { return pDataDescriptor->getBands().size(); }

    std::string getName() { return pRaster->getName(); }

    DataAccessor getAccessor(int band, bool writable = false) {
        FactoryResource<DataRequest> pRequest = FactoryResource<DataRequest>();
        pRequest->setRows(pDataDescriptor->getActiveRow(0), pDataDescriptor->getActiveRow(getRowCount() - 1));
        pRequest->setColumns(pDataDescriptor->getActiveColumn(0), pDataDescriptor->getActiveColumn(getColumnCount() - 1));
        pRequest->setBands(pDataDescriptor->getActiveBand(band),  pDataDescriptor->getActiveBand(band));
        pRequest->setWritable(writable);
        return pRaster->getDataAccessor(pRequest.release());
    }
};

template <class T> class Opticks2DData : public Data2D<T> {
private:
    Image* image;
public:
    DataAccessor dataAccessor;
    Opticks2DData(Image* iImage, int band, bool writable = false) : image(iImage), dataAccessor(DataAccessor(image->getAccessor(band, writable)))
    { if (!dataAccessor.isValid()) { qDebug("Failed to create data accessor"); /* throw exception */ } }

    int getWidth() { return image->getColumnCount(); }
    int getHeight() { return image->getRowCount(); }

    void toPixel(int x, int y) {
        dataAccessor->toPixel(y, x);
        if (!dataAccessor.isValid()) { qDebug("DataAccessor invalid after toPixel"); /* throw exception */ }
    }

    T getPixel(int x, int y) {
        toPixel(x, y);
        return *((T*) dataAccessor->getColumn());
    }

    void setPixel(int x, int y, T value) {
        toPixel(x, y); //qDebug("((%i, %i, %i))", x, y, (int) value);
        *((T*) dataAccessor->getColumn()) = value;//value;//value;
    }
};

namespace {
	template<typename T> void assignResult(T* pPixel, double value) { *pPixel = static_cast<T>(value); }
}

void FFTConvolutionDlg::convolve() { 
    Service<ModelServices> pModel; VERIFYNRV(pModel.get() != NULL);
    Service<DesktopServices> pDesktop; VERIFYNRV(pDesktop.get() != NULL);
    Service<UtilityServices> pUtils; VERIFYNRV(pUtils.get() != NULL);

	mProgressTracker.report("Retrieving image to convolve", 1, NORMAL, true);
    Image imgToConvolve(widgetInputSelection->getImageToConvolveView());
    if (imgToConvolve.pView == NULL) { mProgressTracker.report("Error: Primary data set does not exist. Go back to the Dataset Select Page and select a new Primary Dataset.", 0, ERRORS); return; }

	mProgressTracker.report("Retrieving convolution kernel", 2, NORMAL, true);
    Image imgConvolutionKernel(widgetInputSelection->getConvolutionKernelView());
    if (imgConvolutionKernel.pView == NULL) { mProgressTracker.report("Error: Secondary data set does not exist. Go back to the Dataset Select Page and select a new Secondary Dataset.", 0, ERRORS); return; }

   // Convolved Image Result
mProgressTracker.report("Initializing output", 3, NORMAL, true);
   EncodingType resultType = imgToConvolve.pDataDescriptor->getDataType();
		  mProgressTracker.report((string("Result type ") + intToStr(resultType)).c_str(), 4, NORMAL, true);
   qDebug("resultType = %i", (int) resultType);
      /*if (resultType == INT4SCOMPLEX) { resultType = INT4SBYTES; }
      else if (resultType == FLT8COMPLEX) { resultType = FLT8BYTES; }*/

   qDebug("resultType = %i", (int) resultType);
//      INT1SBYTE = 0,  /**< char */
//      00208    INT1UBYTE = 1,      /**< unsigned char */
//      00209    INT2SBYTES = 2,     /**< short */
//      00210    INT2UBYTES = 3,     /**< unsigned short */
//      00211    INT4SCOMPLEX = 4,   /**< complex short */
//      00212    INT4SBYTES = 5,     /**< int */
//      00213    INT4UBYTES = 6,     /**< unsigned int */
//      00214    FLT4BYTES = 7,      /**< float */
//      00215    FLT8COMPLEX = 8,    /**< complex float */
//      00216    FLT8BYTES = 9       /**< double */

   std::string resultName = imgToConvolve.getName() + " conv " + imgConvolutionKernel.getName();

   ModelResource<RasterElement> pResult(RasterUtilities::createRasterElement(resultName,
         imgToConvolve.getRowCount(), imgToConvolve.getColumnCount(), imgToConvolve.getBandCount(),
         resultType, imgToConvolve.pDataDescriptor->getInterleaveFormat(),
         imgToConvolve.pDataDescriptor->getProcessingLocation() == IN_MEMORY));
      pResult->copyClassification(imgToConvolve.pRaster);
      pResult->getMetadata()->merge(imgToConvolve.pDataDescriptor->getMetadata()); //copy original metadata
      Image imgResult = Image(pResult.get());
      resultRasterElement = imgResult.pRaster;
      if (imgResult.pRaster == NULL) { mProgressTracker.report("Unable to create result data set.", 0, ERRORS, true); return; }

      if (Service<ApplicationServices>()->isBatch()) { return; }
      SpatialDataWindow* pWindow = static_cast<SpatialDataWindow*>(Service<DesktopServices>()->createWindow(resultRasterElement->getName(), SPATIAL_DATA_WINDOW));
      pView = (pWindow == NULL) ? NULL : pWindow->getSpatialDataView();
//      pView = (pWindow == NULL) ? NULL : pWindow->getSpatialDataView();
      if (pView == NULL) {
         Service<DesktopServices>()->deleteWindow(pWindow);
         mProgressTracker.report("Unable to create view.", 0, ERRORS, true); return;
      }
      if (!pView->setPrimaryRasterElement(resultRasterElement)) qDebug("Failed to attach raster element");

      RasterLayer* pLayer = NULL; { UndoLock lock(pView); pLayer = static_cast<RasterLayer*>(pView->createLayer(RASTER, resultRasterElement)); }
      if (pLayer == NULL) {
   #pragma message(__FILE__ "(" STRING(__LINE__) ") : warning : This would be cleaner with a WindowResource. If one becomes available, use it instead. (tclarke)")
         Service<DesktopServices>()->deleteWindow(pWindow);
         mProgressTracker.report("Unable to create layer.", 0, ERRORS, true);
         return;
      }

      // Perform Convolution
   	mProgressTracker.report("Performing convolution", 5, NORMAL, true);
     unsigned int bandCount = imgToConvolve.pDataDescriptor->getBands().size();
//      qDebug("bandCount = %i", bandCount);
  //    qDebug("%i -- %i:%i:%i", (int) imgResult.pDataDescriptor->getDataType(), imgResult.getRowCount(), imgResult.getColumnCount(), imgResult.getBandCount());
      typedef unsigned char T;
//	  ProgressTracker ptBands = ProgressTracker(;
      for (unsigned int bandNum = 0; bandNum < bandCount; ++bandNum) {
		  mProgressTracker.report("Processing band", 5 + (99 - 5)*(bandNum/(double) bandCount), NORMAL, true);
          Opticks2DData<T> dataToConvolve(&imgToConvolve, bandNum);
          Opticks2DData<T> convolutionKernel(&imgConvolutionKernel, bandNum);
          Opticks2DData<T> result(&imgResult, bandNum, true);

          Convolution2D_FFT<T> convolution(&dataToConvolve, &convolutionKernel, &result);
          convolution.convolve();
	  }

   	mProgressTracker.report("Convolution complete", 99, NORMAL, true);

	pResult->updateData();
      pResult.release();
}

SpatialDataView* FFTConvolutionDlg::displayResult() { return pView; }
