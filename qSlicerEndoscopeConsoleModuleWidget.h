/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerEndoscopeConsoleModuleWidget_h
#define __qSlicerEndoscopeConsoleModuleWidget_h

// OpenCV includes
#include "cv.h"
#include "highgui.h"

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerEndoscopeConsoleModuleExport.h"

// Qt includes
#include <qtimer.h>

// VTK includes
#include <vtkSmartPointer.h>
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkCamera.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  test
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Slicer includes
#include <vtkMRMLSliceLogic.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceCompositeNode.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLColorTableNode.h>

// VTK includes
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor2D.h>


class qSlicerEndoscopeConsoleModuleWidgetPrivate;
class vtkMRMLNode;
class QTimer;
class vtkSlicerViewerWidget;
class vtkMRMLSliceLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_ENDOSCOPECONSOLE_EXPORT qSlicerEndoscopeConsoleModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerEndoscopeConsoleModuleWidget(QWidget *parent=0);
  virtual ~qSlicerEndoscopeConsoleModuleWidget();
    
  int ViewerBackgroundOff(vtkRenderer* activeRenderer);
  int ViewerBackgroundOn(vtkRenderer* activeRenderer, vtkImageData* imageData);
  int StartCamera(int channel, const char* path);
  int StopCamera();
  int CameraHandler();
    
  // for timer loop to refresh obtained video images
  QTimer *t;
  int timerFlag;
  
  // video import
  CvCapture* capture;
  CvSize imageSize;
  IplImage* RGBImage;
  IplImage* undistortionImage;
  IplImage*     captureImage;

  vtkImageData* VideoImageData;
  vtkRenderer*   BackgroundRenderer;
  vtkImageActor* BackgroundActor;
    
public slots:
  void onVideoONToggled(bool checked);
  void timerIntrupt();

protected:
  QScopedPointer<qSlicerEndoscopeConsoleModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerEndoscopeConsoleModuleWidget);
  Q_DISABLE_COPY(qSlicerEndoscopeConsoleModuleWidget);
};

#endif
