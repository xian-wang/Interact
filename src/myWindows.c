#include "pebble.h"
#include "myWindows.h"
#include "datalogging.h"
#include "compilerWindows.h"

Window *window_main;
Window *window_training;
Window *window_configuration;
Window *window_user_list;
Window *window_data_logging;
TextLayer *connection_detection_layer;
TextLayer *training_layer;
TextLayer *configuration_layer;
TextLayer *data_logging_layer;
TextLayer *interaction_data_logging_layer;
MenuLayer *menu_main_layer;
MenuLayer *menu_user_list_layer;

char user_list_items[MAX_NUM_USER_ITEMS][MAX_ITEM_USER_LENGTH];
int user_item_count = 0;

char main_menu[NUM_MAIN_MENU][MAX_ITEM_TEXT_LENGTH]={"Training", "Interaction", "Configuration"};
char *connection_error_info = "Please connect to the phone and open the corresponding app";
char *training_info = "Please follow the instructions on the phone to train";
char *configuration_info = "Please follow the instructions on the phone to configure";
char *data_logging_info_start = "Please turn the wrist to start";
char *data_logging_info_restart = "FINISHED\n\nTurn wrist to restart";
char *data_logging_info_waiting = "FINISHED\n\nWaiting for feedback...";
char *data_logging_info_collecting = "Collecting data ...";

//pop window if it is on the top of window stack, if not, remove it from window stack
void myPopWindow(Window* window){
    if(window_stack_get_top_window() != window){
	if(window_stack_contains_window(window)){
	    window_stack_remove(window, false);
	}
    }
    else{
	window_stack_pop(true);
    }

}

//create a text layer on window, and set the initial text
void my_create_text_layer(Window* window, TextLayer** tl, char* info){
    Layer *window_layer = window_get_root_layer(window);

    GRect bounds = layer_get_frame(window_layer);
    *tl = text_layer_create(bounds);
    text_layer_set_text_color(*tl, GColorBlack);
    text_layer_set_background_color(*tl, GColorClear);
    text_layer_set_font(*tl, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(*tl, GTextAlignmentCenter);
    text_layer_set_text(*tl, info);

    layer_add_child(window_layer, text_layer_get_layer(*tl));
}

//basic operations to load a window, without click handlers registration
/*
   void myLoadWindow(Window* window, WindowHandlers whdls){
   window = window_create();
   window_set_window_handlers(window, whdls);
   window_stack_push(window, true );
   if(window_stack_get_top_window() != window){//if the window is already on the top, do nothing
   if(window_stack_contains_window(window)){
   window_stack_remove(window, false);
   }
   window = window_create();
   window_set_window_handlers(window, whdls);
   window_stack_push(window, true Animated );
   }
   }
 */

void myLoadWindow(Window** window, WindowHandler loadW, WindowHandler unloadW){
    /*
     *window = window_create();
     window_set_window_handlers(*window, (WindowHandlers){
     .load = loadW,
     .unload = unloadW
     });
     window_stack_push(*window, true);
     */
    if(window_stack_get_top_window() != *window){//if the window is already on the top, do nothing
	if(window_stack_contains_window(*window)){
	    window_stack_remove(*window, false);
	}
	*window = window_create();
	window_set_window_handlers(*window, (WindowHandlers){
		.load = loadW,
		.unload = unloadW
		});
	window_stack_push(*window, true);
    }
}
//pop all windows on the top of main window. The main window is the first to push, so it must be in the buttom of the stack.
void popOtherThanMain(){
    Window *w = window_stack_get_top_window();
    while(w != window_main){
	window_stack_pop(true);
	w = window_stack_get_top_window();
    }
}

//append a new item with text *data
void user_list_append(char *data) {
    if (user_item_count == MAX_NUM_USER_ITEMS) { 
	return;
    }

    strcpy(user_list_items[user_item_count], data);
    user_item_count++;

    menu_layer_reload_data(menu_user_list_layer);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "leaving user list append");
}

//send KEY_FETCH_USER_NAME to request user names and the phone send them to watch
void user_list_init(void) {
    user_item_count = 0;

    sendUint8(KEY_FETCH_USER_NAME, 0);
}

