// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFDCOMPUTER_H
#define MFDCOMPUTER_H

#include "main.h"
#include "game_hud.h"

class MFDComputer
{
public:
  MFDComputer();
  ~MFDComputer();

  void update(const Real a_dt);

  // make selected
  void activate(bool a_toggle);

  // communicate with mfd aux
  void setLine(const string& a_message,
               usint         a_line);
  string getLine(usint a_i,
                 usint a_j);

  // get the view type
  mfd_view::view_type getViewType();

private:
  // selecting view typeon the mfd aux
  void selectView();

  // whether input is active
  bool Selected = false;

  // what is being shown
  mfd_view::view_type SelectedView = mfd_view::damage_diagram_self;
  usint SelectedPage = 0;
  bool SelectingView = false;
  usint SelectedViewIdx = 0;
  usint NumOfViews = 0;

  // data for hud elements to display
  char mfd_aux_lines[HUD_NUM_OF_COLOURS][HUD_NUM_OF_MFD_AUX_LINES][HUD_MFD_AUX_LINE_LENGTH + 1];
};

inline mfd_view::view_type MFDComputer::getViewType()
{
  return SelectedView;
}

inline string MFDComputer::getLine(usint a_i,
                                   usint a_j)
{
  return mfd_aux_lines[a_i][a_j];
}

#endif // MFDCOMPUTER_H
