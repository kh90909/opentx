/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../opentx.h"

#define BIGSIZE       DBLSIZE
#define BOX_WIDTH     23
#define BOX_CENTERY   (LCD_H-9-BOX_WIDTH/2)
#define LBOX_CENTERX  (LCD_W/4 + 10)
#define RBOX_CENTERX  (3*LCD_W/4 - 10)
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define VSWITCH_X(i)  (16 + 3*i)
#define VSWITCH_Y     (LCD_H-8)
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)

#define TRIM_LEN 27
#define MARKER_WIDTH  5
#define BOX_LIMIT     (BOX_WIDTH-MARKER_WIDTH)

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, i, len ;  // declare temporary variables
  for (x=LCD_W/2-5, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; x+=5, i++) {
    if (IS_POT_AVAILABLE(i)) {
      len = ((calibratedStick[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(x, LCD_H-8, len)
    }
  }
}

void drawStick(coord_t centrex, int16_t xval, int16_t yval)
{
  lcd_square(centrex-BOX_WIDTH/2, BOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  DO_CROSS(centrex, BOX_CENTERY, 3);
  lcd_square(centrex + (xval/((2*RESX)/BOX_LIMIT)) - MARKER_WIDTH/2, BOX_CENTERY - (yval/((2*RESX)/BOX_LIMIT)) - MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], calibStickVert);

  calibStickVert = calibratedStick[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3)], calibStickVert);

  drawPotsBars();
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    coord_t xm, ym;
    xm = x[CONVERT_MODE(i)];

    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

#if !defined(CPUM64) || !defined(FRSKY)
    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }
#endif
    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    }
    else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    }
    else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcd_vline(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcd_vline(xm-1, ym-1,  3);
        lcd_vline(xm+1, ym-1,  3);
      }
      ym -= val;
#if !defined(CPUM64) || !defined(FRSKY)
      lcd_filled_rect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcd_hline(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcd_hline(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcd_hline(xm-1, ym,  3);
      }
#endif
    }
    else {
      ym = 60;
      lcd_hline(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcd_hline(xm-1, ym-1,  3);
      lcd_hline(xm-1, ym+1,  3);
      xm += val;
#if !defined(CPUM64) || !defined(FRSKY)
      lcd_filled_rect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcd_vline(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcd_vline(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcd_vline(xm, ym-1,  3);
      }
#endif
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}


void displayTimers()
{
#if defined(TRANSLATIONS_CZ)
  #define MAINTMR_LBL_COL (9*FW-FW/2-1)
#else
  #define MAINTMR_LBL_COL (9*FW-FW/2+3)
#endif
  // Main timer
  if (g_model.timers[0].mode) {
    TimerState & timerState = timersStates[0];
    uint8_t att = DBLSIZE | (timerState.val<0 ? BLINK|INVERS : 0);
    putsTimer(12*FW+2+10*FWNUM-4, FH*2, timerState.val, att, att);
    uint8_t xLabel = (timerState.val >= 0 ? MAINTMR_LBL_COL : MAINTMR_LBL_COL-7);
#if defined(CPUARM)
    uint8_t len = zlen(g_model.timers[0].name, LEN_TIMER_NAME);
    if (len > 0) {
      xLabel += (LEN_TIMER_NAME-len)*FW;
      lcd_putsnAtt(xLabel, FH*3, g_model.timers[0].name, len, ZCHAR);
    }
    else {
      putsTimerMode(xLabel, FH*3, g_model.timers[0].mode);
    }
#else
    putsTimerMode(xLabel, FH*3, g_model.timers[0].mode);
#endif
  }
}

void displayBattVoltage()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, 0);
  lcd_filled_rect(VBATT_X-25, VBATT_Y+9, 22, 5);
  lcd_vline(VBATT_X-3, VBATT_Y+10, 3);
  uint8_t count = limit<uint8_t>(2, 20 * (g_vbat100mV - g_eeGeneral.vBatMin - 90) / (30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 20);
  for (uint8_t i=0; i<count; i+=2)
    lcd_vline(VBATT_X-24+i, VBATT_Y+10, 3);
  if (g_vbat100mV > g_eeGeneral.vBatWarn || BLINK_ON_PHASE)
    lcd_filled_rect(VBATT_X-26, VBATT_Y, 25, 15);
#else
  LcdFlags att = (g_vbat100mV <= g_eeGeneral.vBatWarn ? BLINK|INVERS : 0) | BIGSIZE;
  putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
  lcd_putc(VBATT_X, VBATTUNIT_Y, 'V');
#endif
}

#if defined(PCBSKY9X)
void displayVoltageOrAlarm()
{
  if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
    putsValueWithUnit(6*FW-1, 2*FH, getTemperature(), UNIT_TEMPERATURE, BLINK|INVERS|DBLSIZE);
  }
  else if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
    putsValueWithUnit(7*FW-1, 2*FH, (g_eeGeneral.mAhUsed + Current_used*(488 + g_eeGeneral.currentCalib)/8192/36)/10, UNIT_MAH, BLINK|INVERS|DBLSIZE);
  }
  else {
    displayBattVoltage();
  }
}
#else
  #define displayVoltageOrAlarm() displayBattVoltage()
