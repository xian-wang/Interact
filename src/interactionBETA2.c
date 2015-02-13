#include "pebble.h"
#include "myWindows.h"
#include "datalogging.h"
#include "compilerWindows.h"

void showErrorMessageRemovingDL(char* info){ // disable data logging and load data logging window but just for showing text information
    switchOffDLProc();
	myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
	window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
	text_layer_set_text(data_logging_layer, info);
	vibes_enqueue_custom_pattern(patError);
}
    
void compilerMsgProc(int msg){
    switch(msg){
	case ACCEPTED: // the gesture chain is accepted, load the confirm window
	    switchOffDLProc();
	    myLoadWindow(&(window_interaction_confirm.window), window_load_confirm, window_unload_confirm);
	    text_layer_set_text(window_interaction_confirm.text_layer, "ACCEPTED");
	    vibes_enqueue_custom_pattern(patAccepted);
	    break;
	case NON_ACC_NON_COMPERR:
	    break;
	case NON_ACC_COMPERR:
	    break;
	case GEST_NAME:
	    break;
	case CONTINUE:
	    break;
	case ROLLBACK:
	    break;    
	case CANCEL_RESTART:
	    break;
	case CONFIRM:
	    break;
	case NEGATE:
	    break;
	case RECV_FEEDBACK:
	    break;
    }

}

// the handler to process all the received messages
static void in_received_handler(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "received");
    Tuple *main_menu_tuple = dict_find(iter, KEY_MAIN_MENU);
    Tuple *training_menu_tuple = dict_find(iter, KEY_TRAINING_MENU);
    Tuple *control_menu_tuple = dict_find(iter, KEY_INTERACTION_MENU);
    Tuple *configuration_menu_tuple = dict_find(iter, KEY_CONFIGURATION_MENU);

    Tuple *data_logging_tuple = dict_find(iter, KEY_DATA_LOGGING_REQUEST);
    Tuple *append_user_name_tuple = dict_find(iter, KEY_APPEND_USER_NAME);
    Tuple *interaction_data_logging_tuple = dict_find(iter, KEY_INTERACTION_DL);

    Tuple *compiler_msg_tuple= dict_find(iter, KEY_COMPILER_MSG);
    Tuple *compiler_nacc_nerr_tuple= dict_find(iter, KEY_NON_ACC_NON_COMPERR);
    Tuple *compiler_nacc_err_tuple= dict_find(iter, KEY_NON_ACC_COMPERR);
    Tuple *command_ack_tuple= dict_find(iter, KEY_COMMAND_ACK);
    Tuple *connection_error_tuple= dict_find(iter, KEY_CONNECTION_ERROR);
    Tuple *motion_too_short_error_tuple= dict_find(iter, KEY_MOTION_TOO_SHORT_ERROR);
    Tuple *user_related_error_tuple= dict_find(iter, KEY_USER_RELATED_ERROR);

    if (main_menu_tuple) {
    	//in case that the previous window is data logging
    	switchOffDLProc();
    	popOtherThanMain();
    	myLoadWindow(&window_main, window_main_load, window_main_unload);
    }

    if (training_menu_tuple) {
	//load training menu
	APP_LOG(APP_LOG_LEVEL_DEBUG, "entering interaction_loading training menu");

	myLoadWindow(&window_training, window_training_load, window_training_unload);
	window_set_click_config_provider(window_training, click_config_provider_training);

	//in case that the previous window is data logging
	switchOffDLProc();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "leaving interaction_loading training menu");
    }

    if (control_menu_tuple) {
        //load data logging menu        
        myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
        window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
        switchOnDLProc(REPEATED_DATALOGGING);
        /*
	myLoadWindow(&window_user_list, window_user_list_load, window_user_list_unload);
	window_set_click_config_provider(window_user_list, click_config_provider_user_list);
	user_list_init();
	switchOffDLProc();
    */
    }

    if (data_logging_tuple) {
	//load data logging menu
	myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
	window_set_click_config_provider(window_data_logging, click_config_provider_data_logging);

        switchOnDLProc(true);//switch on datalogging process in repeated manner
    }

    if (append_user_name_tuple) {
	user_list_append(append_user_name_tuple->value->cstring);
    }

    if (interaction_data_logging_tuple) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "entering interaction data logging");
	//if(window_stack_get_top_window() == window_user_list){//only when the current window is user list
	    myLoadWindow(&window_data_logging, window_data_logging_load, window_data_logging_unload);
	    window_set_click_config_provider(window_data_logging, click_config_provider_interaction_data_logging);
	    switchOnDLProc(REPEATED_DATALOGGING);
	//}
	/*
	   myLoadWindow(&window_interaction_data_logging, window_interaction_data_logging_load, window_interaction_data_logging_unload);
	   window_set_click_config_provider(window_interaction_data_logging, click_config_provider_interaction_data_logging);
	 */

    }

    if (compiler_msg_tuple) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "entering compiler message branch");
	compilerMsgProc(compiler_msg_tuple->value->uint8);
    }

    if (compiler_nacc_nerr_tuple) {
	//in case that the previous window is data logging
	//deRegisterDatalogging();
	switchOffDLProc();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "entering continue window loading without error");
	myLoadWindow(&(window_interaction_continue.window), window_load_continue, window_unload_continue);
	text_layer_set_text(window_interaction_continue.text_layer, compiler_nacc_nerr_tuple->value->cstring);
	vibes_enqueue_custom_pattern(patCorrect);
	switchOnDLProc(REPEATED_DATALOGGING);
    }
    if (compiler_nacc_err_tuple) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "entering compiler message branch with error");
	switchOffDLProc();
	myLoadWindow(&(window_interaction_continue.window), window_load_continue, window_unload_continue);
	text_layer_set_text(window_interaction_continue.text_layer, compiler_nacc_err_tuple->value->cstring);

	vibes_enqueue_custom_pattern(patError);
    switchOnDLProc(REPEATED_DATALOGGING);
	/*
	   char msg[50]; 
	//	strcpy(msg, compiler_nacc_err_tuple->value->cstring);
	snprintf(msg, 50, "msg");
	text_layer_set_text(window_interaction_continue.text_layer, msg);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "after strcpy");
	APP_LOG(APP_LOG_LEVEL_DEBUG, msg);
	strcat(msg, "\nCompile error");
	//text_layer_set_text(window_interaction_continue.text_layer, strcat(msg, "\nCompile error"));
	text_layer_set_text(window_interaction_continue.text_layer, msg);
	APP_LOG(APP_LOG_LEVEL_DEBUG, msg);
	 */
    }
    if (command_ack_tuple) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "entering feedback branch");
	myLoadWindow(&(window_interaction_feedback.window), window_load_feedback, window_unload_feedback);
	int value = command_ack_tuple->value->uint8;
	char msg[50]; 
	switch(value){
	    case 1:
		//snprintf(msg, 50, "COMMAND CONFIRMED");
		text_layer_set_text(window_interaction_feedback.text_layer, "COMMAND CONFIRMED");
		break;
	    case 2: 
		//snprintf(msg, 50, "COMMAND CANCELED");
		text_layer_set_text(window_interaction_feedback.text_layer, "COMMAND CANCELED");
		break;
	}

	vibes_enqueue_custom_pattern(patFeedback);

    }
    if (connection_error_tuple) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "connection_error_tuple value = %d", connection_error_tuple -> value -> uint8);
	if(connection_error_tuple->value->uint8 == 0){
	    showErrorMessageRemovingDL("Connection error, press BACK to retry");
	}
	else if(connection_error_tuple->value->uint8 == 1){
        showErrorMessageRemovingDL("Receive timeout, press BACK to retry");
	}
	else if(connection_error_tuple->value->uint8 == 2){
        showErrorMessageRemovingDL("Server may be closed, press BACK to retry");
	}
    }
    if(motion_too_short_error_tuple){
        showErrorMessageRemovingDL("Motion too short, do the gesture again");
	    switchOnDLProc(REPEATED_DATALOGGING);
    }
    
    if(user_related_error_tuple){
        if(user_related_error_tuple->value->uint8 == 0){
            showErrorMessageRemovingDL("No user logged in");
        }
        else if(user_related_error_tuple->value->uint8 == 1){
            showErrorMessageRemovingDL("No trained data");
        }
    }
}

