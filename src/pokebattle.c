#include "pebble.h"
#include "time.h"


static Window *window;

static GBitmap *background_image;
static BitmapLayer *background_layer;

static GFont *level_font;
static GFont *date_font;
static GFont *time_font;

#define NUM_LEVEL_PKEY  0
#define NUM_LEVEL_FRESH 5

int level_int = 5;					
TextLayer *text_level_ally_layer;
TextLayer *text_level_enemy_layer;

TextLayer *text_name_ally_layer;
TextLayer *text_name_enemy_layer;

TextLayer *text_time_layer;
TextLayer *text_date_layer;

//--BATTERY
static GBitmap *icon_battery_charge;
static uint8_t battery_level;
static bool battery_plugged;
static Layer *battery_layer;

static bool initiate_watchface = true;

int NUM_POK_ALLY  = 22;
int NUM_POK_ENEMY = 28;

//random number shenanigans
int seed_enemy2, seed_ally2;
int start_number_enemy, start_number_ally, random_enemy, random_ally;	//values needed for random number generation 

/* ---POKEMON ENEMY--- */
const int POK_ENEMY_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_ENEMY_BLUE,	 	// 0
	RESOURCE_ID_IMAGE_ENEMY_CHARMANDER,	// 1
	RESOURCE_ID_IMAGE_ENEMY_CYNDAQUIL,	// 2
	RESOURCE_ID_IMAGE_ENEMY_EEVEE,	 	// 3
	RESOURCE_ID_IMAGE_ENEMY_DITTO,	 	// 4
 	RESOURCE_ID_IMAGE_ENEMY_FROAKIE,	// 5
 	RESOURCE_ID_IMAGE_ENEMY_MAGIKARP,	// 6
 	RESOURCE_ID_IMAGE_ENEMY_MISSINGNO,	// 7
 	RESOURCE_ID_IMAGE_ENEMY_OSHAWATT,	// 8
 	RESOURCE_ID_IMAGE_ENEMY_TORCHIC,	// 9
 	RESOURCE_ID_IMAGE_ENEMY_TURTWIG,	// 10
 	RESOURCE_ID_IMAGE_ENEMY_WEEDLE,	 	// 11
 	RESOURCE_ID_IMAGE_ENEMY_PIKACHU,	// 12
 	RESOURCE_ID_IMAGE_ENEMY_PSYDUCK,	// 13
 	RESOURCE_ID_IMAGE_ENEMY_GASTLY,	 	// 14
 	RESOURCE_ID_IMAGE_ENEMY_SEEL,	 	// 15
 	RESOURCE_ID_IMAGE_ENEMY_HAUNTER,	// 16
 	RESOURCE_ID_IMAGE_ENEMY_FERROSEED,	// 17 NEU
 	RESOURCE_ID_IMAGE_ENEMY_FOONGUS,	// 18 NEU
	RESOURCE_ID_IMAGE_ENEMY_SLOWPOKE,	// 19 NEU
	RESOURCE_ID_IMAGE_ENEMY_HOUNDOUR,	// 20 NEU
	RESOURCE_ID_IMAGE_ENEMY_LORD_HELIX,	// 21 NEU
  RESOURCE_ID_IMAGE_ENEMY_MEW,
  RESOURCE_ID_IMAGE_ENEMY_EXEGGUTOR,
  RESOURCE_ID_IMAGE_ENEMY_GENGAR,
  RESOURCE_ID_IMAGE_ENEMY_STARMIE,
  RESOURCE_ID_IMAGE_ENEMY_GOLEM,
  RESOURCE_ID_IMAGE_ENEMY_ZAPDOS
};

/* ---POKEMON ENEMY NAME--- */
char* NAMES_ENEMY_IMAGE_RESOURCE_IDS[] = {
 	"BLUE",
  "CHARMANDER",
 	"CYNDAQUIL",
 	"EEVEE",
 	"DITTO",
 	"FROAKIE",
 	"MAGIKARP",
 	"MISSINGNO",
 	"OSHAWATT",
 	"TORCHIC",
 	"TURTWIG",
 	"WEEDLE",
 	"PIKACHU",
	"PSYDUCK",
 	"GASTLY",
 	"SEEL",
 	"HAUNTER",
 	"FERROSEED",
 	"FOONGUS",
	"SLOWPOKE",
	"HOUNDOUR",
	"LORD_HELIX",
  "MEW",
  "EXEGGUTOR",
  "GENGAR",
  "STARMIE",
  "GOLEM",
  "ZAPDOS"
};

