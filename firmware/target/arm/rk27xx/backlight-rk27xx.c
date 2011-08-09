/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2011 by Marcin Bukat
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
#include <stdbool.h>

#include "config.h"
#include "backlight.h"
#include "backlight-target.h"
#include "system.h"

static int brightness = DEFAULT_BRIGHTNESS_SETTING;

/* Based on http://www.poynton.com/PDFs/SMPTE93_Gamma.pdf
 * CIE standarized function that relates physical luminance
 * to perceived lightness
 *
 * L* = 116*(Y/Yn)^(1/3) - 16 (Y/Yn > 0 008856)
 * where Yn is luminance of reference white
 *
 * Actual function is lightly tweaked to account for the fact
 * that fill factor of the PWM below ~15% gives black.
 * So the function used to calculate the values in the matrix was:
 * f(x) = 42000 * ((100*x/31 + 16)/116)^3 + 8000
 */
static const unsigned short lin_brightness[] = {
     8110,  8191,  8304,  8455,  8649,  8892,  9189,  9545,
     9966, 10457, 11024, 11671, 12406, 13232, 14156, 15182,
    16316, 17565, 18932, 20423, 22045, 23801, 25699, 27742,
    29937, 32289, 34803, 37485, 40340, 43374, 46592, 50000
};

bool _backlight_init(void)
{
    /* configure PD4 as output */
    GPIO_PDCON |= (1<<4);

    /* set PD4 low (backlight off) */
    GPIO_PDDR &= ~(1<<4);

    /* IOMUXB - set PWM0 pin as GPIO */
    SCU_IOMUXB_CON &= ~(1 << 11); /* type<<11<<channel */

    /* DIV/2, PWM reset */
    PWMT0_CTRL = (0<<9) | (1<<7);

    /* set pwm frequency to 500Hz - my lcd panel can't cope more reliably */
    /* (apb_freq/pwm_freq)/pwm_div = (50 000 000/500)/2 */
    PWMT0_LRC = 50000; 
    PWMT0_HRC = lin_brightness[brightness];

    /* reset counter */
    PWMT0_CNTR = 0x00;
    
    /* DIV/2, PWM output enable, PWM timer enable */
    PWMT0_CTRL = (0<<9) | (1<<3) | (1<<0);

    return true;
}

void _backlight_on(void)
{
    /* enable PWM clock */
    SCU_CLKCFG &= ~(1<<29);

    /* set output pin as PWM pin */
    SCU_IOMUXB_CON |= (1<<11); /* type<<11<<channel */

    /* pwm enable */
    PWMT0_CTRL |= (1<<3) | (1<<0);
}

void _backlight_off(void)
{
    /* setup PWM0 pin as GPIO which is pulled low */
    SCU_IOMUXB_CON &= ~(1<<11);

    /* stop pwm timer */
    PWMT0_CTRL &= ~(1<<3) | (1<<0);

    /* disable PWM clock */
    SCU_CLKCFG |= (1<<29);
}

void _backlight_set_brightness(int val)
{
    brightness = val & 0x1f;
    PWMT0_HRC = lin_brightness[brightness];
}
