/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: sokoban.c,v 0.01 2002/06/15 
 *
 * Copyright (C) 2002 Eric Linenberg
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "config.h"

#ifdef HAVE_LCD_BITMAP

#include "sokoban.h"
#include "lcd.h"
#include "button.h"
#include "kernel.h"

#ifdef SIMULATOR
#include <stdio.h>
#endif
#include <string.h>

#define SOKOBAN_TITLE   "Sokoban"
#define SOKOBAN_TITLE_FONT  2
#define NUM_LEVELS  4

int board[16][20];
int current_level=0;
int moves=0;
int row=0;
int col=0;
int boxes_to_go=0;
int current_spot=1;

/* 320 boxes per level */
int levels[320*NUM_LEVELS] = {
/* level 01 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,2,3,2,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,2,1,2,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,2,4,1,4,3,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,3,1,4,5,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,2,2,4,2,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,2,3,2,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* level 02 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,2,5,4,4,2,0,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,2,1,4,1,2,0,2,3,2,0,0,0,0,0,0,
0,0,0,0,0,2,2,2,1,2,2,2,3,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,1,1,1,1,3,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,1,1,1,2,1,1,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,1,1,1,2,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* level 03 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,
0,0,0,0,0,0,2,1,1,1,1,1,2,2,2,0,0,0,0,0,
0,0,0,0,0,2,2,4,2,2,2,1,1,1,2,0,0,0,0,0,
0,0,0,0,0,2,1,5,1,4,1,1,4,1,2,0,0,0,0,0,
0,0,0,0,0,2,1,3,3,2,1,4,1,2,2,0,0,0,0,0,
0,0,0,0,0,2,2,3,3,2,1,1,1,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* level 04 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,2,2,1,1,3,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,1,1,1,4,1,1,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,1,4,4,1,4,3,2,0,0,0,0,0,0,
0,0,0,0,0,0,2,2,5,2,2,3,3,2,0,0,0,0,0,0,
0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
/* level 40 
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,
0,0,0,0,0,2,1,1,1,1,1,1,2,0,0,0,0,0,0,0,
0,0,0,0,0,2,1,5,1,4,2,1,2,0,0,0,0,0,0,0,
0,0,0,0,0,2,2,1,2,1,1,1,2,0,0,0,0,0,0,0,
0,0,0,0,0,2,1,4,2,3,2,1,2,0,0,0,0,0,0,0,
0,0,0,0,0,2,1,1,3,4,3,4,2,0,0,0,0,0,0,0,
0,0,0,0,0,2,2,1,2,3,2,1,2,0,0,0,0,0,0,0,
0,0,0,0,0,0,2,1,2,3,1,1,2,2,0,0,0,0,0,0,
0,0,0,0,0,2,2,1,4,3,2,1,1,2,0,0,0,0,0,0,
0,0,0,0,0,2,1,1,2,1,4,1,1,2,0,0,0,0,0,0,
0,0,0,0,0,2,1,1,1,1,1,2,2,2,0,0,0,0,0,0,
0,0,0,0,0,2,1,1,2,2,2,2,0,0,0,0,0,0,0,0,
0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 */
};



void load_level(int level_to_load) {
    int a = 0;
    int b = 0;
    int c = 0;

    /* load level into board */
    /* get to the current level in the level array */
    a = level_to_load*320;
    
    for(b=0 ; b<16 ; b++) {
        for (c=0 ; c<20 ; c++) {
            board[b][c] = levels[a];
            a++;
            if (board[b][c]==5) {
                row = b;
                col = c;
            }
            if (board[b][c]==3)
                boxes_to_go++;        
        }
    }
    return;
}

void update_screen(void) {
    int b = 0;
    int c = 0;
    char s[25];

    /* load the board to the screen */
    for(b=0 ; b<16 ; b++) {
        for (c=0 ; c<20 ; c++) {
   
            /* this is a black space */
            if (board[b][c]==0) {
                lcd_drawrect (c*4, b*4, c*4+3, b*4+3);
                lcd_drawrect (c*4+1, b*4+1, c*4+2, b*4+2);
            }    
            /* this is a wall */
            if (board[b][c]==2) {
                lcd_drawpixel (c*4, b*4);
                lcd_drawpixel (c*4+2, b*4);
                lcd_drawpixel (c*4+1, b*4+1);
                lcd_drawpixel (c*4+3, b*4+1);
                lcd_drawpixel (c*4,   b*4+2);
                lcd_drawpixel (c*4+2, b*4+2);
                lcd_drawpixel (c*4+1, b*4+3);
                lcd_drawpixel (c*4+3, b*4+3);
            }    
            /* this is a box */
            if (board[b][c]==4) {
                lcd_drawrect (c*4, b*4, c*4+3, b*4+3);
            }    
            /* this is a home location */
            if (board[b][c]==3) {
                lcd_drawrect (c*4+1, b*4+1, c*4+2, b*4+2);
            }  
            /* this is you */
            if (board[b][c]==5) {
                lcd_drawline (c*4+1, b*4, c*4+2, b*4);
                lcd_drawline (c*4, b*4+1, c*4+3, b*4+1);
                lcd_drawline (c*4+1, b*4+2, c*4+2, b*4+2);
                lcd_drawpixel (c*4, b*4+3);
                lcd_drawpixel (c*4+3, b*4+3);
            }
            /* this is a box on a home spot */ 
            if (board[b][c]==7) {
                lcd_drawrect (c*4, b*4, c*4+3, b*4+3);
                lcd_drawrect (c*4+1, b*4+1, c*4+2, b*4+2);                
            }       
        }
    }
    

    snprintf (s, sizeof(s), "%d", current_level+1);
    lcd_putsxy (86, 20, s, 0);
    snprintf (s, sizeof(s), "%d", moves);
    lcd_putsxy (86, 52, s, 0);

    lcd_drawrect (80,0,111,31);
    lcd_drawrect (80,32,111,63);
    lcd_putsxy (81, 10, "Level", 0);
    lcd_putsxy (81, 42, "Moves", 0);
    /* print out the screen */
    lcd_update();
}



