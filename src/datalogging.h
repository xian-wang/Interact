#ifndef DATALOGGING_H
#define DATALOGGING_H

#include "pebble.h"

#define NUM_SAMPLES 2 
#define SAMPLING_RATE ACCEL_SAMPLING_25HZ
#define TEXT_LENGTH 50
#define OUT_BOUND 5000 //the maximum possible value of acceleration is 4000

#define TIMER_PERIOD 100 //in ms
#define TIMER_PERIOD_LONG 1000 //in ms

#define DEVIDER_SHORT 1 //determined based on NUM_SAMPLES and SAMPLING_RATE
#define DEVIDER_LONG 25//determined based on NUM_SAMPLES and SAMPLING_RATE, for 25HZ sampling rate and 2 NUM_SAMPLES, it's equivalent to 2s

#define QUEUE_LENGTH 10 //the length of round-robin queue
#define U_BOUND_GRAVITY_2 (100+10) //upper bound of square of gravity
#define L_BOUND_GRAVITY_2 (100-10) //lower bound of square of gravity
#define BOUND_S_DEVIATION 100 //threshold of standard deviation of square of gravity, when the watch is static
#define MINI_SAMPLES 25 //the minimum samples for one gesture
#define REPEATED_DATALOGGING false //parameter of myRegisterAcc to make the datalogging only runs once

extern char text[TEXT_LENGTH];

extern const uint32_t LOG_TAG;

extern DataLoggingSessionRef logging_session;

extern AppTimer *timer;

extern int initialAcc[3]; //the direction of positive z axis

extern int logSwitch; //the switch of collecting data

extern int frequencyDevider; //used to control the frequency of wrist turn detection

extern int counter; //counter of accel samples

extern int isAccRegistered; //whether the accel data service is registered

extern double loopQueue[2][QUEUE_LENGTH]; //round-robin queue to monitor the module of acceleration, and its square

extern int qIdx; //index of round-robin queue

extern int globalCounter; //counter the number of samples sent when the watch is not vibrating

extern double lastMean;

extern double lastMean2;

extern bool isRepeated;

extern bool softSwitchAcc;
// ----------------- for watch windows ----------------------
extern TextLayer* data_logging_layer;
extern char* data_logging_info_collecting;
extern char* data_logging_info_restart;
extern char* data_logging_info_waiting;

double sum(double * vec, int length);

double myMean(double lastMean, double oldest, double newest, int length);

double mySd(double mean, double mean2);

void initQueue();

//deregister accel service if it's open
void deregisterAcc();

void deregisterDatalogging();

void registerAcc();

//switch on datalogging process, the parameter determines whether the process is repeatable or only once
void switchOnDLProc(bool flag);

//switch off datalogging process, deregister datalogging process and switch off the variable softSwitchAcc
void switchOffDLProc();

void accel_data_handler(AccelData *accel_data, uint32_t num_samples) ;

void timer_callback(void *data);

#endif