#endif

#define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
#define EVT_KEY_CONTEXT_MENU EVT_KEY_BREAK(KEY_MENU)

#if defined(NAVIGATION_MENUS)
void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
#if defined(CPUARM)
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
  else if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
  else if (result == STR_RESET_SUBMENU) {
    MENU_ADD_ITEM(STR_RESET_FLIGHT);
    MENU_ADD_ITEM(STR_RESET_TIMER1);
    MENU_ADD_ITEM(STR_RESET_TIMER2);
    MENU_ADD_ITEM(STR_RESET_TIMER3);
#if defined(FRSKY)
    MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
  }
#endif
#if defined(FRSKY)
  else if (result == STR_RESET_TELEMETRY) {
    telemetryReset();
  }
#endif
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
#if defined(CPUARM)
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
#endif
}
#endif

void menuMainView(uint8_t event)
{
  STICK_SCROLL_DISABLE();

  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  switch(event) {

    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

    /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
    case EVT_KEY_BREAK(KEY_MENU):
      if (view_base == VIEW_TIMER2) {
        Timer2_running = !Timer2_running;
        AUDIO_KEYPAD_UP();
      }
    break;
    */

    case EVT_KEY_BREAK(KEY_RIGHT):
    case EVT_KEY_BREAK(KEY_LEFT):
      if (view_base <= VIEW_INPUTS) {
#if defined(PCBSKY9X)
        if (view_base == VIEW_INPUTS)
          g_eeGeneral.view ^= ALTERNATE_VIEW;
        else
          g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) + ((event==EVT_KEY_BREAK(KEY_LEFT)) ? -ALTERNATE_VIEW : ALTERNATE_VIEW)) % (4*ALTERNATE_VIEW);
#else
        g_eeGeneral.view ^= ALTERNATE_VIEW;
#endif
        eeDirty(EE_GENERAL);
        AUDIO_KEYPAD_UP();
      }
      break;

#if defined(NAVIGATION_MENUS)
    case EVT_KEY_CONTEXT_MENU:
      killEvents(event);

#if defined(CPUARM)
      if (modelHasNotes()) {
        MENU_ADD_ITEM(STR_VIEW_NOTES);
      }
#endif

#if defined(CPUARM)
      MENU_ADD_ITEM(STR_RESET_SUBMENU);
#else
      MENU_ADD_ITEM(STR_RESET_TIMER1);
      MENU_ADD_ITEM(STR_RESET_TIMER2);
#if defined(FRSKY)
      MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
      MENU_ADD_ITEM(STR_RESET_FLIGHT);
#endif

      MENU_ADD_ITEM(STR_STATISTICS);
#if defined(CPUARM)
      MENU_ADD_ITEM(STR_ABOUT_US);
#endif
      menuHandler = onMainViewMenu;
      break;
#endif

#if MENUS_LOCK != 2/*no menus*/
    case EVT_KEY_LONG(KEY_MENU):// go to last menu
      pushMenu(lastPopMenu());
      killEvents(event);
      break;

    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_MODEL_MENU:
      pushMenu(menuModelSelect);
      killEvents(event);
      break;

    CASE_EVT_ROTARY_LONG
    case EVT_KEY_GENERAL_MENU:
      pushMenu(menuGeneralSetup);
      killEvents(event);
      break;
#endif

    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_DOWN):
      g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;

    case EVT_KEY_STATISTICS:
      chainMenu(menuStatisticsView);
      killEvents(event);
      break;

    case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
      if (!IS_FAI_ENABLED())
        chainMenu(menuTelemetryFrsky);
#elif defined(JETI)
      JETI_EnableRXD(); // enable JETI-Telemetry reception
      chainMenu(menuTelemetryJeti);
#elif defined(ARDUPILOT)
      ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
      chainMenu(menuTelemetryArduPilot);
#elif defined(NMEA)
      NMEA_EnableRXD(); // enable NMEA-Telemetry reception
      chainMenu(menuTelemetryNMEA);
#elif defined(MAVLINK)
      chainMenu(menuTelemetryMavlink);
#else
      chainMenu(menuStatisticsDebug);
#endif
      killEvents(event);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
#if defined(GVARS) && !defined(PCBSTD)
      if (s_gvar_timer > 0) {
        s_gvar_timer = 0;
      }
#endif
      if (view == VIEW_TIMER2) {
        timerReset(1);
      }
      AUDIO_KEYPAD_UP();
      break;

#if !defined(NAVIGATION_MENUS)
    case EVT_KEY_LONG(KEY_EXIT):
      flightReset();
      AUDIO_KEYPAD_UP();
      break;
#endif
  }

  {
    // Flight Phase Name
    uint8_t phase = mixerCurrentFlightMode;
    lcd_putsnAtt(PHASE_X, PHASE_Y, g_model.flightModeData[phase].name, sizeof(g_model.flightModeData[phase].name), ZCHAR|PHASE_FLAGS);

    // Model Name
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.header.name, g_eeGeneral.currModel, BIGSIZE);

    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Timers
    displayTimers();

    // Trims sliders
    displayTrims(phase);
  }

  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcd_hlineStip(38, 34, 54, DOTTED);
