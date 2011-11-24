/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from
 * http://www.gnu.org/licenses/lgpl.html
 */


#ifndef CONVOLUTION2DFFT_ALGORITHMSHELL_H
#define CONVOLUTION2DFFT_ALGORITHMSHELL_H

#include "ExecutableShell.h"
#include "QDialogConvolutionFFT.h"
#include "ProgressTracker.h"

class TutorialA : public ExecutableShell {
public:
   TutorialA();
   virtual ~TutorialA() { }

   virtual bool getInputSpecification(PlugInArgList*& pInArgList);
   virtual bool getOutputSpecification(PlugInArgList*& pOutArgList);
   virtual bool execute(PlugInArgList* pInArgList, PlugInArgList* pOutArgList);

private:
   ProgressTracker mProgressTracker;
   FFTConvolutionDlg* mpConvolutionDlg;
};
#endif // CONVOLUTION2DFFT_ALGORITHMSHELL_H
