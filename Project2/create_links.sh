#!/bin/bash
rm BBG/include/common/communication_object.h
rm BBG/include/common/communication_interface.h
rm BBG/src/communication_interface.c
ln Project2_FreeRTOS_TIVA/include/communication_object.h BBG/include/common/communication_object.h
ln Project2_FreeRTOS_TIVA/include/communication_interface.h BBG/include/common/communication_interface.h
ln Project2_FreeRTOS_TIVA/communication_interface.c BBG/src/communication_interface.c
