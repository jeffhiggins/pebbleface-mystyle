/*
Copyright (C) 2014 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pebble.h"

static AppSync sync;
static uint8_t sync_buffer[256];

static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_CLEAR_DAY,
  RESOURCE_ID_CLEAR_NIGHT,
  RESOURCE_ID_WINDY,
  RESOURCE_ID_COLD,
  RESOURCE_ID_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_HAZE,
  RESOURCE_ID_CLOUD,
  RESOURCE_ID_RAIN,
  RESOURCE_ID_SNOW,
  RESOURCE_ID_HAIL,
  RESOURCE_ID_CLOUDY,
  RESOURCE_ID_STORM,
  RESOURCE_ID_FOG,
  RESOURCE_ID_NA,
};

enum WeatherKey {
  WEATHER_ICON_KEY = 0x0,
  WEATHER_TEMPERATURE_KEY = 0x1,
  INVERT_COLOR_KEY = 0x2,
  HIDE_SEC_KEY = 0x3,
  BLUETOOTHVIBE_KEY = 0x4,
  HOURLYVIBE_KEY = 0x5,
  BACKGROUND_KEY = 0x6,
  BACKGROUND2_KEY = 0x7,
  BACKGROUND3_KEY = 0x8,
  BACKGROUND4_KEY = 0x9,
  BACKGROUND5_KEY = 0xA,
  DATE2_KEY = 0xB,
  HIDEW_KEY = 0xC,
  TEXT_KEY = 0xD,
  HIDEBT_KEY = 0xE,
  HIDEBATT_KEY = 0xF
};

static bool appStarted = false;

static int invert;
static int hidesec;
static int bluetoothvibe;
static int hourlyvibe;
static int background;
static int background2;
static int background3;
static int background4;
static int textcol;
static int date2;
static int hidew;
static int bigtext;
static int hidebt;
static int hidebatt;

GColor background_color = GColorBlack;

static Window *window;
static Layer *window_layer;

int cur_day = -1;

//TextLayer *layer_wday_text;

static GFont *date_font;
static GFont *bt_font;
TextLayer *layer_date_text;
TextLayer *layer_date_text2;
TextLayer *layer_secs_text;

static GFont *bigdate_font;
TextLayer *layer_bigdate_text;
TextLayer *layer_bigdate_text2;
TextLayer *layer_bigsecs_text;

static GBitmap *background_image;
static BitmapLayer *background_layer;

static GBitmap *background_image2;
static BitmapLayer *background_layer2;

static GBitmap *background_image3;
static BitmapLayer *background_layer3;

static GBitmap *background_image4;
static BitmapLayer *background_layer4;

static GBitmap *background_image5;
static BitmapLayer *background_layer5;

BitmapLayer *icon_layer;
GBitmap *icon_bitmap = NULL;
TextLayer *temp_layer;

TextLayer *battery_text_layer;
TextLayer *bttext;
TextLayer *bigbattery_text_layer;
TextLayer *bigbttext;
int charge_percent = 0;

static GBitmap *separator_image;
static BitmapLayer *separator_layer;

//static GBitmap *day_name_image;
//static BitmapLayer *day_name_layer;
/*
const int DAY_NAME_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DAY_NAME_SUN,
  RESOURCE_ID_IMAGE_DAY_NAME_MON,
  RESOURCE_ID_IMAGE_DAY_NAME_TUE,
  RESOURCE_ID_IMAGE_DAY_NAME_WED,
  RESOURCE_ID_IMAGE_DAY_NAME_THU,
  RESOURCE_ID_IMAGE_DAY_NAME_FRI,
  RESOURCE_ID_IMAGE_DAY_NAME_SAT
};
*/

#define TOTAL_TIME_DIGITS 4
static GBitmap *time_digits_images[TOTAL_TIME_DIGITS];
static BitmapLayer *time_digits_layers[TOTAL_TIME_DIGITS];

const int TINY_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_TINY_0,
  RESOURCE_ID_IMAGE_TINY_1,
  RESOURCE_ID_IMAGE_TINY_2,
  RESOURCE_ID_IMAGE_TINY_3,
  RESOURCE_ID_IMAGE_TINY_4,
  RESOURCE_ID_IMAGE_TINY_5,
  RESOURCE_ID_IMAGE_TINY_6,
  RESOURCE_ID_IMAGE_TINY_7,
  RESOURCE_ID_IMAGE_TINY_8,
  RESOURCE_ID_IMAGE_TINY_9
};

