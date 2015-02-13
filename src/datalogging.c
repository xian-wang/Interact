#include "pebble.h"
#include "datalogging.h"

char text[TEXT_LENGTH];

const uint32_t LOG_TAG = 0x2;

DataLoggingSessionRef logging_session;

AppTimer *timer;

int initialAcc[3] = {0, 0, 1}; //the direction of positive z axis

int logSwitch = 0; //the switch of collecting data

int frequencyDevider = DEVIDER_SHORT; //used to control the frequency of wrist turn detection

int counter = 0; //counter of accel samples

int isAccRegistered = 0; //whether the accel data service is registered

double loopQueue[2][QUEUE_LENGTH]; //round-robin queue to monitor the module of acceleration, and its square

int qIdx = 0; //index of round-robin queue

int globalCounter = 0; //counter the number of samples sent when the watch is not vibrating

double lastMean = 0;

double lastMean2 = 0;

bool isRepeated = false;

bool softSwitchAcc = true;

// ----------------- data logging ----------------------
double sum(double * vec, int length){
    double s = 0;
    for(int i=0;i<length;i++){
	s += vec[i];
    }
    return s;
}

double myMean(double lastMean, double oldest, double newest, int length){
    double mean = (lastMean * length - oldest + newest) / length;

    return mean;
}

double mySd(double mean, double mean2){
    double sd = mean2 - mean * mean;

    return sd;
}

void initQueue(){
    qIdx = 0;
    lastMean = 0;
    lastMean2 = 0;
    for(int i=0;i<QUEUE_LENGTH;i++){
	loopQueue[0][i] = 0;
	loopQueue[1][i] = 0;
    }
}

//deregister accel service if it's open
void deregisterAcc(){
    if(isAccRegistered == 1){
	accel_data_service_unsubscribe();
	isAccRegistered = 0;
    }
}

//close datalogging session if it's open, switch off the variable softSwitchAcc
void deregisterDatalogging(){
    if(logSwitch == 1){ 
	data_logging_finish(logging_session);
	logSwitch = 0;
	frequencyDevider = DEVIDER_LONG;//because now the watch is vibrating, we should omit the vibrating
	counter = 0;
    }
}

void registerAcc(){
    if(isAccRegistered == 0){
	accel_data_service_subscribe(NUM_SAMPLES, &accel_data_handler);
	accel_service_set_sampling_rate(SAMPLING_RATE);
	isAccRegistered = 1;
    } 
}

//switch on datalogging process, the parameter determines whether the process is repeatable or only once
void switchOnDLProc(bool flag){
    isRepeated = flag;// 0 is not repeated, 1 is repeated
    softSwitchAcc = true;//the first time it is always true
    registerAcc();
}

//switch off datalogging process, deregister datalogging process and switch off the variable softSwitchAcc
void switchOffDLProc(){
    deregisterDatalogging();
    softSwitchAcc = false;
}

