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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerEndoscopeConsoleModuleWidget.h"
#include "ui_qSlicerEndoscopeConsoleModuleWidget.h"

// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerWidget.h"

// QVTK includes
#include <QVTKWidget.h>

// qMRMLWidget includes
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDWidget.h"

// Qt includes
#include <QApplication>
#include <QTimer>

// VTK includes
#include <vtkNew.h>
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLSliceNode.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerEndoscopeConsoleModuleWidgetPrivate: public Ui_qSlicerEndoscopeConsoleModuleWidget
{
public:
  qSlicerEndoscopeConsoleModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerEndoscopeConsoleModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleModuleWidgetPrivate::qSlicerEndoscopeConsoleModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerEndoscopeConsoleModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleModuleWidget::qSlicerEndoscopeConsoleModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerEndoscopeConsoleModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleModuleWidget::~qSlicerEndoscopeConsoleModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::setup()
{
  Q_D(qSlicerEndoscopeConsoleModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
    
  connect(d->VideoON, SIGNAL(toggled(bool)),
          this, SLOT(onVideoONToggled(bool)));

  // QTimer setup
  t = new QTimer();
  this->timerFlag = 0;
  connect(t,SIGNAL(timeout()),this,SLOT(timerIntrupt()));
    
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::timerIntrupt()
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    if(this->timerFlag == 1)
    {
        this->CameraHandler();
        // test viewer
        //cvShowImage( "Endoscope Viewer", this->RGBImage);
    }
    
}

//-----------------------------------------------------------------------------
int qSlicerEndoscopeConsoleModuleWidget::CameraHandler()
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
 
    IplImage* captureImageTmp = NULL;
    CvSize   newImageSize;
    
    if (this->capture)
    {
        if(NULL == (captureImageTmp = cvQueryFrame( this->capture )))
        {
            fprintf(stdout, "\n\nCouldn't take a picture\n\n");
            return 0;
        }

        newImageSize = cvGetSize( captureImageTmp );
        
        // check if the image size is changed
        if (newImageSize.width != this->imageSize.width ||
            newImageSize.height != this->imageSize.height)
        {
            this->imageSize.width = newImageSize.width;
            this->imageSize.height = newImageSize.height;
            this->captureImage = cvCreateImage(this->imageSize, IPL_DEPTH_8U,3);
            this->RGBImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
            this->undistortionImage = cvCreateImage( this->imageSize, IPL_DEPTH_8U, 3);
            
            this->VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
            this->VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
            this->VideoImageData->SetNumberOfScalarComponents(3);
            this->VideoImageData->SetScalarTypeToUnsignedChar();
            this->VideoImageData->AllocateScalars();
            this->VideoImageData->Update();
            
            qSlicerApplication *  app = qSlicerApplication::application();
            qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
            vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
            activeRenderer->SetLayer(1);
            
            ViewerBackgroundOff(activeRenderer);
            ViewerBackgroundOn(activeRenderer, this->VideoImageData);
        }

        // Display video image
        //cvFlip(captureImageTmp, this->captureImage, 0);
        //cvCvtColor( this->captureImage, this->RGBImage, CV_BGR2RGB);
        cvCvtColor(captureImageTmp, this->RGBImage, CV_BGR2RGB);

        unsigned char* idata;
        idata = (unsigned char*) RGBImage->imageData;
        
        int dsize = this->imageSize.width*this->imageSize.height*3;
        memcpy((void*)this->VideoImageData->GetScalarPointer(), (void*)this->RGBImage->imageData, dsize);

        if (this->VideoImageData && this->BackgroundRenderer)
        {
            this->VideoImageData->Modified();
            this->BackgroundRenderer->GetRenderWindow()->Render();
            
            /*
            qSlicerApplication *  app = qSlicerApplication::application();
            qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
            vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
            activeRenderer->SetLayer(1);
            activeRenderer->Render();
            */
            
        }
        
    }
    
    return 1;
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::onVideoONToggled(bool checked)
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);

    
    if(checked)
    {
        // QTimer start
        if( t->isActive())
            t->stop();
        this->timerFlag = 0;
        t->start(100); // 1000 = 1s
        
        this->StartCamera(0, NULL);

    }
    else
    {
        this->StopCamera();
        if(this->capture)
        {
            cvReleaseCapture(&this->capture);
        }
        this->timerFlag = 0;
        // Qtimer stop
        if( t->isActive())
            t->stop();
    }
    
}

