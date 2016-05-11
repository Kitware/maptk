/*ckwg +29
 * Copyright 2016 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "DepthMapOptions.h"
#include "ui_DepthMapOptions.h"

#include <qtUiState.h>
#include <qtUiStateItem.h>

#include <vtkActor.h>

//-----------------------------------------------------------------------------
class DepthMapOptionsPrivate
{
public:
  Ui::DepthMapOptions UI;
  qtUiState uiState;

//  QList<vtkActor*> actors;
  std::map<std::string, vtkProp3D*> actors;

};

QTE_IMPLEMENT_D_FUNC(DepthMapOptions)

DepthMapOptions::DepthMapOptions(QString const& settingsGroup,
                                 QWidget* parent, Qt::WindowFlags flags) :
  QWidget(parent, flags), d_ptr(new DepthMapOptionsPrivate)
{
  QTE_D();

  // Set up UI
  d->UI.setupUi(this);


  // Set up option persistence
  d->uiState.setCurrentGroup(settingsGroup);

  d->uiState.restore();

  // Connect signals/slots
  connect(d->UI.radioPoints, SIGNAL(toggled(bool)),
          this, SLOT(switchsVisible(bool)));

  connect(d->UI.radioSurfaces, SIGNAL(toggled(bool)),
          this, SLOT(switchVisible(bool)));
}

DepthMapOptions::~DepthMapOptions()
{
  QTE_D();
  d->uiState.save();
}

void DepthMapOptions::addActor(std::string type, vtkProp3D* actor)
{
  QTE_D();

  d->actors.insert(std::pair<std::string, vtkProp3D*>(type,actor));

}

//void DepthMapOptions::switchPointsVisible(bool state)
//{
//  QTE_D();

//  d->actors[0]->SetVisibility(state);

//  emit this->depthMapChanged();
//}

//void DepthMapOptions::switchSurfacesVisible(bool state)
//{
//  QTE_D();
//  std::cout << "toggled" << std::endl;
//  d->actors[1]->SetVisibility(state);

//  emit this->depthMapChanged();
//}

void DepthMapOptions::switchVisible(bool state)
{
  QTE_D();

  if (d->UI.radioPoints->isEnabled())
    d->actors["points"]->SetVisibility(d->UI.radioPoints->isChecked());
  if (d->UI.radioSurfaces->isEnabled())
    d->actors["surfaces"]->SetVisibility(d->UI.radioSurfaces->isChecked());

  emit this->depthMapChanged();

}

//void DepthMapOptions::enablePoints()
//{
//  QTE_D();

//  d->UI.radioPoints->setEnabled(true);
//  d->UI.radioPoints->setChecked(true);
//}

//void DepthMapOptions::enableSurfaces()
//{
//  QTE_D();

//  d->UI.radioSurfaces->setEnabled(true);

//  if(!d->UI.radioPoints->isEnabled())
//    d->UI.radioSurfaces->setChecked(true);
//}

void DepthMapOptions::enableDM(std::string type)
{
  QTE_D();
  if(type == "vtp")
  {
    d->UI.radioPoints->setEnabled(true);
    d->UI.radioPoints->setChecked(true);
  }
  else if(type == "vts")
  {
    d->UI.radioSurfaces->setEnabled(true);

    if(!d->UI.radioPoints->isEnabled())
      d->UI.radioSurfaces->setChecked(true);
  }
}

bool DepthMapOptions::isPointsChecked()
{
  QTE_D();

  return d->UI.radioPoints->isChecked();
}

bool DepthMapOptions::isSurfacesChecked()
{
  QTE_D();

  return d->UI.radioSurfaces->isChecked();
}
