

/*
 * main.c
 *
 *  This is part of the "BasicAirVantageDataExchange" Project
 *  which is meant to demonstrate basic SierraWireless basic 
 *  Data communication 
 *  
 *  Created on: June 19 2022 
 *
 *      Author: Thomas Schmidt, SWI
 */


#include "legato.h"
#include "interfaces.h"
#include "AVSInterface.h"



#define AV_PATH_DEMO_VALUE      "avsDemo.demovalue"
#define AV_PATH_IMEI            "avsDemo.imei"

#define AV_CMD_PATH_DEMOCOMMAND "/avsDemo/democommand"


const int POLL_TIMER_SEC   = 10;                                        // "Sensor" poll timer in (poll every 10 sec) 

const int REPORT_TIMER_SEC = 60;                                        // every 1 minutes tell AirVantage (this is likely a 
                                                                        // little bit too high cadence but for demo OK)




static le_timer_Ref_t reportingTimer = NULL;                            // the interval timer reference
static unsigned int simValue = 0;                                       // the "sensor" data - just an incremented variable to be simple on this

char imei[LE_INFO_IMEI_MAX_BYTES];                                      // buffer for the device IMEI



/** ------------------------------------------------------------------------
 *
 *   Handle System Signals on termination of the legato application
 *
 * ------------------------------------------------------------------------
 */

static void main_SigHandler(int signal) {                               // in case the Legato App is terminated...
        LE_INFO("Terminating AV Demo service");
        if(reportingTimer != NULL) le_timer_Stop(reportingTimer);       // stop the interval timer
        avsService_destroy();                                           // and tear down the AirVantage service connections
}

/** ------------------------------------------------------------------------
 *
 * called by timer periodically to perform the data collection
 *
 * @param reference to the calling timer  
 *
 * ------------------------------------------------------------------------
 */
void collectData(le_timer_Ref_t timerRef) {



    LE_INFO("Periodic sensor poll called by timer");

    ++simValue;                                                         // this is our "sensor" data ... just counting this value up

    
    LE_INFO("Adding meassurement to the record: path=%s; data=%d", AV_PATH_DEMO_VALUE, simValue);
    avsService_recordData(AV_PATH_DEMO_VALUE, &simValue, INT);


    if((simValue % (REPORT_TIMER_SEC / POLL_TIMER_SEC)) == 0) {         // every REPORT_TIMER_SEC push data to AV

        avsService_recordData(AV_PATH_IMEI, imei, STRING);              // this is more for demo than nessesary. The JSON on AirVantage 
                                                                        // contains the communication.id which is the device IMEI
        avsService_pushData();
    }
    
}



/** ------------------------------------------------------------------------
 *
 * Command handler called when comamnds are comming in from AirVantage
 * This is a very simple example showing just that this function is 
 * triggered i ncase a custom command is issued on AirVantage
 *
 * ------------------------------------------------------------------------
 */

void avCmdCallback(const char *path) {
    LE_INFO("------------ Got command: path=%s ------------", path);
} 

/** ------------------------------------------------------------------------
 *
 * Initialization ("main") routine
 *
 * ------------------------------------------------------------------------
 */

COMPONENT_INIT {
    LE_INFO("Starting AV Demo service");

    LE_ASSERT(le_info_GetImei(imei, sizeof(imei)) == LE_OK);



    le_sig_Block(SIGINT);                                                   // catch the termination of the Application
    le_sig_SetEventHandler(SIGINT, main_SigHandler);                        // to clean up allocated resources
    le_sig_Block(SIGTERM);                                                  // catch the termination of the Application
    le_sig_SetEventHandler(SIGTERM, main_SigHandler);                       // to clean up allocated resources

    avsService_init();


    avsService_registerCommand(AV_CMD_PATH_DEMOCOMMAND,&avCmdCallback);


    reportingTimer = le_timer_Create("demoAVPollSensors");                  // set up a Timer to poll sensors
//    le_timer_SetContextPtr(reportingTimer, cmdRef);
    le_timer_SetHandler(reportingTimer, collectData);                       // the timer will call periodically collectData()
    le_timer_SetRepeat(reportingTimer, 0);                                  // repeat forever
    le_timer_SetMsInterval(reportingTimer, POLL_TIMER_SEC * 1000);          // here the timer interval is set
    le_timer_Start(reportingTimer);                                         // start the interval timer
}
