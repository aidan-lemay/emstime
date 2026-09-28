#include <pebble.h>

static Window *s_window;
static TextLayer *s_layer_24h, *s_layer_dateLong, *s_layer_seconds, *s_layer_dateShort, *s_layer_12h;

static GColor s_fg_color = GColorWhite;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char b24[10], bLong[32], bSec[10], bShort[16], b12[10];
  
  strftime(b24, sizeof(b24), "%H:%M", tick_time);
  strftime(bLong, sizeof(bLong), "%A, %B %d", tick_time);
  strftime(bSec, sizeof(bSec), "%S", tick_time);
  strftime(bShort, sizeof(bShort), "%m/%d/%Y", tick_time);
  strftime(b12, sizeof(b12), "%I:%M %p", tick_time);

  text_layer_set_text(s_layer_24h, b24);
  text_layer_set_text(s_layer_dateLong, bLong);
  text_layer_set_text(s_layer_seconds, bSec);
  text_layer_set_text(s_layer_dateShort, bShort);
  text_layer_set_text(s_layer_12h, b12);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  window_set_background_color(window, GColorBlack);

  int time_h = 32;   // For GOTHIC_24_BOLD (or 36 for GOTHIC_28_BOLD)
  int date_h = 24;   // For GOTHIC_18_BOLD
  int sec_h = 48;    // For BITHAM_42_BOLD center seconds display
  
  int total_content_height = (time_h * 2) + (date_h * 2) + sec_h;
  int spacing = (bounds.size.h - total_content_height) / 4;

  int y0 = 0;
  int y1 = y0 + time_h + spacing;
  int y2 = y1 + date_h + spacing;
  int y3 = y2 + sec_h + spacing;
  int y4 = bounds.size.h - time_h;

  // Initialize the four layers
  s_layer_24h = text_layer_create(GRect(0, y0, bounds.size.w, time_h));
  s_layer_dateLong = text_layer_create(GRect(0, y1, bounds.size.w, date_h));
  s_layer_seconds = text_layer_create(GRect(0, y2, bounds.size.w, sec_h));
  s_layer_dateShort = text_layer_create(GRect(0, y3, bounds.size.w, date_h));
  s_layer_12h = text_layer_create(GRect(0, y4, bounds.size.w, time_h));

  // Configure appearance
  TextLayer *layers[] = {s_layer_24h, s_layer_dateLong, s_layer_seconds, s_layer_dateShort, s_layer_12h};

  for (int i = 0; i < 5; i++) {
    text_layer_set_text_alignment(layers[i], GTextAlignmentCenter);
    text_layer_set_background_color(layers[i], GColorClear);
    text_layer_set_text_color(layers[i], s_fg_color);
    layer_add_child(window_layer, text_layer_get_layer(layers[i]));
  }

  text_layer_set_font(s_layer_24h, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_font(s_layer_dateLong, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_font(s_layer_seconds, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(s_layer_dateShort, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_font(s_layer_12h, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  update_time();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_layer_24h);
  text_layer_destroy(s_layer_dateLong);
  text_layer_destroy(s_layer_seconds);
  text_layer_destroy(s_layer_dateShort);
  text_layer_destroy(s_layer_12h);
}

static void prv_init(void) {
  s_window = window_create();

  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  window_stack_push(s_window, true);
}

static void prv_deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}