
/*
 * AVSInterface.c
 *
 *  This is part of the "BX31_ATService",
 *  and of the "BasicAirVantageDataExchange" Project
 *  Created on: Apr 8 2019
 *  modified: June 19 2022 - minor naming corrections
 *
 *      Author: Thomas Schmidt, SWI
 */

#include "legato.h"
#include "interfaces.h"
#include "AVSInterface.h"



static le_avdata_RequestSessionObjRef_t avsSession = NULL;
static le_avdata_RecordRef_t avsRecordRef = NULL;

static avsService_callbackOnComamnd_t cmdCallback = NULL;

/** ------------------------------------------------------------------------
 *
 * avsService_init(): turn up the connection to AirVantage
 *
 * ------------------------------------------------------------------------
 */
le_result_t avsService_init() {
        avsSession = le_avdata_RequestSession();

        if (NULL == avsSession) {
                LE_ERROR("AirVantage Connection Controller does not start.");
                return LE_UNAVAILABLE;
        }

        LE_ASSERT(le_avdata_SetNamespace(LE_AVDATA_NAMESPACE_GLOBAL) == LE_OK);


        return LE_OK;
}


/** ------------------------------------------------------------------------
 *
 * PushRecordCallbackHandler(): called after the data push - here some error
 * handling might could take place
 *
 * ------------------------------------------------------------------------
 */
void PushRecordCallbackHandler(le_avdata_PushStatus_t status, void* contextPtr) {
        if (status == LE_AVDATA_PUSH_SUCCESS) {
                LE_INFO("Push Timeserie OK");
        } else {
                LE_INFO("Failed to push Timeserie");
        }
}

/** ------------------------------------------------------------------------
 *
 * avsService_recordData(): this is wrapping the Legato API for AirVantage 
 * data le_avdata* and writes meassurements to to a record
 * In case the record is not created this function will automatically 
 * create one
 * 
 * ------------------------------------------------------------------------
 */
le_result_t avsService_recordData(char *path, void *data, avsService_DataType_t type) {
        struct timeval  tv;
        gettimeofday(&tv, NULL);
        uint64_t utcMilliSec = (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;

        if(avsRecordRef ==NULL) {
                LE_ASSERT( (avsRecordRef = le_avdata_CreateRecord()) != NULL);    // a record is to collect a series of events over
                                                                                  // time and push the series later. We use the
                                                                                  // record to keep track even if we have not
                                                                                  // been able to push it now, because of coverage
        }                                                                         // etc.


        le_result_t recordResult;

        switch(type)  {
                case INT:       recordResult = le_avdata_RecordInt   (avsRecordRef, path, *((int32_t *) data), utcMilliSec); break;
                case FLOAT:     recordResult = le_avdata_RecordFloat (avsRecordRef, path, *((double  *) data), utcMilliSec); break;
                case BOOL:      recordResult = le_avdata_RecordBool  (avsRecordRef, path, *((bool    *) data), utcMilliSec); break;
                case STRING:    recordResult = le_avdata_RecordString(avsRecordRef, path,  ((char    *) data), utcMilliSec); break;
                default:  LE_ERROR("Invalid Data Type"); return LE_FAULT; break;
        }

        if(recordResult != LE_OK) {
                LE_WARN("Failed to push data to record result was : %d", recordResult);
                return recordResult;
        }




        return LE_OK;
}


/** ------------------------------------------------------------------------
 *
 * avsService_pushData(): pushes the data from the Record over the air to
 * AirVantage after this PushRecordCallbackHandler() is called for error
 * handling
 *
 * ------------------------------------------------------------------------
 */
le_result_t avsService_pushData() {
        le_result_t result = LE_OK;
        if ((result= le_avdata_PushRecord(avsRecordRef, PushRecordCallbackHandler, NULL)) == LE_OK) {
                le_avdata_DeleteRecord(avsRecordRef);
                avsRecordRef = NULL;
        } else {
                LE_WARN("Failed pushing time series, will retry next time");
        }
        return result;
}




/** ------------------------------------------------------------------------
 *
 *
 * ------------------------------------------------------------------------
 */
static void avsService_executeCommandCallback(const char* path,
                                                le_avdata_AccessType_t accessType,
                                                        le_avdata_ArgumentListRef_t argumentList,
                                                                void* contextPtr) {
        if(cmdCallback == NULL) {
                LE_WARN("No command callback set - got command \"%s\" - but unable to execute", path);
                return;
        }
        cmdCallback(path);

}


/** ------------------------------------------------------------------------
 *
 *
 * ------------------------------------------------------------------------
 */


le_result_t avsService_registerCommand(const char *path, avsService_callbackOnComamnd_t callback) {

        LE_INFO("Registering AV Command \"%s\"", path);

        le_result_t result = LE_OK;

        cmdCallback = callback;

        if ((result= le_avdata_CreateResource(path, LE_AVDATA_ACCESS_COMMAND)) == LE_OK) {
                le_avdata_AddResourceEventHandler(path, avsService_executeCommandCallback, NULL);
        } else {
                LE_WARN("Failed to register AV Command \"%s\"", path);
        }

        return result;
}



/** ------------------------------------------------------------------------
 *
 * avsService_destroy(): delete and free the memory of the record where 
 * the meassurements are stored and which might have been allocated in 
 * avsService_recordData()
 * Terminates as well the AirVantage Session
 *
 * ------------------------------------------------------------------------
 */
void avsService_destroy() {
        if(avsRecordRef) le_avdata_DeleteRecord(avsRecordRef);
        if(avsSession) le_avdata_ReleaseSession(avsSession);
}