/* ---POKEMON ALLY--- */
const int POK_ALLY_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_ALLY_CHIKORITA,	// 0
 	RESOURCE_ID_IMAGE_ALLY_BULBASAUR,	// 1
 	RESOURCE_ID_IMAGE_ALLY_FENNEKIN,	// 2
 	RESOURCE_ID_IMAGE_ALLY_DITTO,	 	// 3
 	RESOURCE_ID_IMAGE_ALLY_MEWTWO,	 	// 4
 	RESOURCE_ID_IMAGE_ALLY_MAGIKARP,	// 5
 	RESOURCE_ID_IMAGE_ALLY_TEPIG,	 	// 6
 	RESOURCE_ID_IMAGE_ALLY_TREECKO,	 	// 7
 	RESOURCE_ID_IMAGE_ALLY_RED,	 		// 8
 	RESOURCE_ID_IMAGE_ALLY_PIPLUP,	 	// 9
 	RESOURCE_ID_IMAGE_ALLY_PIKACHU,	 	// 10
 	RESOURCE_ID_IMAGE_ALLY_OLDMAN,	  	// 11
 	RESOURCE_ID_IMAGE_ALLY_PIDGEY,	  	// 12
 	RESOURCE_ID_IMAGE_ALLY_TEDDYURSA,  	// 13
 	RESOURCE_ID_IMAGE_ALLY_PSYDUCK,	  	// 14
 	RESOURCE_ID_IMAGE_ALLY_LUGIA,	  	// 15 NEU
 	RESOURCE_ID_IMAGE_ALLY_KRABBY,	  	// 16 NEU
	RESOURCE_ID_IMAGE_ALLY_ARON,		// 17 NEU
	RESOURCE_ID_IMAGE_ALLY_GEODUDE,		// 18 NEU
	RESOURCE_ID_IMAGE_ALLY_LARVITAR,		// 19 NEU
  RESOURCE_ID_IMAGE_ALLY_HAUNTER,
  RESOURCE_ID_IMAGE_ALLY_STARMIE
};

/* ---POKEMON ALLY NAME-- */
char* NAMES_ALLY_IMAGE_RESOURCE_IDS[] = {
 	"CHIKORITA",
 	"BULBASAUR",
 	"FENNEKIN",
 	"DITTO",
 	"MEWTWO",
 	"MAGIKARP",
 	"TEPIG",
 	"TREECKO",
 	"RED",
 	"PIPLUP",
 	"PIKACHU",
 	"OLDMAN",
 	"PIDGEY",
 	"TEDDYURSA",
 	"PSYDUCK",
 	"LUGIA",
 	"KRABBY",
	"ARON",
	"GEODUDE",
	"LARVITAR",
  "HAUNTER",
  "STARMIE",
};

/* ---HEALTH BAR ENEMY--- */  // --> ALLY health bar represents Pebble's battery!!!
const int HP_ENEMY_IMAGE_RESOURCE_IDS[] = {
 	RESOURCE_ID_IMAGE_HEALTH_0,	
 	RESOURCE_ID_IMAGE_HEALTH_2,	
 	RESOURCE_ID_IMAGE_HEALTH_4,	
 	RESOURCE_ID_IMAGE_HEALTH_6,	
 	RESOURCE_ID_IMAGE_HEALTH_8	
};

/* BLUETOOTH (SLEEP) */
const int BT_WHITE_IMAGE_RESOURCE_IDS[] = {
  	RESOURCE_ID_IMAGE_POKE_SLEEP_WHITE
};

const int BT_BLACK_IMAGE_RESOURCE_IDS[] = {
 	RESOURCE_ID_IMAGE_POKE_SLEEP_BLACK
};
/* ----------------- */

#define TOTAL_POK_DIGITS 2
static GBitmap *pok_digits_images[TOTAL_POK_DIGITS];
static BitmapLayer *pok_digits_layers[TOTAL_POK_DIGITS];

#define TOTAL_HP_DIGITS 1
static GBitmap *hp_digits_images[TOTAL_HP_DIGITS];
static BitmapLayer *hp_digits_layers[TOTAL_HP_DIGITS];