InverterLayer *inverter_layer = NULL;


void text_color (bool textcol) {
	
		if (textcol) {
    			text_layer_set_text_color(bttext, GColorWhite);
    			text_layer_set_text_color(bigbttext, GColorWhite);
    			text_layer_set_text_color(layer_date_text, GColorWhite);
    			text_layer_set_text_color(layer_bigdate_text, GColorWhite);
			    text_layer_set_text_color(layer_date_text2, GColorWhite);
			    text_layer_set_text_color(layer_bigdate_text2, GColorWhite);
			    text_layer_set_text_color(battery_text_layer, GColorWhite);
			    text_layer_set_text_color(layer_secs_text, GColorWhite);
			    text_layer_set_text_color(layer_bigsecs_text, GColorWhite);
							
		} else {
    			text_layer_set_text_color(bttext, GColorBlack);
    			text_layer_set_text_color(bigbttext, GColorBlack);
    			text_layer_set_text_color(layer_date_text, GColorBlack);
    			text_layer_set_text_color(layer_bigdate_text, GColorBlack);
			    text_layer_set_text_color(layer_date_text2, GColorBlack);
			    text_layer_set_text_color(layer_bigdate_text2, GColorBlack);
			    text_layer_set_text_color(battery_text_layer, GColorBlack);
			    text_layer_set_text_color(layer_secs_text, GColorBlack);	
			    text_layer_set_text_color(layer_bigsecs_text, GColorBlack);	
		}	
}

void size (bool bigtext) {
	
	
		if (bigtext) {
					  		layer_set_hidden(text_layer_get_layer(layer_date_text), true);			
					  		layer_set_hidden(text_layer_get_layer(layer_date_text2), true);
					  		layer_set_hidden(text_layer_get_layer(layer_secs_text), true);
					  		layer_set_hidden(text_layer_get_layer(bttext), true);
			
					  		layer_set_hidden(text_layer_get_layer(bigbttext), false);

		}  else {
					  		layer_set_hidden(text_layer_get_layer(layer_bigdate_text), true);			
					  		layer_set_hidden(text_layer_get_layer(layer_bigdate_text2), true);
					  		layer_set_hidden(text_layer_get_layer(layer_bigsecs_text), true);
					  		layer_set_hidden(text_layer_get_layer(bigbttext), true);
			
					  		layer_set_hidden(text_layer_get_layer(bttext), false);
		}	
}

void date_format (bool date2) {
	
		  if (date2) {
		  		layer_set_hidden(text_layer_get_layer(layer_date_text), true);
		  		layer_set_hidden(text_layer_get_layer(layer_date_text2), false);
			  
			  	layer_set_hidden(text_layer_get_layer(layer_bigdate_text), true);
		  		layer_set_hidden(text_layer_get_layer(layer_bigdate_text2), false);
			  
	  } else {
	  		  	layer_set_hidden(text_layer_get_layer(layer_date_text2), true);
		  		layer_set_hidden(text_layer_get_layer(layer_date_text), false);
			  
				layer_set_hidden(text_layer_get_layer(layer_bigdate_text2), true);
		  		layer_set_hidden(text_layer_get_layer(layer_bigdate_text), false);  
	  }
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed);

void secs (bool hidesec) {
	
		if (hidesec) {
		    layer_set_hidden(text_layer_get_layer(layer_secs_text), true);
		    layer_set_hidden(text_layer_get_layer(layer_bigsecs_text), true);
	  	    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
		}  else {
			layer_set_hidden(text_layer_get_layer(layer_secs_text), false);
			layer_set_hidden(text_layer_get_layer(layer_bigsecs_text), false);
			tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
		}
}


void set_invert_color(bool invert) {
  if (invert && inverter_layer == NULL) {
    // Add inverter layer
    Layer *window_layer = window_get_root_layer(window);

    inverter_layer = inverter_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
  } else if (!invert && inverter_layer != NULL) {
    // Remove Inverter layer
    layer_remove_from_parent(inverter_layer_get_layer(inverter_layer));
    inverter_layer_destroy(inverter_layer);
    inverter_layer = NULL;
  }
  // No action required
}


