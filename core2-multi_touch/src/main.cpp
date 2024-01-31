#include <Arduino.h>
#include <M5Core2.h>

#define ENABLE_SERIAL 1

char *message = (char *)malloc(32);
uint32_t message_limit = 0;
uint32_t screen_color = BLACK;

Button screen(0, 0, 320, 280, "touch");

#define TOUCH_SENSITIVITY_MS 200

bool touched = false;
bool second_touched = false;
uint32_t touch_started_at = 0;
bool moving = false;
int16_t prev_x = 0;
int16_t prev_y = 0;

void touch_handler(Event &e)
{
  Button &b = *e.button;
  unsigned long now = millis();
#if ENABLE_SERIAL
  Serial.printf("t:%12d f:%d e:%4x x:%4d y:%4d\n", now, e.finger, e.type, e.to.x, e.to.y);
#endif

  if (e.type == E_TOUCH && e.finger == 0)
  {
    touched = true;
    touch_started_at = now;
  }
  if (e.type == E_TOUCH && e.finger == 1)
  {
    second_touched = true;
  }
  if (e.type == E_MOVE && e.finger == 0)
  {
    moving = true;
    if (second_touched)
    {
      sprintf(message, "scroll x:%3d y:%3d", e.to.x - prev_x, e.to.y - prev_y);
    }
    else
    {
      sprintf(message, "move x:%3d y:%3d", e.to.x - prev_x, e.to.y - prev_y);
    }
    message_limit = now + 500;
  }

  if (e.type == E_RELEASE && e.finger == 0)
  {
    if (!moving)
    {
      if (now < touch_started_at + TOUCH_SENSITIVITY_MS)
      {
        if (second_touched)
        {
          sprintf(message, "right click");
#if ENABLE_SERIAL
          Serial.printf("@@right click\n");
#endif
        }
        else
        {
          sprintf(message, "left click");
#if ENABLE_SERIAL
          Serial.printf("@@left click\n");
#endif
        }
        message_limit = now + 500;
      }
    }

    touched = false;
    second_touched = false;
    moving = false;
  }

  prev_x = e.to.x;
  prev_y = e.to.y;

  if (moving && second_touched)
  {
    screen_color = NAVY;
  }
  else if (moving)
  {
    screen_color = PURPLE;
  }
  else if (second_touched)
  {
    screen_color = DARKGREEN;
  }
  else if (touched)
  {
    screen_color = DARKCYAN;
  }
  else
  {
    screen_color = BLACK;
  }
}

void setup()
{
  M5.begin();
  sprintf(message, "");
  M5.Buttons.addHandler(touch_handler, E_ALL);
}

uint32_t prev_color = 0;
uint32_t prev_message_limit = 0;
bool message_displayed = false;

void describe_screen()
{
  unsigned long now = millis();

  bool need_update = false;

  if (prev_color != screen_color)
  {
    need_update = true;
  }

  if (prev_message_limit != message_limit)
  {
    need_update = true;
  }
  if (message_displayed && now > message_limit)
  {
    need_update = true;
  }

  prev_color = screen_color;
  prev_message_limit = message_limit;

  if (!need_update)
  {
    return;
  }

  M5.Lcd.fillScreen(screen_color);

  if (now < message_limit)
  {
    message_displayed = true;

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString(message, 160, 120, 2);
    M5.Lcd.print(M5.Lcd.getTextDatum());
    M5.Lcd.setTextColor(WHITE);
  }
  else
  {
    message_displayed = false;
  }
}

void loop()
{
  M5.update();
  describe_screen();
}