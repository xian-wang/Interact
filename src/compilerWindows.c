#include "pebble.h"
#include "myWindows.h"
#include "compilerWindows.h"

//window_data_logging can be reused in the compiler, just override the back click handler

//--------------- resource definitions of action bars ----------------------
const int RESOURCE_IDS[4] = {
    RESOURCE_ID_IMAGE_ACTION_ICON_UNDO,
    RESOURCE_ID_IMAGE_ACTION_ICON_CANCEL,
    RESOURCE_ID_IMAGE_ACTION_ICON_CHECK,
    RESOURCE_ID_IMAGE_ACTION_ICON_RESTART
};

//names of action options appearing beside of action icons
char strActionOptions[5][20] = {
    "rollback",
    "cancel",
    "confirm",
    "negate",
    "restart"
};

char infoContinue[TEXT_LENGTH] = {'\0'};
char infoConfirm[TEXT_LENGTH] = {'\0'};
char infoFeedback[TEXT_LENGTH] = {'\0'};

//--------------- positions of text layers of action bars ----------------------
const GRect NUMBER_POSITIONS[3] =  {
    {{/* x: */ 60, /* y: */ 12 }, {/* width: */ 60, /* height: */ 28}},
    {{/* x: */ 60, /* y: */ 60 }, {/* width: */ 60, /* height: */ 28}},
    {{/* x: */ 60, /* y: */ 107 }, {/* width: */ 60, /* height: */ 28}}
};

//--------------- vibration patterns ----------------------
 // Vibe pattern: ON for 200ms, OFF for 100ms, ON for 200ms:
//const uint32_t const segmentsCorrect[] = { 200, 100, 200 };
const uint32_t const segmentsCorrect[] = { 100 };
VibePattern patCorrect = {
    .durations = segmentsCorrect,
    .num_segments = ARRAY_LENGTH(segmentsCorrect),
};
//vibes_enqueue_custom_pattern(pat);

 // Vibe pattern: ON for 200ms, OFF for 100ms, ON for 100ms:
const uint32_t const segmentsError[] = { 200, 100, 100 };
VibePattern patError = {
    .durations = segmentsError,
    .num_segments = ARRAY_LENGTH(segmentsError),
};

 // Vibe pattern: ON for 400ms:
const uint32_t const segmentsAccepted[] = { 400 };
VibePattern patAccepted = {
    .durations = segmentsAccepted,
    .num_segments = ARRAY_LENGTH(segmentsAccepted),
};

 // Vibe pattern: ON for 100ms, OFF for 100ms, ON for 200ms:
const uint32_t const segmentsFeedback[] = { 100, 100, 200 };
VibePattern patFeedback = {
    .durations = segmentsFeedback,
    .num_segments = ARRAY_LENGTH(segmentsFeedback),
};

WindowActionBar window_interaction_continue;
WindowActionBar window_interaction_confirm;
WindowActionBar window_interaction_feedback;

//--------------- click handlers for continue window ----------------------
void up_click_handler_continue(ClickRecognizerRef recognizer, void *context) {
    //rollback
    sendUint8(KEY_COMPILER_MSG, ROLLBACK);
    myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
    window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
    switchOnDLProc(REPEATED_DATALOGGING);
}

void select_click_handler_continue(ClickRecognizerRef recognizer, void *context) {
    /*
    // A wonderful bird is the pelican
    // His bill will hold more than his belican
    // He can take in his beak
    // Food enough for a week,
    // But I'm damned if I see how the helican
    AnimalData *animal_data = &s_animal_datas[1];
    count_animal(animal_data);
     */
}

void down_click_handler_continue(ClickRecognizerRef recognizer, void *context) {
    //cancel all gestures
    sendUint8(KEY_COMPILER_MSG, CANCEL_RESTART);
    myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
    window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
    switchOnDLProc(REPEATED_DATALOGGING);
}

//--------------- click handlers for confirm window ----------------------
void up_click_handler_confirm(ClickRecognizerRef recognizer, void *context) {
    //confirm
    sendUint8(KEY_COMPILER_MSG, CONFIRM);
    myLoadWindow(&(window_interaction_feedback.window), window_load_feedback, window_unload_feedback);
    window_set_click_config_provider(window_interaction_feedback.window, config_provider_feedback);
}