static void sync_tuple_changed_callback(const uint32_t key,
                                        const Tuple* new_tuple,
                                        const Tuple* old_tuple,
                                        void* context) {

  // App Sync keeps new_tuple in sync_buffer, so we may use it directly
  switch (key) {
    case WEATHER_ICON_KEY:
    if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }

      icon_bitmap = gbitmap_create_with_resource(
          WEATHER_ICONS[new_tuple->value->uint8]);
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
    break;
	  
    case WEATHER_TEMPERATURE_KEY:
      text_layer_set_text(temp_layer, new_tuple->value->cstring);
    break;

    case INVERT_COLOR_KEY:
      invert = new_tuple->value->uint8 != 0;
      persist_write_bool(INVERT_COLOR_KEY, invert);
      set_invert_color(invert);
    break;
	  
	case HIDE_SEC_KEY:
      hidesec = new_tuple->value->uint8 != 0;
	  persist_write_bool(HIDE_SEC_KEY, hidesec);
	  secs(hidesec);
	break; 

    case BLUETOOTHVIBE_KEY:
      bluetoothvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(BLUETOOTHVIBE_KEY, bluetoothvibe);
    break; 
		
    case HOURLYVIBE_KEY:
      hourlyvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(HOURLYVIBE_KEY, hourlyvibe);	  
    break;
	  
	case BACKGROUND_KEY:
	  background = new_tuple->value->uint8 != 0;
	  persist_write_bool(BACKGROUND_KEY, background);
		if (background) {
			layer_set_hidden(bitmap_layer_get_layer(background_layer), true);
		} else {
			layer_set_hidden(bitmap_layer_get_layer(background_layer), false);	
		}

	break; 

    case BACKGROUND2_KEY:
	  background2 = new_tuple->value->uint8 != 0;
	  persist_write_bool(BACKGROUND2_KEY, background2);
		if (background2) {
			layer_set_hidden(bitmap_layer_get_layer(background_layer2), true);
		} else {
			layer_set_hidden(bitmap_layer_get_layer(background_layer2), false);	
		}
	break; 

	case BACKGROUND3_KEY:
	  background3 = new_tuple->value->uint8 != 0;
	  persist_write_bool(BACKGROUND3_KEY, background3);
		if (background3) {
			layer_set_hidden(bitmap_layer_get_layer(background_layer3), true);
		} else {
			layer_set_hidden(bitmap_layer_get_layer(background_layer3), false);	
		}
	break; 

	case BACKGROUND4_KEY:
	  background4 = new_tuple->value->uint8 != 0;
	  persist_write_bool(BACKGROUND4_KEY, background4);
		if (background4) {
			layer_set_hidden(bitmap_layer_get_layer(background_layer4), true);
		} else {
			layer_set_hidden(bitmap_layer_get_layer(background_layer4), false);	
		}
	break; 

	case BACKGROUND5_KEY:
	  textcol = new_tuple->value->uint8 != 0;
	  persist_write_bool(BACKGROUND5_KEY, textcol);
	  text_color(textcol);
	break; 
	  
	case DATE2_KEY:
      date2 = new_tuple->value->uint8 != 0;
	  persist_write_bool(DATE2_KEY, date2);	 
	  date_format(date2);
   break;
	  
	case HIDEW_KEY:
	  hidew = new_tuple->value->uint8 != 0;
	  persist_write_bool(HIDEW_KEY, hidew);
      if (hidew) {
			layer_set_hidden(bitmap_layer_get_layer(icon_layer), true);
			layer_set_hidden(text_layer_get_layer(temp_layer), true);
			layer_set_hidden(bitmap_layer_get_layer(background_layer5), false);
	  } else {
			layer_set_hidden(bitmap_layer_get_layer(icon_layer), false);	
			layer_set_hidden(text_layer_get_layer(temp_layer), false);
			layer_set_hidden(bitmap_layer_get_layer(background_layer5), true);
	  }
	break;
	  
    case TEXT_KEY:
      bigtext = new_tuple->value->uint8 != 0;
	  persist_write_bool(TEXT_KEY, bigtext);
      size(bigtext);
	break;
	  
    case HIDEBT_KEY:
	  hidebt = new_tuple->value->uint8 != 0;
	  persist_write_bool(HIDEBT_KEY, hidebt);
      if (hidebt) {
			layer_set_hidden(text_layer_get_layer(bttext), true);
			layer_set_hidden(text_layer_get_layer(bigbttext), true);
	  } else {
			//ayer_set_hidden(text_layer_get_layer(bttext), false);
			//layer_set_hidden(text_layer_get_layer(bigbttext), false);
	  }
	break;
	  
	case HIDEBATT_KEY:
	  hidebatt = new_tuple->value->uint8 != 0;
	  persist_write_bool(HIDEBATT_KEY, hidebatt);
      if (hidebatt) {
			layer_set_hidden(text_layer_get_layer(battery_text_layer), true);
	  } else {
			layer_set_hidden(text_layer_get_layer(battery_text_layer), false);
	  }
	break;
  }
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;
  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect frame = (GRect) {
    .origin = origin,
    .size = (*bmp_image)->bounds.size
  };
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);
  gbitmap_destroy(old_image);
}

