// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFDCOMPUTER_H
#define MFDCOMPUTER_H

#include "main.h"
#include "hud_design.h"

class MFDComputer
{
public:
  MFDComputer();
  ~MFDComputer()
  {
  }

  void update(Real a_dt);

  // make selected
  void activate(bool a_toggle);

  // communicate with mfd aux
  void setLine(const string& a_message,
               usint         a_line);
  string getLine(usint a_i,
                 usint a_j);

  // get the view type
  mfd_view::view_type getViewType()
  {
    return selected_view;
  }

  // get values for the mfd

private:
  // selecting view typeon the mfd aux
  void selectView();

  // whether input is active
  bool selected;

  // what is being shown
  mfd_view::view_type selected_view;
  usint selected_page;
  bool selecting_view;
  usint selected_view_index;
  usint num_of_views;

  // data for hud elements to display
  char mfd_aux_lines[hud_num_of_colours][hud_num_of_mfd_aux_lines][hud_mfd_aux_line_length + 1];
};

inline string MFDComputer::getLine(usint a_i,
                                   usint a_j)
{
  return mfd_aux_lines[a_i][a_j];
}

#endif // MFDCOMPUTER_H
