/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2004 Jörg Hohensohn
 *
 * This module collects the Talkbox and voice UI functions.
 * (Talkbox reads directory names from mp3 clips called thumbnails,
 *  the voice UI lets menus and screens "talk" from a voicefont in memory.
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

#ifndef __TALK_H__
#define __TALK_H__

#include <stdbool.h>
#include <inttypes.h>
#include "time.h"

#define VOICE_VERSION 400 /* 4.00 - if you change this, change it in voicefont too */

enum {
    /* See array "unit_voiced" in talk.c function "talk_value" */
    UNIT_INT = 1, /* plain number */
    UNIT_SIGNED,  /* number with mandatory sign (even if positive) */
    UNIT_MS,      /* milliseconds */
    UNIT_SEC,     /* seconds */
    UNIT_MIN,     /* minutes */
    UNIT_HOUR,    /* hours */
    UNIT_KHZ,     /* kHz */
    UNIT_DB,      /* dB, mandatory sign */
    UNIT_PERCENT, /* % */
    UNIT_MAH,     /* milliAmp hours */
    UNIT_PIXEL,   /* pixels */
    UNIT_PER_SEC, /* per second */
    UNIT_HERTZ,   /* hertz */
    UNIT_MB,      /* Megabytes */
    UNIT_KBIT,    /* kilobits per sec */
    UNIT_PM_TICK, /* peak meter units per tick */
    UNIT_TIME,    /* time duration/interval in seconds, says hours,mins,secs */
    UNIT_LAST     /* END MARKER */
};

#define UNIT_SHIFT (32-5) /* this many bits left from UNIT_xx enum */

#define DECIMAL_SHIFT (32 - 8)

/* make a "talkable" ID from number + unit
   unit is upper 4 bits, number the remaining (in regular 2's complement) */
#define TALK_ID(n,u) (((long)(u))<<UNIT_SHIFT | ((n) & ~(-1L<<UNIT_SHIFT)))

/* make a "talkable" ID from a decimal number + unit, the decimal number
   is represented like x*10^d where d is the number of decimal digits */
#define TALK_ID_DECIMAL(n,d,u) (((long)(u))<<UNIT_SHIFT |\
                             ((long)(d))<<DECIMAL_SHIFT |\
                             ((n) & ~(-1L<<DECIMAL_SHIFT)))

/* convenience macro to have both virtual pointer and ID as arguments */
#define STR(id) ID2P(id), id

/* publish these strings, so they're stored only once (better than #define) */
extern const char* const dir_thumbnail_name; /* "_dirname.talk" */
extern const char* const file_thumbnail_ext; /* ".talk" for file voicing */

void talk_init(void);
#if CONFIG_CODEC == SWCODEC
bool talk_voice_required(void); /* returns true if voice codec required */
#endif
int talk_get_bufsize(void); /* get the loaded voice file size */
size_t talkbuf_init(char* bufstart);
void talk_buffer_steal(void); /* claim the mp3 buffer e.g. for play/record */
bool is_voice_queued(void); /* Are there more voice clips to be spoken? */
int talk_id(int32_t id, bool enqueue); /* play a voice ID from voicefont */
/* play a thumbnail from file */
int talk_file(const char *root, const char *dir, const char *file,
              const char *ext, const long *prefix_ids, bool enqueue);
/* play file's thumbnail or spell name */
int talk_file_or_spell(const char *dirname, const char* filename,
                       const long *prefix_ids, bool enqueue);
/* play dir's thumbnail or spell name */
int talk_dir_or_spell(const char* filename,
                      const long *prefix_ids, bool enqueue);
int talk_number(long n, bool enqueue); /* say a number */
int talk_value(long n, int unit, bool enqueue); /* say a numeric value */
int talk_value_decimal(long n, int unit, int decimals, bool enqueue);
int talk_spell(const char* spell, bool enqueue); /* spell a string */
void talk_setting(const void *global_settings_variable); /* read a setting */
void talk_disable(bool disable); /* temporarily disable (or re-enable) talking (temporarily, not persisted) */
void talk_force_shutup(void); /* kill voice unconditionally */
void talk_shutup(void); /* Interrupt voice, as when enqueue is false */

/* helper function for speaking fractional numbers */
void talk_fractional(char *tbuf, int value, int unit);

#if CONFIG_RTC
void talk_time(const struct tm *tm, bool enqueue);
void talk_date(const struct tm *tm, bool enqueue);
#endif /* CONFIG_RTC */

/* This (otherwise invalid) ID signals the end of the array. */
#define TALK_FINAL_ID LANG_LAST_INDEX_IN_ARRAY

/* Enqueue next utterance even if enqueue parameter is false: don't
   interrupt the current utterance. */
void talk_force_enqueue_next(void);

/* speaks one or more IDs (from an array)). */
int talk_idarray(const long *idarray, bool enqueue);
/* This makes an initializer for the array of IDs and takes care to
   put the final sentinel element at the end. */
#define TALK_IDARRAY(ids...) ((long[]){ids,TALK_FINAL_ID})
/* And this handy macro makes it look like a variadic function. */
#define talk_ids(enqueue, ids...) talk_idarray(TALK_IDARRAY(ids), enqueue)
/* This version talks only if talking menus are enabled, and does not
   enqueue the initial id. */
#define cond_talk_ids(ids...) do { \
        if (global_settings.talk_menu) \
            talk_ids(false, ids); \
    } while(0)
/* And a version that takes the array parameter... */
#define cond_talk_idarray(idarray) do { \
        if (global_settings.talk_menu \
            talk_idarray(idarray, false); \
    } while(0)
/* Convenience macro to conditionally speak something and not have
   it interrupted. */
#define cond_talk_ids_fq(ids...) do { \
        if (global_settings.talk_menu) { \
            talk_ids(false, ids);                 \
            talk_force_enqueue_next(); \
        } \
    }while(0)

#endif /* __TALK_H__ */
