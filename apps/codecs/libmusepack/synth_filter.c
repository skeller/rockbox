/*
  Copyright (c) 2005, The Musepack Development Team
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the following
  disclaimer in the documentation and/or other materials provided
  with the distribution.

  * Neither the name of the The Musepack Development Team nor the
  names of its contributors may be used to endorse or promote
  products derived from this software without specific prior
  written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/// \file synth_filter.c
/// Synthesis functions.
/// \todo document me

#include "musepack.h"
#include "internal.h"

/* S E T T I N G S */
// choose speed vs. accuracy for MPC_FIXED_POINT
// speed-setting will increase decoding speed on ARM only (+20%), loss of accuracy equals about 5 dB SNR (15bit output precision)
// to not use the speed-optimization -> comment OPTIMIZE_FOR_SPEED
#if defined(MPC_FIXED_POINT)
   #if defined(CPU_COLDFIRE)
      // do nothing
   #elif defined(CPU_ARM)
      #define OPTIMIZE_FOR_SPEED
   #else
      #define OPTIMIZE_FOR_SPEED
   #endif
#else
    // do nothing
#endif

/* C O N S T A N T S */
#undef _

#if defined(MPC_FIXED_POINT)
   #if defined(OPTIMIZE_FOR_SPEED)
      // round to +/- 2^14 as pre-shift before 32=32x32-multiply
      #define _(value)  (MPC_SHR_RND(value, 3))
      
      // round to +/- 2^17 as pre-shift before 32=32x32-multiply
      #define MPC_V_PRESHIFT(X) MPC_SHR_RND(X, 14)
   #else
      // saturate to +/- 2^31 (= value << (31-17)), D-values are +/- 2^17
      #define _(value)  (value << (14))
      
      // do not perform pre-shift
      #define MPC_V_PRESHIFT(X) (X)
   #endif
#else
   // IMPORTANT: internal scaling is somehow strange for floating point, therefore we scale the coefficients Di_opt
   // by the correct amount to have proper scaled output
   #define _(value)  MAKE_MPC_SAMPLE((double)value*(double)(0x1000))
   
   // do not perform pre-shift
   #define MPC_V_PRESHIFT(X) (X)
#endif
    
