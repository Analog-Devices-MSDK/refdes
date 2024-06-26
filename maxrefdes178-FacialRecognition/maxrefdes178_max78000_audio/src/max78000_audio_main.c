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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <board.h>
#include <dma.h>
#include <fcr_regs.h>
#include <gcfr_regs.h>
#include <i2s.h>
#include <i2s_regs.h>
#include <icc.h>
#include <mxc.h>
#include <mxc_delay.h>
#include <mxc_device.h>
#include <mxc_sys.h>
#include <nvic_table.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tmr.h>

#include "max78000_audio_cnn.h"
#include "max78000_debug.h"
#include "max78000_qspi_slave.h"
#include "maxrefdes178_definitions.h"
#include "maxrefdes178_utility.h"
#include "maxrefdes178_version.h"


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define S_MODULE_NAME   "main"

/* Enable/Disable Features */
//#define ENABLE_PRINT_ENVELOPE            // enables printing average waveform envelope for samples
//#define ENABLE_CLASSIFICATION_DISPLAY    // enables printing classification result
#define ENABLE_SILENCE_DETECTION         // Starts collecting only after avg > THRESHOLD_HIGH, otherwise starts from first sample
#undef EIGHT_BIT_SAMPLES                 // samples from Mic or Test vectors are eight bit, otherwise 16-bit

/*-----------------------------*/
/* keep following unchanged */
#define SAMPLE_SIZE         16384   // size of input vector for CNN, keep it multiple of 128
#define CHUNK               128     // number of data points to read at a time and average for threshold, keep multiple of 128
#define TRANSPOSE_WIDTH     128     // width of 2d data model to be used for transpose
#define NUM_OUTPUTS         CNN_NUM_OUTPUTS      // number of classes
#define I2S_RX_BUFFER_SIZE  64      // I2S buffer size
#define TFT_BUFF_SIZE       50      // TFT buffer size
/*-----------------------------*/

/* Adjustables */
#define SAMPLE_SCALE_FACTOR         5//4//3       // multiplies 16-bit samples by this scale factor before converting to 8-bit
#define THRESHOLD_HIGH              350     // voice detection threshold to find beginning of a keyword
#define THRESHOLD_LOW               100     // voice detection threshold to find end of a keyword
#define SILENCE_COUNTER_THRESHOLD   20      // [>20] number of back to back CHUNK periods with avg < THRESHOLD_LOW to declare the end of a word
#define PREAMBLE_SIZE               30*CHUNK// how many samples before beginning of a keyword to include
#define INFERENCE_THRESHOLD         75      // min probability (0-100) to accept an inference

/* MAX9867 Audio Codec */
#define MAX9867_I2C        MXC_I2C1
#define MAX9867_I2C_FREQ   400000
#define MAX9867_I2C_ADDR   0x18
#define MAX9867_REV_ID     0x42

/* MAX9867 Register addresses */
#define MAX9867_STATUS_ADDR      0x00
#define MAX9867_REV_ID_ADDR      0xFF


//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
static const mxc_gpio_cfg_t gpio_cnn_boost = MAX78000_AUDIO_CNN_BOOST_PIN;
static const mxc_gpio_cfg_t gpio_audio_osc = MAX78000_AUDIO_AUDIO_OSC_PIN;
static const mxc_gpio_cfg_t gpio_mic_en    = MAX78000_AUDIO_MIC_EN_PIN;
static const mxc_gpio_cfg_t gpio_mic_sel   = MAX78000_AUDIO_MIC_SEL_PIN;
static const mxc_gpio_cfg_t gpio_sram_cs   = MAX78000_AUDIO_SRAM_CS_PIN;
static const mxc_gpio_cfg_t gpio_i2c       = MAX78000_AUDIO_I2C_PINS;
static const mxc_gpio_cfg_t gpio_debug_sel = MAX78000_AUDIO_DEBUG_SEL_PIN;
static const mxc_gpio_cfg_t gpio_exp_out1  = MAX78000_AUDIO_EXPANDER_OUT1_PIN;
static const mxc_gpio_cfg_t gpio_exp_out2  = MAX78000_AUDIO_EXPANDER_OUT2_PIN;
static const mxc_gpio_cfg_t gpio_video_int = MAX78000_AUDIO_VIDEO_INT_PIN;
static const mxc_gpio_cfg_t gpio_red       = MAX78000_AUDIO_LED_RED_PIN;
static const mxc_gpio_cfg_t gpio_green     = MAX78000_AUDIO_LED_GREEN_PIN;
static const mxc_gpio_cfg_t gpio_blue      = MAX78000_AUDIO_LED_BLUE_PIN;