void sendUint8(int key, uint8_t value){
    DictionaryIterator *iter;

    if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
	return;
    }

    if (dict_write_uint8(iter, key, value) != DICT_OK) {
	return;
    }
    app_message_outbox_send();
}
//--------------- callbacks of menu layer ----------------------
int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    return 44;
}

int16_t get_cell_height_userlist_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    return 44;
}

void draw_row_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) {
    const int index = cell_index->row;

    menu_cell_basic_draw(ctx, cell_layer, main_menu[index], NULL, NULL);
}

void draw_row_userlist_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) {
    const int index = cell_index->row;

    menu_cell_basic_draw(ctx, cell_layer, user_list_items[index], NULL, NULL);
}

uint16_t get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return NUM_MAIN_MENU;
}

uint16_t get_num_rows_userlist_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return user_item_count;
}

void select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    const int index = cell_index->row;

    switch(index){
	case 0: //the training item
	    APP_LOG(APP_LOG_LEVEL_DEBUG, "entering case 0");
	    /*
	       window_training = window_create();
	       window_set_window_handlers(window_training, (WindowHandlers){
	       .load = window_training_load,
	       .unload = window_training_unload
	       });
	       window_stack_push(window_training, true);
	     */
	    myLoadWindow(&window_training, window_training_load, window_training_unload);
	    window_set_click_config_provider(window_training, click_config_provider_training);
	    sendUint8(KEY_TRAINING_MENU, 0);
	    /*
	    //in case that the previous window is data logging
	    APP_LOG(APP_LOG_LEVEL_DEBUG, "before data logging finish");
	    if(logSwitch == 1){ 
	    data_logging_finish(logging_session);
	    logSwitch = 0;
	    frequencyDevider = DEVIDER_LONG;//because now the watch is vibrating, we should omit the vibrating
	    counter = 0;
	    }
	    if(isAccRegistered == 1){
	    accel_data_service_unsubscribe();
	    }
	     */
	    APP_LOG(APP_LOG_LEVEL_DEBUG, "leaving case 0");

	    break;
	case 1: //the user list item
	    myLoadWindow(&window_user_list, window_user_list_load, window_user_list_unload);
	    window_set_click_config_provider(window_user_list, click_config_provider_user_list);

	    sendUint8(KEY_INTERACTION_MENU, 0);
	    user_list_init();
	    break;

	case 2: //the configuration item
	    myLoadWindow(&window_configuration, window_configuration_load, window_configuration_unload);
	    sendUint8(KEY_CONFIGURATION_MENU, 0);
	    break;



    }

    menu_layer_reload_data(menu_layer);
}

//--------------- back click handler to return to main menu----------------------
void back_click_handler_to_main_menu(Window* w){
    //myPopWindow(w);
    sendUint8(KEY_MAIN_MENU, 0);

    popOtherThanMain();
    myLoadWindow(&window_main, window_main_load, window_main_unload);
}

//--------------- click handlers for user list window ----------------------
void up_click_handler_userlist(ClickRecognizerRef recognizer, void *context){
    menu_layer_set_selected_next(menu_user_list_layer, true, MenuRowAlignTop, true);
}

void down_click_handler_userlist(ClickRecognizerRef recognizer, void *context){
    menu_layer_set_selected_next(menu_user_list_layer, false, MenuRowAlignTop, true);
}

void select_click_handler_userlist(ClickRecognizerRef recognizer, void *context){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "on select call back of user list window");
    MenuIndex mi = menu_layer_get_selected_index(menu_user_list_layer);
    const int index = mi.row;

    //send the index of selected user name
    sendUint8(KEY_SELECTED_USER_NAME, index);
    myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
    window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
    /*
    myLoadWindow(&window_interaction_data_logging, window_interaction_data_logging_load, window_interaction_data_logging_unload);
    window_set_click_config_provider(window_interaction_data_logging, click_config_provider_interaction_data_logging);
    */
    //myLoadWindow(&(window_interaction_continue.window), window_load_continue, window_unload_continue);
    //timer = app_timer_register(TIMER_PERIOD /* milliseconds */, timer_callback, NULL);
    switchOnDLProc(REPEATED_DATALOGGING);

    menu_layer_reload_data(menu_user_list_layer);
}

