#include <pebble.h>
#include "main.h"
  
static Window *s_main_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minute_layer;
static TextLayer *s_day_layer;
static TextLayer *s_month_layer;
static TextLayer *s_conditions_layer;
static TextLayer *s_temp_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_description_layer;

static GFont s_time_font;
static GFont s_temp_font;
static GFont s_weekday_font;
static GFont s_conditions_font;
static GFont s_date_font;

static BitmapLayer *s_field1_layer;
static BitmapLayer *s_field2_layer;
static BitmapLayer *s_field3_layer;
static BitmapLayer *s_field4_layer;
static BitmapLayer *s_background_black_layer;
static GBitmap *s_field1_bitmap;
static GBitmap *s_field2_bitmap;
static GBitmap *s_field3_bitmap;
static GBitmap *s_field4_bitmap;
static GBitmap *s_background_black_bitmap;

const char* lang;
const char* temp_mode;


static char hourbuffer[5];
static char minutebuffer[5];
static char daybuffer[5];
static char monthbuffer[5];
static char weekdaybuffer[16];

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);


  
lang = i18n_get_system_locale();  
setlocale(LC_ALL, lang);

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h format
    strftime(hourbuffer, sizeof(hourbuffer), "%H", tick_time);
    strftime(minutebuffer, sizeof(minutebuffer), "%M", tick_time);
    strftime(daybuffer, sizeof(daybuffer), "%d", tick_time);
    strftime(monthbuffer, sizeof(monthbuffer), "%m", tick_time);
    strftime(weekdaybuffer, sizeof(weekdaybuffer), "%A", tick_time);
  } else {
    //Use 12h format
    strftime(hourbuffer, sizeof(hourbuffer), "%I", tick_time);
    strftime(minutebuffer, sizeof(minutebuffer), "%M", tick_time);
    strftime(daybuffer, sizeof(daybuffer), "%d", tick_time);
    strftime(monthbuffer, sizeof(monthbuffer), "%m", tick_time);
    strftime(weekdaybuffer, sizeof(weekdaybuffer), "%A", tick_time);
  }
  
  // Display time on TimeTextLayer
  text_layer_set_text(s_hour_layer, hourbuffer);
  text_layer_set_text(s_minute_layer, minutebuffer);
  // Display date on DateTextLayer
  text_layer_set_text(s_day_layer, daybuffer);
  text_layer_set_text(s_month_layer, monthbuffer);
  text_layer_set_text(s_weekday_layer, weekdaybuffer);
  
}

static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  BatteryChargeState charge_state = battery_state_service_peek();
  if (charge_state.is_charging) {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), ">%d%%", new_state.charge_percent);
    text_layer_set_text(s_battery_layer, s_battery_buffer);
  } else {
    if (charge_state.is_plugged ) {
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), ">|%d%%", new_state.charge_percent);
      text_layer_set_text(s_battery_layer, s_battery_buffer);
    } else {
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", new_state.charge_percent);
      text_layer_set_text(s_battery_layer, s_battery_buffer);
    }
  }
}