static int32_t ml_data[NUM_OUTPUTS];
static q15_t ml_softmax[NUM_OUTPUTS];
static uint8_t pAI85Buffer[SAMPLE_SIZE];
static uint8_t pPreambleCircBuffer[PREAMBLE_SIZE];
static int16_t Max, Min;
static uint16_t thresholdHigh = THRESHOLD_HIGH;
static uint16_t thresholdLow = THRESHOLD_LOW;
static int16_t  x0, x1, Coeff;
static int32_t  y0, y1;
static int8_t enable_audio = 0; // Disabled audio for facial recognition demo
static int8_t enable_sleep = 0;
static volatile int8_t button_pressed = 0;

static volatile uint8_t i2s_flag = 0;
static int32_t i2s_rx_buffer[I2S_RX_BUFFER_SIZE];
static max78000_statistics_t max78000_statistics = {0};
static version_t version = {S_VERSION_MAJOR, S_VERSION_MINOR, S_VERSION_BUILD};
static char demo_name[] = FACIAL_RECOGNITION_DEMO_NAME;
static uint8_t qspi_rx_buffer[100];

/* **** Constants **** */
typedef enum _mic_processing_state {
    STOP = 0,     /* No processing  */
    SILENCE = 1,  /* Threshold not detected yet  */
    KEYWORD = 2   /* Threshold has been detected, gathering keyword samples */
} mic_processing_state;

/* Set of detected words */
static const char keywords[NUM_OUTPUTS][10] = { "UP", "DOWN", "LEFT", "RIGHT", "STOP",
                                         "GO", "YES", "NO", "ON", "OFF", "ONE", "TWO", "THREE", "FOUR", "FIVE",
                                         "SIX", "SEVEN", "EIGHT", "NINE", "ZERO", "CUBE", "Unknown"
                                       };


//-----------------------------------------------------------------------------
// Local function declarations
//-----------------------------------------------------------------------------
static void fail(void);
static uint8_t cnn_load_data(uint8_t* pIn);
static uint8_t MicReadChunk(uint8_t* pBuff, uint16_t* avg);
static uint8_t AddTranspose(uint8_t* pIn, uint8_t* pOut, uint16_t inSize,
                     uint16_t outSize, uint16_t width);
static uint8_t check_inference(q15_t* ml_soft, int32_t* ml_data,
                        int16_t* out_class, double* out_prob);
static void I2SInit();
static void HPF_init(void);
static int16_t HPF(int16_t input);
static int max9867_init(void);


//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
void i2s_isr(void)
{
    i2s_flag = 1;
    /* Clear I2S interrupt flag */
    MXC_I2S_ClearFlags(MXC_F_I2S_INTFL_RX_THD_CH0);
}

void button_int(void *cbdata)
{
    button_pressed = 1;
}

void sleep_defer_int()
{
    // Clear interrupt
    MXC_TMR_ClearFlags(MAX78000_AUDIO_SLEEP_DEFER_TMR);

    enable_sleep = 1;
}

