/*******************************************************************************
 * Copyright (C) 2020-2022 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */

#ifndef _MAX78000_VIDEO_CNN_H_
#define _MAX78000_VIDEO_CNN_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdint.h>
typedef int32_t q31_t;
typedef int16_t q15_t;

/* Return codes */
#define CNN_FAIL 0
#define CNN_OK   1

/*
  SUMMARY OF OPS
  Hardware: 200,391,136 ops (199,391,616 macc; 999,520 comp; 0 add; 0 mul; 0 bitwise)
    Layer 0: 4,906,496 ops (4,731,264 macc; 175,232 comp; 0 add; 0 mul; 0 bitwise)
    Layer 1: 50,642,048 ops (50,466,816 macc; 175,232 comp; 0 add; 0 mul; 0 bitwise)
    Layer 2: 25,496,256 ops (25,233,408 macc; 262,848 comp; 0 add; 0 mul; 0 bitwise)
    Layer 3: 50,554,432 ops (50,466,816 macc; 87,616 comp; 0 add; 0 mul; 0 bitwise)
    Layer 4: 12,151,296 ops (11,943,936 macc; 207,360 comp; 0 add; 0 mul; 0 bitwise)
    Layer 5: 11,964,672 ops (11,943,936 macc; 20,736 comp; 0 add; 0 mul; 0 bitwise)
    Layer 6: 23,929,344 ops (23,887,872 macc; 41,472 comp; 0 add; 0 mul; 0 bitwise)
    Layer 7 (backbone_conv8): 11,954,304 ops (11,943,936 macc; 10,368 comp; 0 add; 0 mul; 0 bitwise)
    Layer 8 (backbone_conv9): 759,456 ops (746,496 macc; 12,960 comp; 0 add; 0 mul; 0 bitwise)
    Layer 9 (backbone_conv10): 152,576 ops (147,456 macc; 5,120 comp; 0 add; 0 mul; 0 bitwise)
    Layer 10 (conv12_1): 73,984 ops (73,728 macc; 256 comp; 0 add; 0 mul; 0 bitwise)
    Layer 11 (conv12_2): 9,536 ops (9,216 macc; 320 comp; 0 add; 0 mul; 0 bitwise)
    Layer 12: 1,492,992 ops (1,492,992 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 13: 373,248 ops (373,248 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 14: 73,728 ops (73,728 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 15: 9,216 ops (9,216 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 16: 4,478,976 ops (4,478,976 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 17: 1,119,744 ops (1,119,744 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 18: 221,184 ops (221,184 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 19: 27,648 ops (27,648 macc; 0 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 339,552 bytes out of 442,368 bytes total (77%)
  Bias memory:   832 bytes out of 2,048 bytes total (41%)
*/

/* Number of outputs for this network */
#define CNN_NUM_OUTPUTS 192

/* Use this timer to time the inference */
#define CNN_INFERENCE_TIMER MXC_TMR0

/* Port pin actions used to signal that processing is active */
#define CNN_START
#define CNN_COMPLETE
#define SYS_START
#define SYS_COMPLETE

/* Run software SoftMax on unloaded data */
void softmax_q17p14_q15(const q31_t* vec_in, const uint16_t dim_vec, q15_t* p_out);
/* Shift the input, then calculate SoftMax */
void softmax_shift_q17p14_q15(q31_t* vec_in, const uint16_t dim_vec, uint8_t in_shift,
                              q15_t* p_out);

/* Stopwatch - holds the runtime when accelerator finishes */
extern volatile uint32_t cnn_time;

/* Custom memcopy routines used for weights and data */
void memcpy32(uint32_t* dst, const uint32_t* src, int n);
void memcpy32_const(uint32_t* dst, int n);

/* Enable clocks and power to accelerator, enable interrupt */
int cnn_enable(uint32_t clock_source, uint32_t clock_divider);

/* Disable clocks and power to accelerator */
int cnn_disable(void);

/* Perform minimum accelerator initialization so it can be configured */
int cnn_init(void);

/* Configure accelerator for the given network */
int cnn_configure(void);

/* Load accelerator weights */
int cnn_load_weights(void);

/* Verify accelerator weights (debug only) */
int cnn_verify_weights(void);

/* Load accelerator bias values (if needed) */
int cnn_load_bias(void);

/* Start accelerator processing */
int cnn_start(void);

/* Force stop accelerator */
int cnn_stop(void);

/* Continue accelerator after stop */
int cnn_continue(void);

/* Unload results from accelerator */
int cnn_unload(uint32_t* out_buf);

/* Turn on the boost circuit */
int cnn_boost_enable(mxc_gpio_regs_t* port, uint32_t pin);

/* Turn off the boost circuit */
int cnn_boost_disable(mxc_gpio_regs_t* port, uint32_t pin);

#endif /* _MAX78000_VIDEO_CNN_H_ */