void update_battery_state(BatteryChargeState charge_state) {
    static char battery_text[] = "x100%";

    if (charge_state.is_charging) {

        snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
        
    }
    charge_percent = charge_state.charge_percent;
    
    text_layer_set_text(battery_text_layer, battery_text);
}

void handle_bluetooth(bool connected) {

	if (connected) {
		static char bt_text[] = "x";  
	    snprintf(bt_text, sizeof(bt_text), "a");
	    text_layer_set_text(bttext, bt_text);
	    text_layer_set_text(bigbttext, bt_text);
	} else {
		static char bt_text[] = "x";
        snprintf(bt_text, sizeof(bt_text), "r");
        text_layer_set_text(bttext, bt_text);
        text_layer_set_text(bigbttext, bt_text);
	}

    if (appStarted && bluetoothvibe) {     
        vibes_long_pulse();
	}
}

void bluetooth_connection_callback(bool connected) {
  handle_bluetooth(connected);
}

void force_update(void) {
    update_battery_state(battery_state_service_peek());
    handle_bluetooth(bluetooth_connection_service_peek());
}

unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  // Converts "0" to "12"
  return display_hour ? display_hour : 12;
}

static void update_month (struct tm *tick_time) {

    static char date_text[] = "xx/xx";
    static char date_text2[] = "xx/xx";
	
	strftime(date_text, sizeof(date_text), "%m/%d", tick_time);
	strftime(date_text2, sizeof(date_text2), "%d/%m", tick_time);
	
   	text_layer_set_text(layer_date_text, date_text);
	text_layer_set_text(layer_date_text2, date_text2);
	text_layer_set_text(layer_bigdate_text, date_text);
	text_layer_set_text(layer_bigdate_text2, date_text2);
}

static void update_hours(struct tm *tick_time) {
  
   unsigned short display_hour = get_display_hour(tick_time->tm_hour);

  set_container_image(&time_digits_images[0], time_digits_layers[0], TINY_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(6, 52));
  set_container_image(&time_digits_images[1], time_digits_layers[1], TINY_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(36, 52));

  if (!clock_is_24h_style()) {
    if (display_hour/10 == 0) {
      layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), true);
    }
    else {
      layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), false);
    }
  }
}

static void update_minutes(struct tm *tick_time) {
  set_container_image(&time_digits_images[2], time_digits_layers[2], TINY_IMAGE_RESOURCE_IDS[tick_time->tm_min/10], GPoint(81, 52));
  set_container_image(&time_digits_images[3], time_digits_layers[3], TINY_IMAGE_RESOURCE_IDS[tick_time->tm_min%10], GPoint(111, 52));
}