#define TOTAL_BT_DIGITS 2
static GBitmap *bt_digits_images[TOTAL_BT_DIGITS];
static BitmapLayer *bt_digits_layers[TOTAL_BT_DIGITS];

//setting up bmp containers
static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
	GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   		.origin = origin,
   		.size = (*bmp_image)->bounds.size
	};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 		gbitmap_destroy(old_image);
	}
}

// BT connectivity
static void handle_bluetooth(bool connected) {
	
	if (connected) {
		// normal mode, show the watchface the way it should look
		if (initiate_watchface) {
   			layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), true);
   			layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), true);
		}
		// if the Pebble comes from disconnected to connected state the watchface will look  will look normal AND VIBRATE twice
		else {
			layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), true);
   			layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), true);
			vibes_double_pulse();
		}
	}
	else {
		// if the watchface gets started in a disconnected state it will show the SLP-screen, but won't vibrate (that would be annoying while browsing through your watchfaces)
		if (initiate_watchface) {
		   	layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), false);
			bitmap_layer_set_compositing_mode(bt_digits_layers[0], GCompOpOr);
			set_container_image(&bt_digits_images[0], bt_digits_layers[0], BT_WHITE_IMAGE_RESOURCE_IDS[0], GPoint(0, 0));
	   		layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), false);
    		bitmap_layer_set_compositing_mode(bt_digits_layers[1], GCompOpClear);
			set_container_image(&bt_digits_images[1], bt_digits_layers[1], BT_BLACK_IMAGE_RESOURCE_IDS[0], GPoint(0, 0));
		}
		// if Pebble disconnects while the watchface is active it will show the SLP-screen AND VIBRATE three times
		else {
		   	layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), false);
			bitmap_layer_set_compositing_mode(bt_digits_layers[0], GCompOpOr);
			set_container_image(&bt_digits_images[0], bt_digits_layers[0], BT_WHITE_IMAGE_RESOURCE_IDS[0], GPoint(0, 0));
	   		layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), false);
    		bitmap_layer_set_compositing_mode(bt_digits_layers[1], GCompOpClear);
			set_container_image(&bt_digits_images[1], bt_digits_layers[1], BT_BLACK_IMAGE_RESOURCE_IDS[0], GPoint(0, 0));

			vibes_enqueue_custom_pattern( (VibePattern) {
   				.durations = (uint32_t []) {100, 100, 100, 100, 100},
   				.num_segments = 5
			} );
		}	
	}
}

