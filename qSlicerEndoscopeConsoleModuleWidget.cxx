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
#include <QPushButton>
#include <QVBoxLayout>
#include <QProcessEnvironment>
#include <QWebView>

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
  this->timerFlag = 0; // for test
  connect(t,SIGNAL(timeout()),this,SLOT(timerIntrupt()));
    
  // video import setup
  this->captureImage = NULL;
  this->RGBImage     = NULL;
  this->undistortionImage = NULL;
  this->imageSize.width = 0;
  this->imageSize.height = 0;
    
  this->VideoImageData = NULL;
    
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::timerIntrupt()
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    //printf("timer!\n");
    this->timerFlag++;
    
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::onVideoONToggled(bool checked)
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    if(checked)
    {
        CvCapture* capture;
        
        // Open /dev/video0
        capture = cvCaptureFromCAM(0);
        
        assert( capture != NULL);
        
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FPS,30);
        
        IplImage* bgr_frame = cvQueryFrame( capture );
        
        cvNamedWindow( "Endoscope Viewer", CV_WINDOW_NORMAL);
        
        // QTimer start
        if( t->isActive())
            t->stop();
        this->timerFlag = 0;
        t->start(1000); // 1s
        
        // ---------------------------------------------------------
        // Set up
        IplImage* captureImageTmp = NULL;
        CvSize   newImageSize;
        
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
        
        // VTK/Qt wedded
        //this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(BackgroundRenderer);
        

        if (capture)
        {
            if(NULL == (captureImageTmp = cvQueryFrame( capture )))
            {
                fprintf(stdout, "\n\nCouldn't take a picture\n\n");
                return;
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
                
                
                // Reffered qSlicerWidgetTest2.cxx
                //qSlicerWidget* widget = new qSlicerWidget();
                //widget->setParent(&parentWidget);

                // test; code from setup() function in qSlicerColorsModuleWidget.cxx
                qSlicerApplication *  app = qSlicerApplication::application();
                qMRMLThreeDView* threeDView = app->layoutManager()->threeDWidget(0)->threeDView();
                vtkRenderer* activeRenderer = app->layoutManager()->activeThreeDRenderer();
                
                vtkRenderWindow* activeRenderWindow = vtkRenderWindow::New();
                activeRenderWindow = activeRenderer->GetRenderWindow();
                
                //QVTKWidget* vtkWidget = new QVTKWidget();
                //vtkWidget->SetRenderWindow(activeRenderWindow);
                
                vtkRenderer* BackgroundRenderer2 = vtkRenderer::New();
                activeRenderWindow->AddRenderer(BackgroundRenderer2);
                
                //vtkWidget->SetRenderer(threeDView);
                
                //vtkMRMLSliceNode *sliceNode = this->GetSliceNode();
                //vtkMRMLScalarVolumeNode* volumeNode =
                
                //vtkMRMLScalarVolumeNode::SafeDownCast( this->GetLayerVolumeNode (0) );
                
                //vtkWidget->setParent(&parentWidget);
                //vbox.addWidget(vtkWidget);
                //vtkWidget->GetRenderWindow()->Render();
                
                //vtkImageData test = this->GetBackgroundImageData();

                //vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
                
                //ViewerBackgroundOff(vwidget);
                //ViewerBackgroundOn(this->VideoImageData);
                
                //ViewerBackgroundOn(vwidget, this->VideoImageData);
                
            }

        }
        // ---------------------------------------------------------

        while( ((bgr_frame = cvQueryFrame( capture )) != NULL) && checked)
        {
            // video image process
            //this->cameraHandler();
            
            // Display image on OpenCV window
            cvShowImage( "Endoscope Viewer", bgr_frame);
            char c = cvWaitKey(33);
            if( c == 27 /* esc key */ /*|| this->timerFlag > 10 /* for test */) break;
        }
        
        d->VideoON->setChecked(false);
        d->VideoOFF->setChecked(true);
        
        // Qtimer stop
        t->stop();
        
        cvReleaseCapture(&capture);
        cvDestroyWindow( "Endoscope Viewer" );
    }
}


//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::ViewerBackgroundOn(vtkImageData* imageData)
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    //vtkKWRenderWidget* rwidget;
    QVTKWidget* vtkWidget = new QVTKWidget(); // replace rwidget?
    //vtkRenderWindow* rwindow;

    if (1//vwidget&&
        //(rwidget = vwidget->GetMainViewer()) &&
        /*(rwindow = rwidget->GetRenderWindow())*/)
    {
        //if (rwidget->GetNumberOfRenderers() == 1)
        //{
    
            // VTK Renderer
            this->BackgroundActor = vtkImageActor::New();
            this->BackgroundActor->SetInput(imageData);

            this->BackgroundRenderer = vtkRenderer::New();
            this->BackgroundRenderer->AddActor(this->BackgroundActor);
            this->BackgroundRenderer->InteractiveOff();
            this->BackgroundRenderer->SetLayer(0);

            // Adjust camera position so that image covers the draw area.
    
            this->BackgroundActor->Modified();
            //rwidget->GetNthRenderer(0)->SetLayer(1);
            //rwidget->AddRenderer(this->BackgroundRenderer);
            vtkWidget->GetRenderWindow()->AddRenderer(this->BackgroundRenderer);
            //rwindow->Render();
            vtkWidget->GetRenderWindow()->Render(); //this->BackgroundRenderer->Render();
    
            vtkCamera* camera = this->BackgroundRenderer->GetActiveCamera();
            double x, y, z;
            camera->GetPosition(x, y, z);
            camera->SetViewAngle(90.0);
            camera->SetPosition(x, y, y);

            // The following code fixes a issue that
            // video doesn't show up on the viewer.
            /*
             vtkCamera* fcamera = rwidget->GetNthRenderer(0)->GetActiveCamera();
             if (fcamera)
             {
             fcamera->Modified();
             }
             */
        //}
    }
    
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModuleWidget::ViewerBackgroundOff(/*vtkSlicerViewerWidget* vwidget*/)
{
    Q_D(qSlicerEndoscopeConsoleModuleWidget);
    
    
    //vtkKWRenderWidget* rwidget;
    //vtkRenderWindow* rwindow;
    QVTKWidget* vtkWidget = new QVTKWidget(); // replace rwidget?
    
    
    if (1/*vwidget&&
        (rwidget = vwidget->GetMainViewer()) &&
        (rwindow = rwidget->GetRenderWindow())*/)
    {
        //if (rwidget->GetNumberOfRenderers() > 1)
        //{
            //rwidget->RemoveNthRenderer(1);
            //rwidget->GetNthRenderer(0)->SetLayer(0);
            vtkWidget->GetRenderWindow()->AddRenderer(this->BackgroundRenderer);
            //rwindow->Render();
            ////vtkWidget->GetRenderWindow()->Render();
            vtkWidget->GetRenderWindow()->Render();
    
            this->BackgroundRenderer = NULL;
            this->BackgroundActor = NULL;
        //}
    }
    
    /*
    return 0;
    */
    
}