static void update_seconds(struct tm *tick_time) {

      static char secs_text[] = "00";

      strftime(secs_text, sizeof(secs_text), "%S", tick_time);
      text_layer_set_text(layer_secs_text, secs_text);
      text_layer_set_text(layer_bigsecs_text, secs_text);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if (units_changed & MONTH_UNIT) {
    update_month(tick_time);
  }
  if (units_changed & HOUR_UNIT) {
    update_hours(tick_time);
  }
  if (units_changed & MINUTE_UNIT) {
    update_minutes(tick_time);
  }	
    if (units_changed & SECOND_UNIT) {
    update_seconds(tick_time);
  }		
}

void set_style(void) {
    background_color  = GColorBlack;
    window_set_background_color(window, background_color);
}
	
static void init(void) {
  memset(&time_digits_layers, 0, sizeof(time_digits_layers));
  memset(&time_digits_images, 0, sizeof(time_digits_images));

	
 // Setup messaging
  const int inbound_size = 256;
  const int outbound_size = 256;
  app_message_open(inbound_size, outbound_size);	
	
  window = window_create();
  if (window == NULL) {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
      return;
  }

  set_style();

  window_stack_push(window, true /* Animated */);
  window_layer = window_get_root_layer(window);
	
  background_image4 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND4);
  GRect frame4 = (GRect) {
    .origin = { .x = 0, .y = 0 },
    .size = background_image4->bounds.size
  };
  background_layer4 = bitmap_layer_create(frame4);
  bitmap_layer_set_bitmap(background_layer4, background_image4);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer4)); 
	
  background_image3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND3);
  GRect frame1 = (GRect) {
    .origin = { .x = 0, .y = 38 },
    .size = background_image3->bounds.size
  };
  background_layer3 = bitmap_layer_create(frame1);
  bitmap_layer_set_bitmap(background_layer3, background_image3);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer3)); 
	
  background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  GRect frame2 = (GRect) {
    .origin = { .x = 7, .y = 39 },
    .size = background_image->bounds.size
  };
  background_layer = bitmap_layer_create(frame2);
  bitmap_layer_set_bitmap(background_layer, background_image);
	GCompOp compositing_mode_back = GCompOpOr;
  bitmap_layer_set_compositing_mode(background_layer, compositing_mode_back);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));  
	
  background_image2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND2);
  GRect frame3 = (GRect) {
    .origin = { .x = 7, .y = 54 },
    .size = background_image2->bounds.size
  };
  background_layer2 = bitmap_layer_create(frame3);
  bitmap_layer_set_bitmap(background_layer2, background_image2);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer2)); 
		
  separator_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SEPARATOR);
  GRect frame = (GRect) {
    .origin = { .x = 66, .y = 52 },
    .size = separator_image->bounds.size
  };
  separator_layer = bitmap_layer_create(frame);
  bitmap_layer_set_bitmap(separator_layer, separator_image);
  GCompOp compositing_mode2 = GCompOpOr;
  bitmap_layer_set_compositing_mode(separator_layer, compositing_mode2);
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer));   

  Layer *weather_holder = layer_create(GRect(0, 0, 144, 168 ));
  layer_add_child(window_layer, weather_holder);
	
  icon_layer = bitmap_layer_create(GRect(4, 132, 100, 10));
  GCompOp compositing_mode1 = GCompOpOr;
  bitmap_layer_set_compositing_mode(icon_layer, compositing_mode1);
  layer_add_child(weather_holder, bitmap_layer_get_layer(icon_layer));

  date_font  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_14 ) );
  bigdate_font  = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_CUSTOM_26 ) );
  bt_font =  fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_SYM_18 ) );
	
	
  temp_layer = text_layer_create(GRect(97, 128, 44, 30));
  text_layer_set_text_color(temp_layer, GColorWhite);
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_font(temp_layer,fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_font(temp_layer, date_font);
  text_layer_set_text_alignment(temp_layer, GTextAlignmentRight);
  layer_add_child(weather_holder, text_layer_get_layer(temp_layer));
	
	
	layer_date_text = text_layer_create(GRect(0, 36, 140, 22));
    text_layer_set_background_color(layer_date_text, GColorClear);
    text_layer_set_text_color(layer_date_text, GColorWhite);
    text_layer_set_text_alignment(layer_date_text, GTextAlignmentRight);
    text_layer_set_font(layer_date_text, date_font);
    layer_add_child(window_layer, text_layer_get_layer(layer_date_text));
	
	layer_date_text2 = text_layer_create(GRect(0, 36, 140, 22));
    text_layer_set_background_color(layer_date_text2, GColorClear);
    text_layer_set_text_color(layer_date_text2, GColorWhite);
    text_layer_set_text_alignment(layer_date_text2, GTextAlignmentRight);
    text_layer_set_font(layer_date_text2, date_font);
    layer_add_child(window_layer, text_layer_get_layer(layer_date_text2));
	
    layer_secs_text = text_layer_create(GRect(62, 36, 142, 22));
    text_layer_set_background_color(layer_secs_text, GColorClear);
    text_layer_set_text_color(layer_secs_text, GColorWhite);
    text_layer_set_text_alignment(layer_secs_text, GTextAlignmentLeft);
    text_layer_set_font(layer_secs_text, date_font);
    layer_add_child(window_layer, text_layer_get_layer(layer_secs_text));
	
	battery_text_layer = text_layer_create(GRect(7, 36, 100, 22));
    text_layer_set_background_color(battery_text_layer, GColorClear);
    text_layer_set_text_color(battery_text_layer, GColorWhite);
    text_layer_set_text_alignment(battery_text_layer, GTextAlignmentLeft);
    text_layer_set_font(battery_text_layer, date_font);
    layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));
	
	bttext  = text_layer_create(GRect(43, 32, 20, 30));
	text_layer_set_background_color(bttext, GColorClear);
    text_layer_set_text_color(bttext, GColorWhite);
	text_layer_set_text_alignment(bttext, GTextAlignmentCenter);
    text_layer_set_font(bttext, bt_font);
    layer_add_child(window_layer, text_layer_get_layer(bttext));

	
	layer_bigdate_text = text_layer_create(GRect(0, 24, 143, 26));
    text_layer_set_background_color(layer_bigdate_text, GColorClear);
    text_layer_set_text_color(layer_bigdate_text, GColorWhite);
    text_layer_set_text_alignment(layer_bigdate_text, GTextAlignmentRight);
    text_layer_set_font(layer_bigdate_text, bigdate_font);
    layer_add_child(window_layer, text_layer_get_layer(layer_bigdate_text));
	
	layer_bigdate_text2 = text_layer_create(GRect(0, 24, 143, 26));
    text_layer_set_background_color(layer_bigdate_text2, GColorClear);
    text_layer_set_text_color(layer_bigdate_text2, GColorWhite);
    text_layer_set_text_alignment(layer_bigdate_text2, GTextAlignmentRight);
    text_layer_set_font(layer_bigdate_text2, bigdate_font);
    layer_add_child(window_layer, text_layer_get_layer(layer_bigdate_text2));
	
    layer_bigsecs_text = text_layer_create(GRect(92, 15, 31, 20));
    text_layer_set_background_color(layer_bigsecs_text, GColorClear);
    text_layer_set_text_color(layer_bigsecs_text, GColorWhite);
    text_layer_set_text_alignment(layer_bigsecs_text, GTextAlignmentRight);
    text_layer_set_font(layer_bigsecs_text, date_font);
    layer_add_child(window_layer, text_layer_get_layer(layer_bigsecs_text));
	
	bigbttext  = text_layer_create(GRect(121, 11, 22, 30));
	text_layer_set_background_color(bigbttext, GColorClear);
    text_layer_set_text_color(bigbttext, GColorWhite);
	text_layer_set_text_alignment(bigbttext, GTextAlignmentRight);
    text_layer_set_font(bigbttext, bt_font);
    layer_add_child(window_layer, text_layer_get_layer(bigbttext));
	
  background_image5 = gbitmap_create_with_resource(RESOURCE_ID_BLANK);
  GRect frame5 = (GRect) {
    .origin = { .x = 7, .y = 131 },
    .size = background_image5->bounds.size
  };
  background_layer5 = bitmap_layer_create(frame5);
  bitmap_layer_set_bitmap(background_layer5, background_image5);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer5)); 	
	
  // Create time and date layers
   GRect dummy_frame = { {0, 0}, {0, 0} };