void select_click_handler_confirm(ClickRecognizerRef recognizer, void *context) {
    //negate
    sendUint8(KEY_COMPILER_MSG, NEGATE);
    myLoadWindow(&(window_interaction_feedback.window), window_load_feedback, window_unload_feedback);
    window_set_click_config_provider(window_interaction_feedback.window, config_provider_feedback);
}

void down_click_handler_confirm(ClickRecognizerRef recognizer, void *context) {
    //rollback
    sendUint8(KEY_COMPILER_MSG, ROLLBACK);
    myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
    window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
    switchOnDLProc(REPEATED_DATALOGGING);
}

//--------------- click handlers for feedback window ----------------------
void select_click_handler_feedback(ClickRecognizerRef recognizer, void *context) {
    //restart
    sendUint8(KEY_COMPILER_MSG, CANCEL_RESTART);
    myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
    window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
    switchOnDLProc(REPEATED_DATALOGGING);
}

//--------------- back click handlers ----------------------
void back_click_handler_continue(ClickRecognizerRef recognizer, void *context) {
    back_click_handler_to_userlist(window_interaction_continue.window);
}

void back_click_handler_confirm(ClickRecognizerRef recognizer, void *context) {
    back_click_handler_to_userlist(window_interaction_confirm.window);
}

void back_click_handler_feedback(ClickRecognizerRef recognizer, void *context) {
    back_click_handler_to_userlist(window_interaction_feedback.window);
}

void config_provider_continue(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_continue);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_continue);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_continue);
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_continue);
}

void config_provider_confirm(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_confirm);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_confirm);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_confirm);
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_confirm);
}

void config_provider_feedback(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_feedback);
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_feedback);
}