void accel_data_handler(AccelData *accel_data, uint32_t num_samples) {
    if(logSwitch == 1){ //send data only when the data logging is on
	// Adding log of accel data 
	DataLoggingResult r = data_logging_log(logging_session, accel_data, num_samples);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "logging status: %d", r);

	double oldest[2][NUM_SAMPLES];
	double newest[2][NUM_SAMPLES];
	for(uint32_t i=0;i < num_samples; i++){
	    double acc[3];
	    acc[0] = (double)(accel_data+i)->x / 1000 * 9.8;
	    acc[1] = (double)(accel_data+i)->y / 1000 * 9.8;
	    acc[2] = (double)(accel_data+i)->z / 1000 * 9.8;

	    APP_LOG(APP_LOG_LEVEL_DEBUG, "accel[0]= %d, accel[1]=%d, accel[2]=%d",(accel_data+i)->x,(accel_data+i)->y,(accel_data+i)->z);
	    APP_LOG(APP_LOG_LEVEL_DEBUG, "acc[0]= %d, acc[1]=%d, acc[2]=%d",(int)acc[0],(int)acc[1],(int)acc[2]);

	    oldest[0][i] = loopQueue[0][qIdx];
	    oldest[1][i] = loopQueue[1][qIdx];

	    loopQueue[0][qIdx] = acc[0] * acc[0] + acc[1] * acc[1] + acc[2] * acc[2];//square of the module of acceleration
	    loopQueue[1][qIdx] = loopQueue[0][qIdx] * loopQueue[0][qIdx]; 

	    newest[0][i] = loopQueue[0][qIdx];
	    newest[1][i] = loopQueue[1][qIdx];

	    qIdx = (qIdx + 1) % QUEUE_LENGTH;
	    if((accel_data+i)->did_vibrate == false){
		globalCounter ++;
	    }
	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "oldest = %d, newest = %d", (int)oldest[0][0],(int)newest[0][0]);
    // the update to the round robin queue is that num_samples of new buffered accel data replace num_samples of oldest data in this queue
	double mean = myMean(lastMean, sum(*oldest,num_samples), sum(*newest,num_samples), QUEUE_LENGTH);
	lastMean = mean;
	double mean2 = myMean(lastMean2, sum(*(oldest+1),num_samples), sum(*(newest+1),num_samples), QUEUE_LENGTH);
	lastMean2 = mean2;
	double sd = mySd(mean, mean2);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "mean = %d, sd = %d", (int)mean,(int)sd);

	if(mean > L_BOUND_GRAVITY_2 && mean < U_BOUND_GRAVITY_2 && sd < BOUND_S_DEVIATION && globalCounter > MINI_SAMPLES){ 
	    //if the wrist is static, stop recording
	    vibes_short_pulse();
	    AccelData myAcc;
	    myAcc.x = OUT_BOUND; myAcc.y = OUT_BOUND; myAcc.z = OUT_BOUND;
	    myAcc.did_vibrate = false; myAcc.timestamp = 0;
	    DataLoggingResult r = data_logging_log(logging_session, &myAcc, 1); //send a out-bound data to notify the end

	    data_logging_finish(logging_session);
	    APP_LOG(APP_LOG_LEVEL_DEBUG, "logging_session=  %p", logging_session);

	    logSwitch = 0;
	    frequencyDevider = DEVIDER_LONG;//because now the watch is vibrating, we should omit the vibrating
	    counter = 0;

/*
	    //restart the accelerometer service
	    if(isAccRegistered == 1){
		accel_data_service_unsubscribe();
	    }
	    accel_data_service_subscribe(NUM_SAMPLES, &accel_data_handler);
	    accel_service_set_sampling_rate(SAMPLING_RATE);
	    isAccRegistered = 1;
*/
	    if(isRepeated == true){
		text_layer_set_text(data_logging_layer, data_logging_info_restart);
		softSwitchAcc = true;
	    }
	    else{
		text_layer_set_text(data_logging_layer, data_logging_info_waiting);
		softSwitchAcc = false;
	    }
	} 

    }

    else{
	counter = (counter + 1) % frequencyDevider;//frequencyDevider is used to control the frequency of wrist turn detection
	if(counter % frequencyDevider == 0){
	    if(frequencyDevider == DEVIDER_LONG){
		frequencyDevider = DEVIDER_SHORT;//change to the detection with higher frequency
		counter = 0;
	    }
	    //int product = acc[0] * initialAcc[0] + acc[1] * initialAcc[1] + acc[2] * initialAcc[2];
	    int product = (accel_data+num_samples-1)->x * initialAcc[0] + (accel_data+num_samples-1)->y * initialAcc[1] + (accel_data+num_samples-1)->z * initialAcc[2];
	    //float moduleProduct = module(initialAcc[0],initialAcc[1],initialAcc[2]) * module(accel.x,accel.y,accel.z);
	    //APP_LOG(APP_LOG_LEVEL_DEBUG, "dot product = %d, acc={%ld,%ld,%ld}",product,acc[0],acc[1],acc[2]);
	    //APP_LOG(APP_LOG_LEVEL_DEBUG, "dot product = %d, accel_data={%d,%d,%d}",product,(accel_data+i)->x,(accel_data+i)->y,(accel_data+i)->z);

	    APP_LOG(APP_LOG_LEVEL_DEBUG, "dot product = %d, accel_data={%d,%d,%d}",product,(accel_data+num_samples-1)->x,(accel_data+num_samples-1)->y,(accel_data+num_samples-1)->z);
	    if(product > 0 && softSwitchAcc){//wrist turn is detected and softSwitchAcc is true
		vibes_short_pulse();
		//timerPeriod = TIMER_PERIOD_LONG;

		if(logSwitch == 0){
		    logging_session = data_logging_create(LOG_TAG, DATA_LOGGING_BYTE_ARRAY, sizeof(AccelData), true);
		    APP_LOG(APP_LOG_LEVEL_DEBUG, "on creation: logging_session=  %p", logging_session);

		    registerAcc();

		    logSwitch = 1;
		    initQueue();
		    globalCounter = 0;

		    APP_LOG(APP_LOG_LEVEL_DEBUG, "before setting text");
		    /*
		    char text[TEXT_LENGTH]; 
		    snprintf(text, TEXT_LENGTH, "Collecting data...");
		    text_layer_set_text(data_logging_layer, text);
		    */
		    text_layer_set_text(data_logging_layer, data_logging_info_collecting);
		    APP_LOG(APP_LOG_LEVEL_DEBUG, "after setting text");
		}
		//frequencyDevider = DEVIDER_LONG;
		//counter = 0;
	    }
	}

    }
}
void timer_callback(void *data){
    registerAcc();
}