/*   day_name_layer = bitmap_layer_create(dummy_frame);
   GCompOp compositing_mode7 = GCompOpOr;
   bitmap_layer_set_compositing_mode(day_name_layer, compositing_mode7);
   layer_add_child(window_layer, bitmap_layer_get_layer(day_name_layer));	
	*/
    for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
    time_digits_layers[i] = bitmap_layer_create(dummy_frame);
	GCompOp compositing_mode3 = GCompOpOr;
    bitmap_layer_set_compositing_mode(time_digits_layers[i], compositing_mode3);		
    layer_add_child(window_layer, bitmap_layer_get_layer(time_digits_layers[i]));
    }
	
	
Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 14),
    TupletCString(WEATHER_TEMPERATURE_KEY, ""),
    TupletInteger(INVERT_COLOR_KEY, persist_read_bool(INVERT_COLOR_KEY)),
	TupletInteger(HIDE_SEC_KEY, persist_read_bool(HIDE_SEC_KEY)),
    TupletInteger(HOURLYVIBE_KEY, persist_read_bool(HOURLYVIBE_KEY)),
    TupletInteger(BLUETOOTHVIBE_KEY, persist_read_bool(BLUETOOTHVIBE_KEY)),
    TupletInteger(BACKGROUND_KEY, persist_read_bool(BACKGROUND_KEY)),
    TupletInteger(BACKGROUND2_KEY, persist_read_bool(BACKGROUND2_KEY)),
    TupletInteger(BACKGROUND3_KEY, persist_read_bool(BACKGROUND3_KEY)),
    TupletInteger(BACKGROUND4_KEY, persist_read_bool(BACKGROUND4_KEY)),
    TupletInteger(BACKGROUND5_KEY, persist_read_bool(BACKGROUND5_KEY)),
    TupletInteger(DATE2_KEY, persist_read_bool(DATE2_KEY)),
    TupletInteger(HIDEW_KEY, persist_read_bool(HIDEW_KEY)),
    TupletInteger(TEXT_KEY, persist_read_bool(TEXT_KEY)),
    TupletInteger(HIDEBT_KEY, persist_read_bool(HIDEBT_KEY)),
    TupletInteger(HIDEBATT_KEY, persist_read_bool(HIDEBATT_KEY)),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values,
                ARRAY_LENGTH(initial_values), sync_tuple_changed_callback,
                NULL, NULL);

  appStarted = true;

  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  handle_tick(tick_time, MONTH_UNIT + DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);

  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);	

  // handlers
  battery_state_service_subscribe(&update_battery_state);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
	
  // draw first frame
  force_update();

}

