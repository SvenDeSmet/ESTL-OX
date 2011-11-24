/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AlgorithmShellConvolutionFFT.h"
#include "AppVerify.h"
#include "PlugInArgList.h"
#include "PlugInManagerServices.h"
#include "PlugInRegistration.h"
#include "Progress.h"

REGISTER_PLUGIN_BASIC(OpticksTutorialA, TutorialA);

TutorialA::TutorialA() : mpConvolutionDlg(NULL) {
   setDescriptorId("{5D8F5DD0-9B20-42B1-A060-589DFBC85D00}");
   setDescription("ESTL Opticks Extension"); setName("ESTL-OX"); setVersion("0.0.1 pre-beta");
   setCreator("Sven De Smet"); setCopyright("Copyright (C) 2011, Sven De Smet.");
   setProductionStatus(false);
   setType("Transformation");
   setMenuLocation("[General Algorithms]/FFT-based convolution");
   setAbortSupported(false);
}

bool TutorialA::getInputSpecification(PlugInArgList*& pInArgList) {
   VERIFY(pInArgList = Service<PlugInManagerServices>()->getPlugInArgList());
   pInArgList->addArg<Progress>(Executable::ProgressArg(), NULL, "Progress reporter");
   return true;
}

bool TutorialA::getOutputSpecification(PlugInArgList*& pOutArgList) {
	//pOutArgList = NULL;	return true;
    VERIFY(pOutArgList = Service<PlugInManagerServices>()->getPlugInArgList());
    VERIFY(pOutArgList->addArg<RasterElement>("Data Element", "The new raster element with the convolved data."));
    if (!Service<ApplicationServices>()->isBatch()) {
       VERIFY(pOutArgList->addArg<SpatialDataView>("View", "The newly created view. This will be NULL if the application is non-interactive."));
    }
   return true;
}

bool TutorialA::execute(PlugInArgList* pInputArgs, PlugInArgList* pOutArgList) {
    if (pInputArgs == NULL) return false;
    PlugInArg* pArg = NULL;
    Progress* pProgress = ((pInputArgs->getArg(Executable::ProgressArg(), pArg)) && (pArg != NULL)) ? reinterpret_cast<Progress*>(pArg->getActualValue()) : NULL;
    mProgressTracker.initialize(pProgress, "Executing Convolution Plug-In", "app", "B7E1EF13-70D1-4a45-A95F-0126373C3171");
    
	mpConvolutionDlg = new FFTConvolutionDlg(this, mProgressTracker, Service<DesktopServices>()->getMainWidget());
    if (mpConvolutionDlg->exec() == QDialog::Accepted) {
        if (!Service<ApplicationServices>()->isBatch()) pOutArgList->setPlugInArgValue("View", mpConvolutionDlg->displayResult());
        pOutArgList->setPlugInArgValue("Data Element", mpConvolutionDlg->resultRasterElement);
    }
	delete mpConvolutionDlg;

    mProgressTracker.upALevel();

    return true;
}