#if defined(PCBSKY9X)
    lcd_hline(38 + (g_eeGeneral.view / ALTERNATE_VIEW) * 13, 34, 13, SOLID);
#else
    lcd_hline((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);
#endif

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
#if defined(PCBSKY9X)
      uint8_t chan = 8*(g_eeGeneral.view / ALTERNATE_VIEW) + i;
#else
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;
#endif

      int16_t val = channelOutputs[chan];

      switch(view_base)
      {
        case VIEW_OUTPUTS_VALUES:
          x0 = (i%4*9+3)*FW/2;
          y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
          lcd_outdezAtt(x0+4*FW , y0, PPM_CH_CENTER(chan)+val/2, 0);
#elif defined(PPM_UNIT_PERCENT_PREC1)
          lcd_outdezAtt(x0+4*FW , y0, calcRESXto1000(val), PREC1);
#else
          lcd_outdezAtt(x0+4*FW , y0, calcRESXto1000(val)/10, 0); // G: Don't like the decimal part*
#endif
          break;

        case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
          x0       = i<4 ? LCD_W/4+2 : LCD_W*3/4-2;
          y0       = 38+(i%4)*5;

          uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
          int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

          if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
          lcd_hlineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
          lcd_vline(x0,y0-2,5);
          if (val>0)
            x0+=1;
          else
            x0-=len;
          lcd_hline(x0,y0+1,len);
          lcd_hline(x0,y0-1,len);
          break;
      }
    }
  }
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
      for (uint8_t i=SWSRC_THR; i<=SWSRC_TRN; i++) {
        int8_t sw = (i == SWSRC_TRN ? (switchState(SW_ID0) ? SWSRC_ID0 : (switchState(SW_ID1) ? SWSRC_ID1 : SWSRC_ID2)) : i);
        uint8_t x = 2*FW-2, y = i*FH+1;
        if (i>=SWSRC_AIL) {
          x = 17*FW-1;
          y -= 3*FH;
        }
        putsSwitches(x, y, sw, getSwitch(i) ? INVERS : 0);
      }
    }
    else {
#if defined(PCBMEGA2560) && defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if ROTARY_ENCODERS > 2
#define V_BAR_W 5
        V_BAR(LCD_W/2-8+V_BAR_W*i, LCD_H-8, len)
#else
#define V_BAR_W 5
        V_BAR(LCD_W/2-3+V_BAR_W*i, LCD_H-8, len)
#endif
      }
#endif // PCBGRUVIN9X && ROTARY_ENCODERS

      // Logical Switches
#if defined(PCBSKY9X)
      for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
        int8_t len = getSwitch(SWSRC_SW1+i) ? BAR_HEIGHT : 1;
        uint8_t x = VSWITCH_X(i);
        lcd_vline(x-1, VSWITCH_Y-len, len);
        lcd_vline(x,   VSWITCH_Y-len, len);
      }
#elif defined(CPUM2560)
      for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++)
        putsSwitches(2*FW-3 + (i/3)*(i/3>2 ? 3*FW+2 : (3*FW-1)) + (i/3>2 ? 2*FW : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#elif !defined(PCBSTD)
      for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW-1), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#else
      for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++)
        putsSwitches(2*FW-3 + (i/3)*(4*FW), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#endif
    }
  }
  else { // timer2
#if defined(TRANSLATIONS_CZ)
  #define TMR2_LBL_COL (20-FW/2+1)
#else
  #define TMR2_LBL_COL (20-FW/2+5)
#endif
    putsTimer(33+FW+2+10*FWNUM-4, FH*5, timersStates[1].val, DBLSIZE, DBLSIZE);
    putsTimerMode(timersStates[1].val >= 0 ? TMR2_LBL_COL : TMR2_LBL_COL-7, FH*6, g_model.timers[1].mode);
    // lcd_outdezNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }

  // And ! in case of unexpected shutdown
  if (unexpectedShutdown) {
    lcd_putcAtt(REBOOT_X, 0*FH, '!', INVERS);
  }

#if defined(GVARS) && !defined(PCBSTD)
  if (s_gvar_timer > 0) {
    s_gvar_timer--;
    s_warning = STR_GLOBAL_VAR;
    displayBox();
    lcd_putsnAtt(16, 5*FH, g_model.gvars[s_gvar_last].name, LEN_GVAR_NAME, ZCHAR);
    lcd_putsAtt(16+7*FW, 5*FH, PSTR("[\010]"), BOLD);
    lcd_outdezAtt(16+7*FW+4*FW+FW/2, 5*FH, GVAR_VALUE(s_gvar_last, getGVarFlightPhase(mixerCurrentFlightMode, s_gvar_last)), BOLD);
    s_warning = NULL;
  }
#endif

#if defined(DSM2)
  if (moduleFlag[0] == MODULE_BIND) {
    // Issue 98
    lcd_putsAtt(15*FW, 0, PSTR("BIND"), 0);
  }
#endif
}
