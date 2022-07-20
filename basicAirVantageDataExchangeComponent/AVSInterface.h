/*
 * AVSInterface.h
 *
 *  This is part of the "BX31_ATService" Project
 *  Created on: Apr 8, 2019
 *      Author: Thomas Schmidt, SWI
 */

#include "legato.h"
#include "interfaces.h"

#ifndef AVSINTERFACE_H_
#define AVSINTERFACE_H_


typedef enum {
        INT,
        FLOAT,
        BOOL,
        STRING
} avsService_DataType_t;




typedef void (*avsService_callbackOnComamnd_t)(const char *path);

le_result_t avsService_init();
le_result_t avsService_recordData(char *path, void *data, avsService_DataType_t type);
le_result_t avsService_registerCommand(const char *path, avsService_callbackOnComamnd_t callback);
le_result_t avsService_pushData();
void avsService_destroy();

#endif /* AVSINTERFACE_H_ */
