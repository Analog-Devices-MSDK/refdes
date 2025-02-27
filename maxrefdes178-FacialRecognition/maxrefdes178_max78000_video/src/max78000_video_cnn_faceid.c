/*******************************************************************************
 * Copyright (C) 2020-2021 Maxim Integrated Products, Inc., All rights Reserved.
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

// faceid
// Created using ./ai8xize.py --verbose --log --test-dir sdk/Examples/MAX78000/CNN --prefix faceid --checkpoint-file trained/ai85-faceid-qat8-q.pth.tar --config-file networks/faceid.yaml --fifo --device MAX78000 --compact-data --mexpress --timer 0 --display-checkpoint

// DO NOT EDIT - regenerate this file instead!

// Configuring 9 layers:
// Layer 0: 3x160x120 (streaming HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 16x160x120 output
// Layer 1: 16x160x120 (streaming HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 32x80x60 output
// Layer 2: 32x80x60 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 32x40x30 output
// Layer 3: 32x40x30 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x20x15 output
// Layer 4: 64x20x15 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x10x7 output
// Layer 5: 64x10x7 (HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x10x7 output
// Layer 6: 64x10x7 (HWC data), no pooling, conv2d with kernel size 3x3, stride 1/1, pad 1/1, 64x10x7 output
// Layer 7: 64x10x7 (HWC data), 2x2 max pool with stride 2/2, conv2d with kernel size 1x1, stride 1/1, pad 0/0, 512x5x3 output
// Layer 8: 512x5x3 (HWC data), 5x3 avg pool with stride 1/1, no convolution, 512x1x1 output

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <gcfr_regs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mxc.h>
#include "bias_2.h"
#include "max78000_video_cnn_faceid.h"
//#include "max78000_video_weights.h"


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
volatile uint32_t cnn_time_faceid; // Stopwatch


//-----------------------------------------------------------------------------
// Local function declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
void CNN_ISR_faceid(void)
{
  // Acknowledge interrupt to all quadrants
  *((volatile uint32_t *) 0x50100000) &= ~((1<<12) | (1<<14) | 1);
  *((volatile uint32_t *) 0x50500000) &= ~((1<<12) | (1<<14) | 1);
  *((volatile uint32_t *) 0x50900000) &= ~((1<<12) | (1<<14) | 1);
  *((volatile uint32_t *) 0x50d00000) &= ~((1<<12) | (1<<14) | 1);

  CNN_COMPLETE; // Signal that processing is complete
#ifdef CNN_INFERENCE_TIMER
  cnn_time_faceid = MXC_TMR_SW_Stop(CNN_INFERENCE_TIMER);
#else
  cnn_time_faceid = 1;
#endif
}

int cnn_continue_faceid(void)
{
  cnn_time_faceid = 0;

  *((volatile uint32_t *) 0x50100000) |= 1; // Re-enable group 0

  return CNN_OK;
}

int cnn_stop_faceid(void)
{
  *((volatile uint32_t *) 0x50100000) &= ~1; // Disable group 0

  return CNN_OK;
}

void memcpy32_faceid(uint32_t *dst, const uint32_t *src, int n)
{
  while (n-- > 0) {
    *dst++ = *src++;
  }
}

static void memcpy_8to32(uint32_t *dst, const uint8_t *src, int n)
{
    while (n-- > 0) {
        *dst++ = *src++;
    }
}

static const uint8_t bias_0[] = BIAS_0;
static const uint8_t bias_1[] = BIAS_1;
static const uint8_t bias_2[] = BIAS_2;
static const uint8_t bias_3[] = BIAS_3;
int cnn_load_bias_faceid(void)
{
  memcpy_8to32((uint32_t *) 0x50108000, bias_0, sizeof(uint8_t) * 320);
  memcpy_8to32((uint32_t *) 0x50508000, bias_1, sizeof(uint8_t) * 336);
  memcpy_8to32((uint32_t *) 0x50908000, bias_2, sizeof(uint8_t) * 320);
  memcpy_8to32((uint32_t *) 0x50d08000, bias_3, sizeof(uint8_t) * 320);

    return CNN_OK;
}
int cnn_init_faceid(void)
{
  *((volatile uint32_t *) 0x50001000) = 0x00000000; // AON control
  *((volatile uint32_t *) 0x50100000) = 0x00108008; // Stop SM
  *((volatile uint32_t *) 0x50100004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50100008) = 0x00000011; // Layer count
  *((volatile uint32_t *) 0x50500000) = 0x00108008; // Stop SM
  *((volatile uint32_t *) 0x50500004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50500008) = 0x00000011; // Layer count
  *((volatile uint32_t *) 0x50900000) = 0x00108008; // Stop SM
  *((volatile uint32_t *) 0x50900004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50900008) = 0x00000011; // Layer count
  *((volatile uint32_t *) 0x50d00000) = 0x00108008; // Stop SM
  *((volatile uint32_t *) 0x50d00004) = 0x0000040e; // SRAM control
  *((volatile uint32_t *) 0x50d00008) = 0x00000011; // Layer count

  return CNN_OK;
}

int cnn_configure_faceid(void)
{
  // Layer 0 group 0
  // Layer 0 quadrant 0
  *((volatile uint32_t *) 0x50100010) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50100090) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50100310) = 0x00010800; // SRAM write ptr
  *((volatile uint32_t *) 0x50100410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100590) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50100a10) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100610) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50100690) = 0x0000006f; // TRAM ptr max
  *((volatile uint32_t *) 0x50100790) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100710) = 0x00070007; // Mask and processor enables
  *((volatile uint32_t *) 0x50100810) = 0x00000001; // Stream processing start
  *((volatile uint32_t *) 0x50100910) = 0x00000002; // Rollover
  *((volatile uint32_t *) 0x50100990) = 0x00003100; // Input frame size

  // Layer 0 quadrant 1
  *((volatile uint32_t *) 0x50500010) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50500090) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50500310) = 0x00010800; // SRAM write ptr
  *((volatile uint32_t *) 0x50500410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500590) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a10) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500610) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50500690) = 0x0000006f; // TRAM ptr max
  *((volatile uint32_t *) 0x50500790) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500810) = 0x00000001; // Stream processing start
  *((volatile uint32_t *) 0x50500910) = 0x00000002; // Rollover
  *((volatile uint32_t *) 0x50500990) = 0x00003100; // Input frame size

  // Layer 0 quadrant 2
  *((volatile uint32_t *) 0x50900010) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50900090) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50900310) = 0x00010800; // SRAM write ptr
  *((volatile uint32_t *) 0x50900410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900590) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a10) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50900610) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50900690) = 0x0000006f; // TRAM ptr max
  *((volatile uint32_t *) 0x50900790) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50900810) = 0x00000001; // Stream processing start
  *((volatile uint32_t *) 0x50900910) = 0x00000002; // Rollover
  *((volatile uint32_t *) 0x50900990) = 0x00003100; // Input frame size

  // Layer 0 quadrant 3
  *((volatile uint32_t *) 0x50d00010) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50d00090) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50d00310) = 0x00010800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00410) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00590) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a10) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00610) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50d00690) = 0x0000006f; // TRAM ptr max
  *((volatile uint32_t *) 0x50d00790) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50d00810) = 0x00000001; // Stream processing start
  *((volatile uint32_t *) 0x50d00910) = 0x00000002; // Rollover
  *((volatile uint32_t *) 0x50d00990) = 0x00003100; // Input frame size

  // Layer 1 quadrant 0
  *((volatile uint32_t *) 0x50100014) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50100094) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50100194) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100214) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50100294) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100314) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50100414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100514) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50100594) = 0x0000cba0; // Layer control
  *((volatile uint32_t *) 0x50100a14) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50100614) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50100694) = 0x007000a7; // TRAM ptr max
  *((volatile uint32_t *) 0x50100794) = 0x00026000; // Post processing register
  *((volatile uint32_t *) 0x50100814) = 0x0000015c; // Stream processing start
  *((volatile uint32_t *) 0x50100894) = 0x00740021; // Stream processing delta
  *((volatile uint32_t *) 0x50100914) = 0x000001ce; // Rollover

  // Layer 1 quadrant 1
  *((volatile uint32_t *) 0x50500014) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50500094) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50500194) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500214) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50500294) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500314) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500514) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50500594) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50500a14) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50500614) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50500694) = 0x007000a7; // TRAM ptr max
  *((volatile uint32_t *) 0x50500794) = 0x00026000; // Post processing register
  *((volatile uint32_t *) 0x50500814) = 0x0000015c; // Stream processing start
  *((volatile uint32_t *) 0x50500894) = 0x00740021; // Stream processing delta
  *((volatile uint32_t *) 0x50500914) = 0x000001ce; // Rollover

  // Layer 1 quadrant 2
  *((volatile uint32_t *) 0x50900014) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50900094) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50900194) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900214) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50900294) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900314) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50900414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900514) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50900594) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50900a14) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50900614) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50900694) = 0x007000a7; // TRAM ptr max
  *((volatile uint32_t *) 0x50900794) = 0x00026000; // Post processing register
  *((volatile uint32_t *) 0x50900714) = 0xffffffff; // Mask and processor enables
  *((volatile uint32_t *) 0x50900814) = 0x0000015c; // Stream processing start
  *((volatile uint32_t *) 0x50900894) = 0x00740021; // Stream processing delta
  *((volatile uint32_t *) 0x50900914) = 0x000001ce; // Rollover

  // Layer 1 quadrant 3
  *((volatile uint32_t *) 0x50d00014) = 0x00010071; // Rows
  *((volatile uint32_t *) 0x50d00094) = 0x00010071; // Columns
  *((volatile uint32_t *) 0x50d00194) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00214) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d00294) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00314) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00414) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00514) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d00594) = 0x00000ba0; // Layer control
  *((volatile uint32_t *) 0x50d00a14) = 0x0000f800; // Layer control 2
  *((volatile uint32_t *) 0x50d00614) = 0x000000f8; // Mask offset and count
  *((volatile uint32_t *) 0x50d00694) = 0x007000a7; // TRAM ptr max
  *((volatile uint32_t *) 0x50d00794) = 0x00026000; // Post processing register
  *((volatile uint32_t *) 0x50d00714) = 0xffffffff; // Mask and processor enables
  *((volatile uint32_t *) 0x50d00814) = 0x0000015c; // Stream processing start
  *((volatile uint32_t *) 0x50d00894) = 0x00740021; // Stream processing delta
  *((volatile uint32_t *) 0x50d00914) = 0x000001ce; // Rollover

  // Layer 2 quadrant 0
  *((volatile uint32_t *) 0x50100018) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x50100098) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x50100418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50100518) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50100598) = 0x00006b20; // Layer control
  *((volatile uint32_t *) 0x50100a18) = 0x0000fc00; // Layer control 2
  *((volatile uint32_t *) 0x50100618) = 0x010001fc; // Mask offset and count
  *((volatile uint32_t *) 0x50100698) = 0x00000037; // TRAM ptr max
  *((volatile uint32_t *) 0x50100798) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50100718) = 0xffffffff; // Mask and processor enables

  // Layer 2 quadrant 1
  *((volatile uint32_t *) 0x50500018) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x50500098) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x50500418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50500518) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50500598) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a18) = 0x0000fc00; // Layer control 2
  *((volatile uint32_t *) 0x50500618) = 0x010001fc; // Mask offset and count
  *((volatile uint32_t *) 0x50500698) = 0x00000037; // TRAM ptr max
  *((volatile uint32_t *) 0x50500798) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50500718) = 0xffffffff; // Mask and processor enables

  // Layer 2 quadrant 2
  *((volatile uint32_t *) 0x50900018) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x50900098) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x50900418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50900518) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50900598) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a18) = 0x0000fc00; // Layer control 2
  *((volatile uint32_t *) 0x50900618) = 0x010001fc; // Mask offset and count
  *((volatile uint32_t *) 0x50900698) = 0x00000037; // TRAM ptr max
  *((volatile uint32_t *) 0x50900798) = 0x00c050c0; // Post processing register

  // Layer 2 quadrant 3
  *((volatile uint32_t *) 0x50d00018) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x50d00098) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x50d00418) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d00518) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d00598) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a18) = 0x0000fc00; // Layer control 2
  *((volatile uint32_t *) 0x50d00618) = 0x010001fc; // Mask offset and count
  *((volatile uint32_t *) 0x50d00698) = 0x00000037; // TRAM ptr max
  *((volatile uint32_t *) 0x50d00798) = 0x00c04000; // Post processing register

  // Layer 3 quadrant 0
  *((volatile uint32_t *) 0x5010001c) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x5010009c) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x5010019c) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5010021c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x5010029c) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5010031c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x5010041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5010059c) = 0x0000e9a0; // Layer control
  *((volatile uint32_t *) 0x50100a1c) = 0x0000bc00; // Layer control 2
  *((volatile uint32_t *) 0x5010061c) = 0x020002bc; // Mask offset and count
  *((volatile uint32_t *) 0x5010069c) = 0x0000001b; // TRAM ptr max
  *((volatile uint32_t *) 0x5010079c) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x5010071c) = 0xffffffff; // Mask and processor enables

  // Layer 3 quadrant 1
  *((volatile uint32_t *) 0x5050001c) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x5050009c) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x5050019c) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5050021c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x5050029c) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5050031c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x5050041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5050059c) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50500a1c) = 0x0000bc00; // Layer control 2
  *((volatile uint32_t *) 0x5050061c) = 0x020002bc; // Mask offset and count
  *((volatile uint32_t *) 0x5050069c) = 0x0000001b; // TRAM ptr max
  *((volatile uint32_t *) 0x5050079c) = 0x00c03120; // Post processing register
  *((volatile uint32_t *) 0x5050071c) = 0xffffffff; // Mask and processor enables

  // Layer 3 quadrant 2
  *((volatile uint32_t *) 0x5090001c) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x5090009c) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x5090019c) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5090021c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x5090029c) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5090031c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x5090041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5090059c) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50900a1c) = 0x0000bc00; // Layer control 2
  *((volatile uint32_t *) 0x5090061c) = 0x020002bc; // Mask offset and count
  *((volatile uint32_t *) 0x5090069c) = 0x0000001b; // TRAM ptr max
  *((volatile uint32_t *) 0x5090079c) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x5090071c) = 0xffffffff; // Mask and processor enables

  // Layer 3 quadrant 3
  *((volatile uint32_t *) 0x50d0001c) = 0x00010039; // Rows
  *((volatile uint32_t *) 0x50d0009c) = 0x00010039; // Columns
  *((volatile uint32_t *) 0x50d0019c) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d0021c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d0029c) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d0031c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d0041c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d0059c) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50d00a1c) = 0x0000bc00; // Layer control 2
  *((volatile uint32_t *) 0x50d0061c) = 0x020002bc; // Mask offset and count
  *((volatile uint32_t *) 0x50d0069c) = 0x0000001b; // TRAM ptr max
  *((volatile uint32_t *) 0x50d0079c) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50d0071c) = 0xffffffff; // Mask and processor enables

  // Layer 4 quadrant 0
  *((volatile uint32_t *) 0x50100020) = 0x0000001b; // Rows
  *((volatile uint32_t *) 0x501000a0) = 0x0000001b; // Columns
  *((volatile uint32_t *) 0x501003a0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004a0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50100520) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x501005a0) = 0x00006b20; // Layer control
  *((volatile uint32_t *) 0x50100a20) = 0x0000fc20; // Layer control 2
  *((volatile uint32_t *) 0x50100620) = 0x18c01bbc; // Mask offset and count
  *((volatile uint32_t *) 0x50100120) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007a0) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50100720) = 0xffffffff; // Mask and processor enables

  // Layer 4 quadrant 1
  *((volatile uint32_t *) 0x50500020) = 0x0000001b; // Rows
  *((volatile uint32_t *) 0x505000a0) = 0x0000001b; // Columns
  *((volatile uint32_t *) 0x505003a0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004a0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50500520) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x505005a0) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a20) = 0x0000fc20; // Layer control 2
  *((volatile uint32_t *) 0x50500620) = 0x18c01bbc; // Mask offset and count
  *((volatile uint32_t *) 0x50500120) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007a0) = 0x00c03000; // Post processing register
  *((volatile uint32_t *) 0x50500720) = 0xffffffff; // Mask and processor enables

  // Layer 4 quadrant 2
  *((volatile uint32_t *) 0x50900020) = 0x0000001b; // Rows
  *((volatile uint32_t *) 0x509000a0) = 0x0000001b; // Columns
  *((volatile uint32_t *) 0x509003a0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004a0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50900520) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x509005a0) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a20) = 0x0000fc20; // Layer control 2
  *((volatile uint32_t *) 0x50900620) = 0x18c01bbc; // Mask offset and count
  *((volatile uint32_t *) 0x50900120) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007a0) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50900720) = 0xffffffff; // Mask and processor enables

  // Layer 4 quadrant 3
  *((volatile uint32_t *) 0x50d00020) = 0x0000001b; // Rows
  *((volatile uint32_t *) 0x50d000a0) = 0x0000001b; // Columns
  *((volatile uint32_t *) 0x50d003a0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00420) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004a0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d00520) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005a0) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a20) = 0x0000fc20; // Layer control 2
  *((volatile uint32_t *) 0x50d00620) = 0x18c01bbc; // Mask offset and count
  *((volatile uint32_t *) 0x50d00120) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007a0) = 0x00c02000; // Post processing register

  // Layer 5 quadrant 0
  *((volatile uint32_t *) 0x50100024) = 0x0001001d; // Rows
  *((volatile uint32_t *) 0x501000a4) = 0x0001001d; // Columns
  *((volatile uint32_t *) 0x501001a4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50100224) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002a4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50100324) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50100424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005a4) = 0x0000e9a0; // Layer control
  *((volatile uint32_t *) 0x50100a24) = 0x00007e02; // Layer control 2
  *((volatile uint32_t *) 0x50100624) = 0x0320049e; // Mask offset and count
  *((volatile uint32_t *) 0x501006a4) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x501007a4) = 0x00805100; // Post processing register
  *((volatile uint32_t *) 0x50100724) = 0xffffffff; // Mask and processor enables

  // Layer 5 quadrant 1
  *((volatile uint32_t *) 0x50500024) = 0x0001001d; // Rows
  *((volatile uint32_t *) 0x505000a4) = 0x0001001d; // Columns
  *((volatile uint32_t *) 0x505001a4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50500224) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002a4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50500324) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005a4) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50500a24) = 0x00007e02; // Layer control 2
  *((volatile uint32_t *) 0x50500624) = 0x0320049e; // Mask offset and count
  *((volatile uint32_t *) 0x505006a4) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x505007a4) = 0x00804000; // Post processing register
  *((volatile uint32_t *) 0x50500724) = 0xffffffff; // Mask and processor enables

  // Layer 5 quadrant 2
  *((volatile uint32_t *) 0x50900024) = 0x0001001d; // Rows
  *((volatile uint32_t *) 0x509000a4) = 0x0001001d; // Columns
  *((volatile uint32_t *) 0x509001a4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50900224) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x509002a4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50900324) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50900424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509005a4) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50900a24) = 0x00007e02; // Layer control 2
  *((volatile uint32_t *) 0x50900624) = 0x0320049e; // Mask offset and count
  *((volatile uint32_t *) 0x509006a4) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x509007a4) = 0x00804000; // Post processing register
  *((volatile uint32_t *) 0x50900724) = 0xffffffff; // Mask and processor enables

  // Layer 5 quadrant 3
  *((volatile uint32_t *) 0x50d00024) = 0x0001001d; // Rows
  *((volatile uint32_t *) 0x50d000a4) = 0x0001001d; // Columns
  *((volatile uint32_t *) 0x50d001a4) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d00224) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d002a4) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d00324) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00424) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d005a4) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50d00a24) = 0x00007e02; // Layer control 2
  *((volatile uint32_t *) 0x50d00624) = 0x0320049e; // Mask offset and count
  *((volatile uint32_t *) 0x50d006a4) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007a4) = 0x00804000; // Post processing register
  *((volatile uint32_t *) 0x50d00724) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 0
  *((volatile uint32_t *) 0x50100028) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x501000a8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50100328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x501003a8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004a8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50100528) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x501005a8) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x50100a28) = 0x0000fc10; // Layer control 2
  *((volatile uint32_t *) 0x50100628) = 0x29a02b9c; // Mask offset and count
  *((volatile uint32_t *) 0x50100128) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007a8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50100728) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 1
  *((volatile uint32_t *) 0x50500028) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x505000a8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50500328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x505003a8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004a8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50500528) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x505005a8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a28) = 0x0000fc10; // Layer control 2
  *((volatile uint32_t *) 0x50500628) = 0x29a02b9c; // Mask offset and count
  *((volatile uint32_t *) 0x50500128) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007a8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50500728) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 2
  *((volatile uint32_t *) 0x50900028) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x509000a8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50900328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x509003a8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004a8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50900528) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x509005a8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a28) = 0x0000fc10; // Layer control 2
  *((volatile uint32_t *) 0x50900628) = 0x29a02b9c; // Mask offset and count
  *((volatile uint32_t *) 0x50900128) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007a8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50900728) = 0xffffffff; // Mask and processor enables

  // Layer 6 quadrant 3
  *((volatile uint32_t *) 0x50d00028) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x50d000a8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50d00328) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d003a8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00428) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004a8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d00528) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005a8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a28) = 0x0000fc10; // Layer control 2
  *((volatile uint32_t *) 0x50d00628) = 0x29a02b9c; // Mask offset and count
  *((volatile uint32_t *) 0x50d00128) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007a8) = 0x00c05000; // Post processing register
  *((volatile uint32_t *) 0x50d00728) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 0
  *((volatile uint32_t *) 0x5010002c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x501000ac) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x5010042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5010052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005ac) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x50100a2c) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x5010062c) = 0x04e005de; // Mask offset and count
  *((volatile uint32_t *) 0x501006ac) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x501007ac) = 0x00806000; // Post processing register
  *((volatile uint32_t *) 0x5010072c) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 1
  *((volatile uint32_t *) 0x5050002c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x505000ac) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x5050042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5050052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005ac) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500a2c) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x5050062c) = 0x04e005de; // Mask offset and count
  *((volatile uint32_t *) 0x505006ac) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x505007ac) = 0x00806000; // Post processing register
  *((volatile uint32_t *) 0x5050072c) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 2
  *((volatile uint32_t *) 0x5090002c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x509000ac) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x5090042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x5090052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x509005ac) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900a2c) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x5090062c) = 0x04e005de; // Mask offset and count
  *((volatile uint32_t *) 0x509006ac) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x509007ac) = 0x00807100; // Post processing register
  *((volatile uint32_t *) 0x5090072c) = 0xffffffff; // Mask and processor enables

  // Layer 7 quadrant 3
  *((volatile uint32_t *) 0x50d0002c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x50d000ac) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x50d0042c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d0052c) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005ac) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00a2c) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x50d0062c) = 0x04e005de; // Mask offset and count
  *((volatile uint32_t *) 0x50d006ac) = 0x0000000d; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007ac) = 0x00806000; // Post processing register
  *((volatile uint32_t *) 0x50d0072c) = 0xffffffff; // Mask and processor enables

  // Layer 8 quadrant 0
  *((volatile uint32_t *) 0x50100030) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x501000b0) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50100330) = 0x00000001; // SRAM write ptr
  *((volatile uint32_t *) 0x501003b0) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100530) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x501005b0) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50100a30) = 0x00000010; // Layer control 2
  *((volatile uint32_t *) 0x50100130) = 0x00000103; // 1D
  *((volatile uint32_t *) 0x501007b0) = 0x03000000; // Post processing register
  *((volatile uint32_t *) 0x50100730) = 0x0000ffff; // Mask and processor enables

  // Layer 8 quadrant 1
  *((volatile uint32_t *) 0x50500030) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x505000b0) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50500330) = 0x00008001; // SRAM write ptr
  *((volatile uint32_t *) 0x505003b0) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500530) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x505005b0) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500a30) = 0x00000010; // Layer control 2
  *((volatile uint32_t *) 0x50500130) = 0x00000103; // 1D
  *((volatile uint32_t *) 0x505007b0) = 0x03000000; // Post processing register
  *((volatile uint32_t *) 0x50500730) = 0x0000ffff; // Mask and processor enables

  // Layer 8 quadrant 2
  *((volatile uint32_t *) 0x50900030) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x509000b0) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50900330) = 0x00010001; // SRAM write ptr
  *((volatile uint32_t *) 0x509003b0) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900530) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x509005b0) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900a30) = 0x00000010; // Layer control 2
  *((volatile uint32_t *) 0x50900130) = 0x00000103; // 1D
  *((volatile uint32_t *) 0x509007b0) = 0x03000000; // Post processing register
  *((volatile uint32_t *) 0x50900730) = 0x0000ffff; // Mask and processor enables

  // Layer 8 quadrant 3
  *((volatile uint32_t *) 0x50d00030) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x50d000b0) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50d00330) = 0x00018001; // SRAM write ptr
  *((volatile uint32_t *) 0x50d003b0) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00530) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005b0) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00a30) = 0x00000010; // Layer control 2
  *((volatile uint32_t *) 0x50d00130) = 0x00000103; // 1D
  *((volatile uint32_t *) 0x50d007b0) = 0x03000000; // Post processing register
  *((volatile uint32_t *) 0x50d00730) = 0x0000ffff; // Mask and processor enables

  // Layer 9 quadrant 0
  *((volatile uint32_t *) 0x50100034) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x501000b4) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50100334) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x501003b4) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x501005b4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50100134) = 0x00046003; // 1D
  *((volatile uint32_t *) 0x501007b4) = 0x01000000; // Post processing register
  *((volatile uint32_t *) 0x50100734) = 0x0000ffff; // Mask and processor enables

  // Layer 9 quadrant 1
  *((volatile uint32_t *) 0x50500034) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x505000b4) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50500334) = 0x00008800; // SRAM write ptr
  *((volatile uint32_t *) 0x505003b4) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x505005b4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500134) = 0x00046003; // 1D
  *((volatile uint32_t *) 0x505007b4) = 0x01000000; // Post processing register
  *((volatile uint32_t *) 0x50500734) = 0x0000ffff; // Mask and processor enables

  // Layer 9 quadrant 2
  *((volatile uint32_t *) 0x50900034) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x509000b4) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50900334) = 0x00010800; // SRAM write ptr
  *((volatile uint32_t *) 0x509003b4) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x509005b4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900134) = 0x00046003; // 1D
  *((volatile uint32_t *) 0x509007b4) = 0x01000000; // Post processing register
  *((volatile uint32_t *) 0x50900734) = 0x0000ffff; // Mask and processor enables

  // Layer 9 quadrant 3
  *((volatile uint32_t *) 0x50d00034) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x50d000b4) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50d00334) = 0x00018800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d003b4) = 0x00002000; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d005b4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00134) = 0x00046003; // 1D
  *((volatile uint32_t *) 0x50d007b4) = 0x01000000; // Post processing register
  *((volatile uint32_t *) 0x50d00734) = 0x0000ffff; // Mask and processor enables

  // Layer 10 quadrant 0
  *((volatile uint32_t *) 0x50100038) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x501000b8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x501003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004b8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50100538) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x501005b8) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x50100a38) = 0x0000fc30; // Layer control 2
  *((volatile uint32_t *) 0x50100638) = 0x34e038dc; // Mask offset and count
  *((volatile uint32_t *) 0x50100138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007b8) = 0x00c03000; // Post processing register
  *((volatile uint32_t *) 0x50100738) = 0xffffffff; // Mask and processor enables

  // Layer 10 quadrant 1
  *((volatile uint32_t *) 0x50500038) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x505000b8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x505003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004b8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50500538) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x505005b8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a38) = 0x0000fc30; // Layer control 2
  *((volatile uint32_t *) 0x50500638) = 0x34e038dc; // Mask offset and count
  *((volatile uint32_t *) 0x50500138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007b8) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50500738) = 0xffffffff; // Mask and processor enables

  // Layer 10 quadrant 2
  *((volatile uint32_t *) 0x50900038) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x509000b8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x509003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004b8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50900538) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x509005b8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a38) = 0x0000fc30; // Layer control 2
  *((volatile uint32_t *) 0x50900638) = 0x34e038dc; // Mask offset and count
  *((volatile uint32_t *) 0x50900138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007b8) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50900738) = 0xffffffff; // Mask and processor enables

  // Layer 10 quadrant 3
  *((volatile uint32_t *) 0x50d00038) = 0x0000000d; // Rows
  *((volatile uint32_t *) 0x50d000b8) = 0x0000000d; // Columns
  *((volatile uint32_t *) 0x50d003b8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00438) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004b8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d00538) = 0x00000800; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005b8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a38) = 0x0000fc30; // Layer control 2
  *((volatile uint32_t *) 0x50d00638) = 0x34e038dc; // Mask offset and count
  *((volatile uint32_t *) 0x50d00138) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007b8) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50d00738) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 0
  *((volatile uint32_t *) 0x5010003c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x501000bc) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x501001bc) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5010023c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x501002bc) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5010033c) = 0x00009000; // SRAM write ptr
  *((volatile uint32_t *) 0x5010043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004bc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x501005bc) = 0x0000e9a0; // Layer control
  *((volatile uint32_t *) 0x50100a3c) = 0x0000bc13; // Layer control 2
  *((volatile uint32_t *) 0x5010063c) = 0x06600c5c; // Mask offset and count
  *((volatile uint32_t *) 0x501006bc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x501007bc) = 0x00c00000; // Post processing register
  *((volatile uint32_t *) 0x5010073c) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 1
  *((volatile uint32_t *) 0x5050003c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x505000bc) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x505001bc) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5050023c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x505002bc) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5050033c) = 0x00009000; // SRAM write ptr
  *((volatile uint32_t *) 0x5050043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004bc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x505005bc) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50500a3c) = 0x0000bc13; // Layer control 2
  *((volatile uint32_t *) 0x5050063c) = 0x06600c5c; // Mask offset and count
  *((volatile uint32_t *) 0x505006bc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x505007bc) = 0x00c010c0; // Post processing register
  *((volatile uint32_t *) 0x5050073c) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 2
  *((volatile uint32_t *) 0x5090003c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x509000bc) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x509001bc) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x5090023c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x509002bc) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x5090033c) = 0x00009000; // SRAM write ptr
  *((volatile uint32_t *) 0x5090043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004bc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x509005bc) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50900a3c) = 0x0000bc13; // Layer control 2
  *((volatile uint32_t *) 0x5090063c) = 0x06600c5c; // Mask offset and count
  *((volatile uint32_t *) 0x509006bc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x509007bc) = 0x00c00000; // Post processing register
  *((volatile uint32_t *) 0x5090073c) = 0xffffffff; // Mask and processor enables

  // Layer 11 quadrant 3
  *((volatile uint32_t *) 0x50d0003c) = 0x0001000f; // Rows
  *((volatile uint32_t *) 0x50d000bc) = 0x0001000f; // Columns
  *((volatile uint32_t *) 0x50d001bc) = 0x00000001; // Pooling rows
  *((volatile uint32_t *) 0x50d0023c) = 0x00000001; // Pooling columns
  *((volatile uint32_t *) 0x50d002bc) = 0x00000001; // Stride
  *((volatile uint32_t *) 0x50d0033c) = 0x00009000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d0043c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004bc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d005bc) = 0x000009a0; // Layer control
  *((volatile uint32_t *) 0x50d00a3c) = 0x0000bc13; // Layer control 2
  *((volatile uint32_t *) 0x50d0063c) = 0x06600c5c; // Mask offset and count
  *((volatile uint32_t *) 0x50d006bc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007bc) = 0x00c00000; // Post processing register
  *((volatile uint32_t *) 0x50d0073c) = 0xffffffff; // Mask and processor enables

  // Layer 12 quadrant 0
  *((volatile uint32_t *) 0x50100040) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x501000c0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x501003c0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100440) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004c0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50100540) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x501005c0) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x50100a40) = 0x0000fc21; // Layer control 2
  *((volatile uint32_t *) 0x50100640) = 0x6f60755c; // Mask offset and count
  *((volatile uint32_t *) 0x50100140) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007c0) = 0x00c02000; // Post processing register

  // Layer 12 quadrant 1
  *((volatile uint32_t *) 0x50500040) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x505000c0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x505003c0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500440) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004c0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50500540) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x505005c0) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a40) = 0x0000fc21; // Layer control 2
  *((volatile uint32_t *) 0x50500640) = 0x6f60755c; // Mask offset and count
  *((volatile uint32_t *) 0x50500140) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007c0) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50500740) = 0xffffffff; // Mask and processor enables

  // Layer 12 quadrant 2
  *((volatile uint32_t *) 0x50900040) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x509000c0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x509003c0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900440) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004c0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50900540) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x509005c0) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a40) = 0x0000fc21; // Layer control 2
  *((volatile uint32_t *) 0x50900640) = 0x6f60755c; // Mask offset and count
  *((volatile uint32_t *) 0x50900140) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007c0) = 0x00c03000; // Post processing register
  *((volatile uint32_t *) 0x50900740) = 0xffffffff; // Mask and processor enables

  // Layer 12 quadrant 3
  *((volatile uint32_t *) 0x50d00040) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x50d000c0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x50d003c0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00440) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004c0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d00540) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005c0) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a40) = 0x0000fc21; // Layer control 2
  *((volatile uint32_t *) 0x50d00640) = 0x6f60755c; // Mask offset and count
  *((volatile uint32_t *) 0x50d00140) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007c0) = 0x00c02000; // Post processing register
  *((volatile uint32_t *) 0x50d00740) = 0xffffffff; // Mask and processor enables

  // Layer 13 quadrant 0
  *((volatile uint32_t *) 0x50100044) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x501000c4) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x50100344) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50100444) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004c4) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x501005c4) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x50100a44) = 0x00007e12; // Layer control 2
  *((volatile uint32_t *) 0x50100644) = 0x0d20101e; // Mask offset and count
  *((volatile uint32_t *) 0x501006c4) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x501007c4) = 0x00804000; // Post processing register
  *((volatile uint32_t *) 0x50100744) = 0xffffffff; // Mask and processor enables

  // Layer 13 quadrant 1
  *((volatile uint32_t *) 0x50500044) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x505000c4) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x50500344) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50500444) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004c4) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x505005c4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500a44) = 0x00007e12; // Layer control 2
  *((volatile uint32_t *) 0x50500644) = 0x0d20101e; // Mask offset and count
  *((volatile uint32_t *) 0x505006c4) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x505007c4) = 0x00804000; // Post processing register
  *((volatile uint32_t *) 0x50500744) = 0xffffffff; // Mask and processor enables

  // Layer 13 quadrant 2
  *((volatile uint32_t *) 0x50900044) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x509000c4) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x50900344) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50900444) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004c4) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x509005c4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900a44) = 0x00007e12; // Layer control 2
  *((volatile uint32_t *) 0x50900644) = 0x0d20101e; // Mask offset and count
  *((volatile uint32_t *) 0x509006c4) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x509007c4) = 0x00804000; // Post processing register
  *((volatile uint32_t *) 0x50900744) = 0xffffffff; // Mask and processor enables

  // Layer 13 quadrant 3
  *((volatile uint32_t *) 0x50d00044) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x50d000c4) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x50d00344) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d00444) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004c4) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d005c4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00a44) = 0x00007e12; // Layer control 2
  *((volatile uint32_t *) 0x50d00644) = 0x0d20101e; // Mask offset and count
  *((volatile uint32_t *) 0x50d006c4) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007c4) = 0x00805080; // Post processing register
  *((volatile uint32_t *) 0x50d00744) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 0
  *((volatile uint32_t *) 0x50100048) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x501000c8) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x501003c8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100448) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004c8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50100548) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x501005c8) = 0x0000eb20; // Layer control
  *((volatile uint32_t *) 0x50100a48) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x50100648) = 0x9120951c; // Mask offset and count
  *((volatile uint32_t *) 0x50100148) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007c8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50100748) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 1
  *((volatile uint32_t *) 0x50500048) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x505000c8) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x505003c8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500448) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004c8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50500548) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x505005c8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50500a48) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x50500648) = 0x9120951c; // Mask offset and count
  *((volatile uint32_t *) 0x50500148) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007c8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50500748) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 2
  *((volatile uint32_t *) 0x50900048) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x509000c8) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x509003c8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900448) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004c8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50900548) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x509005c8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50900a48) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x50900648) = 0x9120951c; // Mask offset and count
  *((volatile uint32_t *) 0x50900148) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007c8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50900748) = 0xffffffff; // Mask and processor enables

  // Layer 14 quadrant 3
  *((volatile uint32_t *) 0x50d00048) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x50d000c8) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x50d003c8) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00448) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004c8) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d00548) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005c8) = 0x00000b20; // Layer control
  *((volatile uint32_t *) 0x50d00a48) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x50d00648) = 0x9120951c; // Mask offset and count
  *((volatile uint32_t *) 0x50d00148) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007c8) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50d00748) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 0
  *((volatile uint32_t *) 0x5010004c) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x501000cc) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x5010034c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x5010044c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004cc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x501005cc) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x50100a4c) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x5010064c) = 0x10a0149c; // Mask offset and count
  *((volatile uint32_t *) 0x501006cc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x501007cc) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x5010074c) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 1
  *((volatile uint32_t *) 0x5050004c) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x505000cc) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x5050034c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x5050044c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004cc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x505005cc) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500a4c) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x5050064c) = 0x10a0149c; // Mask offset and count
  *((volatile uint32_t *) 0x505006cc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x505007cc) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x5050074c) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 2
  *((volatile uint32_t *) 0x5090004c) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x509000cc) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x5090034c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x5090044c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004cc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x509005cc) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900a4c) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x5090064c) = 0x10a0149c; // Mask offset and count
  *((volatile uint32_t *) 0x509006cc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x509007cc) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x5090074c) = 0xffffffff; // Mask and processor enables

  // Layer 15 quadrant 3
  *((volatile uint32_t *) 0x50d0004c) = 0x00010008; // Rows
  *((volatile uint32_t *) 0x50d000cc) = 0x00010008; // Columns
  *((volatile uint32_t *) 0x50d0034c) = 0x00001000; // SRAM write ptr
  *((volatile uint32_t *) 0x50d0044c) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004cc) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d005cc) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00a4c) = 0x0000fc11; // Layer control 2
  *((volatile uint32_t *) 0x50d0064c) = 0x10a0149c; // Mask offset and count
  *((volatile uint32_t *) 0x50d006cc) = 0x00000006; // TRAM ptr max
  *((volatile uint32_t *) 0x50d007cc) = 0x00c04000; // Post processing register
  *((volatile uint32_t *) 0x50d0074c) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 0
  *((volatile uint32_t *) 0x50100050) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x501000d0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x501001d0) = 0x00000006; // Pooling rows
  *((volatile uint32_t *) 0x50100250) = 0x00000006; // Pooling columns
  *((volatile uint32_t *) 0x501003d0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100450) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501004d0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50100550) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x501005d0) = 0x0000e8a0; // Layer control
  *((volatile uint32_t *) 0x50100a50) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x50100650) = 0xb9a0bb9e; // Mask offset and count
  *((volatile uint32_t *) 0x50100150) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007d0) = 0x0080e000; // Post processing register
  *((volatile uint32_t *) 0x50100750) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 1
  *((volatile uint32_t *) 0x50500050) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x505000d0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x505001d0) = 0x00000006; // Pooling rows
  *((volatile uint32_t *) 0x50500250) = 0x00000006; // Pooling columns
  *((volatile uint32_t *) 0x505003d0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500450) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505004d0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50500550) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x505005d0) = 0x000008a0; // Layer control
  *((volatile uint32_t *) 0x50500a50) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x50500650) = 0xb9a0bb9e; // Mask offset and count
  *((volatile uint32_t *) 0x50500150) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007d0) = 0x0080e000; // Post processing register
  *((volatile uint32_t *) 0x50500750) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 2
  *((volatile uint32_t *) 0x50900050) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x509000d0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x509001d0) = 0x00000006; // Pooling rows
  *((volatile uint32_t *) 0x50900250) = 0x00000006; // Pooling columns
  *((volatile uint32_t *) 0x509003d0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900450) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509004d0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50900550) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x509005d0) = 0x000008a0; // Layer control
  *((volatile uint32_t *) 0x50900a50) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x50900650) = 0xb9a0bb9e; // Mask offset and count
  *((volatile uint32_t *) 0x50900150) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007d0) = 0x0080e000; // Post processing register
  *((volatile uint32_t *) 0x50900750) = 0xffffffff; // Mask and processor enables

  // Layer 16 quadrant 3
  *((volatile uint32_t *) 0x50d00050) = 0x00000006; // Rows
  *((volatile uint32_t *) 0x50d000d0) = 0x00000006; // Columns
  *((volatile uint32_t *) 0x50d001d0) = 0x00000006; // Pooling rows
  *((volatile uint32_t *) 0x50d00250) = 0x00000006; // Pooling columns
  *((volatile uint32_t *) 0x50d003d0) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00450) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d004d0) = 0x00000001; // Write ptr multi-pass channel offs
  *((volatile uint32_t *) 0x50d00550) = 0x00001000; // SRAM read ptr
  *((volatile uint32_t *) 0x50d005d0) = 0x000008a0; // Layer control
  *((volatile uint32_t *) 0x50d00a50) = 0x00007e11; // Layer control 2
  *((volatile uint32_t *) 0x50d00650) = 0xb9a0bb9e; // Mask offset and count
  *((volatile uint32_t *) 0x50d00150) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007d0) = 0x0080e000; // Post processing register
  *((volatile uint32_t *) 0x50d00750) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 0
  *((volatile uint32_t *) 0x50100354) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x501003d4) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50100454) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x501005d4) = 0x0000e920; // Layer control
  *((volatile uint32_t *) 0x50100a54) = 0x0001f801; // Layer control 2
  *((volatile uint32_t *) 0x50100654) = 0xbbe0bfd8; // Mask offset and count
  *((volatile uint32_t *) 0x50100154) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x501007d4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50100754) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 1
  *((volatile uint32_t *) 0x50500354) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x505003d4) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50500454) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x505005d4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50500a54) = 0x0001f801; // Layer control 2
  *((volatile uint32_t *) 0x50500654) = 0xbbe0bfd8; // Mask offset and count
  *((volatile uint32_t *) 0x50500154) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x505007d4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50500754) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 2
  *((volatile uint32_t *) 0x50900354) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x509003d4) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50900454) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x509005d4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50900a54) = 0x0001f801; // Layer control 2
  *((volatile uint32_t *) 0x50900654) = 0xbbe0bfd8; // Mask offset and count
  *((volatile uint32_t *) 0x50900154) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x509007d4) = 0x00024000; // Post processing register
  *((volatile uint32_t *) 0x50900754) = 0xffffffff; // Mask and processor enables

  // Layer 17 quadrant 3
  *((volatile uint32_t *) 0x50d00354) = 0x00000800; // SRAM write ptr
  *((volatile uint32_t *) 0x50d003d4) = 0x00000001; // Write ptr time slot offs
  *((volatile uint32_t *) 0x50d00454) = 0x00002000; // Write ptr mask offs
  *((volatile uint32_t *) 0x50d005d4) = 0x00000920; // Layer control
  *((volatile uint32_t *) 0x50d00a54) = 0x0001f801; // Layer control 2
  *((volatile uint32_t *) 0x50d00654) = 0xbbe0bfd8; // Mask offset and count
  *((volatile uint32_t *) 0x50d00154) = 0x00000100; // 1D
  *((volatile uint32_t *) 0x50d007d4) = 0x00025100; // Post processing register
  *((volatile uint32_t *) 0x50d00754) = 0xffffffff; // Mask and processor enables


  *((volatile uint32_t *) 0x50000000) = 0x00001908; // FIFO control

  return CNN_OK;
}

int cnn_start_faceid(void)
{
  cnn_time_faceid = 0;

  *((volatile uint32_t *) 0x50100000) = 0x0018c808; // Enable quadrant 0
  *((volatile uint32_t *) 0x50500000) = 0x0018c809; // Enable quadrant 1
  *((volatile uint32_t *) 0x50900000) = 0x0018c809; // Enable quadrant 2
  *((volatile uint32_t *) 0x50d00000) = 0x0018c809; // Enable quadrant 3

#ifdef CNN_INFERENCE_TIMER
  MXC_TMR_SW_Start(CNN_INFERENCE_TIMER);
#endif

  CNN_START; // Allow capture of processing time
  *((volatile uint32_t *) 0x50100000) = 0x0018c809; // Master enable quadrant 0

  return CNN_OK;
}

// Custom unload for this network: 8-bit data, shape: (512, 1, 1)
int cnn_unload_faceid(uint32_t *out_buf32)
{
  uint8_t *out_buf = (uint8_t *) out_buf32;
  uint32_t val;
  volatile uint32_t *addr;
  int i;

  // Custom unload for this network, layer 17: 8-bit data, shape: (64, 1, 1)
  addr = (volatile uint32_t *) 0x50402000;
  for (i = 0; i < 4; i++) {
    val = *addr;
    addr += 0x2000;
    *out_buf++ = val & 0xff;
    *out_buf++ = (val >> 8) & 0xff;
    *out_buf++ = (val >> 16) & 0xff;
    *out_buf++ = (val >> 24) & 0xff;
  }
  addr = (volatile uint32_t *) 0x50802000;
  for (i = 0; i < 4; i++) {
    val = *addr;
    addr += 0x2000;
    *out_buf++ = val & 0xff;
    *out_buf++ = (val >> 8) & 0xff;
    *out_buf++ = (val >> 16) & 0xff;
    *out_buf++ = (val >> 24) & 0xff;
  }
  addr = (volatile uint32_t *) 0x50c02000;
  for (i = 0; i < 4; i++) {
    val = *addr;
    addr += 0x2000;
    *out_buf++ = val & 0xff;
    *out_buf++ = (val >> 8) & 0xff;
    *out_buf++ = (val >> 16) & 0xff;
    *out_buf++ = (val >> 24) & 0xff;
  }
  addr = (volatile uint32_t *) 0x51002000;
  for (i = 0; i < 4; i++) {
    val = *addr;
    addr += 0x2000;
    *out_buf++ = val & 0xff;
    *out_buf++ = (val >> 8) & 0xff;
    *out_buf++ = (val >> 16) & 0xff;
    *out_buf++ = (val >> 24) & 0xff;
  }

  return CNN_OK;
}

int cnn_enable_faceid(uint32_t clock_source, uint32_t clock_divider)
{
  // Reset all domains, restore power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
  MXC_GCFR->reg1 = 0xf; // Mask memory
  MXC_GCFR->reg0 = 0xf; // Power
  MXC_GCFR->reg2 = 0x0; // Iso
  MXC_GCFR->reg3 = 0x0; // Reset

  MXC_GCR->pclkdiv = (MXC_GCR->pclkdiv & ~(MXC_F_GCR_PCLKDIV_CNNCLKDIV | MXC_F_GCR_PCLKDIV_CNNCLKSEL))
                     | clock_divider | clock_source;
  MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN); // Enable CNN clock

  MXC_NVIC_SetVector(CNN_IRQn, CNN_ISR_faceid); // Set CNN complete vector

  return CNN_OK;
}

int cnn_boost_enable_faceid(mxc_gpio_regs_t *port, uint32_t pin)
{
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = port;
  gpio_out.mask = pin;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);

  return CNN_OK;
}

int cnn_boost_disable_faceid(mxc_gpio_regs_t *port, uint32_t pin)
{
  mxc_gpio_cfg_t gpio_out;
  gpio_out.port = port;
  gpio_out.mask = pin;
  gpio_out.pad = MXC_GPIO_PAD_NONE;
  gpio_out.func = MXC_GPIO_FUNC_OUT;
  MXC_GPIO_Config(&gpio_out);
  MXC_GPIO_OutClr(gpio_out.port, gpio_out.mask);

  return CNN_OK;
}

int cnn_disable_faceid(void)
{
  // Disable CNN clock
  MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

  // Disable power to CNN
  MXC_GCFR->reg3 = 0xf; // Reset
MXC_GCFR->reg2 |= 0xf; // Iso
  MXC_GCFR->reg0 = 0x0; // Power
  MXC_GCFR->reg1 = 0x0; // Mask memory
    MXC_GCFR->reg3 = 0x0; // Reset

  return CNN_OK;
}
