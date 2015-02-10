#ifndef COMPILER_WINDOWS_H
#define COMPILER_WINDOWS_H

#include "pebble.h"
#include "myWindows.h"
#include "datalogging.h"

//--------------- resource definitions of action bars ----------------------
extern const int RESOURCE_IDS[4];

//names of action options appearing beside of action icons
extern char strActionOptions[5][20];

extern char infoContinue[TEXT_LENGTH];
extern char infoConfirm[TEXT_LENGTH];
extern char infoFeedback[TEXT_LENGTH];

//--------------- positions of text layers of action bars ----------------------
extern const GRect NUMBER_POSITIONS[3];

typedef struct {
  TextLayer *text_layer;
  char text[20];
  GBitmap *bitmap;
} ActionBarElement;

typedef struct {
  Window *window;
  ActionBarElement actionBarElement[3]; 
  ActionBarLayer *action_bar_layer;
  TextLayer *text_layer;
} WindowActionBar;


//--------------- vibration patterns ----------------------
extern const uint32_t const segmentsCorrect[];
extern VibePattern patCorrect;
extern const uint32_t const segmentsError[];
extern VibePattern patError;
extern const uint32_t const segmentsAccepted[];
extern VibePattern patAccepted;
extern const uint32_t const segmentsFeedback[];
extern VibePattern patFeedback;

extern WindowActionBar window_interaction_continue;
extern WindowActionBar window_interaction_confirm;
extern WindowActionBar window_interaction_feedback;

//--------------- click handlers for continue window ----------------------
void up_click_handler_continue(ClickRecognizerRef recognizer, void *context);
void select_click_handler_continue(ClickRecognizerRef recognizer, void *context);
void down_click_handler_continue(ClickRecognizerRef recognizer, void *context);

//--------------- click handlers for confirm window ----------------------
void up_click_handler_confirm(ClickRecognizerRef recognizer, void *context);
void select_click_handler_confirm(ClickRecognizerRef recognizer, void *context);
void down_click_handler_confirm(ClickRecognizerRef recognizer, void *context); 

//--------------- click handlers for feedback window ----------------------
void select_click_handler_feedback(ClickRecognizerRef recognizer, void *context);

//--------------- back click handlers ----------------------
void back_click_handler_continue(ClickRecognizerRef recognizer, void *context);
void back_click_handler_confirm(ClickRecognizerRef recognizer, void *context);
void back_click_handler_feedback(ClickRecognizerRef recognizer, void *context);

void config_provider_continue(void *ctx);
void config_provider_confirm(void *ctx);
void config_provider_feedback(void *ctx);

//initialize text layers and bitmap resources of action bar
void init_action_elements(WindowActionBar *wab, int numElements, int* actionOptionsIdx, int* gRectIdx, int* resourceIdx);

//initialize action bar layer and set click handler and icons
void action_bar_init(WindowActionBar *wab, ClickConfigProvider config_provider, ButtonId* buttonId, int numElements);

//de-initialize action bar elements including text layer and bitmap
void deinit_action_elements(WindowActionBar *wab, int numElements);

//function to create and set text the text layer of the window containing action bar
void action_bar_text_layer_setting(TextLayer** textLayer, Window* window, char* info);

//--------------- load and unload methods of window of continue ----------------------
void window_load_continue(Window *window);

void window_unload_continue(Window *window);

//--------------- load and unload methods of window of confirm ----------------------
void window_load_confirm(Window *window);
void window_unload_confirm(Window *window);

//--------------- load and unload methods of window of feedback ----------------------
void window_load_feedback(Window *window);
void window_unload_feedback(Window *window);

//--------------- destroy windows ----------------------
void window_destroy_action_bar();

//--------------- back click handler to return to user list window ----------------------
void back_click_handler_to_userlist(Window *w);

//--------------- back click handler for interaction data logging window ----------------------
void back_click_handler_interaction_data_logging(ClickRecognizerRef recognizer, void *context);
void click_config_provider_interaction_data_logging(void *ctx);
#endif