int main(void)
{
    MXC_Delay(MXC_DELAY_MSEC(500)); // Wait supply to be ready

    uint32_t sampleCounter = 0;
    mxc_tmr_unit_t units;

    uint8_t pChunkBuff[CHUNK];

    uint16_t avg = 0;
    uint16_t preambleCounter = 0;
    uint16_t ai85Counter = 0;
    uint16_t wordCounter = 0;

    uint16_t avgSilenceCounter = 0;

    classification_result_t classification_result = {0};
    qspi_state_e qspi_rx_state;
    qspi_packet_header_t qspi_rx_header;

    mic_processing_state procState = STOP;

    /* Enable cache */
    MXC_ICC_Enable(MXC_ICC0);

    /* Switch to 100 MHz clock */
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    /* Enable peripheral, enable CNN interrupt, turn on CNN clock */
    /* CNN clock: 50 MHz div 1 */
    cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);

    /* Configure CNN Boost pin */
    GPIO_SET(gpio_cnn_boost);
    MXC_GPIO_Config(&gpio_cnn_boost);

    /* Configure audio OSC pin */
    GPIO_SET(gpio_audio_osc);
    MXC_GPIO_Config(&gpio_audio_osc);

    /* Configure audio microphone power pin */
    GPIO_SET(gpio_mic_en);
    MXC_GPIO_Config(&gpio_mic_en);

    /* Configure audio microphone select pin */
    GPIO_CLR(gpio_mic_sel);
    MXC_GPIO_Config(&gpio_mic_sel);

    // Configure LEDs
    GPIO_CLR(gpio_red);
    MXC_GPIO_Config(&gpio_red);

    GPIO_CLR(gpio_green);
    MXC_GPIO_Config(&gpio_green);

    GPIO_CLR(gpio_blue);
    MXC_GPIO_Config(&gpio_blue);

    // Configure unused pins as high-z
    MXC_GPIO_Config(&gpio_sram_cs);
    MXC_GPIO_Config(&gpio_i2c);
    MXC_GPIO_Config(&gpio_debug_sel);
    MXC_GPIO_Config(&gpio_exp_out1);
    MXC_GPIO_Config(&gpio_exp_out2);
    MXC_GPIO_Config(&gpio_video_int);

    PR_INFO("maxrefdes178_max78000_audio %s v%d.%d.%d [%s]",
            demo_name, version.major, version.minor, version.build, S_BUILD_TIMESTAMP);

    memset(pAI85Buffer, 0x0, sizeof(pAI85Buffer));
    memset(pPreambleCircBuffer, 0x0, sizeof(pPreambleCircBuffer));

    PR_DEBUG("pChunkBuff: %d", sizeof(pChunkBuff));
    PR_DEBUG("pPreambleCircBuffer: %d", sizeof(pPreambleCircBuffer));
    PR_DEBUG("pAI85Buffer: %d", sizeof(pAI85Buffer));

    if (MXC_DMA_Init() != E_NO_ERROR) {
        PR_ERROR("DMA INIT ERROR");
        fail();
    }

    if (qspi_slave_init() != E_NO_ERROR) {
        PR_ERROR("qspi_slave_init fail");
        fail();
    }

    /* Bring state machine into consistent state */
    cnn_init();
    /* Load kernels */
    cnn_load_weights();
    /* Configure state machine */
    cnn_configure();

    /* switch to silence state*/
    procState = SILENCE;

    /* initialize I2S interface to Mic */
    I2SInit();

    /* initialize audio codec */
    max9867_init();

    /* Init button interrupt */
    PB_RegisterCallback(0, (pb_callback) button_int);

    /* Init sleep defer timer interrupt */
    mxc_tmr_cfg_t tmr;
    MXC_TMR_Shutdown(MAX78000_AUDIO_SLEEP_DEFER_TMR);
    tmr.pres = TMR_PRES_128;
    tmr.mode = TMR_MODE_ONESHOT;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_APB_CLK;
    tmr.cmp_cnt = MAX78000_SLEEP_DEFER_DURATION * PeripheralClock / 128;
    tmr.pol = 0;
    MXC_NVIC_SetVector(MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(MAX78000_AUDIO_SLEEP_DEFER_TMR)), sleep_defer_int);
    NVIC_EnableIRQ(MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(MAX78000_AUDIO_SLEEP_DEFER_TMR)));
    MXC_TMR_Init(MAX78000_AUDIO_SLEEP_DEFER_TMR, &tmr, false);
    MXC_TMR_EnableInt(MAX78000_AUDIO_SLEEP_DEFER_TMR);
    MXC_TMR_Start(MAX78000_AUDIO_SLEEP_DEFER_TMR);

    PR_INFO("** READY ***");

    /* Read samples */
    while (1) {

        /* Check if QSPI RX has data */
        qspi_rx_state = qspi_slave_get_rx_state();
        if (qspi_rx_state == QSPI_STATE_CS_DEASSERTED_HEADER) {
            qspi_rx_header = qspi_slave_get_rx_header();

            if (sizeof(qspi_rx_buffer) < qspi_rx_header.info.packet_size) {
                PR_ERROR("payload too big %d", qspi_rx_header.info.packet_size);
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                continue;
            }

            qspi_slave_set_rx_data(qspi_rx_buffer, qspi_rx_header.info.packet_size);
            qspi_slave_trigger();
            qspi_slave_wait_rx();

            // Check payload crc again
            if (qspi_rx_header.payload_crc16 != crc16_sw(qspi_rx_buffer, qspi_rx_header.info.packet_size)) {
                PR_ERROR("Invalid payload crc %x", qspi_rx_header.payload_crc16);
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                continue;
            }

            // QSPI commands with payload
            switch(qspi_rx_header.info.packet_type) {
            case QSPI_PACKET_TYPE_TEST:
                PR_INFO("test data received %d", qspi_rx_header.info.packet_size);
                for (int i = 0; i < qspi_rx_header.info.packet_size; i++) {
                    printf("%02hhX ", qspi_rx_buffer[i]);
                }
                printf("\n");
                break;
            default:
                PR_ERROR("Invalid packet %d", qspi_rx_header.info.packet_type);
                break;
            }

            qspi_slave_set_rx_state(QSPI_STATE_IDLE);

        } else if (qspi_rx_state == QSPI_STATE_COMPLETED) {
            qspi_rx_header = qspi_slave_get_rx_header();

            // QSPI commands without payload
            switch(qspi_rx_header.info.packet_type) {
            case QSPI_PACKET_TYPE_AUDIO_VERSION_CMD:
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                qspi_slave_send_packet((uint8_t *) &version, sizeof(version),
                        QSPI_PACKET_TYPE_AUDIO_VERSION_RES);
                break;
            case QSPI_PACKET_TYPE_AUDIO_DEMO_NAME_CMD:
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                qspi_slave_send_packet((uint8_t *) demo_name, strlen(demo_name),
                        QSPI_PACKET_TYPE_AUDIO_DEMO_NAME_RES);
                break;
            case QSPI_PACKET_TYPE_AUDIO_SERIAL_CMD:
                // TODO
                break;
            case QSPI_PACKET_TYPE_AUDIO_ENABLE_CMD:
                PR_INFO("enable audio");



                enable_audio = 1;
                break;
            case QSPI_PACKET_TYPE_AUDIO_DISABLE_CMD:
                PR_INFO("disable audio");



                enable_audio = 0;
                break;
            case QSPI_PACKET_TYPE_AUDIO_ENABLE_CNN_CMD:
                // TODO
                break;
            case QSPI_PACKET_TYPE_AUDIO_DISABLE_CNN_CMD:
                // TODO
                break;
            case QSPI_PACKET_TYPE_AUDIO_DEBUG_CMD:
                PR_INFO("dont sleep for %ds to let debugger connection", MAX78000_SLEEP_DEFER_DURATION);
                enable_sleep = 0;
                MXC_TMR_Start(MAX78000_VIDEO_SLEEP_DEFER_TMR);
                break;
            default:
                PR_ERROR("Invalid packet %d", qspi_rx_header.info.packet_type);
                break;
            }

            qspi_slave_set_rx_state(QSPI_STATE_IDLE);

        }

        if (button_pressed) {
            button_pressed = 0;
            PR_INFO("button B pressed");

            qspi_slave_send_packet(NULL, 0, QSPI_PACKET_TYPE_AUDIO_BUTTON_PRESS_RES);
        }

        if (!enable_audio) {
            if (enable_sleep) {
//                MXC_LP_EnterSleepMode();
                __WFI();
            }
            continue;
        }

        /* Read from Mic driver to get CHUNK worth of samples, otherwise next sample*/
        if (MicReadChunk(pChunkBuff, &avg) == 0) {
            continue;
        }

        sampleCounter += CHUNK;

#ifdef ENABLE_SILENCE_DETECTION       // disable to start collecting data immediately.

        /* copy the preamble data*/
        /* add the new chunk to the end of circular buffer*/
        memcpy(&pPreambleCircBuffer[preambleCounter], pChunkBuff,
                sizeof(uint8_t) * CHUNK);

        /* increment circular buffer pointer*/
        preambleCounter = (preambleCounter + CHUNK) % (PREAMBLE_SIZE);

        /* Display average envelope as a bar */
#ifdef ENABLE_PRINT_ENVELOPE
        printf("%.6d|",sampleCounter);
        for (int i = 0; i < avg / 10; i++)
            printf("=");
        if (avg >= thresholdHigh)
            printf("*");
        printf("[%d]\n",avg);
#endif

        /* if we have not detected voice, check the average*/
        if (procState == SILENCE) {

            classification_result.classification = CLASSIFICATION_NOTHING;

            /* compute average, proceed if greater than threshold */
            if (avg >= thresholdHigh) {
                /* switch to keyword data collection*/
                procState = KEYWORD;
                PR_DEBUG("%.6d Word starts from index: %d, avg:%d > %d",
                        sampleCounter, sampleCounter - PREAMBLE_SIZE - CHUNK,
                        avg, thresholdHigh);

                /* reorder circular buffer according to time at the beginning of pAI85Buffer */
                if (preambleCounter == 0) {
                    /* copy latest samples afterwards */
                    if (AddTranspose(&pPreambleCircBuffer[0], pAI85Buffer,
                            PREAMBLE_SIZE, SAMPLE_SIZE, TRANSPOSE_WIDTH)) {
                        PR_ERROR("ERROR: Transpose ended early");
                    }
                } else {
                    /* copy oldest samples to the beginning*/
                    if (AddTranspose(&pPreambleCircBuffer[preambleCounter],
                            pAI85Buffer, PREAMBLE_SIZE - preambleCounter,
                            SAMPLE_SIZE, TRANSPOSE_WIDTH)) {
                        PR_ERROR("ERROR: Transpose ended early");
                    }

                    /* copy latest samples afterwards */
                    if (AddTranspose(&pPreambleCircBuffer[0], pAI85Buffer,
                            preambleCounter, SAMPLE_SIZE, TRANSPOSE_WIDTH)) {
                        PR_ERROR("ERROR: Transpose ended early");
                    }

                }

                /* preamble is copied and state is changed, start adding keyword samples next run */
                ai85Counter += PREAMBLE_SIZE;
                continue;

            }
        }
        /* if it is in data collection, add samples to buffer*/
        else if (procState == KEYWORD)
#endif  //#ifdef ENABLE_SILENCE_DETECTION

        {
            uint8_t ret=0;

            /* add sample, rearrange buffer */
            ret = AddTranspose(pChunkBuff, pAI85Buffer, CHUNK, SAMPLE_SIZE,
                    TRANSPOSE_WIDTH);

            /* increment number of stored samples */
            ai85Counter += CHUNK;

            /* if there is silence after at least 1/3 of samples passed, increment number of times back to back silence to find end of keyword */
            if ((avg < thresholdLow) && (ai85Counter >= SAMPLE_SIZE / 3)) {
                avgSilenceCounter ++;
            } else {
                avgSilenceCounter = 0;
            }


            /* if this is the last sample and there are not enough samples to
             * feed to CNN, or if it is long silence after keyword,  append with zero (for reading file)
             */
            if (avgSilenceCounter > SILENCE_COUNTER_THRESHOLD)
            {
                memset(pChunkBuff,0,CHUNK);
                PR_DEBUG("%.6d: Word ends, Appends %d zeros", sampleCounter,
                        SAMPLE_SIZE - ai85Counter);
                ret = 0;
                while (!ret) {
                    ret = AddTranspose(pChunkBuff, pAI85Buffer, CHUNK,
                            SAMPLE_SIZE, TRANSPOSE_WIDTH);
                    ai85Counter += CHUNK;
                }
            }

            /* if enough samples are collected, start CNN */
            if (ai85Counter >= SAMPLE_SIZE) {
                int16_t out_class = -1;
                double probability = 0;

                GPIO_SET(gpio_green);

                /* reset counters */
                ai85Counter = 0;
                avgSilenceCounter = 0;

                /* new word */
                wordCounter++;

                /* change state to silence */
                procState = SILENCE;

                /* sanity check, last transpose should have returned 1, as enough samples should have already been added */
                if (ret != 1) {
                    PR_ERROR("ERROR: Transpose incomplete!");
                    fail();
                }

                //----------------------------------  : invoke AI85 CNN
                PR_DEBUG("%.6d: Starts CNN: %d", sampleCounter, wordCounter);

                /* load to CNN */
                if (!cnn_load_data(pAI85Buffer)) {
                    PR_ERROR("ERROR: Loading data to CNN!");
                    fail();
                }

                /* Start CNN */
                if (!cnn_start()) {
                    PR_ERROR("ERROR: Starting CNN!");
                    fail();
                }

                /* Wait for CNN  to complete */
                while (cnn_time == 0) {
                    __WFI();
                }

                /* read data */
                cnn_unload((uint32_t *)ml_data);

                /* Get time */
                MXC_TMR_GetTime(MXC_TMR0, cnn_time, (uint32_t*) &cnn_time, &units);
                PR_DEBUG("%.6d: Completes CNN: %d", sampleCounter, wordCounter);

                switch (units) {
                case TMR_UNIT_NANOSEC:
                    cnn_time /= 1000;
                    break;
                case TMR_UNIT_MILLISEC:
                    cnn_time *= 1000;
                    break;
                case TMR_UNIT_SEC:
                    cnn_time *= 1000000;
                    break;
                default:
                    break;
                }
                PR_DEBUG("CNN Time: %d us", cnn_time);

                /* run softmax */
                softmax_q17p14_q15((const q31_t*) ml_data, NUM_OUTPUTS,
                        ml_softmax);

#ifdef ENABLE_CLASSIFICATION_DISPLAY
                PR_INFO("Classification results:");
                for (int i = 0; i < NUM_OUTPUTS; i++) {
                    int digs = (1000 * ml_softmax[i] + 0x4000) >> 15;
                    int tens = digs % 10;
                    digs = digs/10;

                    printf("[%+.7d] -> Class %.2d %8s: %d.%d%%\n", ml_data[i],
                            i, keywords[i], digs, tens);
                }
#endif
                /* find detected class with max probability */
                ret = check_inference(ml_softmax, ml_data, &out_class, &probability);

                if (!ret) {
                    classification_result.classification = CLASSIFICATION_LOW_CONFIDENCE;
                    PR_INFO("Low confidence: %s (%0.1f%%)", keywords[out_class], probability);
                } else {
                    classification_result.classification = CLASSIFICATION_DETECTED;
                    PR_INFO("Detected: %s (%0.1f%%)", keywords[out_class], probability);
                }

                if (strcmp(keywords[out_class], "Unknown") == 0) {
                    classification_result.classification = CLASSIFICATION_UNKNOWN;
                }

                memcpy(classification_result.result, keywords[out_class], sizeof(classification_result.result));
                classification_result.probability = probability;

                qspi_slave_send_packet((uint8_t *) &classification_result, sizeof(classification_result),
                        QSPI_PACKET_TYPE_AUDIO_CLASSIFICATION_RES);

                max78000_statistics.cnn_duration_us = cnn_time;
                qspi_slave_send_packet((uint8_t *) &max78000_statistics, sizeof(max78000_statistics),
                        QSPI_PACKET_TYPE_AUDIO_STATISTICS_RES);

#ifdef ENABLE_CLASSIFICATION_DISPLAY
                printf("\n----------------------------------------- \n");
#endif
                PR_INFO("Min: %d Max:%d", Min, Max);
                Max = 0;
                Min = 0;

                GPIO_CLR(gpio_green);
            }
        }
    }
}