// Di_opt coefficients are +/- 2^17
static const MPC_SAMPLE_FORMAT  Di_opt [32] [16] ICONST_ATTR = {
    /*    0        1        2         3         4         5          6          7         8         9       10        11       12       13      14     15  */
    { _(  0), _( -29), _( 213), _( -459), _( 2037), _(-5153), _(  6574), _(-37489), _(75038), _(37489), _(6574), _( 5153), _(2037), _( 459), _(213), _(29) },
    { _( -1), _( -31), _( 218), _( -519), _( 2000), _(-5517), _(  5959), _(-39336), _(74992), _(35640), _(7134), _( 4788), _(2063), _( 401), _(208), _(26) },
    { _( -1), _( -35), _( 222), _( -581), _( 1952), _(-5879), _(  5288), _(-41176), _(74856), _(33791), _(7640), _( 4425), _(2080), _( 347), _(202), _(24) },
    { _( -1), _( -38), _( 225), _( -645), _( 1893), _(-6237), _(  4561), _(-43006), _(74630), _(31947), _(8092), _( 4063), _(2087), _( 294), _(196), _(21) },
    { _( -1), _( -41), _( 227), _( -711), _( 1822), _(-6589), _(  3776), _(-44821), _(74313), _(30112), _(8492), _( 3705), _(2085), _( 244), _(190), _(19) },
    { _( -1), _( -45), _( 228), _( -779), _( 1739), _(-6935), _(  2935), _(-46617), _(73908), _(28289), _(8840), _( 3351), _(2075), _( 197), _(183), _(17) },
    { _( -1), _( -49), _( 228), _( -848), _( 1644), _(-7271), _(  2037), _(-48390), _(73415), _(26482), _(9139), _( 3004), _(2057), _( 153), _(176), _(16) },
    { _( -2), _( -53), _( 227), _( -919), _( 1535), _(-7597), _(  1082), _(-50137), _(72835), _(24694), _(9389), _( 2663), _(2032), _( 111), _(169), _(14) },
    { _( -2), _( -58), _( 224), _( -991), _( 1414), _(-7910), _(    70), _(-51853), _(72169), _(22929), _(9592), _( 2330), _(2001), _(  72), _(161), _(13) },
    { _( -2), _( -63), _( 221), _(-1064), _( 1280), _(-8209), _(  -998), _(-53534), _(71420), _(21189), _(9750), _( 2006), _(1962), _(  36), _(154), _(11) },
    { _( -2), _( -68), _( 215), _(-1137), _( 1131), _(-8491), _( -2122), _(-55178), _(70590), _(19478), _(9863), _( 1692), _(1919), _(   2), _(147), _(10) },
    { _( -3), _( -73), _( 208), _(-1210), _(  970), _(-8755), _( -3300), _(-56778), _(69679), _(17799), _(9935), _( 1388), _(1870), _( -29), _(139), _( 9) },
    { _( -3), _( -79), _( 200), _(-1283), _(  794), _(-8998), _( -4533), _(-58333), _(68692), _(16155), _(9966), _( 1095), _(1817), _( -57), _(132), _( 8) },
    { _( -4), _( -85), _( 189), _(-1356), _(  605), _(-9219), _( -5818), _(-59838), _(67629), _(14548), _(9959), _(  814), _(1759), _( -83), _(125), _( 7) },
    { _( -4), _( -91), _( 177), _(-1428), _(  402), _(-9416), _( -7154), _(-61289), _(66494), _(12980), _(9916), _(  545), _(1698), _(-106), _(117), _( 7) },
    { _( -5), _( -97), _( 163), _(-1498), _(  185), _(-9585), _( -8540), _(-62684), _(65290), _(11455), _(9838), _(  288), _(1634), _(-127), _(111), _( 6) },
    { _( -5), _(-104), _( 146), _(-1567), _(  -45), _(-9727), _( -9975), _(-64019), _(64019), _( 9975), _(9727), _(   45), _(1567), _(-146), _(104), _( 5) },
    { _( -6), _(-111), _( 127), _(-1634), _( -288), _(-9838), _(-11455), _(-65290), _(62684), _( 8540), _(9585), _( -185), _(1498), _(-163), _( 97), _( 5) },
    { _( -7), _(-117), _( 106), _(-1698), _( -545), _(-9916), _(-12980), _(-66494), _(61289), _( 7154), _(9416), _( -402), _(1428), _(-177), _( 91), _( 4) },
    { _( -7), _(-125), _(  83), _(-1759), _( -814), _(-9959), _(-14548), _(-67629), _(59838), _( 5818), _(9219), _( -605), _(1356), _(-189), _( 85), _( 4) },
    { _( -8), _(-132), _(  57), _(-1817), _(-1095), _(-9966), _(-16155), _(-68692), _(58333), _( 4533), _(8998), _( -794), _(1283), _(-200), _( 79), _( 3) },
    { _( -9), _(-139), _(  29), _(-1870), _(-1388), _(-9935), _(-17799), _(-69679), _(56778), _( 3300), _(8755), _( -970), _(1210), _(-208), _( 73), _( 3) },
    { _(-10), _(-147), _(  -2), _(-1919), _(-1692), _(-9863), _(-19478), _(-70590), _(55178), _( 2122), _(8491), _(-1131), _(1137), _(-215), _( 68), _( 2) },
    { _(-11), _(-154), _( -36), _(-1962), _(-2006), _(-9750), _(-21189), _(-71420), _(53534), _(  998), _(8209), _(-1280), _(1064), _(-221), _( 63), _( 2) },
    { _(-13), _(-161), _( -72), _(-2001), _(-2330), _(-9592), _(-22929), _(-72169), _(51853), _(  -70), _(7910), _(-1414), _( 991), _(-224), _( 58), _( 2) },
    { _(-14), _(-169), _(-111), _(-2032), _(-2663), _(-9389), _(-24694), _(-72835), _(50137), _(-1082), _(7597), _(-1535), _( 919), _(-227), _( 53), _( 2) },
    { _(-16), _(-176), _(-153), _(-2057), _(-3004), _(-9139), _(-26482), _(-73415), _(48390), _(-2037), _(7271), _(-1644), _( 848), _(-228), _( 49), _( 1) },
    { _(-17), _(-183), _(-197), _(-2075), _(-3351), _(-8840), _(-28289), _(-73908), _(46617), _(-2935), _(6935), _(-1739), _( 779), _(-228), _( 45), _( 1) },
    { _(-19), _(-190), _(-244), _(-2085), _(-3705), _(-8492), _(-30112), _(-74313), _(44821), _(-3776), _(6589), _(-1822), _( 711), _(-227), _( 41), _( 1) },
    { _(-21), _(-196), _(-294), _(-2087), _(-4063), _(-8092), _(-31947), _(-74630), _(43006), _(-4561), _(6237), _(-1893), _( 645), _(-225), _( 38), _( 1) },
    { _(-24), _(-202), _(-347), _(-2080), _(-4425), _(-7640), _(-33791), _(-74856), _(41176), _(-5288), _(5879), _(-1952), _( 581), _(-222), _( 35), _( 1) },
    { _(-26), _(-208), _(-401), _(-2063), _(-4788), _(-7134), _(-35640), _(-74992), _(39336), _(-5959), _(5517), _(-2000), _( 519), _(-218), _( 31), _( 1) }
};

#undef  _

// V-coefficients were expanded (<<) by V_COEFFICIENT_EXPAND
#define V_COEFFICIENT_EXPAND 27

#if defined(MPC_FIXED_POINT)
   #if defined(OPTIMIZE_FOR_SPEED)
      // define 32=32x32-multiplication for DCT-coefficients with samples, vcoef will be pre-shifted on creation
      // samples are rounded to +/- 2^19 as pre-shift before 32=32x32-multiply
      #define MPC_MULTIPLY_V(sample, vcoef) ( MPC_SHR_RND(sample, 12) * vcoef )
      
      // round to +/- 2^16 as pre-shift before 32=32x32-multiply
      #define MPC_MAKE_INVCOS(value) (MPC_SHR_RND(value, 15))
   #else
      // define 64=32x32-multiplication for DCT-coefficients with samples. Via usage of MPC_FRACT highly optimized assembler might be used
      // MULTIPLY_FRACT will do >>32 after multiplication, as V-coef were expanded by V_COEFFICIENT_EXPAND we'll correct this on the result.
      // Will loose 5bit accuracy on result in fract part without effect on final audio result
      #define MPC_MULTIPLY_V(sample, vcoef) ( (MPC_MULTIPLY_FRACT(sample, vcoef)) << (32-V_COEFFICIENT_EXPAND) )
      
      // directly use accurate 32bit-coefficients
      #define MPC_MAKE_INVCOS(value) (value)
   #endif
#else
   // for floating point use the standard multiplication macro
   #define MPC_MULTIPLY_V(sample, vcoef) ( MPC_MULTIPLY(sample, vcoef) )
   
   // downscale the accurate 32bit-coefficients and convert to float
   #define MPC_MAKE_INVCOS(value) MAKE_MPC_SAMPLE((double)value/(double)(1<<V_COEFFICIENT_EXPAND))
#endif

