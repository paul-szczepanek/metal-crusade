// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_radar_display.h"
#include "radar_computer.h"
#include "game.h"
#include "unit.h"
#include "game_camera.h"
#include "game_controller.h"

const usint num_of_initial_dots = 16;
const usint num_of_radar_ranges = 4;
const Real radar_ranges[num_of_radar_ranges] = { 100, 200, 400, 800 };
const string dot_green_name("hud_radar_dot_green");
const string dot_red_name("hud_radar_dot_red");

HudRadarDisplay::HudRadarDisplay(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design)
{
  // start off with the radar active status read from the radar computer
  Enabled = Game::Hud->radar->getActive();
  Game::Hud->Controller->ControlBlock.radar = Enabled;
  Game::Hud->radar->setRadarRange(radar_ranges[RangeIndex]);

  GreenMaterial = GameHud::createOverlayMaterial(dot_green_name);
  RedMaterial = GameHud::createOverlayMaterial(dot_red_name);

  for (usint i = 0; i < num_of_initial_dots; ++i) {
    string id = string("radar_dot_") + intoString(i);
    DotElements.push_back(createPanel(id, dot_green_name,
                                       0, 0, 8, 8, Container));
    DotElements.back()->hide();
  }
}

HudRadarDisplay::~HudRadarDisplay()
{

}

void HudRadarDisplay::update(Real a_dt)
{

  if (Enabled != Game::Hud->radar->getActive()) {
    Enabled != Enabled;
    if (!Enabled) {
      for (size_t i = 0, for_size = DotElements.size(); i < for_size; ++i) {
        DotElements[i]->hide();
      }
    }
  }

  if(!Enabled) {
    return;
  }

  // radar radius change
  if (take(Game::Hud->Controller->ControlBlock.radar_zoom_in)) {
    // see smaller area
    if (RangeIndex > 0) { --RangeIndex; }

    // adjust radar sphere
    Game::Hud->radar->setRadarRange(radar_ranges[RangeIndex]);

  } else if (take(Game::Hud->Controller->ControlBlock.radar_zoom_out)) {
    // see larger area
    if (++RangeIndex == num_of_radar_ranges) { --RangeIndex; }

    // adjust radar sphere
    Game::Hud->radar->setRadarRange(radar_ranges[RangeIndex]);
  }

  // how many dots on the screen?
  vector<radar::CorpusDot>& dots = Game::Hud->radar->getDots();
  size_t num_of_dots = dots.size();

  // grow number of dots if needed
  if (DotElements.size() < num_of_dots) {
    for (size_t i = 0, for_size = num_of_dots - DotElements.size(); i < for_size; ++i) {
      string id = string("radar_dot_") + intoString(DotElements.size());
      DotElements.push_back(createPanel(id, dot_green_name,
                                         0, 0, 8, 8, Container));
      DotElements.back()->hide();
    }
  }

  Real radar_angle = (Game::Camera->getOrientation().getYaw()).valueRadians();

  Real x0 = Game::Hud->PlayerUnit->getX();
  Real y0 = Game::Hud->PlayerUnit->getZ();
  Real range_scale = size.first / Game::Hud->radar->getRadarRange();

  for (size_t i = 0, for_size = DotElements.size(); i < for_size; ++i) {
    if (i < num_of_dots && dots[i].detected) {
      Real dot_size = max(dots[i].size * range_scale, Real(8));
      DotElements[i]->setDimensions(dot_size, dot_size);
      Real x = (dots[i].position.x - x0) * range_scale;
      Real y = (dots[i].position.z - y0) * range_scale;

      x = (cos(radar_angle) * x - sin(radar_angle) * y) * 0.5
          + (size.first - dot_size) * 0.5;
      y = (sin(radar_angle) * x + cos(radar_angle) * y) * 0.5
          + (size.first - dot_size) * 0.5;

      DotElements[i]->setPosition(x, y);
      DotElements[i]->show();
    } else {
      DotElements[i]->hide();
    }
  }
}