static void I2SInit()
{
    mxc_i2s_req_t req;
    int32_t err;

    PR_INFO("*** I2S & Mic Init ***");
    /* Initialize High Pass Filter */
    HPF_init();
    /* Initialize I2S RX buffer */
    memset(i2s_rx_buffer, 0, sizeof(i2s_rx_buffer));
    /* Configure I2S interface parameters */
    req.wordSize    = MXC_I2S_DATASIZE_WORD;
    req.sampleSize  = MXC_I2S_SAMPLESIZE_THIRTYTWO;
    req.justify     = MXC_I2S_MSB_JUSTIFY;
    req.wsPolarity	= MXC_I2S_POL_NORMAL;
    req.channelMode	= MXC_I2S_INTERNAL_SCK_WS_0;
    /* Get only left channel data from on-board microphone. Right channel samples are zeros */
    req.stereoMode	= MXC_I2S_MONO_LEFT_CH;
    req.bitOrder	= MXC_I2S_MSB_FIRST;
    /* I2S clock = PT freq / (2*(req.clkdiv + 1)) */
    /* I2S sample rate = I2S clock/64 = 16kHz */
    req.clkdiv      = 5;
    req.rawData     = NULL;
    req.txData      = NULL;
    req.rxData      = i2s_rx_buffer;
    req.length      = I2S_RX_BUFFER_SIZE;


    if((err = MXC_I2S_Init(&req)) != E_NO_ERROR) {
        PR_ERROR("\nError in I2S_Init: %d", err);
        fail();
    }

    /* Set I2S RX FIFO threshold to generate interrupt */
    MXC_I2S_SetRXThreshold(4);
    MXC_NVIC_SetVector(I2S_IRQn, i2s_isr);
    NVIC_EnableIRQ(I2S_IRQn);
    /* Enable RX FIFO Threshold Interrupt */
    MXC_I2S_EnableInt(MXC_F_I2S_INTEN_RX_THD_CH0);
    MXC_I2S_RXEnable();
    __enable_irq();
}