//initialize text layers and bitmap resources of action bar
void init_action_elements(WindowActionBar *wab, int numElements, int* actionOptionsIdx, int* gRectIdx, int* resourceIdx) {
    for (int i = 0; i < numElements; i++) {
	wab->actionBarElement[i].text_layer = text_layer_create(NUMBER_POSITIONS[gRectIdx[i]]);
	layer_add_child(window_get_root_layer(wab->window), text_layer_get_layer(wab->actionBarElement[i].text_layer));
	wab->actionBarElement[i].bitmap = gbitmap_create_with_resource(RESOURCE_IDS[resourceIdx[i]]);
	//	text_layer_set_text(wab->actionBarElement[i].text_layer, *strActionOptions + actionOptionsIdx[i]);
	text_layer_set_font(wab->actionBarElement[i].text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    }
}

//initialize action bar layer and set click handler and icons
void action_bar_init(WindowActionBar *wab, ClickConfigProvider config_provider, ButtonId* buttonId, int numElements) {
    wab->action_bar_layer = action_bar_layer_create();
    action_bar_layer_add_to_window(wab->action_bar_layer, wab->window);
    action_bar_layer_set_click_config_provider(wab->action_bar_layer, config_provider);
    for (int i = 0; i < numElements; i++) {
	action_bar_layer_set_icon(wab->action_bar_layer, buttonId[i], wab->actionBarElement[i].bitmap);
    }
}

//de-initialize action bar elements including text layer and bitmap
void deinit_action_elements(WindowActionBar *wab, int numElements) {
    for (int i = 0; i < numElements; i++) {
	text_layer_destroy(wab->actionBarElement[i].text_layer);
	gbitmap_destroy(wab->actionBarElement[i].bitmap);
    }
}

//function to create and set text the text layer of the window containing action bar
void action_bar_text_layer_setting(TextLayer** textLayer, Window* window, char* info){
    *textLayer = text_layer_create(GRect(7, 40, 100, 93));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(*textLayer));
    text_layer_set_font(*textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text(*textLayer, info);
    text_layer_set_text_alignment(*textLayer, GTextAlignmentCenter);
}

//--------------- load and unload methods of window of continue ----------------------
void window_load_continue(Window *window) {
    int numElements = 2; //the action bar has 2 elements: rollback and cancel
    int actionOptionsIdx[2] = {0, 1}; //the indices of words of names of action options
    int gRectIdx[2] = {0, 2}; // the indices of NUMBER_POSITIONS, which specifies the positions of text layers of action bar
    int resourceIdx[2] = {0, 1}; // the indices of RESOURCE_IDS, which specifies the bitmap of one option
    init_action_elements(&window_interaction_continue, numElements, actionOptionsIdx, gRectIdx, resourceIdx);

    ButtonId buttonId[2]= {BUTTON_ID_UP, BUTTON_ID_DOWN};
    action_bar_init(&window_interaction_continue, config_provider_continue, buttonId, numElements);

    action_bar_text_layer_setting(&(window_interaction_continue.text_layer), window_interaction_continue.window, infoContinue);
}

void window_unload_continue(Window *window) {
    int numElements = 2; //the action bar has 2 elements: rollback and cancel
    deinit_action_elements(&window_interaction_continue, numElements);

    action_bar_layer_destroy(window_interaction_continue.action_bar_layer);
    text_layer_destroy(window_interaction_continue.text_layer);
}

//--------------- load and unload methods of window of confirm ----------------------
void window_load_confirm(Window *window) {
    int numElements = 3; //the action bar has 3 elements: confirm, cancel and rollback
    int actionOptionsIdx[3] = {2, 1, 0}; //the indices of words of names of action options
    int gRectIdx[3] = {0, 1, 2}; // the indices of NUMBER_POSITIONS, which specifies the positions of text layers of action bar
    int resourceIdx[3] = {2, 1, 0}; // the indices of RESOURCE_IDS, which specifies the bitmap of one option
    init_action_elements(&window_interaction_confirm, numElements, actionOptionsIdx, gRectIdx, resourceIdx);

    ButtonId buttonId[3]= {BUTTON_ID_UP, BUTTON_ID_SELECT, BUTTON_ID_DOWN};
    action_bar_init(&window_interaction_confirm, config_provider_confirm, buttonId, numElements);

    action_bar_text_layer_setting(&(window_interaction_confirm.text_layer), window_interaction_confirm.window, infoConfirm);
}

void window_unload_confirm(Window *window) {
    int numElements = 3; //the action bar has 3 elements: confirm, cancel and rollback 
    deinit_action_elements(&window_interaction_confirm, numElements);

    action_bar_layer_destroy(window_interaction_confirm.action_bar_layer);
    text_layer_destroy(window_interaction_confirm.text_layer);
}

//--------------- load and unload methods of window of feedback ----------------------
void window_load_feedback(Window *window) {
    int numElements = 1; //the action bar has 1 element: restart
    int actionOptionsIdx[1] = {3}; //the indices of words of names of action options
    int gRectIdx[1] = {1}; // the indices of NUMBER_POSITIONS, which specifies the positions of text layers of action bar
    int resourceIdx[1] = {3}; // the indices of RESOURCE_IDS, which specifies the bitmap of one option
    init_action_elements(&window_interaction_feedback, numElements, actionOptionsIdx, gRectIdx, resourceIdx);

    ButtonId buttonId[1]= {BUTTON_ID_SELECT};
    action_bar_init(&window_interaction_feedback, config_provider_feedback, buttonId, numElements);

    action_bar_text_layer_setting(&(window_interaction_feedback.text_layer), window_interaction_feedback.window, infoFeedback);
}

void window_unload_feedback(Window *window) {
    int numElements = 1; //the action bar has 1 element: restart
    deinit_action_elements(&window_interaction_feedback, numElements);

    action_bar_layer_destroy(window_interaction_feedback.action_bar_layer);
    text_layer_destroy(window_interaction_feedback.text_layer);
}


//--------------- destroy windows ----------------------
void window_destroy_action_bar(){
    myWindowDestroy(window_interaction_continue.window);
    myWindowDestroy(window_interaction_confirm.window);
    myWindowDestroy(window_interaction_feedback.window);
}

//--------------- back click handler to return to user list window ----------------------
void back_click_handler_to_userlist(Window *w){
    myPopWindow(w);
    sendUint8(KEY_INTERACTION_MENU, 0);

    //load userlist menu
    myLoadWindow(&window_user_list, window_user_list_load, window_user_list_unload);
    window_set_click_config_provider(window_user_list, click_config_provider_user_list);

    switchOffDLProc();
}
//--------------- back click handler for interaction data logging window ----------------------
void back_click_handler_interaction_data_logging(ClickRecognizerRef recognizer, void *context) {
    back_click_handler_to_userlist(window_data_logging);
}

void click_config_provider_interaction_data_logging(void *ctx) {
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_interaction_data_logging);
}