//-----------------------------------------------------------------------------
int qSlicerEndoscopeConsoleModuleWidget::StartCamera(int channel, const char* path)
{
    // if channel = -1, OpenCV will read image from the video file specified by path
    
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    // video import setup
    this->capture = NULL;
    this->captureImage = NULL;
    this->RGBImage     = NULL;
    this->undistortionImage = NULL;
    this->imageSize.width = 0;
    this->imageSize.height = 0;
    this->VideoImageData = NULL;
    
    qSlicerApplication *  app = qSlicerApplication::application();
    qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
    vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
    activeRenderer->SetLayer(1);
    
    if (channel < 0 && path != NULL)
    {
        this->capture = cvCaptureFromAVI(path);
    }
    else
    {
        this->capture = cvCaptureFromCAM(channel);
    }
    
    if (this->capture == NULL)
    {
        return 0;
    }

    // test viewer
    //this->bgr_frame = NULL;
    //this->bgr_frame = cvQueryFrame( this->capture );
    //cvNamedWindow( "Endoscope Viewer", CV_WINDOW_NORMAL);

    this->timerFlag = 1;
    
    if (!this->VideoImageData)
    {
        this->VideoImageData = vtkImageData::New();
        this->VideoImageData->SetDimensions(64, 64, 1);
        this->VideoImageData->SetExtent(0, 63, 0, 63, 0, 0 );
        this->VideoImageData->SetSpacing(1.0, 1.0, 1.0);
        this->VideoImageData->SetOrigin(0.0, 0.0, 0.0);
        this->VideoImageData->SetNumberOfScalarComponents(3);
        this->VideoImageData->SetScalarTypeToUnsignedChar();
        this->VideoImageData->AllocateScalars();
    }
    this->VideoImageData->Update();
    this->ViewerBackgroundOn(activeRenderer, this->VideoImageData);
    
    return 1;
    
}

//-----------------------------------------------------------------------------
int qSlicerEndoscopeConsoleModuleWidget::StopCamera()
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    // Qtimer stop
    if(this->capture)
    {
        cvReleaseCapture(&this->capture);
    }
    if( t->isActive())
        t->stop();

    qSlicerApplication *  app = qSlicerApplication::application();
    qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
    vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
    activeRenderer->SetLayer(1);
    
    ViewerBackgroundOff(activeRenderer);
    
}

//-----------------------------------------------------------------------------
int qSlicerEndoscopeConsoleModuleWidget::ViewerBackgroundOn(vtkRenderer* activeRenderer,vtkImageData* imageData)
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    vtkRenderWindow* activeRenderWindow = activeRenderer->GetRenderWindow();

    if (activeRenderer)
    {
        
        // VTK Renderer
        this->BackgroundActor = vtkImageActor::New();
        this->BackgroundActor->SetInput(imageData);

        this->BackgroundRenderer = vtkRenderer::New();
        this->BackgroundRenderer->AddActor(this->BackgroundActor);
        this->BackgroundRenderer->InteractiveOff();
        this->BackgroundRenderer->SetLayer(0);
        
        this->BackgroundActor->Modified();
        activeRenderWindow->AddRenderer(this->BackgroundRenderer);
        activeRenderer->Render();
        
        // Adjust camera position so that image covers the draw area.
        
        vtkCamera* camera = this->BackgroundRenderer->GetActiveCamera();
        double x, y, z;
        camera->GetPosition(x, y, z);
        camera->SetViewAngle(90.0);
        camera->SetPosition(x, y, y);
        
        // The following code fixes a issue that
        // video doesn't show up on the viewer.
        //vtkCamera* fcamera = rwidget->GetNthRenderer(0)->GetActiveCamera();
        vtkCamera* fcamera = activeRenderer->GetActiveCamera();
        if (fcamera)
        {
         fcamera->Modified();
        }
        
    }
    
    return 0;
    
}

//-----------------------------------------------------------------------------
int qSlicerEndoscopeConsoleModuleWidget::ViewerBackgroundOff(vtkRenderer* activeRenderer)
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    vtkRenderWindow* activeRenderWindow = activeRenderer->GetRenderWindow();
    
    
    if (activeRenderer)
    {
        //activeRenderWindow->AddRenderer(this->BackgroundRenderer);
        //activeRenderWindow->RemoveNthRenderer(1);
        //activeRenderer->SetLayer(0);
        activeRenderer->Render();
        
        
        this->BackgroundRenderer = NULL;
        this->BackgroundActor = NULL;
    }
    
    return 0;

}