static uint8_t check_inference(q15_t *ml_soft, int32_t *ml_data,
        int16_t *out_class, double *out_prob) {
    int32_t temp[NUM_OUTPUTS];
    q15_t max = 0;    // soft_max output is 0->32767
    int32_t max_ml = 1 << 31;  // ml before going to soft_max
    int16_t max_index = -1;

    memcpy(temp, ml_data, sizeof(int32_t) * NUM_OUTPUTS);

    /* find the top 5 classes */
    for (int top = 0; top < 5; top++) {
        /* find the class with highest */
        for (int i = 0; i < NUM_OUTPUTS; i++) {
            if ((int32_t) temp[i] > max_ml) {
                max_ml = (int32_t) temp[i];

                max = ml_soft[i];
                max_index = i;
            }
        }

        /* print top 1 separately */
        if (top == 0) {

            *out_class = max_index;
            *out_prob = 100.0 * max / 32768.0;
            break;
        }
    }

    PR_DEBUG("Min: %d,   Max: %d", Min, Max);

    /* check if probability is low */
    if (*out_prob > INFERENCE_THRESHOLD) {
        return 1;
    }
    else {
        return 0;
    }
}

static uint8_t cnn_load_data(uint8_t* pIn)
{
    uint32_t mem;
    uint16_t index = 0;

    /* data should already be formatted correctly */
    /* pIn is 16KB, each 1KB belongs to a memory group */
    for (mem = 0x50400000; mem <= 0x50418000; mem += 0x8000) {
        memcpy((uint8_t*)mem, &pIn[index], 1024);
        //printf("%.10X \n",(uint8_t *)mem);
        index += 1024;
    }

    for (mem = 0x50800000; mem <= 0x50818000; mem += 0x8000) {
        memcpy((uint8_t*)mem, &pIn[index], 1024);
        index += 1024;
    }

    for (mem = 0x50C00000; mem <= 0x50C18000; mem += 0x8000) {
        memcpy((uint8_t*)mem, &pIn[index], 1024);
        index += 1024;
    }

    for (mem = 0x51000000; mem <= 0x51018000; mem += 0x8000) {
        memcpy((uint8_t*)mem, &pIn[index], 1024);
        index += 1024;
    }

    return CNN_OK;
}

