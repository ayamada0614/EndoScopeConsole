/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerEndoscopeConsoleFooBarWidget.h"
#include "ui_qSlicerEndoscopeConsoleFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_EndoscopeConsole
class qSlicerEndoscopeConsoleFooBarWidgetPrivate
  : public Ui_qSlicerEndoscopeConsoleFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerEndoscopeConsoleFooBarWidget);
protected:
  qSlicerEndoscopeConsoleFooBarWidget* const q_ptr;

public:
  qSlicerEndoscopeConsoleFooBarWidgetPrivate(
    qSlicerEndoscopeConsoleFooBarWidget& object);
  virtual void setupUi(qSlicerEndoscopeConsoleFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerEndoscopeConsoleFooBarWidgetPrivate
::qSlicerEndoscopeConsoleFooBarWidgetPrivate(
  qSlicerEndoscopeConsoleFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerEndoscopeConsoleFooBarWidgetPrivate
::setupUi(qSlicerEndoscopeConsoleFooBarWidget* widget)
{
  this->Ui_qSlicerEndoscopeConsoleFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerEndoscopeConsoleFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleFooBarWidget
::qSlicerEndoscopeConsoleFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerEndoscopeConsoleFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerEndoscopeConsoleFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleFooBarWidget
::~qSlicerEndoscopeConsoleFooBarWidget()
{
}