static void appMessageInit(void) {
    // Reduce the sniff interval for more responsive messaging at the expense of
    // increased energy consumption by the Bluetooth module
    // The sniff interval will be restored by the system after the app has been
    // unloaded
    app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
    // Register message handlers
    app_message_register_inbox_received(in_received_handler);
    // Init buffers    
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static bool checkConnection(){
    DictionaryIterator *iter;
    bool flag = true;
    AppMessageResult r = app_message_outbox_begin(&iter);
    if (r == APP_MSG_NOT_CONNECTED || r == APP_MSG_APP_NOT_RUNNING) {
	flag = false;
    }

    if (dict_write_uint8(iter, KEY_TEST, 0) != DICT_OK) {
	flag = false;
    }
    AppMessageResult r2 = app_message_outbox_send();
    if (r2 == APP_MSG_NOT_CONNECTED || r2 == APP_MSG_APP_NOT_RUNNING) {
	flag = false;
    }

    return flag;
}

static void init() {  
    appMessageInit();

    if(! checkConnection()){//the phone is not connected or the app on the phone is not running
	window_main = window_create();
	window_set_window_handlers(window_main, (WindowHandlers) {
		.load = window_connection_detection_load,
		.unload = window_connection_detection_unload
		});
	window_stack_push(window_main, true /* Animated */);
    }
    else{
	// configure window

	//myLoadWindow(&window_main, window_main_load, window_main_unload);
	window_main = window_create();
	window_set_window_handlers(window_main, (WindowHandlers) {
		.load = window_main_load,
		.unload = window_main_unload
		});
	window_stack_push(window_main, true );

    }

}

static void deinit() {
    // delete objects
    switchOffDLProc();
    deregisterAcc();
    deinitWindows();
}

int main(void) {
    init();

    // init done, start event loop
    app_event_loop();

    deinit();
}