static uint8_t AddTranspose(uint8_t *pIn, uint8_t *pOut, uint16_t inSize,
        uint16_t outSize, uint16_t width) {
    /* Data order in Ai85 memory (transpose is included):
	input(series of 8 bit samples): (0,0) ...  (0,127)  (1,0) ... (1,127) ...... (127,0)...(127,127)    16384 samples
	output (32bit word): 16K samples in a buffer. Later, each 1K goes to a separate CNN memory group
	0x0000:
		(0,3)(0,2)(0,1)(0,0)
		(0,67)(0,66)(0,65)(0,64)
		(1,3)(1,2)(1,1)(1,0)
		(1,67)(1,66)(1,65)(1,64)
		....
		(127,67)(127,66)(127,65)(127,64)
	0x0400:
		(0,7)(0,6)(0,5)(0,4)
		(0,71)(0,70)(0,69)(0,68)
		....
		(127,71)(127,70)(127,69)(127,68)
	...
	0x3C00:
		(0,63)(0,62)(0,61)(0,60)
		(0,127)(0,126)(0,125)(0,124)
		....
		(127,127)(127,126)(127,125)(127,124)
     */

    static uint16_t row = 0, col = 0, total = 0;
    uint16_t  secondHalf = 0, wordRow = 0, byteInWord = 0, group = 0, index = 0;
    for (int i = 0; i < inSize; i++) {
        /* is it above 63? */
        if (col >= (width >> 1)) {
            secondHalf = 1; // odd word rows
        } else {
            secondHalf = 0; // even word rows
        }

        /* which group (0-15) it should be */
        group = (col % (width >> 1)) / 4;

        /* which word row (0-255) within the group */
        wordRow = secondHalf + (row << 1);

        /* which byte (0-3) in the word */
        byteInWord = col % 4;

        /* find output index */
        index = 1024 * group + 4 * wordRow + byteInWord;

        /* place sample in correct output location */
        pOut[index] = pIn[i];

        total++;

        /* increment row and col index */
        col++;
        if (col >= width) {
            col = 0;
            row++;
        }
    }

    if (total >= outSize) {
        /* sanity check */
        if (row != width) {
            PR_ERROR("ERROR: Rearranging!");
        }

        total = 0;
        row = 0;
        col = 0;
        return 1;
    } else {
        return 0;
    }
}

