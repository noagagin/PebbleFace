#include <pebble.h>

static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static TextLayer *s_time_layer;
static TextLayer *s_wday_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;
static TextLayer *s_daynum_layer;


static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "Full";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "Charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);
}

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
  // Needs to be static because it's used by the system later.
  static char s_time_text[] = "00:00";
  static char s_date_text[] = "                  ";
  static char s_daynum_text[] = "00";

  //strftime(s_time_text, sizeof(s_time_text), clock_is_24h_style()?"%T":"%I:%M %p", tick_time);
    	char *time_format;
		if	 (clock_is_24h_style()) {time_format = "%R";}
		else {time_format = "%I:%M";}

	strftime(s_time_text, sizeof(s_time_text), time_format, tick_time);
	
	if  (!clock_is_24h_style() && (s_time_text[0] =='0'))
		{memmove(s_time_text, &s_time_text[1], sizeof(s_time_text) - 1);}
	
  text_layer_set_text(s_time_layer, s_time_text);
 
  
  //strftime(s_time_text, sizeof(s_time_text), "%T", tick_time);
  //text_layer_set_text(s_time_layer, s_time_text);

  
  strftime( s_date_text, sizeof( s_date_text ), "%b", tick_time );

  text_layer_set_text( s_date_layer, s_date_text );

	
  // Display day num 
  strftime( s_daynum_text, sizeof( s_daynum_text ), "%e", tick_time );
  text_layer_set_text( s_daynum_layer, s_daynum_text );	
	
	
	
  //dayname
	int wday_flag = tick_time -> tm_wday;
	//tm_wday: 0(sun) to 6(sat)
	if		 (wday_flag == 0) {text_layer_set_text(s_wday_layer, " Sunday");}
	else if  (wday_flag == 1) {text_layer_set_text(s_wday_layer, " Monday");}
	else if  (wday_flag == 2) {text_layer_set_text(s_wday_layer, " Tuesday");}
	else if  (wday_flag == 3) {text_layer_set_text(s_wday_layer, " Wednesday");}
	else if  (wday_flag == 4) {text_layer_set_text(s_wday_layer, " Thursday ");}
	else if  (wday_flag == 5) {text_layer_set_text(s_wday_layer, " Friday");}
	else if  (wday_flag == 6) {text_layer_set_text(s_wday_layer, " Saturday");}
}


static void handle_bluetooth(bool connected) {
  text_layer_set_text(s_connection_layer, connected ? "" : "OFF");
}




static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
// Destroy GBitmap
gbitmap_destroy(s_background_bitmap);

// Destroy BitmapLayer
bitmap_layer_destroy(s_background_layer);
  
s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

// Create BitmapLayer to display the GBitmap
s_background_layer = bitmap_layer_create(bounds);

// Set the bitmap onto the layer and add to the window
bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  s_time_layer = text_layer_create(GRect(0,30, bounds.size.w, 60));
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  s_date_layer = text_layer_create(GRect(50, 105, bounds.size.w, 34));
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  s_daynum_layer = text_layer_create(GRect(40, 120, bounds.size.w, 34));
  text_layer_set_text_color(s_daynum_layer, GColorBlack);
  text_layer_set_background_color(s_daynum_layer, GColorClear);
  text_layer_set_font(s_daynum_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_daynum_layer, GTextAlignmentCenter);
  
  
  s_wday_layer = text_layer_create(GRect(0, 9, bounds.size.w, 34));
  text_layer_set_text_color(s_wday_layer, GColorBlack);
  text_layer_set_background_color(s_wday_layer, GColorClear);
  text_layer_set_font(s_wday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_wday_layer, GTextAlignmentCenter);


  s_connection_layer = text_layer_create(GRect(-43, 125, bounds.size.w, 34));
  text_layer_set_text_color(s_connection_layer, GColorRed);
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
  handle_bluetooth(connection_service_peek_pebble_app_connection());

  s_battery_layer = text_layer_create(GRect( 25, 149, bounds.size.w, 34));
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_text(s_battery_layer, "100%");

  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  battery_state_service_subscribe(handle_battery);

  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bluetooth
  });

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_wday_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_daynum_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

  handle_battery(battery_state_service_peek());
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  text_layer_destroy(s_time_layer);
//  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_wday_layer);
  text_layer_destroy(s_connection_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy( s_daynum_layer );
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
  window_set_background_color(s_main_window, GColorBlack);
}




int main(void) {
  init();
  app_event_loop();
  deinit();
}
