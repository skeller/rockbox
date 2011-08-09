/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2010 by Thomas Martitz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/


#include <jni.h>
#include <stdbool.h>
#include "config.h"

extern JNIEnv *env_ptr;
extern jclass  RockboxService_class;
extern jobject RockboxService_instance;

static jfieldID _battery_level;
static jobject BatteryMonitor_instance;

void powermgmt_init_target(void)
{
    JNIEnv e = *env_ptr;
    jclass class = e->FindClass(env_ptr, "org/rockbox/monitors/BatteryMonitor");
    jmethodID constructor = e->GetMethodID(env_ptr, class,
                                            "<init>",
                                            "(Landroid/content/Context;)V");
    BatteryMonitor_instance = e->NewObject(env_ptr, class,
                                            constructor,
                                            RockboxService_instance);

    /* cache the battery level field id */
    _battery_level = (*env_ptr)->GetFieldID(env_ptr,
                                            class,
                                            "mBattLevel", "I");
}

int battery_level(void)
{
    return (*env_ptr)->GetIntField(env_ptr, BatteryMonitor_instance, _battery_level);
}

int battery_time(void)
{   /* cannot calculate yet */
    return 0;
}

/* should always be safe on android targets, the host shuts us down before */
bool battery_level_safe(void)
{
    return true;
}

/* TODO */
unsigned battery_voltage(void)
{
    return 0;
}