void sokoban_loop(void) {
    int ii = 0;
    int b = 0;
    moves = 0;
    current_level = 0;
    load_level(current_level);
    update_screen();

    while(1) {
        b = button_get(false);

        if ( b & BUTTON_OFF ) {
            /* get out of here */
            return; 
        }


        if ( b & BUTTON_F1 ) {
            /* get out of here */
            boxes_to_go=0; 
        }

        if ( b & BUTTON_LEFT ) {
            /* if it is a blank spot */
            if (board[row][col-1]==1) {
                 board[row][col-1]=5;
                 board[row][col]=current_spot;
                 current_spot=1;
                 col--;
                 moves++;
            }
            /* if it is a home spot */
            else if (board[row][col-1]==3) {
                 board[row][col-1]=5;
                 board[row][col]=current_spot;
                 current_spot=3;
                 col--;
                 moves++;
            }
            else if (board[row][col-1]==4) {
                 /* if there is a wall then do not move the box */
                 if(board[row][col-2]==2) {
                     /* do nothing */
                 }
                 /* if we are going from blank to blank */
                 else if(board[row][col-2]==1) {
                     board[row][col-2]=board[row][col-1];
                     board[row][col-1]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=1;
                     col--;
                     moves++;
                 }
                 /* if we are going from a blank to home */
                 else if(board[row][col-2]==3) {
                     board[row][col-2]=7;
                     board[row][col-1]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=1;
                     col--; 
                     boxes_to_go--;
                     moves++;    
                 }        
            }
            else if (board[row][col-1]==7) {
                 /* if there is a wall then do not move the box */
                 if(board[row][col-2]==2) {
                     /* do nothing */
                 }
                 /* we are going from a home to a blank */
                 else if(board[row][col-2]==1) {
                     board[row][col-2]=4;
                     board[row][col-1]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=3;
                     col--;
                     boxes_to_go++;
                     moves++;
                 }
                 /* if we are going from a home to home */
                 else if(board[row][col-2]==3) {
                     board[row][col-2]=7;
                     board[row][col-1]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=3;
                     col--;
                     moves++;      
                 }
            }
            lcd_clear_display();
            update_screen();          
        }


        if ( b & BUTTON_RIGHT ) {
            /* if it is a blank spot */
            if (board[row][col+1]==1) {
                 board[row][col+1]=5;
                 board[row][col]=current_spot;
                 current_spot=1;
                 col++;
                 moves++;
            }
            /* if it is a home spot */
            else if (board[row][col+1]==3) {
                 board[row][col+1]=5;
                 board[row][col]=current_spot;
                 current_spot=3;
                 col++;
                 moves++;
            }
            else if (board[row][col+1]==4) {
                 /* if there is a wall then do not move the box */
                 if(board[row][col+2]==2) {
                     /* do nothing */
                 }
                 /* if we are going from blank to blank */
                 else if(board[row][col+2]==1) {
                     board[row][col+2]=board[row][col+1];
                     board[row][col+1]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=1;
                     col++;
                     moves++;
                 }
                 /* if we are going from a blank to home */
                 else if(board[row][col+2]==3) {
                     board[row][col+2]=7;
                     board[row][col+1]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=1;
                     col++; 
                     boxes_to_go--;
                     moves++;    
                 }        
            }
            else if (board[row][col+1]==7) {
                 /* if there is a wall then do not move the box */
                 if(board[row][col+2]==2) {
                     /* do nothing */
                 }
                 /* we are going from a home to a blank */
                 else if(board[row][col+2]==1) {
                     board[row][col+2]=4;
                     board[row][col+1]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=3;
                     col++;
                     boxes_to_go++;
                     moves++;
                 }
                 /* if we are going from a home to home */
                 else if(board[row][col+2]==3) {
                     board[row][col+2]=7;
                     board[row][col+1]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=3;
                     col++;
                     moves++;      
                 }
            }
            lcd_clear_display();
            update_screen();          
        }

        if ( b & BUTTON_UP ) {
            /* if it is a blank spot */
            if (board[row-1][col]==1) {
                 board[row-1][col]=5;
                 board[row][col]=current_spot;
                 current_spot=1;
                 row--;
                 moves++;
            }
            /* if it is a home spot */
            else if (board[row-1][col]==3) {
                 board[row-1][col]=5;
                 board[row][col]=current_spot;
                 current_spot=3;
                 row--;
                 moves++;
            }
            else if (board[row-1][col]==4) {
                 /* if there is a wall then do not move the box */
                 if(board[row-2][col]==2) {
                     /* do nothing */
                 }
                 /* if we are going from blank to blank */
                 else if(board[row-2][col]==1) {
                     board[row-2][col]=board[row-1][col];
                     board[row-1][col]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=1;
                     row--;
                     moves++;
                 }
                 /* if we are going from a blank to home */
                 else if(board[row-2][col]==3) {
                     board[row-2][col]=7;
                     board[row-1][col]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=1;
                     row--; 
                     boxes_to_go--;
                     moves++;    
                 }        
            }
            else if (board[row-1][col]==7) {
                 /* if there is a wall then do not move the box */
                 if(board[row-2][col]==2) {
                     /* do nothing */
                 }
                 /* we are going from a home to a blank */
                 else if(board[row-2][col]==1) {
                     board[row-2][col]=4;
                     board[row-1][col]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=3;
                     row--;
                     boxes_to_go++;
                     moves++;
                 }
                 /* if we are going from a home to home */
                 else if(board[row-2][col]==3) {
                     board[row-2][col]=7;
                     board[row-1][col]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=3;
                     row--;
                     moves++;      
                 }
            }
            lcd_clear_display();
            update_screen();          
        }

        if ( b & BUTTON_DOWN ) {
            /* if it is a blank spot */
            if (board[row+1][col]==1) {
                 board[row+1][col]=5;
                 board[row][col]=current_spot;
                 current_spot=1;
                 row++;
                 moves++;
            }
            /* if it is a home spot */
            else if (board[row+1][col]==3) {
                 board[row+1][col]=5;
                 board[row][col]=current_spot;
                 current_spot=3;
                 row++;
                 moves++;
            }
            else if (board[row+1][col]==4) {
                 /* if there is a wall then do not move the box */
                 if(board[row+2][col]==2) {
                     /* do nothing */
                 }
                 /* if we are going from blank to blank */
                 else if(board[row+2][col]==1) {
                     board[row+2][col]=board[row+1][col];
                     board[row+1][col]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=1;
                     row++;
                     moves++;
                 }
                 /* if we are going from a blank to home */
                 else if(board[row+2][col]==3) {
                     board[row+2][col]=7;
                     board[row+1][col]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=1;
                     row++; 
                     boxes_to_go--;
                     moves++;    
                 }        
            }
            else if (board[row+1][col]==7) {
                 /* if there is a wall then do not move the box */
                 if(board[row+2][col]==2) {
                     /* do nothing */
                 }
                 /* we are going from a home to a blank */
                 else if(board[row+2][col]==1) {
                     board[row+2][col]=4;
                     board[row+1][col]=board[row][col];
                     board[row][col]=current_spot;
                     current_spot=3;
                     row++;
                     boxes_to_go++;
                     moves++;
                 }
                 /* if we are going from a home to home */
                 else if(board[row+2][col]==3) {
                     board[row+2][col]=7;
                     board[row+1][col]=board[row][col];
                     board[row][col]=current_spot; 
                     current_spot=3;
                     row++;
                     moves++;      
                 }
            }
            lcd_clear_display();
            update_screen();          
        }

        if (boxes_to_go==0) {
            moves=0;
            current_level++;
            if (current_level == NUM_LEVELS) {
                lcd_clear_display();
                lcd_putsxy(10, 20, "YOU WIN!!", 2);

                lcd_update();
                for (ii=0 ; ii<20 ; ii++) {
                    lcd_invertrect(0,0,111,63);
                    lcd_update();
                }
                return;
            }
            load_level(current_level);
            lcd_clear_display();
            update_screen();
        }
    }
}


void sokoban(void)
{
    int w, h;
    int len = strlen(SOKOBAN_TITLE);

    lcd_getfontsize(SOKOBAN_TITLE_FONT, &w, &h);

    /* Get horizontel centering for text */
    len *= w;
    if (len%2 != 0)
        len = ((len+1)/2)+(w/2);
    else
        len /= 2;

    if (h%2 != 0)
        h = (h/2)+1;
    else
        h /= 2;

    lcd_clear_display();
    lcd_putsxy(LCD_WIDTH/2-len, (LCD_HEIGHT/2)-h, SOKOBAN_TITLE, 
               SOKOBAN_TITLE_FONT);
    lcd_putsxy( 3,42,  "[Off] to stop", 0);
    lcd_putsxy( 3,52, "[F1] + level",0);

    lcd_update();
    sleep(HZ*2);
    lcd_clear_display();
    sokoban_loop();
}

#endif