static uint8_t MicReadChunk(uint8_t *pBuff, uint16_t * avg)
{
    static uint16_t chunkCount = 0;
    static uint16_t sum = 0;

    static uint32_t index = 0;

    int32_t sample = 0;
    int16_t temp = 0;
    uint32_t rx_size = 0;

    /* sample not ready */
    if (!i2s_flag) {
        *avg = 0;
        return 0;
    }

    /* Clear flag */
    i2s_flag = 0;
    /* Read number of samples in I2S RX FIFO */
    rx_size = MXC_I2S->dmach0 >> MXC_F_I2S_DMACH0_RX_LVL_POS;
    //	printf("%d ", rx_size);

    /* read until fifo is empty or enough samples are collected */
    while ((rx_size--) && (chunkCount < CHUNK)) {
        /* Read microphone sample from I2S FIFO */
        sample = (int32_t)MXC_I2S->fifoch0;
        /* The actual value is 18 MSB of 32-bit word */
        temp = sample >> 14;

        /* Remove DC from microphone signal */
        sample = HPF((int16_t)temp); // filter needs about 1K sample to converge

        /* Discard first 10k samples due to microphone charging cap effect */
        if (index++ < 10000)
            continue;

        /* absolute for averaging */
        if (sample >= 0)
            sum += sample;
        else
            sum -= sample;

        /* Convert to 8 bit unsigned */
        pBuff[chunkCount] = (uint8_t)((sample)*SAMPLE_SCALE_FACTOR/256);

        temp=(int8_t)pBuff[chunkCount];

        chunkCount++;

        /* record max and min */
        if (temp > Max) {
            Max = temp;
        }

        if (temp < Min) {
            Min = temp;
        }

    }

    /* if not enough samples, return 0 */
    if (chunkCount < CHUNK) {
        *avg = 0;
        return 0;
    }

    /* enough samples are collected, calculate average and return 1 */
    *avg = ((uint16_t)(sum / CHUNK));

    chunkCount = 0;
    sum = 0;
    return 1;
}