// define constants for DCT-synthesis
// INVCOSxx = (0.5 / cos(xx*PI/64)) << 27, <<27 to saturate to +/- 2^31
#define INVCOS01 MPC_MAKE_INVCOS(  67189797)
#define INVCOS02 MPC_MAKE_INVCOS(  67433575)
#define INVCOS03 MPC_MAKE_INVCOS(  67843164)
#define INVCOS04 MPC_MAKE_INVCOS(  68423604)
#define INVCOS05 MPC_MAKE_INVCOS(  69182167)
#define INVCOS06 MPC_MAKE_INVCOS(  70128577)
#define INVCOS07 MPC_MAKE_INVCOS(  71275330)
#define INVCOS08 MPC_MAKE_INVCOS(  72638111)
#define INVCOS09 MPC_MAKE_INVCOS(  74236348)
#define INVCOS10 MPC_MAKE_INVCOS(  76093940)
#define INVCOS11 MPC_MAKE_INVCOS(  78240207)
#define INVCOS12 MPC_MAKE_INVCOS(  80711144)
#define INVCOS13 MPC_MAKE_INVCOS(  83551089)
#define INVCOS14 MPC_MAKE_INVCOS(  86814950)
#define INVCOS15 MPC_MAKE_INVCOS(  90571242)
#define INVCOS16 MPC_MAKE_INVCOS(  94906266)
#define INVCOS17 MPC_MAKE_INVCOS(  99929967)
#define INVCOS18 MPC_MAKE_INVCOS( 105784321)
#define INVCOS19 MPC_MAKE_INVCOS( 112655602)
#define INVCOS20 MPC_MAKE_INVCOS( 120792764)
#define INVCOS21 MPC_MAKE_INVCOS( 130535899)
#define INVCOS22 MPC_MAKE_INVCOS( 142361749)
#define INVCOS23 MPC_MAKE_INVCOS( 156959571)
#define INVCOS24 MPC_MAKE_INVCOS( 175363913)
#define INVCOS25 MPC_MAKE_INVCOS( 199201203)
#define INVCOS26 MPC_MAKE_INVCOS( 231182936)
#define INVCOS27 MPC_MAKE_INVCOS( 276190692)
#define INVCOS28 MPC_MAKE_INVCOS( 343988688)
#define INVCOS29 MPC_MAKE_INVCOS( 457361460)
#define INVCOS30 MPC_MAKE_INVCOS( 684664578)
#define INVCOS31 MPC_MAKE_INVCOS(1367679739)