static void update_display(struct tm *current_time) {
	
	static char level_string[8]; //define length of string used to display int 'level_int'
	
 	if( ((current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (initiate_watchface == true) ) { 

		//only calculate the the base for the random number generators ONCE @ watchface initiation
		if (initiate_watchface){
			start_number_ally = (current_time->tm_sec) + NUM_POK_ALLY;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "generated additional value for ALLY random number [%d].", start_number_ally);
		
			start_number_enemy = (current_time->tm_sec) + (current_time->tm_min) + NUM_POK_ENEMY;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "generated additional value for ENEMY random numbers [%d].", start_number_enemy);
		}
		
		//the real random numbers will be calculated just when they're needed (@ watchface initiation AND @ hour change)
		static long seed_ally = 100;
		seed_ally  = (((seed_ally * 214013L + 2531011L) >> 16) & 32767);
		seed_ally2 = seed_ally + start_number_ally;
		random_ally = (seed_ally2 % NUM_POK_ALLY);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "random ALLY generated [#%d].", random_ally);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "random ALLY is [%s]", NAMES_ALLY_IMAGE_RESOURCE_IDS[random_ally]);
    text_layer_set_text(text_name_ally_layer, NAMES_ALLY_IMAGE_RESOURCE_IDS[random_ally]);

   
		static long seed_enemy = 100;
		seed_enemy  = (((seed_enemy * 214013L + 2531011L) >> 16) & 32767);
		seed_enemy2 = seed_enemy + start_number_enemy;
		random_enemy = (seed_enemy2 % NUM_POK_ENEMY);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "random ENEMY generated [#%d].", random_enemy);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "random ENEMY is [%s]", NAMES_ENEMY_IMAGE_RESOURCE_IDS[random_enemy]);
    text_layer_set_text(text_name_enemy_layer, NAMES_ENEMY_IMAGE_RESOURCE_IDS[random_enemy]);

			
 	  set_container_image(&pok_digits_images[1], pok_digits_layers[1], POK_ENEMY_IMAGE_RESOURCE_IDS[random_enemy], GPoint(86, 5));
 	  set_container_image(&pok_digits_images[0], pok_digits_layers[0], POK_ALLY_IMAGE_RESOURCE_IDS[random_ally], GPoint(5, 60));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "drawing Pokemon complete!!!");
	}
	
	if (initiate_watchface) {
		// LEVEL TEXT
		//this had to be done with a leading space to avoid rendering problems! rendering issues appeared just at certain positions on the screen anyways... that was weird!
		snprintf(level_string, sizeof(level_string), " %d", level_int); 
		text_layer_set_text(text_level_enemy_layer, level_string);
		text_layer_set_text(text_level_ally_layer, level_string);
	}
	
	//'level_int++' was originally inside the 'if( ((current_time->tm_min == 0) && (current_time->' -if clause, but had to be isolated for the persistent storage used to save the level state!
	//...otherwhise it would have add 1 level everytime the watchface got initiated... and that would be CHEATING! @_@
	if ((current_time->tm_min == 0) && (current_time->tm_sec == 0)) {
		level_int++;
   		APP_LOG(APP_LOG_LEVEL_DEBUG, "+1 [level] added! :)");
		
		// LEVEL TEXT
		//same as before, but with actual value of 'level_int'
		snprintf(level_string, sizeof(level_string), " %d", level_int);
		text_layer_set_text(text_level_enemy_layer, level_string);
		text_layer_set_text(text_level_ally_layer, level_string);

		APP_LOG(APP_LOG_LEVEL_INFO, "current level = %d", level_int);
	}
	
	//'simulation' of a decreasing enemy's HP bar
 	if ((current_time->tm_min) >= 49) {
 		set_container_image(&hp_digits_images[0], hp_digits_layers[0], HP_ENEMY_IMAGE_RESOURCE_IDS[4], GPoint(27, 31));
	}
 	else if ((current_time->tm_min) >= 37) {
 		set_container_image(&hp_digits_images[0], hp_digits_layers[0], HP_ENEMY_IMAGE_RESOURCE_IDS[3], GPoint(27, 31));
	}
 	else if ((current_time->tm_min) >= 25) {
 		set_container_image(&hp_digits_images[0], hp_digits_layers[0], HP_ENEMY_IMAGE_RESOURCE_IDS[2], GPoint(27, 31));
	}
 	else if ((current_time->tm_min) >= 13) {
 		set_container_image(&hp_digits_images[0], hp_digits_layers[0], HP_ENEMY_IMAGE_RESOURCE_IDS[1], GPoint(27, 31));
	}
 	else if (((current_time->tm_min) > 0) || ((current_time->tm_min) == 0)) {
 		set_container_image(&hp_digits_images[0], hp_digits_layers[0], HP_ENEMY_IMAGE_RESOURCE_IDS[0], GPoint(27, 31));
	}
}

//--BATTERY LAYER
void battery_layer_update_callback(Layer *layer, GContext *ctx) {

  	graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  	graphics_context_set_stroke_color(ctx, GColorBlack);
  	graphics_context_set_fill_color(ctx, GColorBlack);

  	if (!battery_plugged) {
    	graphics_fill_rect(ctx, GRect(0, 0, (uint8_t)(battery_level)/2, 2), 0, GCornerNone);
 	}
  	else {	
    	graphics_draw_bitmap_in_rect(ctx, icon_battery_charge, GRect(1, 0, 50, 2));
 	}
}

