#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_date_layer;
static TextLayer *s_time_layer;
static TextLayer *s_unixtime_title_layer;
static TextLayer *s_unixtime_layer;
static TextLayer *s_step_title_layer;
static TextLayer *s_step_layer;

/* ----------------
  TickTimerService
---------------- */
static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // 現在時刻
  static char s_date_buffer[32];
  strftime(s_date_buffer, sizeof(s_date_buffer), "%Y-%m-%d %A", tick_time);
  text_layer_set_text(s_date_layer, s_date_buffer);
  static char s_time_buffer[16];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M:%S" : "%I:%M:%S", tick_time);
  text_layer_set_text(s_time_layer, s_time_buffer);

  // UNIXTIME
  static char s_unixtime_buffer[16];
  snprintf(s_unixtime_buffer, sizeof(s_unixtime_buffer), "%d", (int)temp);
  text_layer_set_text(s_unixtime_layer, s_unixtime_buffer);

  // 歩数
  static char s_step_buffer[16];
  snprintf(s_step_buffer, sizeof(s_step_buffer), "%d", (int)health_service_sum_today(HealthMetricStepCount));
  text_layer_set_text(s_step_layer, s_step_buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

/* ----------------
  window load/unload
---------------- */
static void main_window_load(Window *window) {
  // ウィンドウ情報取得
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // 現在時刻テキストレイヤー作成
  s_date_layer = text_layer_create(GRect(0, 0, bounds.size.w, 22));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(      s_date_layer, GColorBlack);
  text_layer_set_font(            s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  s_time_layer = text_layer_create(GRect(0, 10, bounds.size.w, 36));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(      s_time_layer, GColorBlack);
  text_layer_set_font(            s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // UNIXTIME
  s_unixtime_title_layer = text_layer_create(GRect(0, 40, bounds.size.w, 22));
  text_layer_set_background_color(s_unixtime_title_layer, GColorClear);
  text_layer_set_text_color(      s_unixtime_title_layer, GColorBlack);
  text_layer_set_font(            s_unixtime_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(            s_unixtime_title_layer, "UNIXTIME:");
  layer_add_child(window_layer, text_layer_get_layer(s_unixtime_title_layer));

  s_unixtime_layer = text_layer_create(GRect(0, 50, bounds.size.w, 36));
  text_layer_set_background_color(s_unixtime_layer, GColorClear);
  text_layer_set_text_color(      s_unixtime_layer, GColorBlack);
  text_layer_set_font(            s_unixtime_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(s_unixtime_layer));

  // 歩数
  s_step_title_layer = text_layer_create(GRect(0, 80, bounds.size.w, 22));
  text_layer_set_background_color(s_step_title_layer, GColorClear);
  text_layer_set_text_color(      s_step_title_layer, GColorBlack);
  text_layer_set_font(            s_step_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(            s_step_title_layer, "Step Count:");
  layer_add_child(window_layer, text_layer_get_layer(s_step_title_layer));

  s_step_layer = text_layer_create(GRect(0, 90, bounds.size.w, 36));
  text_layer_set_background_color(s_step_layer, GColorClear);
  text_layer_set_text_color(      s_step_layer, GColorBlack);
  text_layer_set_font(            s_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  update_time();
}

/* ----------------
  init/deinit
---------------- */
static void init() {
  // メインウィンドウエレメントのポインタ定義
  s_main_window = window_create();

  // ウィンドウ内の要素用のハンドラ
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load   = main_window_load,
    .unload = main_window_unload
  });
  // TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}
 
/* ----------------
  メインループ
---------------- */
int main(void) {
  init();
  app_event_loop();
  deinit();
}

 