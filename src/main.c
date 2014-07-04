#include <pebble.h>
  
Window *window;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *dow_layer;
TextLayer *battery_layer;
TextLayer *bt_layer;
InverterLayer *inv_layer;

char time_buffer[] = "00:00";
char date_buffer[] = "00 September";
char long_dow_buffer[] = "000";
char dow_buffer[] = "00";

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  //Here we will update the watchface display
  //Format the buffer string using tick_time as the time source
  strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  strftime(date_buffer, sizeof("00 September"), "%d %B", tick_time);
  strftime(long_dow_buffer, sizeof("000"), "%a", tick_time);
  strncpy(dow_buffer, long_dow_buffer, 2);

  //Change the TextLayer text to show the new time!
  text_layer_set_text(time_layer, time_buffer);
  text_layer_set_text(date_layer, date_buffer);
  text_layer_set_text(dow_layer, dow_buffer);
}


static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";
  snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  text_layer_set_text(battery_layer, battery_text);
 }

void handle_bluetooth(bool connected) {
  text_layer_set_text(bt_layer, connected ? "BT" : "");
  vibes_double_pulse();
}

void add_time_layer(ResHandle time_font) {
  time_layer = text_layer_create(GRect(0, 53, 144, 62));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorBlack);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_load_custom_font(time_font));
  layer_add_child(window_get_root_layer(window), (Layer*) time_layer);
}

void add_date_layer(ResHandle text_font) {
  date_layer = text_layer_create(GRect(30, 148, 114, 20));
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorBlack);
  text_layer_set_text_alignment(date_layer, GTextAlignmentRight);
  text_layer_set_font(date_layer, fonts_load_custom_font(text_font));
  layer_add_child(window_get_root_layer(window), (Layer*) date_layer);
}

void add_dow_layer(ResHandle text_font) {
  dow_layer = text_layer_create(GRect(0, 148, 30, 20));
  text_layer_set_background_color(dow_layer, GColorClear);
  text_layer_set_text_color(dow_layer, GColorBlack);
  text_layer_set_text_alignment(dow_layer, GTextAlignmentLeft);
  text_layer_set_font(dow_layer, fonts_load_custom_font(text_font));
  layer_add_child(window_get_root_layer(window), (Layer*) dow_layer);
}

void add_battery_layer(ResHandle text_font) {
  battery_layer = text_layer_create(GRect (72, 0, 72, 20));
  text_layer_set_background_color(battery_layer, GColorClear);
  text_layer_set_text_color(battery_layer, GColorBlack);
  text_layer_set_text_alignment(battery_layer, GTextAlignmentRight);
  text_layer_set_font(battery_layer, fonts_load_custom_font(text_font));
  BatteryChargeState charge_state = battery_state_service_peek();
  static char battery_text[] = "100%";
  snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  text_layer_set_text(battery_layer, battery_text);
  layer_add_child(window_get_root_layer(window), (Layer*) battery_layer);
}

void add_bluetooth_layer(ResHandle text_font) {
  bt_layer = text_layer_create(GRect (0, 0, 72, 20));
  text_layer_set_background_color(bt_layer, GColorClear);
  text_layer_set_text_color(bt_layer, GColorBlack);
  text_layer_set_text_alignment(bt_layer, GTextAlignmentLeft);
  text_layer_set_font(bt_layer, fonts_load_custom_font(text_font));
  text_layer_set_text(bt_layer, (bluetooth_connection_service_peek() ? "BT" : ""));
  layer_add_child(window_get_root_layer(window), (Layer*) bt_layer);
}

void window_load(Window *window)
{
  ResHandle time_font = resource_get_handle(RESOURCE_ID_FONT_FUTURA_50);
  ResHandle text_font = resource_get_handle(RESOURCE_ID_FONT_UBUNTU_18);
  add_time_layer(time_font);
  add_date_layer(text_font);
  add_dow_layer(text_font);
  add_bluetooth_layer(text_font);
  add_battery_layer(text_font);
  
  inv_layer = inverter_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);
  

  //Manually call the tick handler when the window is loading
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);
  tick_handler(t, MINUTE_UNIT);
}

void window_unload(Window *window)
{
  //We will safely destroy the Window's elements here!
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(dow_layer);
  text_layer_destroy(battery_layer);
  text_layer_destroy(bt_layer);
  inverter_layer_destroy(inv_layer);
}

void init()
{
  //Initialize the app elements here!
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  window_stack_push(window, true);
}
 
void deinit()
{
  //De-initialize elements here to save memory!
  window_destroy(window);
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}

