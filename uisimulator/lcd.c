/***************************************************************************
 *             __________               __   ___.                  
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___  
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /  
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <   
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \  
 *                     \/            \/     \/    \/            \/ 
 * $Id$
 *
 * Copyright (C) 2002 by Daniel Stenberg <daniel@haxx.se>
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

/*
 * This file is meant for generic LCD defines and global variables etc.
 */

#define	DISP_X 112		/* Display width in pixels */
#define	DISP_Y 64		/* Display height in pixels */

#define	CHAR_X   6		/* Character width in pixels */
#define	CHAR_Y   8		/* Character height in pixels */
#define	COL_MASK 0xff		/* Character column pixels changed */

/*
 * ASCII character generation table
 *
 * This contains only the printable characters (0x20-0x7f).
 * Each row in this table is a 5x8 pixel character pattern.
 * Each byte is a column of pixels, with the top pixel in the LSB.
 */
#define	ASCII_MIN			0x20	/* First char in table */
#define	ASCII_MAX			0x7f	/* Last char in table */

static const uchar char_gen[ASCII_MAX-ASCII_MIN+1][CHAR_X-1] =
{
  0x00, 0x00, 0x00, 0x00, 0x00,	/* 0x20-0x2f */
  0x00, 0x00, 0x4f, 0x00, 0x00,
  0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7f, 0x14, 0x7f, 0x14,
  0x24, 0x2a, 0x7f, 0x2a, 0x12,
  0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x55, 0x22, 0x50,
  0x00, 0x05, 0x03, 0x00, 0x00,
  0x00, 0x1c, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1c, 0x00,
  0x14, 0x08, 0x3e, 0x08, 0x14,
  0x08, 0x08, 0x3e, 0x08, 0x08,
  0x00, 0xa0, 0x60, 0x00, 0x00,
  0x08, 0x08, 0x08, 0x08, 0x08,
  0x00, 0x60, 0x60, 0x00, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02,
  
  0x3e, 0x51, 0x49, 0x45, 0x3e,	/* 0x30-0x3f */
  0x00, 0x42, 0x7f, 0x40, 0x00,
  0x42, 0x61, 0x51, 0x49, 0x46,
  0x21, 0x41, 0x45, 0x4b, 0x31,
  0x18, 0x14, 0x12, 0x7f, 0x10,
  0x27, 0x45, 0x45, 0x45, 0x39,
  0x3c, 0x4a, 0x49, 0x49, 0x30,
  0x01, 0x71, 0x09, 0x05, 0x03,
  0x36, 0x49, 0x49, 0x49, 0x36,
  0x06, 0x49, 0x49, 0x29, 0x1e,
  0x00, 0x6c, 0x6c, 0x00, 0x00,
  0x00, 0xac, 0x6c, 0x00, 0x00,
  0x08, 0x14, 0x22, 0x41, 0x00,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x41, 0x22, 0x14, 0x08,
  0x02, 0x01, 0x51, 0x09, 0x06,

  0x32, 0x49, 0x79, 0x41, 0x3e,	/* 0x40-0x4f */
  0x7e, 0x11, 0x11, 0x11, 0x7e,
  0x7f, 0x49, 0x49, 0x49, 0x36,
  0x3e, 0x41, 0x41, 0x41, 0x22,
  0x7f, 0x41, 0x41, 0x22, 0x1c,
  0x7f, 0x49, 0x49, 0x49, 0x41,
  0x7f, 0x09, 0x09, 0x09, 0x01,
  0x3e, 0x41, 0x49, 0x49, 0x7a,
  0x7f, 0x08, 0x08, 0x08, 0x7f,
  0x00, 0x41, 0x7f, 0x41, 0x00,
  0x20, 0x40, 0x41, 0x3f, 0x01,
  0x7f, 0x08, 0x14, 0x22, 0x41,
  0x7f, 0x40, 0x40, 0x40, 0x40,
  0x7f, 0x02, 0x0c, 0x02, 0x7f,
  0x7f, 0x04, 0x08, 0x10, 0x7f,
  0x3e, 0x41, 0x41, 0x41, 0x3e,

  0x7f, 0x09, 0x09, 0x09, 0x06,	/* 0x50-0x5f */
  0x3e, 0x41, 0x51, 0x21, 0x5e,
  0x7f, 0x09, 0x19, 0x29, 0x46,
  0x46, 0x49, 0x49, 0x49, 0x31,
  0x01, 0x01, 0x7f, 0x01, 0x01,
  0x3f, 0x40, 0x40, 0x40, 0x3f,
  0x1f, 0x20, 0x40, 0x20, 0x1f,
  0x3f, 0x40, 0x38, 0x40, 0x3f,
  0x63, 0x14, 0x08, 0x14, 0x63,
  0x07, 0x08, 0x70, 0x08, 0x07,
  0x71, 0x51, 0x49, 0x45, 0x43,
  0x00, 0x7f, 0x41, 0x41, 0x00,
  0x02, 0x04, 0x08, 0x10, 0x20,
  0x00, 0x41, 0x41, 0x7f, 0x00,
  0x04, 0x02, 0x01, 0x02, 0x04,
  0x40, 0x40, 0x40, 0x40, 0x40,

  0x00, 0x01, 0x02, 0x04, 0x00,	/* 0x60-0x6f */
  0x20, 0x54, 0x54, 0x54, 0x78,
  0x7f, 0x48, 0x44, 0x44, 0x38,
  0x38, 0x44, 0x44, 0x44, 0x20,
  0x38, 0x44, 0x44, 0x48, 0x7f,
  0x38, 0x54, 0x54, 0x54, 0x18,
  0x08, 0x7e, 0x09, 0x01, 0x02,
  0x18, 0xa4, 0xa4, 0xa4, 0x7c,
  0x7f, 0x08, 0x04, 0x04, 0x78,
  0x00, 0x44, 0x7d, 0x40, 0x00,
  0x40, 0x80, 0x84, 0x7d, 0x00,
  0x7f, 0x10, 0x28, 0x44, 0x00,
  0x00, 0x41, 0x7f, 0x40, 0x00,
  0x7c, 0x04, 0x18, 0x04, 0x78,
  0x7c, 0x08, 0x04, 0x04, 0x78,
  0x38, 0x44, 0x44, 0x44, 0x38,

  0xfc, 0x24, 0x24, 0x24, 0x18,	/* 0x70-0x7f */
  0x18, 0x24, 0x24, 0x24, 0xfc,
  0x7c, 0x08, 0x04, 0x04, 0x08,
  0x48, 0x54, 0x54, 0x54, 0x20,
  0x04, 0x3f, 0x44, 0x40, 0x20,
  0x3c, 0x40, 0x40, 0x20, 0x7c,
  0x1c, 0x20, 0x40, 0x20, 0x1c,
  0x3c, 0x40, 0x30, 0x40, 0x3c,
  0x44, 0x28, 0x10, 0x28, 0x44,
  0x1c, 0xa0, 0xa0, 0xa0, 0x7c,
  0x44, 0x64, 0x54, 0x4c, 0x44,
  0x00, 0x08, 0x36, 0x41, 0x00,
  0x00, 0x00, 0x7f, 0x00, 0x00,
  0x00, 0x41, 0x36, 0x08, 0x00,
  0x04, 0x02, 0x04, 0x08, 0x04,
  0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
};

/*
 * Memory copy of display bitmap
 *
 * This has the same format as the Recorder hardware:
 * Bits within a byte are arranged veritcally, LSB at top.
 * Byte 0 is top left, byte 1 is 2nd left, byte DISP_X starts 2nd row.
 */
static uchar display[DISP_Y/8][DISP_X];

static uint16 lcd_y;	/* Current pixel row */
static uint16 lcd_x;	/* Current pixel column */

/*
 * Set current x,y position
 */
void lcd_position (int x, int y)
{
  if (x >= 0 && x < DISP_X && y >= 0 && y < DISP_Y)
    {
      lcd_x = x;
      lcd_y = y;
    }
}

/*
 * Clear the display
 */
void lcd_clear (void)
{
  lcd_x = 0;
  lcd_y = 0;
  memset (display, 0, sizeof display);
}

