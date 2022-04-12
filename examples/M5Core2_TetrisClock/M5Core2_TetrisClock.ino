/******************************************************************\
 *  Displaying RTC Time on the M5Core2 display
 *
 *   Written by tobozo
 *
 *   Hardware Requirement:
 *     - M5Stack Core2
 *
 *   Library Requirements:
 *     - https://github.com/lovyan03/LovyanGFX
 *     - https://github.com/tobozo/ESP32-Chimera-Core
\******************************************************************/

#include <ESP32-Chimera-Core.h> //
#include <Ticker.h>
#include <TetrisMatrixDraw.h>


Ticker timer_ticker;

static LGFX &tft(M5.Lcd);
static LGFX_Sprite *tetrisSprite = new LGFX_Sprite(&tft);
static LGFX_Sprite *gridSprite   = new LGFX_Sprite(&tft);

TetrisMatrixDraw *tetris;
static bool finished_animating;

bool showColon = true;
String displayedTime;
String currentTime;


void number_updater()
{
  tetrisSprite->fillSprite(TFT_BLACK);
  finished_animating = tetris->drawNumbers( 0, tetrisSprite->height(), showColon);
  gridSprite->pushSprite( tetrisSprite, 0, 0, 1 );
  tetrisSprite->pushRotateZoom( &tft, 160, 120, 0.0, 2.5, 2.5 ); // zoom 2.5 at screen center
}


void getTimeStr( String *out )
{
  RTC_TimeTypeDef RTCtime;
  M5.Rtc.GetTime( &RTCtime );
  char timeStr[16] = {0};
  snprintf( timeStr, 16, "%02d:%02d:%02d", RTCtime.Hours, RTCtime.Minutes, RTCtime.Seconds );
  *out = String( timeStr );
}


void setup()
{
  M5.begin();

  uint8_t scale = 4; // tetris scale
  uint16_t canvas_width = 128; // will be updated
  uint16_t canvas_height = 80; // fixed

  tetrisSprite->setColorDepth(8);
  if( ! tetrisSprite->createSprite( canvas_width, canvas_height ) ) {
    Serial.println("Cannot create sprite, halting");
    while(1);
  }

  gridSprite->setColorDepth(1);
  if( !gridSprite->createSprite( canvas_width, canvas_height ) ) {
    Serial.println("Cannot create sprite, halting");
    while(1);
  }

  gridSprite->fillSprite( 1 );

  for( int i=0; i<gridSprite->height();i++ ) {
    if( i%scale == 0 ) gridSprite->drawFastHLine( 0, i, gridSprite->width(), 0 );
  }

  for( int i=0; i<gridSprite->width();i++ ) {
    if( i%scale == 0 ) gridSprite->drawFastVLine( i, 0, gridSprite->height(), 0 );
  }

  // re-cast lgfx sprite as ref
  static LGFX_Sprite &canvas( *tetrisSprite );
  tetris = new TetrisMatrixDraw(canvas);
  tetris->scale = scale;

  getTimeStr( &currentTime );
  tetris->setTime( currentTime, true );

  delay(2000);
  timer_ticker.attach(0.05, number_updater);
  delay(5000);

}


void loop()
{
  getTimeStr( &currentTime );

  // Time has changed
  if (displayedTime != currentTime)
  {
    tetris->setTime(currentTime);
    displayedTime = currentTime;
    showColon = (currentTime.substring(6, 8).toInt() % 2 == 1);
  }
}