static inline void 
mpc_calculate_new_V ( const MPC_SAMPLE_FORMAT * Sample, MPC_SAMPLE_FORMAT * V )
{
    // Calculating new V-buffer values for left channel
    // calculate new V-values (ISO-11172-3, p. 39)
    // based upon fast-MDCT algorithm by Byeong Gi Lee
    MPC_SAMPLE_FORMAT A[16];
    MPC_SAMPLE_FORMAT B[16];
    MPC_SAMPLE_FORMAT tmp;

    A[ 0] = Sample[ 0] + Sample[31];
    A[ 1] = Sample[ 1] + Sample[30];
    A[ 2] = Sample[ 2] + Sample[29];
    A[ 3] = Sample[ 3] + Sample[28];
    A[ 4] = Sample[ 4] + Sample[27];
    A[ 5] = Sample[ 5] + Sample[26];
    A[ 6] = Sample[ 6] + Sample[25];
    A[ 7] = Sample[ 7] + Sample[24];
    A[ 8] = Sample[ 8] + Sample[23];
    A[ 9] = Sample[ 9] + Sample[22];
    A[10] = Sample[10] + Sample[21];
    A[11] = Sample[11] + Sample[20];
    A[12] = Sample[12] + Sample[19];
    A[13] = Sample[13] + Sample[18];
    A[14] = Sample[14] + Sample[17];
    A[15] = Sample[15] + Sample[16];
    // 16 adds

    B[ 0] = A[ 0] + A[15];
    B[ 1] = A[ 1] + A[14];
    B[ 2] = A[ 2] + A[13];
    B[ 3] = A[ 3] + A[12];
    B[ 4] = A[ 4] + A[11];
    B[ 5] = A[ 5] + A[10];
    B[ 6] = A[ 6] + A[ 9];
    B[ 7] = A[ 7] + A[ 8];;
    B[ 8] = MPC_MULTIPLY_V((A[ 0] - A[15]), INVCOS02);
    B[ 9] = MPC_MULTIPLY_V((A[ 1] - A[14]), INVCOS06);
    B[10] = MPC_MULTIPLY_V((A[ 2] - A[13]), INVCOS10);
    B[11] = MPC_MULTIPLY_V((A[ 3] - A[12]), INVCOS14);
    B[12] = MPC_MULTIPLY_V((A[ 4] - A[11]), INVCOS18);
    B[13] = MPC_MULTIPLY_V((A[ 5] - A[10]), INVCOS22);
    B[14] = MPC_MULTIPLY_V((A[ 6] - A[ 9]), INVCOS26);
    B[15] = MPC_MULTIPLY_V((A[ 7] - A[ 8]), INVCOS30);
    // 8 adds, 8 subs, 8 muls, 8 shifts

    A[ 0] = B[ 0] + B[ 7];
    A[ 1] = B[ 1] + B[ 6];
    A[ 2] = B[ 2] + B[ 5];
    A[ 3] = B[ 3] + B[ 4];
    A[ 4] = MPC_MULTIPLY_V((B[ 0] - B[ 7]), INVCOS04);
    A[ 5] = MPC_MULTIPLY_V((B[ 1] - B[ 6]), INVCOS12);
    A[ 6] = MPC_MULTIPLY_V((B[ 2] - B[ 5]), INVCOS20);
    A[ 7] = MPC_MULTIPLY_V((B[ 3] - B[ 4]), INVCOS28);
    A[ 8] = B[ 8] + B[15];
    A[ 9] = B[ 9] + B[14];
    A[10] = B[10] + B[13];
    A[11] = B[11] + B[12];
    A[12] = MPC_MULTIPLY_V((B[ 8] - B[15]), INVCOS04);
    A[13] = MPC_MULTIPLY_V((B[ 9] - B[14]), INVCOS12);
    A[14] = MPC_MULTIPLY_V((B[10] - B[13]), INVCOS20);
    A[15] = MPC_MULTIPLY_V((B[11] - B[12]), INVCOS28);
    // 8 adds, 8 subs, 8 muls, 8 shifts

    B[ 0] = A[ 0] + A[ 3];
    B[ 1] = A[ 1] + A[ 2];
    B[ 2] = MPC_MULTIPLY_V((A[ 0] - A[ 3]), INVCOS08);
    B[ 3] = MPC_MULTIPLY_V((A[ 1] - A[ 2]), INVCOS24);
    B[ 4] = A[ 4] + A[ 7];
    B[ 5] = A[ 5] + A[ 6];
    B[ 6] = MPC_MULTIPLY_V((A[ 4] - A[ 7]), INVCOS08);
    B[ 7] = MPC_MULTIPLY_V((A[ 5] - A[ 6]), INVCOS24);
    B[ 8] = A[ 8] + A[11];
    B[ 9] = A[ 9] + A[10];
    B[10] = MPC_MULTIPLY_V((A[ 8] - A[11]), INVCOS08);
    B[11] = MPC_MULTIPLY_V((A[ 9] - A[10]), INVCOS24);
    B[12] = A[12] + A[15];
    B[13] = A[13] + A[14];
    B[14] = MPC_MULTIPLY_V((A[12] - A[15]), INVCOS08);
    B[15] = MPC_MULTIPLY_V((A[13] - A[14]), INVCOS24);
    // 8 adds, 8 subs, 8 muls, 8 shifts

    A[ 0] = B[ 0] + B[ 1];
    A[ 1] = MPC_MULTIPLY_V((B[ 0] - B[ 1]), INVCOS16);
    A[ 2] = B[ 2] + B[ 3];
    A[ 3] = MPC_MULTIPLY_V((B[ 2] - B[ 3]), INVCOS16);
    A[ 4] = B[ 4] + B[ 5];
    A[ 5] = MPC_MULTIPLY_V((B[ 4] - B[ 5]), INVCOS16);
    A[ 6] = B[ 6] + B[ 7];
    A[ 7] = MPC_MULTIPLY_V((B[ 6] - B[ 7]), INVCOS16);
    A[ 8] = B[ 8] + B[ 9];
    A[ 9] = MPC_MULTIPLY_V((B[ 8] - B[ 9]), INVCOS16);
    A[10] = B[10] + B[11];
    A[11] = MPC_MULTIPLY_V((B[10] - B[11]), INVCOS16);
    A[12] = B[12] + B[13];
    A[13] = MPC_MULTIPLY_V((B[12] - B[13]), INVCOS16);
    A[14] = B[14] + B[15];
    A[15] = MPC_MULTIPLY_V((B[14] - B[15]), INVCOS16);
    // 8 adds, 8 subs, 8 muls, 8 shifts

    // multiple used expressions: -(A[12] + A[14] + A[15])
    V[48] = -A[ 0];
    V[ 0] =  A[ 1];
    V[40] = -A[ 2] - (V[ 8] = A[ 3]);
    V[36] = -((V[ 4] = A[ 5] + (V[12] = A[ 7])) + A[ 6]);
    V[44] = - A[ 4] - A[ 6] - A[ 7];
    V[ 6] = (V[10] = A[11] + (V[14] = A[15])) + A[13];
    V[38] = (V[34] = -(V[ 2] = A[ 9] + A[13] + A[15]) - A[14]) + A[ 9] - A[10] - A[11];
    V[46] = (tmp = -(A[12] + A[14] + A[15])) - A[ 8];
    V[42] = tmp - A[10] - A[11];
    // 9 adds, 9 subs

    A[ 0] = MPC_MULTIPLY_V((Sample[ 0] - Sample[31]), INVCOS01);
    A[ 1] = MPC_MULTIPLY_V((Sample[ 1] - Sample[30]), INVCOS03);
    A[ 2] = MPC_MULTIPLY_V((Sample[ 2] - Sample[29]), INVCOS05);
    A[ 3] = MPC_MULTIPLY_V((Sample[ 3] - Sample[28]), INVCOS07);
    A[ 4] = MPC_MULTIPLY_V((Sample[ 4] - Sample[27]), INVCOS09);
    A[ 5] = MPC_MULTIPLY_V((Sample[ 5] - Sample[26]), INVCOS11);
    A[ 6] = MPC_MULTIPLY_V((Sample[ 6] - Sample[25]), INVCOS13);
    A[ 7] = MPC_MULTIPLY_V((Sample[ 7] - Sample[24]), INVCOS15);
    A[ 8] = MPC_MULTIPLY_V((Sample[ 8] - Sample[23]), INVCOS17);
    A[ 9] = MPC_MULTIPLY_V((Sample[ 9] - Sample[22]), INVCOS19);
    A[10] = MPC_MULTIPLY_V((Sample[10] - Sample[21]), INVCOS21);
    A[11] = MPC_MULTIPLY_V((Sample[11] - Sample[20]), INVCOS23);
    A[12] = MPC_MULTIPLY_V((Sample[12] - Sample[19]), INVCOS25);
    A[13] = MPC_MULTIPLY_V((Sample[13] - Sample[18]), INVCOS27);
    A[14] = MPC_MULTIPLY_V((Sample[14] - Sample[17]), INVCOS29);
    A[15] = MPC_MULTIPLY_V((Sample[15] - Sample[16]), INVCOS31);
    // 16 subs, 16 muls, 16 shifts

    B[ 0] = A[ 0] + A[15];
    B[ 1] = A[ 1] + A[14];
    B[ 2] = A[ 2] + A[13];
    B[ 3] = A[ 3] + A[12];
    B[ 4] = A[ 4] + A[11];
    B[ 5] = A[ 5] + A[10];
    B[ 6] = A[ 6] + A[ 9];
    B[ 7] = A[ 7] + A[ 8];
    B[ 8] = MPC_MULTIPLY_V((A[ 0] - A[15]), INVCOS02);
    B[ 9] = MPC_MULTIPLY_V((A[ 1] - A[14]), INVCOS06);
    B[10] = MPC_MULTIPLY_V((A[ 2] - A[13]), INVCOS10);
    B[11] = MPC_MULTIPLY_V((A[ 3] - A[12]), INVCOS14);
    B[12] = MPC_MULTIPLY_V((A[ 4] - A[11]), INVCOS18);
    B[13] = MPC_MULTIPLY_V((A[ 5] - A[10]), INVCOS22);
    B[14] = MPC_MULTIPLY_V((A[ 6] - A[ 9]), INVCOS26);
    B[15] = MPC_MULTIPLY_V((A[ 7] - A[ 8]), INVCOS30);
    // 8 adds, 8 subs, 8 muls, 8 shift

    A[ 0] = B[ 0] + B[ 7];
    A[ 1] = B[ 1] + B[ 6];
    A[ 2] = B[ 2] + B[ 5];
    A[ 3] = B[ 3] + B[ 4];
    A[ 4] = MPC_MULTIPLY_V((B[ 0] - B[ 7]), INVCOS04);
    A[ 5] = MPC_MULTIPLY_V((B[ 1] - B[ 6]), INVCOS12);
    A[ 6] = MPC_MULTIPLY_V((B[ 2] - B[ 5]), INVCOS20);
    A[ 7] = MPC_MULTIPLY_V((B[ 3] - B[ 4]), INVCOS28);
    A[ 8] = B[ 8] + B[15];
    A[ 9] = B[ 9] + B[14];
    A[10] = B[10] + B[13];
    A[11] = B[11] + B[12];
    A[12] = MPC_MULTIPLY_V((B[ 8] - B[15]), INVCOS04);
    A[13] = MPC_MULTIPLY_V((B[ 9] - B[14]), INVCOS12);
    A[14] = MPC_MULTIPLY_V((B[10] - B[13]), INVCOS20);
    A[15] = MPC_MULTIPLY_V((B[11] - B[12]), INVCOS28);
    // 8 adds, 8 subs, 8 muls, 8 shift

    B[ 0] = A[ 0] + A[ 3];
    B[ 1] = A[ 1] + A[ 2];
    B[ 2] = MPC_MULTIPLY_V((A[ 0] - A[ 3]), INVCOS08);
    B[ 3] = MPC_MULTIPLY_V((A[ 1] - A[ 2]), INVCOS24);
    B[ 4] = A[ 4] + A[ 7];
    B[ 5] = A[ 5] + A[ 6];
    B[ 6] = MPC_MULTIPLY_V((A[ 4] - A[ 7]), INVCOS08);
    B[ 7] = MPC_MULTIPLY_V((A[ 5] - A[ 6]), INVCOS24);
    B[ 8] = A[ 8] + A[11];
    B[ 9] = A[ 9] + A[10];
    B[10] = MPC_MULTIPLY_V((A[ 8] - A[11]), INVCOS08);
    B[11] = MPC_MULTIPLY_V((A[ 9] - A[10]), INVCOS24);
    B[12] = A[12] + A[15];
    B[13] = A[13] + A[14];
    B[14] = MPC_MULTIPLY_V((A[12] - A[15]), INVCOS08);
    B[15] = MPC_MULTIPLY_V((A[13] - A[14]), INVCOS24);
    // 8 adds, 8 subs, 8 muls, 8 shift

    A[ 0] = B[ 0] + B[ 1];
    A[ 1] = MPC_MULTIPLY_V((B[ 0] - B[ 1]), INVCOS16);
    A[ 2] = B[ 2] + B[ 3];
    A[ 3] = MPC_MULTIPLY_V((B[ 2] - B[ 3]), INVCOS16);
    A[ 4] = B[ 4] + B[ 5];
    A[ 5] = MPC_MULTIPLY_V((B[ 4] - B[ 5]), INVCOS16);
    A[ 6] = B[ 6] + B[ 7];
    A[ 7] = MPC_MULTIPLY_V((B[ 6] - B[ 7]), INVCOS16);
    A[ 8] = B[ 8] + B[ 9];
    A[ 9] = MPC_MULTIPLY_V((B[ 8] - B[ 9]), INVCOS16);
    A[10] = B[10] + B[11];
    A[11] = MPC_MULTIPLY_V((B[10] - B[11]), INVCOS16);
    A[12] = B[12] + B[13];
    A[13] = MPC_MULTIPLY_V((B[12] - B[13]), INVCOS16);
    A[14] = B[14] + B[15];
    A[15] = MPC_MULTIPLY_V((B[14] - B[15]), INVCOS16);
    // 8 adds, 8 subs, 8 muls, 8 shift

    // multiple used expressions: A[ 4]+A[ 6]+A[ 7], A[ 9]+A[13]+A[15]
    V[ 5] = (V[11] = (V[13] = A[ 7] + (V[15] = A[15])) + A[11]) + A[ 5] + A[13];
    V[ 7] = (V[ 9] = A[ 3] + A[11] + A[15]) + A[13];
    V[33] = -(V[ 1] = A[ 1] + A[ 9] + A[13] + A[15]) - A[14];
    V[35] = -(V[ 3] = A[ 5] + A[ 7] + A[ 9] + A[13] + A[15]) - A[ 6] - A[14];
    V[37] = (tmp = -(A[10] + A[11] + A[13] + A[14] + A[15])) - A[ 5] - A[ 6] - A[ 7];
    V[39] = tmp - A[ 2] - A[ 3];
    V[41] = (tmp += A[13] - A[12]) - A[ 2] - A[ 3];
    V[43] = tmp - A[ 4] - A[ 6] - A[ 7];
    V[47] = (tmp = -(A[ 8] + A[12] + A[14] + A[15])) - A[ 0];
    V[45] = tmp - A[ 4] - A[ 6] - A[ 7];
    // 22 adds, 18 subs

    V[32] = -(V[ 0] = MPC_V_PRESHIFT(V[ 0]));
    V[31] = -(V[ 1] = MPC_V_PRESHIFT(V[ 1]));
    V[30] = -(V[ 2] = MPC_V_PRESHIFT(V[ 2]));
    V[29] = -(V[ 3] = MPC_V_PRESHIFT(V[ 3]));
    V[28] = -(V[ 4] = MPC_V_PRESHIFT(V[ 4]));
    V[27] = -(V[ 5] = MPC_V_PRESHIFT(V[ 5]));
    V[26] = -(V[ 6] = MPC_V_PRESHIFT(V[ 6]));
    V[25] = -(V[ 7] = MPC_V_PRESHIFT(V[ 7]));
    V[24] = -(V[ 8] = MPC_V_PRESHIFT(V[ 8]));
    V[23] = -(V[ 9] = MPC_V_PRESHIFT(V[ 9]));
    V[22] = -(V[10] = MPC_V_PRESHIFT(V[10]));
    V[21] = -(V[11] = MPC_V_PRESHIFT(V[11]));
    V[20] = -(V[12] = MPC_V_PRESHIFT(V[12]));
    V[19] = -(V[13] = MPC_V_PRESHIFT(V[13]));
    V[18] = -(V[14] = MPC_V_PRESHIFT(V[14]));
    V[17] = -(V[15] = MPC_V_PRESHIFT(V[15]));
    // 16 adds, 16 shifts (OPTIMIZE_FOR_SPEED only)

    V[63] =  (V[33] = MPC_V_PRESHIFT(V[33]));
    V[62] =  (V[34] = MPC_V_PRESHIFT(V[34]));
    V[61] =  (V[35] = MPC_V_PRESHIFT(V[35]));
    V[60] =  (V[36] = MPC_V_PRESHIFT(V[36]));
    V[59] =  (V[37] = MPC_V_PRESHIFT(V[37]));
    V[58] =  (V[38] = MPC_V_PRESHIFT(V[38]));
    V[57] =  (V[39] = MPC_V_PRESHIFT(V[39]));
    V[56] =  (V[40] = MPC_V_PRESHIFT(V[40]));
    V[55] =  (V[41] = MPC_V_PRESHIFT(V[41]));
    V[54] =  (V[42] = MPC_V_PRESHIFT(V[42]));
    V[53] =  (V[43] = MPC_V_PRESHIFT(V[43]));
    V[52] =  (V[44] = MPC_V_PRESHIFT(V[44]));
    V[51] =  (V[45] = MPC_V_PRESHIFT(V[45]));
    V[50] =  (V[46] = MPC_V_PRESHIFT(V[46]));
    V[49] =  (V[47] = MPC_V_PRESHIFT(V[47]));
    V[48] =  (V[48] = MPC_V_PRESHIFT(V[48]));
    // 16 adds, 16 shifts (OPTIMIZE_FOR_SPEED only)
    
    // OPTIMIZE_FOR_SPEED total: 143 adds, 107 subs, 80 muls, 112 shifts
    //                    total: 111 adds, 107 subs, 80 muls,  80 shifts
}

