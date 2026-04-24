#include <pebble.h>

static Window *s_window;
static TextLayer *s_layer_24h, *s_layer_dateLong, *s_layer_dateShort, *s_layer_12h;

static GColor s_bg_color = GColorBlack;
static GColor s_fg_color = GColorWhite;

// Buffer to hold our formatted time/date strings
static char s_time_buffer[64];

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char b24[10], bLong[32], bShort[16], b12[10];
  
  strftime(b24, sizeof(b24), "%H:%M", tick_time);
  strftime(bLong, sizeof(bLong), "%A, %B %d", tick_time);
  strftime(bShort, sizeof(bShort), "%m/%d/%Y", tick_time);
  strftime(b12, sizeof(b12), "%I:%M %p", tick_time);

  text_layer_set_text(s_layer_24h, b24);
  text_layer_set_text(s_layer_dateLong, bLong);
  text_layer_set_text(s_layer_dateShort, bShort);
  text_layer_set_text(s_layer_12h, b12);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Define line height (e.g., 30 pixels per line)
  int line_h = 30;
  
  // Calculate spacing: Total Screen Height / 5 gaps for 4 items
  int spacing = (bounds.size.h - (4 * line_h)) / 3;

  // Initialize the four layers
  s_layer_24h = text_layer_create(GRect(0, 0, bounds.size.w, line_h));
  s_layer_dateLong = text_layer_create(GRect(0, line_h + spacing, bounds.size.w, line_h));
  s_layer_dateShort = text_layer_create(GRect(0, (line_h + spacing) * 2, bounds.size.w, line_h));
  s_layer_12h = text_layer_create(GRect(0, bounds.size.h - line_h, bounds.size.w, line_h));

  // Configure appearance
  TextLayer *layers[] = {s_layer_24h, s_layer_dateLong, s_layer_dateShort, s_layer_12h};

  // 24hr Time
  text_layer_set_text_alignment(layers[0], GTextAlignmentCenter);
  text_layer_set_font(layers[0], fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(layers[0]));
  text_layer_set_background_color(layers[0], GColorClear);
  text_layer_set_text_color(layers[0], s_fg_color);

  // Long Date
  text_layer_set_text_alignment(layers[1], GTextAlignmentCenter);
  text_layer_set_font(layers[1], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(layers[1]));
  text_layer_set_background_color(layers[1], GColorClear);
  text_layer_set_text_color(layers[1], s_fg_color);

  // Short Date
  text_layer_set_text_alignment(layers[2], GTextAlignmentCenter);
  text_layer_set_font(layers[2], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(layers[2]));
  text_layer_set_background_color(layers[2], GColorClear);
  text_layer_set_text_color(layers[2], s_fg_color);

  // 12hr Time
  text_layer_set_text_alignment(layers[3], GTextAlignmentCenter);
  text_layer_set_font(layers[3], fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(layers[3]));
  text_layer_set_background_color(layers[3], GColorClear);
  text_layer_set_text_color(layers[3], s_fg_color);

  update_time();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_layer_24h);
  text_layer_destroy(s_layer_dateLong);
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
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
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