//--------------- back click handler for training window ----------------------
void back_click_handler_training(ClickRecognizerRef recognizer, void *context) {
    back_click_handler_to_main_menu(window_training);
}

//--------------- back click handler for data logging window ----------------------
void back_click_handler_data_logging(ClickRecognizerRef recognizer, void *context) {
    myPopWindow(window_data_logging);
    sendUint8(KEY_TRAINING_MENU, 0);

    //load training menu
    myLoadWindow(&window_training, window_training_load, window_training_unload);
    window_set_click_config_provider(window_training, click_config_provider_training);

    switchOffDLProc();
}

//--------------- back click handler for user list window ----------------------
void back_click_handler_userlist(ClickRecognizerRef recognizer, void *context) {
    back_click_handler_to_main_menu(window_user_list);
}

//--------------- click handlers for windows ----------------------
void click_config_provider_training(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_training);
}

void click_config_provider_data_logging(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_data_logging);
}

void click_config_provider_user_list(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_userlist);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_userlist);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_userlist);
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_userlist);
}

//--------------- connection window handler ----------------------
void window_connection_detection_load(Window* window) {
    my_create_text_layer(window, &connection_detection_layer, connection_error_info);
}

void window_connection_detection_unload(Window *window) {
    text_layer_destroy(connection_detection_layer);
}

//--------------- main window handler ----------------------
void window_main_load(Window* window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "main window is loading");
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);
    menu_main_layer = menu_layer_create(window_frame);
    menu_layer_set_callbacks(menu_main_layer, NULL, (MenuLayerCallbacks) {
	    .get_cell_height = (MenuLayerGetCellHeightCallback) get_cell_height_callback,
	    .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
	    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows_callback,
	    .select_click = (MenuLayerSelectCallback) select_callback,
	    });
    menu_layer_set_click_config_onto_window(menu_main_layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(menu_main_layer));
}

void window_main_unload(Window *window) {
    menu_layer_destroy(menu_main_layer);
}

//--------------- training window handler ----------------------
void window_training_load(Window* window) {
    my_create_text_layer(window, &training_layer, training_info);
}

void window_training_unload(Window *window) {
    text_layer_destroy(training_layer);
}

//--------------- configuration window handler ----------------------
void window_configuration_load(Window* window) {
    my_create_text_layer(window, &configuration_layer, configuration_info);
}

void window_configuration_unload(Window *window) {
    text_layer_destroy(configuration_layer);
}
//--------------- data logging window handler ----------------------
void window_data_logging_load(Window* window) {
    my_create_text_layer(window, &data_logging_layer, data_logging_info_start);
}

void window_data_logging_unload(Window *window) {
    text_layer_destroy(data_logging_layer);
}

//--------------- user list window handler ----------------------
void window_user_list_load(Window* window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);
    menu_user_list_layer = menu_layer_create(window_frame);
    menu_layer_set_callbacks(menu_user_list_layer, NULL, (MenuLayerCallbacks) {
	    .get_cell_height = (MenuLayerGetCellHeightCallback) get_cell_height_userlist_callback,
	    .draw_row = (MenuLayerDrawRowCallback) draw_row_userlist_callback,
	    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows_userlist_callback,
	    //.select_click = (MenuLayerSelectCallback) select_userlist_callback,
	    });
    //menu_layer_set_click_config_onto_window(menu_user_list_layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(menu_user_list_layer));
}

void window_user_list_unload(Window *window) {
    menu_layer_destroy(menu_user_list_layer);
}

void myWindowDestroy(Window* w){
    if(w != NULL){
	window_destroy(w);
    }
}

void deinitWindows() {
    // delete objects
    myWindowDestroy(window_main);
    myWindowDestroy(window_training);
    myWindowDestroy(window_configuration);
    myWindowDestroy(window_user_list);
    myWindowDestroy(window_data_logging);
    window_destroy_action_bar();
}