static inline void 
mpc_decoder_windowing_D(MPC_SAMPLE_FORMAT * Data, const MPC_SAMPLE_FORMAT * V)
{
   const MPC_SAMPLE_FORMAT *D = (const MPC_SAMPLE_FORMAT *) &Di_opt;
   mpc_int32_t k;
    
   #if defined(OPTIMIZE_FOR_SPEED)
      #if defined(CPU_ARM)
         // 32=32x32-multiply assembler for ARM
         for ( k = 0; k < 32; k++, V++ ) 
         {
             asm volatile (
               "ldmia %[D]!, { r0-r7 } \n\t"
               "ldr r8, [%[V]]         \n\t"
               "mul r9, r0, r8         \n\t"
               "ldr r8, [%[V], #96*4]  \n\t"
               "mla r9, r1, r8, r9     \n\t"
               "ldr r8, [%[V], #128*4] \n\t"
               "mla r9, r2, r8, r9     \n\t"
               "ldr r8, [%[V], #224*4] \n\t"
               "mla r9, r3, r8, r9     \n\t"
               "ldr r8, [%[V], #256*4] \n\t"
               "mla r9, r4, r8, r9     \n\t"
               "ldr r8, [%[V], #352*4] \n\t"
               "mla r9, r5, r8, r9     \n\t"
               "ldr r8, [%[V], #384*4] \n\t"
               "mla r9, r6, r8, r9     \n\t"
               "ldr r8, [%[V], #480*4] \n\t"
               "mla r9, r7, r8, r9     \n\t"
               "ldmia %[D]!, { r0-r7 } \n\t"
               "ldr r8, [%[V], #512*4] \n\t"
               "mla r9, r0, r8, r9     \n\t"
               "ldr r8, [%[V], #608*4] \n\t"
               "mla r9, r1, r8, r9     \n\t"
               "ldr r8, [%[V], #640*4] \n\t"
               "mla r9, r2, r8, r9     \n\t"
               "ldr r8, [%[V], #736*4] \n\t"
               "mla r9, r3, r8, r9     \n\t"
               "ldr r8, [%[V], #768*4] \n\t"
               "mla r9, r4, r8, r9     \n\t"
               "ldr r8, [%[V], #864*4] \n\t"
               "mla r9, r5, r8, r9     \n\t"
               "ldr r8, [%[V], #896*4] \n\t"
               "mla r9, r6, r8, r9     \n\t"
               "ldr r8, [%[V], #992*4] \n\t"
               "mla r9, r7, r8, r9     \n\t"
               "str r9, [%[Data]], #4  \n"  
               : [Data] "+r" (Data), [D] "+r" (D)
               : [V] "r" (V)
               : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9");
         }
      #else
      // 32=32x32-multiply (FIXED_POINT)
      for ( k = 0; k < 32; k++, D += 16, V++ ) 
      {
         *Data = V[  0]*D[ 0] + V[ 96]*D[ 1] + V[128]*D[ 2] + V[224]*D[ 3]
              + V[256]*D[ 4] + V[352]*D[ 5] + V[384]*D[ 6] + V[480]*D[ 7]
              + V[512]*D[ 8] + V[608]*D[ 9] + V[640]*D[10] + V[736]*D[11]
              + V[768]*D[12] + V[864]*D[13] + V[896]*D[14] + V[992]*D[15];
         Data += 1;
         // total: 16 muls, 15 adds
      }
        #endif
   #else
      #if defined(CPU_COLDFIRE)
         // 64=32x32-multiply assembler for Coldfire
         for ( k = 0; k < 32; k++, D += 16, V++ ) 
         {
            asm volatile (
               "movem.l (%[D]), %%d0-%%d3                    \n\t"
               "move.l (%[V]), %%a5                          \n\t"
               "mac.l %%d0, %%a5, (96*4, %[V]), %%a5, %%acc0 \n\t"
               "mac.l %%d1, %%a5, (128*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d2, %%a5, (224*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d3, %%a5, (256*4, %[V]), %%a5, %%acc0\n\t"
               "movem.l (4*4, %[D]), %%d0-%%d3               \n\t"
               "mac.l %%d0, %%a5, (352*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d1, %%a5, (384*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d2, %%a5, (480*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d3, %%a5, (512*4, %[V]), %%a5, %%acc0\n\t"
               "movem.l (8*4, %[D]), %%d0-%%d3               \n\t"
               "mac.l %%d0, %%a5, (608*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d1, %%a5, (640*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d2, %%a5, (736*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d3, %%a5, (768*4, %[V]), %%a5, %%acc0\n\t"
               "movem.l (12*4, %[D]), %%d0-%%d3              \n\t"
               "mac.l %%d0, %%a5, (864*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d1, %%a5, (896*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d2, %%a5, (992*4, %[V]), %%a5, %%acc0\n\t"
               "mac.l %%d3, %%a5, %%acc0                     \n\t"
               "movclr.l %%acc0, %%d0                        \n\t"
               "move.l %%d0, (%[Data])+                      \n"
               : [Data] "+a" (Data)
               : [V] "a" (V), [D] "a" (D)
               : "d0", "d1", "d2", "d3", "a5");
         }
      #elif defined(CPU_ARM)
         // 64=32x32-multiply assembler for ARM
         for ( k = 0; k < 32; k++, V++ ) 
         {
            asm volatile (
               "ldmia %[D]!, { r0-r3 } \n\t"
               "ldr r4, [%[V]]         \n\t"
               "smull r5, r6, r0, r4   \n\t"
               "ldr r4, [%[V], #96*4]  \n\t"
               "smlal r5, r6, r1, r4   \n\t"
               "ldr r4, [%[V], #128*4] \n\t"
               "smlal r5, r6, r2, r4   \n\t"
               "ldr r4, [%[V], #224*4] \n\t"
               "smlal r5, r6, r3, r4   \n\t"
               
               "ldmia %[D]!, { r0-r3 } \n\t"
               "ldr r4, [%[V], #256*4] \n\t"
               "smlal r5, r6, r0, r4   \n\t"
               "ldr r4, [%[V], #352*4] \n\t"
               "smlal r5, r6, r1, r4   \n\t"
               "ldr r4, [%[V], #384*4] \n\t"
               "smlal r5, r6, r2, r4   \n\t"
               "ldr r4, [%[V], #480*4] \n\t"
               "smlal r5, r6, r3, r4   \n\t"
               
               "ldmia %[D]!, { r0-r3 } \n\t"
               "ldr r4, [%[V], #512*4] \n\t"
               "smlal r5, r6, r0, r4   \n\t"
               "ldr r4, [%[V], #608*4] \n\t"
               "smlal r5, r6, r1, r4   \n\t"
               "ldr r4, [%[V], #640*4] \n\t"
               "smlal r5, r6, r2, r4   \n\t"
               "ldr r4, [%[V], #736*4] \n\t"
               "smlal r5, r6, r3, r4   \n\t"
               
               "ldmia %[D]!, { r0-r3 } \n\t"
               "ldr r4, [%[V], #768*4] \n\t"
               "smlal r5, r6, r0, r4   \n\t"
               "ldr r4, [%[V], #864*4] \n\t"
               "smlal r5, r6, r1, r4   \n\t"
               "ldr r4, [%[V], #896*4] \n\t"
               "smlal r5, r6, r2, r4   \n\t"
               "ldr r4, [%[V], #992*4] \n\t"
               "smlal r5, r6, r3, r4   \n\t"
               "mov r4, r6, lsl #1     \n\t"
               "orr r4, r4, r5, lsr #31\n\t"
               "str r4, [%[Data]], #4  \n"  
               : [Data] "+r" (Data), [D] "+r" (D)
               : [V] "r" (V)
               : "r0", "r1", "r2", "r3", "r4", "r5", "r6");
         }
      #else
         // 64=64x64-multiply (FIXED_POINT) or float=float*float (!FIXED_POINT) in C
         for ( k = 0; k < 32; k++, D += 16, V++ ) 
         {
            *Data = MPC_MULTIPLY_EX(V[  0],D[ 0],31) + MPC_MULTIPLY_EX(V[ 96],D[ 1],31) + MPC_MULTIPLY_EX(V[128],D[ 2],31) + MPC_MULTIPLY_EX(V[224],D[ 3],31)
                  + MPC_MULTIPLY_EX(V[256],D[ 4],31) + MPC_MULTIPLY_EX(V[352],D[ 5],31) + MPC_MULTIPLY_EX(V[384],D[ 6],31) + MPC_MULTIPLY_EX(V[480],D[ 7],31)
                  + MPC_MULTIPLY_EX(V[512],D[ 8],31) + MPC_MULTIPLY_EX(V[608],D[ 9],31) + MPC_MULTIPLY_EX(V[640],D[10],31) + MPC_MULTIPLY_EX(V[736],D[11],31)
                  + MPC_MULTIPLY_EX(V[768],D[12],31) + MPC_MULTIPLY_EX(V[864],D[13],31) + MPC_MULTIPLY_EX(V[896],D[14],31) + MPC_MULTIPLY_EX(V[992],D[15],31);
            Data += 1;
            // total: 16 muls, 15 adds, 16 shifts
         }
      #endif
   #endif
}

static void 
mpc_full_synthesis_filter(MPC_SAMPLE_FORMAT *OutData, MPC_SAMPLE_FORMAT *V, const MPC_SAMPLE_FORMAT *Y)
{
    mpc_uint32_t n;
    
    if (NULL != OutData)
    {    
        for ( n = 0; n < 36; n++, Y += 32, OutData += 32 ) 
        {
            V -= 64;
            mpc_calculate_new_V ( Y, V );
            mpc_decoder_windowing_D( OutData, V);
        }
     }
}

void
mpc_decoder_synthese_filter_float(mpc_decoder *d, MPC_SAMPLE_FORMAT *OutData) 
{
    /********* left channel ********/
    memmove(d->V_L + MPC_V_MEM, d->V_L, 960 * sizeof(MPC_SAMPLE_FORMAT) );

    mpc_full_synthesis_filter(
        OutData,
        (MPC_SAMPLE_FORMAT *)(d->V_L + MPC_V_MEM),
        (MPC_SAMPLE_FORMAT *)(d->Y_L [0]));

    /******** right channel ********/
    memmove(d->V_R + MPC_V_MEM, d->V_R, 960 * sizeof(MPC_SAMPLE_FORMAT) );

    mpc_full_synthesis_filter(
        (OutData == NULL ? NULL : OutData + MPC_FRAME_LENGTH),
        (MPC_SAMPLE_FORMAT *)(d->V_R + MPC_V_MEM),
        (MPC_SAMPLE_FORMAT *)(d->Y_R [0]));
}

/*******************************************/
/*                                         */
/*            dithered synthesis           */
/*                                         */
/*******************************************/

static const unsigned char    Parity [256] = {  // parity
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0
};

/*
 *  This is a simple random number generator with good quality for audio purposes.
 *  It consists of two polycounters with opposite rotation direction and different
 *  periods. The periods are coprime, so the total period is the product of both.
 *
 *     -------------------------------------------------------------------------------------------------
 * +-> |31:30:29:28:27:26:25:24:23:22:21:20:19:18:17:16:15:14:13:12:11:10: 9: 8: 7: 6: 5: 4: 3: 2: 1: 0|
 * |   -------------------------------------------------------------------------------------------------
 * |                                                                          |  |  |  |     |        |
 * |                                                                          +--+--+--+-XOR-+--------+
 * |                                                                                      |
 * +--------------------------------------------------------------------------------------+
 *
 *     -------------------------------------------------------------------------------------------------
 *     |31:30:29:28:27:26:25:24:23:22:21:20:19:18:17:16:15:14:13:12:11:10: 9: 8: 7: 6: 5: 4: 3: 2: 1: 0| <-+
 *     -------------------------------------------------------------------------------------------------   |
 *       |  |           |  |                                                                               |
 *       +--+----XOR----+--+                                                                               |
 *                |                                                                                        |
 *                +----------------------------------------------------------------------------------------+
 *
 *
 *  The first has an period of 3*5*17*257*65537, the second of 7*47*73*178481,
 *  which gives a period of 18.410.713.077.675.721.215. The result is the
 *  XORed values of both generators.
 */
mpc_uint32_t
mpc_random_int(mpc_decoder *d) 
{
#if 1
    mpc_uint32_t  t1, t2, t3, t4;

    t3   = t1 = d->__r1;   t4   = t2 = d->__r2;  // Parity calculation is done via table lookup, this is also available
    t1  &= 0xF5;        t2 >>= 25;               // on CPUs without parity, can be implemented in C and avoid unpredictable
    t1   = Parity [t1]; t2  &= 0x63;             // jumps and slow rotate through the carry flag operations.
    t1 <<= 31;          t2   = Parity [t2];

    return (d->__r1 = (t3 >> 1) | t1 ) ^ (d->__r2 = (t4 + t4) | t2 );
#else
    return (d->__r1 = (d->__r1 >> 1) | ((mpc_uint32_t)Parity [d->__r1 & 0xF5] << 31) ) ^
        (d->__r2 = (d->__r2 << 1) |  (mpc_uint32_t)Parity [(d->__r2 >> 25) & 0x63] );
#endif
}