static void main_window_load(Window *window) { 
    
  //Create BackgroundColor
  s_background_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_BLACK);
  s_background_black_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_black_layer, s_background_black_bitmap);
  bitmap_layer_set_background_color(s_background_black_layer, GColorBlack);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_black_layer));
 
  //Create field1
  s_field1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);
  s_field1_layer = bitmap_layer_create(GRect(2, 22, 68, 68));
  bitmap_layer_set_bitmap(s_field1_layer, s_field1_bitmap);
  bitmap_layer_set_background_color(s_field1_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_field1_layer));
  s_hour_layer = text_layer_create(GRect(2, 26, 68, 68));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorBlack);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  text_layer_set_text(s_hour_layer, "00");
  
  //Create field2
  s_field2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);
  s_field2_layer = bitmap_layer_create(GRect(74, 22, 68, 68));
  bitmap_layer_set_bitmap(s_field2_layer, s_field2_bitmap);
  bitmap_layer_set_background_color(s_field2_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_field2_layer));
  s_minute_layer = text_layer_create(GRect(74, 26, 68, 68));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorBlack);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
  text_layer_set_text(s_minute_layer, "00");
  
  //Create field3
  s_field3_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);
  s_field3_layer = bitmap_layer_create(GRect(2, 95, 68, 68));
  bitmap_layer_set_bitmap(s_field3_layer, s_field3_bitmap);
  bitmap_layer_set_background_color(s_field3_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_field3_layer));
  //Create Conditions Layer
  s_conditions_layer = text_layer_create(GRect(2, 95, 68, 68));
  text_layer_set_background_color(s_conditions_layer, GColorClear);
  text_layer_set_text_color(s_conditions_layer, GColorBlack);
  text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentCenter);
  text_layer_set_text(s_conditions_layer, "G");
  s_temp_layer = text_layer_create(GRect(2, 132, 68, 68));
  text_layer_set_background_color(s_temp_layer, GColorClear);
  text_layer_set_text_color(s_temp_layer, GColorBlack);
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_temp_layer, "...");
    
  //Create field4
  s_field4_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SQUARE);
  s_field4_layer = bitmap_layer_create(GRect(74, 95, 68, 68));
  bitmap_layer_set_bitmap(s_field4_layer, s_field4_bitmap);
  bitmap_layer_set_background_color(s_field4_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_field4_layer));
  s_day_layer = text_layer_create(GRect(74, 88, 68, 34));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorBlack);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  text_layer_set_text(s_day_layer, "00");
  s_weekday_layer = text_layer_create(GRect(74, 119, 68, 24));
  text_layer_set_background_color(s_weekday_layer, GColorClear);
  text_layer_set_text_color(s_weekday_layer, GColorBlack);
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weekday_layer, " ");
  s_month_layer = text_layer_create(GRect(74, 126, 68, 34));
  text_layer_set_background_color(s_month_layer, GColorClear);
  text_layer_set_text_color(s_month_layer, GColorBlack);
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_text(s_month_layer, "00");

  
  //Create Statusbar
  s_battery_layer = text_layer_create(GRect(0, 2, 144, 20));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  text_layer_set_text(s_battery_layer, "??%");
  // Conditions description
  s_description_layer = text_layer_create(GRect(2, 2, 130, 20));
  text_layer_set_background_color(s_description_layer, GColorClear);
  text_layer_set_text_color(s_description_layer, GColorWhite);
  text_layer_set_text_alignment(s_description_layer, GTextAlignmentLeft);
  text_layer_set_text(s_description_layer, "...");
  
  //Create GFont
  s_time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49); 
  s_weekday_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_date_font = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  s_conditions_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_METEO_38));
  s_temp_font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  
  //Apply to TextLayer
  text_layer_set_font(s_hour_layer, s_time_font);
  text_layer_set_font(s_minute_layer, s_time_font);
  text_layer_set_font(s_day_layer, s_date_font);
  text_layer_set_font(s_month_layer, s_date_font);  
  text_layer_set_font(s_weekday_layer, s_weekday_font);
  text_layer_set_font(s_battery_layer, s_weekday_font);
  text_layer_set_font(s_conditions_layer, s_conditions_font);
  text_layer_set_font(s_temp_layer, s_temp_font);
  text_layer_set_font(s_description_layer, s_weekday_font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_description_layer, GTextAlignmentLeft);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_conditions_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temp_layer)); 
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_description_layer)); 
  
  // Make sure the time is displayed from the start
  update_time();
  battery_handler(battery_state_service_peek());
  bool keytempmode = false; 
  keytempmode = persist_exists(KEY_TEMP_MODE) ? persist_read_bool(KEY_TEMP_MODE) : false;
  int test=2;
  if(keytempmode==true){test=1;}else{test=0;};
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Persistent read: %s", test == 1 ? "true" : "false");
  if (keytempmode){temp_mode="F";}else{temp_mode="C";};
  APP_LOG(APP_LOG_LEVEL_DEBUG, "temp_mode read persistent if: %s", temp_mode);
  temp_mode = keytempmode ? "F" : "C";
  APP_LOG(APP_LOG_LEVEL_DEBUG, "temp_mode read persistent if: %s", temp_mode);
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_conditions_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_field1_bitmap);
  gbitmap_destroy(s_field2_bitmap);
  gbitmap_destroy(s_field3_bitmap);
  gbitmap_destroy(s_field4_bitmap);
  
  //Destroy BitmapLayer
  bitmap_layer_destroy(s_field1_layer);
  bitmap_layer_destroy(s_field2_layer);
  bitmap_layer_destroy(s_field3_layer);
  bitmap_layer_destroy(s_field4_layer);
  
  // Destroy TimeLayers
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minute_layer);
  
  // Destroy DateLayer
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_weekday_layer);
  // Destroy BatteryLayer
  text_layer_destroy(s_battery_layer);
  
  // Destroy weather elements
  text_layer_destroy(s_temp_layer);
  text_layer_destroy(s_conditions_layer);
  
  battery_state_service_unsubscribe();
  
  // Destroy persistent
  //persist_delete(KEY_TEMP_MODE);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  //static char weather_layer_buffer[32];
  static char conditions_id_buffer[3];
  static char* night;
  
  if (strcmp(hourbuffer, "18") == 0){night="y";}else{
  if (strcmp(hourbuffer, "19") == 0){night="y";}else{
  if (strcmp(hourbuffer, "20") == 0){night="y";}else{
  if (strcmp(hourbuffer, "21") == 0){night="y";}else{
  if (strcmp(hourbuffer, "22") == 0){night="y";}else{
  if (strcmp(hourbuffer, "23") == 0){night="y";}else{
  if (strcmp(hourbuffer, "00") == 0){night="y";}else{
  if (strcmp(hourbuffer, "01") == 0){night="y";}else{
  if (strcmp(hourbuffer, "02") == 0){night="y";}else{
  if (strcmp(hourbuffer, "03") == 0){night="y";}else{
  if (strcmp(hourbuffer, "04") == 0){night="y";}else{
  if (strcmp(hourbuffer, "05") == 0){night="y";}else{
  if (strcmp(hourbuffer, "06") == 0){night="y";}else{
  night="n";}}}}}}}}}}}}}
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Nightmodus: %s", night);
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
  int temp_c=0;
  int temp_f=0;
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE: 
        temp_c = (int)t->value->int32;
      break;
    case KEY_CONDITIONS: 
        snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    case KEY_CONDITIONS_ID:
      if (strcmp(night, "y")==0){  
        if (strcmp(conditions_id_buffer,"B") == 0){
          snprintf(conditions_id_buffer, sizeof(conditions_id_buffer), "%s", "C");
        }else{
          if (strcmp(conditions_id_buffer,"H") == 0){
            snprintf(conditions_id_buffer, sizeof(conditions_id_buffer), "%s", "I");
          }else{
            if (strcmp(conditions_id_buffer,"J") == 0){
              snprintf(conditions_id_buffer, sizeof(conditions_id_buffer), "%s", "K");
            }else{
          snprintf(conditions_id_buffer, sizeof(conditions_id_buffer), "%s", t->value->cstring);
            }}}}
        else{
          snprintf(conditions_id_buffer, sizeof(conditions_id_buffer), "%s", t->value->cstring);
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "ConditionsID(18-06: B=C, H=I, J=K): %s", conditions_id_buffer);
      break;
    case KEY_FARENHEIT:
        temp_f = (int)t->value->int32;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Fahrenheit (temp_f): %d", temp_f);
    case KEY_TEMP_MODE:
        temp_mode = t->value->cstring;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "temp_mode SDK: %s", temp_mode);
        if(strcmp(temp_mode, "F") == 0)
        {
          //FARENHEIT
          persist_write_bool(KEY_TEMP_MODE, true);
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Fahrenheit=true: %d°F", temp_f);
        }
        else if(strcmp(temp_mode, "C") == 0)
        {
          //CELSIUS
          persist_write_bool(KEY_TEMP_MODE, false);
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Fahrenheit=false: %d°C", temp_c);
        }

      bool keytempmode = persist_exists(KEY_TEMP_MODE) ? persist_read_bool(KEY_TEMP_MODE) : false;
      int test=2;
      if(keytempmode==true){test=1;}else{test=0;};
      
        APP_LOG(APP_LOG_LEVEL_DEBUG, "persist bool (1=F, 0=C): %d", test);
        if(keytempmode==true){
          snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°F", temp_f);
        }else{
          if(keytempmode==false){
            snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", temp_c);
          }
    }
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  text_layer_set_text(s_conditions_layer, conditions_id_buffer);
  text_layer_set_text(s_temp_layer, temperature_buffer);
  text_layer_set_text(s_description_layer, conditions_buffer);
}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Subscribe to the Battery State Service
  battery_state_service_subscribe(battery_handler);  
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

