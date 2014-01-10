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
#include <QtPlugin>

// EndoscopeConsole Logic includes
#include <vtkSlicerEndoscopeConsoleLogic.h>

// EndoscopeConsole includes
#include "qSlicerEndoscopeConsoleModule.h"
#include "qSlicerEndoscopeConsoleModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerEndoscopeConsoleModule, qSlicerEndoscopeConsoleModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerEndoscopeConsoleModulePrivate
{
public:
  qSlicerEndoscopeConsoleModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerEndoscopeConsoleModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleModulePrivate
::qSlicerEndoscopeConsoleModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerEndoscopeConsoleModule methods

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleModule
::qSlicerEndoscopeConsoleModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerEndoscopeConsoleModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerEndoscopeConsoleModule::~qSlicerEndoscopeConsoleModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerEndoscopeConsoleModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerEndoscopeConsoleModule::acknowledgementText()const
{
  //return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
  return "";
}

//-----------------------------------------------------------------------------
QStringList qSlicerEndoscopeConsoleModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Atsushi Yamada (Shiga University of Medical Science) Junichi Tokuda (BWH) Laurent Chauvin (BWH)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerEndoscopeConsoleModule::icon()const
{
  return QIcon(":/Icons/EndoscopeConsole.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerEndoscopeConsoleModule::categories() const
{
  return QStringList() << "IGT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerEndoscopeConsoleModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerEndoscopeConsoleModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerEndoscopeConsoleModule
::createWidgetRepresentation()
{
  return new qSlicerEndoscopeConsoleModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerEndoscopeConsoleModule::createLogic()
{
  return vtkSlicerEndoscopeConsoleLogic::New();
}
