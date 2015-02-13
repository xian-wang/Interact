#ifndef MY_WINDOWS_H
#define MY_WINDOWS_H

#include "pebble.h"

enum {
    KEY_MAIN_MENU,
    KEY_TRAINING_MENU,
    KEY_INTERACTION_MENU,
    KEY_CONFIGURATION_MENU,
    KEY_DATA_LOGGING_REQUEST,
    KEY_FETCH_USER_NAME,
    KEY_APPEND_USER_NAME,
    KEY_SELECTED_USER_NAME,
    KEY_INTERACTION_DL,
    KEY_COMPILER_MSG,
    KEY_NON_ACC_NON_COMPERR,
    KEY_NON_ACC_COMPERR,
    KEY_COMMAND_ACK,
    KEY_CONNECTION_ERROR,
    KEY_MOTION_TOO_SHORT_ERROR,
    KEY_USER_RELATED_ERROR,
    KEY_TEST
};

#define ACCEPTED 0
#define NON_ACC_NON_COMPERR 1
#define NON_ACC_COMPERR 2
#define GEST_NAME 3
#define CONTINUE 4
#define ROLLBACK 5
#define CANCEL_RESTART 6
#define CONFIRM 7
#define NEGATE 8
#define RECV_FEEDBACK 9


//other definitions
#define MAX_NUM_USER_ITEMS (20)
#define MAX_ITEM_USER_LENGTH (20)
#define MAX_ITEM_TEXT_LENGTH (16)
#define NUM_MAIN_MENU 3

extern Window *window_main;
extern Window *window_training;
extern Window *window_configuration;
extern Window *window_user_list;
extern Window *window_data_logging;
extern TextLayer *connection_detection_layer;
extern TextLayer *training_layer;
extern TextLayer *configuration_layer;
extern TextLayer *data_logging_layer;
extern TextLayer *interaction_data_logging_layer;
extern MenuLayer *menu_main_layer;
extern MenuLayer *menu_user_list_layer;

extern char user_list_items[MAX_NUM_USER_ITEMS][MAX_ITEM_USER_LENGTH];
extern int user_item_count;

extern char main_menu[NUM_MAIN_MENU][MAX_ITEM_TEXT_LENGTH];
extern char *connection_error_info; 
extern char *training_info;
extern char *configuration_info;
extern char *data_logging_info_start;
extern char *data_logging_info_restart;
extern char *data_logging_info_collecting;

void myPopWindow(Window* window);

void my_create_text_layer(Window* window, TextLayer** tl, char* info);

//basic operations to load a window, without click handlers registration
//void myLoadWindow(Window* window, WindowHandlers whdls);
void myLoadWindow(Window** window, WindowHandler loadW, WindowHandler unloadW);

void popOtherThanMain();
void user_list_append(char *data);
void user_list_init(void);
void sendUint8(int key, uint8_t value);

//--------------- callbacks of menu layer ----------------------
int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data);

int16_t get_cell_height_userlist_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data);

void draw_row_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) ;

void draw_row_userlist_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data);

uint16_t get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data);

uint16_t get_num_rows_userlist_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data);

void select_userlist_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);

void select_long_userlist_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);

void select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);

//--------------- back click handler to return to main menu----------------------
void back_click_handler_to_main_menu(Window* w);

//--------------- back click handler for training window ----------------------
void back_click_handler_training(ClickRecognizerRef recognizer, void *context);

//--------------- back click handler for data logging window ----------------------
void back_click_handler_data_logging(ClickRecognizerRef recognizer, void *context);

//--------------- back click handler for user list window ----------------------
void back_click_handler_userlist(ClickRecognizerRef recognizer, void *context);

void click_config_provider_training(void *ctx);

void click_config_provider_data_logging(void *ctx);

void click_config_provider_user_list(void *ctx);

//--------------- connection window handler ----------------------
void window_connection_detection_load(Window* window);

void window_connection_detection_unload(Window *window);

//--------------- main window handler ----------------------
void window_main_load(Window* window);

void window_main_unload(Window *window);

//--------------- training window handler ----------------------
void window_training_load(Window* window);

void window_training_unload(Window *window);

//--------------- configuration window handler ----------------------
void window_configuration_load(Window* window);

void window_configuration_unload(Window *window);

//--------------- data logging window handler ----------------------
void window_data_logging_load(Window* window);

void window_data_logging_unload(Window *window);

//--------------- user list window handler ----------------------
void window_user_list_load(Window* window);

void window_user_list_unload(Window* window);

void myWindowDestroy(Window* w);

void deinitWindows();

#endif