static void deinit(void) {
  
  app_sync_deinit(&sync);
	
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();

  layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  bitmap_layer_destroy(background_layer);
  gbitmap_destroy(background_image);
  background_image = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer2));
  bitmap_layer_destroy(background_layer2);
  gbitmap_destroy(background_image2);
  background_image2 = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer3));
  bitmap_layer_destroy(background_layer3);
  gbitmap_destroy(background_image3);
  background_image3 = NULL;
		
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer4));
  bitmap_layer_destroy(background_layer4);
  gbitmap_destroy(background_image4);
  background_image4 = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer5));
  bitmap_layer_destroy(background_layer5);
  gbitmap_destroy(background_image5);
  background_image5 = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(separator_layer));
  bitmap_layer_destroy(separator_layer);
  gbitmap_destroy(separator_image);
	
//  layer_remove_from_parent(bitmap_layer_get_layer(day_name_layer));
//  bitmap_layer_destroy(day_name_layer);
//  gbitmap_destroy(day_name_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(icon_layer));
  bitmap_layer_destroy(icon_layer);
  gbitmap_destroy(icon_bitmap);
  icon_bitmap = NULL;

  text_layer_destroy( temp_layer );
  text_layer_destroy( bttext );
  text_layer_destroy( layer_secs_text );
  text_layer_destroy( layer_date_text );
  text_layer_destroy( layer_date_text2 );
  text_layer_destroy( battery_text_layer );
	
    for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(time_digits_layers[i]));
    gbitmap_destroy(time_digits_images[i]);
    bitmap_layer_destroy(time_digits_layers[i]);
    } 

  fonts_unload_custom_font( date_font );
  fonts_unload_custom_font( bt_font );

  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}