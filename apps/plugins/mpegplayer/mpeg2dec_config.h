/* $Id$                                                                    */
#ifndef MPEG2DEC_CONFIG_H
#define MPEG2DEC_CONFIG_H

#define ATTRIBUTE_ALIGNED_MAX 16

#ifdef HAVE_LCD_COLOR
#define MPEG2_COLOR 1
#define MPEG2_COMPONENTS 3
#else
#define MPEG2_COLOR 0
#define MPEG2_COMPONENTS 1
#endif

#endif /* MPEG2DEC_CONFIG_H */