static void HPF_init(void) {
    Coeff = 32604; //0.995
    x0 = 0;
    y0 = 0;
    y1 = y0;
    x1 = x0;
}

static int16_t HPF(int16_t input) {
    int16_t Acc, output;
    int32_t tmp;

    /* a 1st order IIR high pass filter (100 Hz cutoff frequency)  */
    /* y(n)=x(n)-x(n-1)+A*y(n-1) and A =.995*2^15 */

    x0 = input;

    tmp = (Coeff * y1);
    Acc = (int16_t)((tmp + (1 << 14)) >> 15);
    y0 = x0 - x1 + Acc;

    /* Clipping */
    if (y0 > 32767) {
        y0 = 32767;
    }

    if (y0 < -32768) {
        y0 = -32768;
    }

    /* Update filter state */
    y1 = y0;
    x1 = x0;

    output = (int16_t)y0;

    return (output);
}

static void fail(void)
{
    PR_ERROR("fail");

    GPIO_SET(gpio_red);
    GPIO_CLR(gpio_green);
    GPIO_CLR(gpio_blue);

    while(1);
}

static int max9867_init(void)
{
    int err;
    uint8_t rev_id;
    uint8_t buf[1];
    mxc_i2c_req_t i2c_req;

    err = MXC_I2C_Init(MAX9867_I2C, 1, 0);
    if (err != E_NO_ERROR) {
        PR_ERROR("MXC_I2C_Init fail %d", err);
        return err;
    }

    MXC_I2C_SetFrequency(MAX9867_I2C, MAX9867_I2C_FREQ);

    buf[0] = MAX9867_REV_ID_ADDR;
    i2c_req.i2c = MAX9867_I2C;
    i2c_req.addr = MAX9867_I2C_ADDR;
    i2c_req.tx_buf = buf;
    i2c_req.tx_len = 1;
    i2c_req.rx_buf = (unsigned char *) &rev_id;
    i2c_req.rx_len = 1;
    i2c_req.restart = 0;
    i2c_req.callback = NULL;

    err = MXC_I2C_MasterTransaction(&i2c_req);
    if (err != E_NO_ERROR) {
        PR_ERROR("MXC_I2C_Init fail %d", err);
        return err;
    }

    if (rev_id != MAX9867_REV_ID) {
        PR_ERROR("invalid rev id 0x%x", rev_id);
        return E_NOT_SUPPORTED;
    }

    return E_NO_ERROR;
}