//--HANDLE BATTERY
void battery_state_handler(BatteryChargeState charge) {
	battery_level = charge.charge_percent;
   	battery_plugged = charge.is_plugged;
   	layer_mark_dirty(battery_layer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
 	update_display(tick_time);

 	static char time_text[] = "00:00";
 	static char date_text[] = "Xxx,00.00";

 	char *time_format;
 	char *date_format;

 	if (clock_is_24h_style()) {
   		time_format = "%H:%M";	// 00:00
   		//date_format = "%d.%m.";	// 31.12.
   		date_format = "%a,%d.%m";	// Mo,31.12
	} 
 	else {
   		time_format = "%I:%M";	// 0:00
   		//date_format = "%b%e";	// Dec31
   		date_format = "%a,%b%e";	// Mon,Dec31
	}

 	strftime(time_text, sizeof(time_text), time_format, tick_time);
 	strftime(date_text, sizeof(date_text), date_format, tick_time);

 // Kludge to handle lack of non-padded hour format string
 // for twelve hour clock.
 	if (!clock_is_24h_style() && (time_text[0] == '0')) {
   		memmove(time_text, &time_text[1], sizeof(time_text) - 1);
	}

 //Change the TextLayer text to show the new time!
 	text_layer_set_text(text_time_layer, time_text);
 	text_layer_set_text(text_date_layer, date_text);
}

void window_unload(Window *window){
	//destroy resources like you really mean it
  
  	gbitmap_destroy(background_image);
  	bitmap_layer_destroy(background_layer);

  	gbitmap_destroy(icon_battery_charge);

 	for (int i = 0; i < TOTAL_POK_DIGITS; i++) {
   		layer_remove_from_parent(bitmap_layer_get_layer(pok_digits_layers[i]));
   		gbitmap_destroy(pok_digits_images[i]);
   		bitmap_layer_destroy(pok_digits_layers[i]);
	}
 	for (int i = 0; i < TOTAL_HP_DIGITS; i++) {
   		layer_remove_from_parent(bitmap_layer_get_layer(hp_digits_layers[i]));
   		gbitmap_destroy(hp_digits_images[i]);
   		bitmap_layer_destroy(hp_digits_layers[i]);
	}
   	for (int i = 0; i < TOTAL_BT_DIGITS; i++) {
     	layer_remove_from_parent(bitmap_layer_get_layer(bt_digits_layers[i]));
     	gbitmap_destroy(bt_digits_images[i]);
     	bitmap_layer_destroy(bt_digits_layers[i]);
 	}
}

static void init(void) {

	window = window_create();

 	if (window == NULL) {
     	APP_LOG(APP_LOG_LEVEL_WARNING, "OOM: couldn't allocate window!?");
     	return;
	}
 	window_stack_push(window, true /* Animated */);
 	Layer *window_layer = window_get_root_layer(window);

 	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
 	background_layer = bitmap_layer_create(layer_get_frame(window_layer));
 	bitmap_layer_set_bitmap(background_layer, background_image);
 	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

 // Create pokemon&names layers
 	GRect dummy_frame = { {0, 0}, {0, 0} };
	for (int i = 0; i < TOTAL_POK_DIGITS; ++i) {
   		pok_digits_layers[i] = bitmap_layer_create(dummy_frame);
   		layer_add_child(window_layer, bitmap_layer_get_layer(pok_digits_layers[i]));
	}
 	for (int i = 0; i < TOTAL_HP_DIGITS; ++i) {
   		hp_digits_layers[i] = bitmap_layer_create(dummy_frame);
   		layer_add_child(window_layer, bitmap_layer_get_layer(hp_digits_layers[i]));
	}

	level_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LEVEL_10));
 	date_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_7));
 	time_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_24));

	// Get the count from persistent storage for use if it exists, otherwise use the default
  	level_int = persist_exists(NUM_LEVEL_PKEY) ? persist_read_int(NUM_LEVEL_PKEY) : NUM_LEVEL_FRESH;
   	APP_LOG(APP_LOG_LEVEL_INFO, "level status of %d restored!", level_int);
	
 	text_level_enemy_layer = text_layer_create(GRect(24, 19, 70, 12));
 	text_layer_set_text_alignment(text_level_enemy_layer, GTextAlignmentLeft);
 	text_layer_set_text_color(text_level_enemy_layer, GColorBlack);
 	text_layer_set_background_color(text_level_enemy_layer, GColorClear);
 	text_layer_set_font(text_level_enemy_layer, level_font);
 	layer_add_child(window_layer, text_layer_get_layer(text_level_enemy_layer));

 	text_level_ally_layer = text_layer_create(GRect(81, 80, 70, 12));
 	text_layer_set_text_alignment(text_level_ally_layer, GTextAlignmentLeft);
 	text_layer_set_text_color(text_level_ally_layer, GColorBlack);
 	text_layer_set_background_color(text_level_ally_layer, GColorClear);
 	text_layer_set_font(text_level_ally_layer, level_font);
 	layer_add_child(window_layer, text_layer_get_layer(text_level_ally_layer));
  
  text_name_enemy_layer = text_layer_create(GRect(5, 9, 76, 9));
  text_layer_set_text_alignment(text_name_enemy_layer, GTextAlignmentLeft);
 	text_layer_set_text_color(text_name_enemy_layer, GColorBlack);
 	text_layer_set_background_color(text_name_enemy_layer, GColorClear);
 	text_layer_set_font(text_name_enemy_layer, date_font);
 	layer_add_child(window_layer, text_layer_get_layer(text_name_enemy_layer));
	
  text_name_ally_layer = text_layer_create(GRect(67, 70, 76, 9));
  text_layer_set_text_alignment(text_name_ally_layer, GTextAlignmentLeft);
 	text_layer_set_text_color(text_name_ally_layer, GColorBlack);
 	text_layer_set_background_color(text_name_ally_layer, GColorClear);
 	text_layer_set_font(text_name_ally_layer, date_font);
 	layer_add_child(window_layer, text_layer_get_layer(text_name_ally_layer));
  
	text_date_layer = text_layer_create(GRect(59, 97, 76, 10));
	text_layer_set_text_alignment(text_date_layer, GTextAlignmentRight);
 	text_layer_set_text_color(text_date_layer, GColorBlack);
 	text_layer_set_background_color(text_date_layer, GColorClear);
 	text_layer_set_font(text_date_layer, date_font);
 	layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

 	text_time_layer = text_layer_create(GRect(12, 127, 124, 30));
 	text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
 	text_layer_set_text_color(text_time_layer, GColorBlack);
 	text_layer_set_background_color(text_time_layer, GColorClear);
 	text_layer_set_font(text_time_layer, time_font);
 	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

 	//BATTERY
	icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);
 	BatteryChargeState initial = battery_state_service_peek();
 	battery_level = initial.charge_percent;
 	battery_plugged = initial.is_plugged;
 	battery_layer = layer_create(GRect(83,92,50,2)); //SIZE = 50*2
 	layer_set_update_proc(battery_layer, &battery_layer_update_callback);
 	layer_add_child(window_get_root_layer(window), battery_layer);

 	//create BT layer (more down, so it covers all above... and things that might come in the future!)
 	for (int i = 0; i < TOTAL_BT_DIGITS; ++i) {
   		bt_digits_layers[i] = bitmap_layer_create(dummy_frame);
   		layer_add_child(window_layer, bitmap_layer_get_layer(bt_digits_layers[i]));
	}
  	handle_bluetooth(bluetooth_connection_service_peek());

 	// Avoids a blank screen on watch start.
 	time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);
	handle_minute_tick(tick_time, MINUTE_UNIT);
 	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick); //changed from MINUTE_UNIT


 	update_display(tick_time);

	//kill'em all!!!
	window_set_window_handlers(window, (WindowHandlers) {
  		//.load = window_load,
  		.unload = window_unload,
    });
	
 	initiate_watchface = false;
}


static void deinit(void) {
	
	// Save the count into persistent storage on app exit
  	persist_write_int(NUM_LEVEL_PKEY, level_int);
   	APP_LOG(APP_LOG_LEVEL_INFO, "level status of %d saved!", level_int);
	
 	text_layer_destroy(text_level_ally_layer);
 	text_layer_destroy(text_level_enemy_layer);
 	text_layer_destroy(text_time_layer);
 	text_layer_destroy(text_date_layer);
  text_layer_destroy(text_name_ally_layer);
  text_layer_destroy(text_name_enemy_layer);
 	fonts_unload_custom_font(time_font);
 	fonts_unload_custom_font(date_font);
 	fonts_unload_custom_font(level_font);

 	bluetooth_connection_service_unsubscribe();
 	battery_state_service_unsubscribe();
 	layer_destroy(battery_layer);

 	window_destroy(window);
}

int main(void) {
 	init();
 	//tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick); //changed from MINUTE_UNIT
 	bluetooth_connection_service_subscribe(&handle_bluetooth); //BT
 	battery_state_service_subscribe (&battery_state_handler);
 	app_event_loop();
 	deinit();
}