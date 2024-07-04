/******************************************************************************
 *
 * Copyright (C) 2020-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <board.h>
#include <camera.h>
#include <gpio.h>
#include <icc.h>
#include <mxc.h>
#include <mxc_delay.h>
#include <rtc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "max78000_debug.h"
#include "max78000_qspi_slave.h"
#include "max78000_video_cnn.h"
#include "max78000_video_weights.h"
#include "maxrefdes178_definitions.h"
#include "maxrefdes178_utility.h"
#include "maxrefdes178_version.h"


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define S_MODULE_NAME          "main"

//#define PRINT_TIME_CNN

#define CATS_DOGS_HEIGHT 128
#define CATS_DOGS_WIDTH 128
//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* **************************************************************************** */


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static int32_t ml_data[CNN_NUM_OUTPUTS];
static q15_t ml_softmax[CNN_NUM_OUTPUTS];
const char classes[CNN_NUM_OUTPUTS][10] = {"Cat", "Dog"};
int result[CNN_NUM_OUTPUTS];
mxc_tmr_unit_t units;

static const mxc_gpio_cfg_t gpio_flash     = MAX78000_VIDEO_FLASH_LED_PIN;
static const mxc_gpio_cfg_t gpio_camera    = MAX78000_VIDEO_CAMERA_PIN;
static const mxc_gpio_cfg_t gpio_sram_cs   = MAX78000_VIDEO_SRAM_CS_PIN;
static const mxc_gpio_cfg_t gpio_i2c       = MAX78000_VIDEO_I2C_PINS;
static const mxc_gpio_cfg_t gpio_debug_sel = MAX78000_VIDEO_DEBUG_SEL_PIN;
static const mxc_gpio_cfg_t gpio_exp_io    = MAX78000_VIDEO_EXPANDER_IO_PIN;
static const mxc_gpio_cfg_t gpio_exp_out   = MAX78000_VIDEO_EXPANDER_OUT_PIN;
static const mxc_gpio_cfg_t gpio_audio_int = MAX78000_VIDEO_AUDIO_INT_PIN;
static const mxc_gpio_cfg_t gpio_cnn_boost = MAX78000_VIDEO_CNN_BOOST_PIN;
static const mxc_gpio_cfg_t gpio_red       = MAX78000_VIDEO_LED_RED_PIN;
static const mxc_gpio_cfg_t gpio_green     = MAX78000_VIDEO_LED_GREEN_PIN;
static const mxc_gpio_cfg_t gpio_blue      = MAX78000_VIDEO_LED_BLUE_PIN;

static const uint8_t camera_settings[][2] = {
    {0x0e, 0x08}, // Sleep mode
    {0x69, 0x52}, // BLC window selection, BLC enable (default is 0x12)
    {0x1e, 0xb3}, // AddLT1F (default 0xb1)
    {0x48, 0x42},
    {0xff, 0x01}, // Select MIPI register bank
    {0xb5, 0x30},
    {0xff, 0x00}, // Select system control register bank
    {0x16, 0x03}, // (default)
    {0x62, 0x10}, // (default)
    {0x12, 0x01}, // Select Bayer RAW
    {0x17, 0x65}, // Horizontal Window Start Point Control (LSBs), default is 0x69
    {0x18, 0xa4}, // Horizontal sensor size (default)
    {0x19, 0x0c}, // Vertical Window Start Line Control (default)
    {0x1a, 0xf6}, // Vertical sensor size (default)
    {0x37, 0x04}, // PCLK is double system clock (default is 0x0c)
    {0x3e, 0x20}, // (default)
    {0x81, 0x3f}, // sde_en, uv_adj_en, scale_v_en, scale_h_en, uv_avg_en, cmx_en
    {0xcc, 0x02}, // High 2 bits of horizontal output size (default)
    {0xcd, 0x80}, // Low 8 bits of horizontal output size (default)
    {0xce, 0x01}, // Ninth bit of vertical output size (default)
    {0xcf, 0xe0}, // Low 8 bits of vertical output size (default)
    {0x82, 0x01}, // 01: Raw from CIP (default is 0x00)
    {0xc8, 0x02},
    {0xc9, 0x80},
    {0xca, 0x01},
    {0xcb, 0xe0},
    {0xd0, 0x28},
    {0x0e, 0x00}, // Normal mode (not sleep mode)
    {0x70, 0x00},
    {0x71, 0x34},
    {0x74, 0x28},
    {0x75, 0x98},
    {0x76, 0x00},
    {0x77, 0x64},
    {0x78, 0x01},
    {0x79, 0xc2},
    {0x7a, 0x4e},
    {0x7b, 0x1f},
    {0x7c, 0x00},
    {0x11, 0x01}, // CLKRC, Internal clock pre-scalar divide by 2 (default divide by 1)
    {0x20, 0x00}, // Banding filter (default)
    {0x21, 0x57}, // Banding filter (default is 0x44)
    {0x50, 0x4d},
    {0x51, 0x40}, // 60Hz Banding AEC 8 bits (default 0x80)
    {0x4c, 0x7d},
    {0x0e, 0x00},
    {0x80, 0x7f},
    {0x85, 0x00},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x2a},
    {0x8a, 0x22},
    {0x8b, 0x20},
    {0xbb, 0xab},
    {0xbc, 0x84},
    {0xbd, 0x27},
    {0xbe, 0x0e},
    {0xbf, 0xb8},
    {0xc0, 0xc5},
    {0xc1, 0x1e},
    {0xb7, 0x05},
    {0xb8, 0x09},
    {0xb9, 0x00},
    {0xba, 0x18},
    {0x5a, 0x1f},
    {0x5b, 0x9f},
    {0x5c, 0x69},
    {0x5d, 0x42},
    {0x24, 0x78}, // AGC/AEC
    {0x25, 0x68}, // AGC/AEC
    {0x26, 0xb3}, // AGC/AEC
    {0xa3, 0x0b},
    {0xa4, 0x15},
    {0xa5, 0x29},
    {0xa6, 0x4a},
    {0xa7, 0x58},
    {0xa8, 0x65},
    {0xa9, 0x70},
    {0xaa, 0x7b},
    {0xab, 0x85},
    {0xac, 0x8e},
    {0xad, 0xa0},
    {0xae, 0xb0},
    {0xaf, 0xcb},
    {0xb0, 0xe1},
    {0xb1, 0xf1},
    {0xb2, 0x14},
    {0x8e, 0x92},
    {0x96, 0xff},
    {0x97, 0x00},
    {0x14, 0x2b},   // AGC value, manual, set banding (default is 0x30)
    {0x0e, 0x00},
    {0x0c, 0xd6},
    {0x82, 0x3},
    {0x11, 0x00},   // Set clock prescaler
    {0x12, 0x6},
    {0x61, 0x0},
    {0x64, 0x11},
    {0xc3, 0x80},
    {0x81, 0x3f},
    {0x16, 0x3},
    {0x37, 0xc},
    {0x3e, 0x20},
    {0x5e, 0x0},
    {0xc4, 0x1},
    {0xc5, 0x80},
    {0xc6, 0x1},
    {0xc7, 0x80},
    {0xc8, 0x2},
    {0xc9, 0x80},
    {0xca, 0x1},
    {0xcb, 0xe0},
    {0xcc, 0x0},
    {0xcd, 0x40},   // Default to 64 line width
    {0xce, 0x0},
    {0xcf, 0x40},   // Default to 64 lines high
    {0x1c, 0x7f},
    {0x1d, 0xa2},
    {0xee, 0xee}  // End of register list marker 0xee
};

// *****************************************************************************
static volatile int8_t button_pressed = 0;

static uint32_t time_counter = 0;
static int8_t enable_cnn = 0;

static int8_t flash_led = 0;
static int8_t camera_vflip = 1;
static int8_t enable_video = 0;

static int8_t enable_sleep = 0;
static uint8_t *qspi_payload_buffer = NULL;
static version_t version = {S_VERSION_MAJOR, S_VERSION_MINOR, S_VERSION_BUILD};
static char demo_name[] = CATSDOGS_DEMO_NAME;
static uint32_t camera_clock = 15 * 1000 * 1000;

#ifdef PRINT_TIME_CNN
#define PR_TIMER(fmt, args...) if((time_counter % 10) == 0) printf("T[%-5s:%4d] " fmt "\r\n", S_MODULE_NAME, __LINE__, ##args )
#endif


//-----------------------------------------------------------------------------
// Local function declarations
//-----------------------------------------------------------------------------

static void fail(void);
static void send_img(void);
static void run_cnn(int x_offset, int y_offset);
static void run_demo(void);


//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
void button_int(void *cbdata)
{
    button_pressed = 1;
}

void sleep_defer_int(void)
{
    // Clear interrupt
    MXC_TMR_ClearFlags(MAX78000_VIDEO_SLEEP_DEFER_TMR);

    enable_sleep = 1;
}

int main(void)
{
    MXC_Delay(MXC_DELAY_MSEC(500)); // Wait supply to be ready

    int ret = 0;
    int slaveAddress;
    int id;

    // Enable camera
    GPIO_CLR(gpio_camera);
    MXC_GPIO_Config(&gpio_camera);

    // Configure flash pin
    GPIO_SET(gpio_flash);
    MXC_GPIO_Config(&gpio_flash);
    MXC_GPIO0->ds0 |= gpio_flash.mask;
    MXC_GPIO0->ds1 |= gpio_flash.mask;
    GPIO_SET(gpio_flash);

    // Configure CNN boost
    GPIO_CLR(gpio_cnn_boost);
    MXC_GPIO_Config(&gpio_cnn_boost);

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
    MXC_GPIO_Config(&gpio_exp_io);
    MXC_GPIO_Config(&gpio_exp_out);
    MXC_GPIO_Config(&gpio_audio_int);

    /* Enable cache */
    MXC_ICC_Enable(MXC_ICC0);

    /* Set system clock to 100 MHz */
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    PR_INFO("maxrefdes178_max78000_video %s v%d.%d.%d [%s]",
            demo_name, version.major, version.minor, version.build, S_BUILD_TIMESTAMP);

    // Enable peripheral, enable CNN interrupt, turn on CNN clock
    // CNN clock: 50 MHz div 1
    ret  = cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);
    ret &= cnn_init(); // Bring CNN state machine into consistent state
    ret &= cnn_load_weights(); // Load CNN kernels
    ret &= cnn_load_bias(); // Load bias
    ret &= cnn_configure(); // Configure CNN state machine
    if (ret != CNN_OK) {
        PR_ERROR("Could not initialize the CNN accelerator %d", ret);
        fail();
    }


    /* Initialize RTC */
    ret = MXC_RTC_Init(0, 0);
    if (ret != E_NO_ERROR) {
        PR_ERROR("Could not initialize rtc, %d", ret);
        fail();
    }

    ret = MXC_RTC_Start();
    if (ret != E_NO_ERROR) {
        PR_ERROR("Could not start rtc, %d", ret);
        fail();
    }

    ret = MXC_DMA_Init();
    if (ret != E_NO_ERROR) {
        PR_ERROR("DMA INIT ERROR %d", ret);
        fail();
    }

    ret = qspi_slave_init();
    if (ret != E_NO_ERROR) {
        PR_ERROR("qspi_dma_slave_init fail %d", ret);
        fail();
    }

    // Initialize the camera driver.
    ret = camera_init(camera_clock);
    if (ret != E_NO_ERROR) {
        PR_ERROR("Camera init failed! %d", ret);
        fail();
    }

    // Obtain the I2C slave address of the camera.
    slaveAddress = camera_get_slave_address();
    PR_INFO("Camera I2C slave address is 0x%02hhX", slaveAddress);

    // Obtain the product ID of the camera.
    ret = camera_get_product_id(&id);
    if (ret != STATUS_OK) {
        PR_ERROR("Error returned from reading camera product id. Error : %d", ret);
        fail();
    }
    PR_INFO("Camera Product ID is 0x%04hhX", id);

    // Obtain the manufacture ID of the camera.
    ret = camera_get_manufacture_id(&id);
    if (ret != STATUS_OK) {
        PR_ERROR("Error returned from reading camera manufacture id. Error : %d", ret);
        return -1;
    }
    PR_INFO("Camera Manufacture ID is 0x%04hhX", id);

    // set camera registers with default values
    for (int i = 0; (camera_settings[i][0] != 0xee); i++) {
        camera_write_reg(camera_settings[i][0], camera_settings[i][1]);
    }

    // Setup the camera image dimensions, pixel format and data acquiring details.

    ret = camera_setup(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FORMAT, FIFO_FOUR_BYTE, USE_DMA, MAX78000_VIDEO_CAMERA_DMA_CHANNEL);

    if (ret != STATUS_OK) {
        PR_ERROR("Error returned from setting up camera. Error : %d", ret);
        fail();
    }

    // Init button interrupt
    PB_RegisterCallback(0, (pb_callback) button_int);

    // Init sleep defer timer interrupt
    mxc_tmr_cfg_t tmr;
    MXC_TMR_Shutdown(MAX78000_VIDEO_SLEEP_DEFER_TMR);
    tmr.pres = TMR_PRES_128;
    tmr.mode = TMR_MODE_ONESHOT;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_APB_CLK;
    tmr.cmp_cnt = MAX78000_SLEEP_DEFER_DURATION * PeripheralClock / 128;
    tmr.pol = 0;
    MXC_NVIC_SetVector(MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(MAX78000_VIDEO_SLEEP_DEFER_TMR)), sleep_defer_int);
    NVIC_EnableIRQ(MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(MAX78000_VIDEO_SLEEP_DEFER_TMR)));
    MXC_TMR_Init(MAX78000_VIDEO_SLEEP_DEFER_TMR, &tmr, false);
    MXC_TMR_EnableInt(MAX78000_VIDEO_SLEEP_DEFER_TMR);
    MXC_TMR_Start(MAX78000_VIDEO_SLEEP_DEFER_TMR);

    // Use camera interface buffer for QSPI payload buffer
    {
        uint32_t  imgLen;
        uint32_t  w, h;
        camera_get_image(&qspi_payload_buffer, &imgLen, &w, &h);
    }

    // Successfully initialize the program
    PR_INFO("Initialization complete");

    run_demo();

    return 0;
}

static void fail(void)
{
    PR_ERROR("fail");

    GPIO_SET(gpio_red);
    GPIO_CLR(gpio_green);
    GPIO_CLR(gpio_blue);

    while(1);
}


static void run_demo(void)
{
    uint32_t capture_started_time = GET_RTC_MS();
    uint32_t cnn_completed_time = 0;
    uint32_t qspi_completed_time = 0;
    uint32_t capture_completed_time = 0;
    max78000_statistics_t max78000_statistics = {0};
    qspi_packet_header_t qspi_rx_header;
    qspi_state_e qspi_rx_state;


    camera_start_capture_image();

    while (1) { //Capture image and run CNN

        /* Check if QSPI RX has data */
        qspi_rx_state = qspi_slave_get_rx_state();
        if (qspi_rx_state == QSPI_STATE_CS_DEASSERTED_HEADER) {
            qspi_rx_header = qspi_slave_get_rx_header();

            // Use camera interface buffer for QSPI payload
            MXC_PCIF_Stop();

            qspi_slave_set_rx_data(qspi_payload_buffer, qspi_rx_header.info.packet_size);
            qspi_slave_trigger();
            qspi_slave_wait_rx();

            // Check payload crc again
            if (qspi_rx_header.payload_crc16 != crc16_sw(qspi_payload_buffer, qspi_rx_header.info.packet_size)) {
                PR_ERROR("Invalid payload crc %x", qspi_rx_header.payload_crc16);
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                camera_start_capture_image();
                continue;
            }

            // QSPI commands with payload
            switch(qspi_rx_header.info.packet_type) {
                case QSPI_PACKET_TYPE_VIDEO_FACEID_EMBED_UPDATE_CMD:
                    PR_INFO("Command not supported");
                    break;
                case QSPI_PACKET_TYPE_TEST:
                    PR_INFO("test data received %d", qspi_rx_header.info.packet_size);
                    for (int i = 0; i < qspi_rx_header.info.packet_size; i++) {
                        printf("%02hhX ", qspi_payload_buffer[i]);
                    }
                    printf("\n");
                    break;
                default:
                    PR_ERROR("Invalid packet %d", qspi_rx_header.info.packet_type);
                    break;
            }

            qspi_slave_set_rx_state(QSPI_STATE_IDLE);
            camera_start_capture_image();

        } else if (qspi_rx_state == QSPI_STATE_COMPLETED) {
            qspi_rx_header = qspi_slave_get_rx_header();

            // QSPI commands without payload
            switch(qspi_rx_header.info.packet_type) {
            case QSPI_PACKET_TYPE_VIDEO_VERSION_CMD:
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                qspi_slave_send_packet((uint8_t *) &version, sizeof(version),
                        QSPI_PACKET_TYPE_VIDEO_VERSION_RES);
                break;
            case QSPI_PACKET_TYPE_VIDEO_DEMO_NAME_CMD:
                qspi_slave_set_rx_state(QSPI_STATE_IDLE);
                qspi_slave_send_packet((uint8_t *) demo_name, strlen(demo_name),
                        QSPI_PACKET_TYPE_VIDEO_DEMO_NAME_RES);
                break;
            case QSPI_PACKET_TYPE_VIDEO_SERIAL_CMD:
                // TODO
                break;
            case QSPI_PACKET_TYPE_VIDEO_ENABLE_CMD:
                PR_INFO("enable video");
                enable_video = 1;
                // Also CNN
                PR_INFO("enable cnn");
                enable_cnn = 1;
                // Enable camera
                GPIO_CLR(gpio_camera);
                camera_start_capture_image();
                break;
            case QSPI_PACKET_TYPE_VIDEO_DISABLE_CMD:
                PR_INFO("disable video");
                enable_video = 0;
                MXC_PCIF_Stop();
                // Disable camera
                GPIO_SET(gpio_camera);
                GPIO_CLR(gpio_red);
                GPIO_CLR(gpio_green);
                // Also CNN
                PR_INFO("disable cnn");
                enable_cnn = 0;
                break;
            case QSPI_PACKET_TYPE_VIDEO_ENABLE_CNN_CMD:
                PR_INFO("enable cnn");
                enable_cnn = 1;
                break;
            case QSPI_PACKET_TYPE_VIDEO_DISABLE_CNN_CMD:
                PR_INFO("disable cnn");
                enable_cnn = 0;
                GPIO_CLR(gpio_red);
                GPIO_CLR(gpio_green);
                break;
            case QSPI_PACKET_TYPE_VIDEO_FACEID_SUBJECTS_CMD:
            	PR_INFO("command not supported!");
                //MXC_PCIF_Stop();
                break;
            case QSPI_PACKET_TYPE_VIDEO_ENABLE_FLASH_LED_CMD:
                PR_INFO("enable flash");
                GPIO_CLR(gpio_flash);
                flash_led = 1;
                break;
            case QSPI_PACKET_TYPE_VIDEO_DISABLE_FLASH_LED_CMD:
                PR_INFO("disable flash");
                GPIO_SET(gpio_flash);
                flash_led = 0;
                break;
            case QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_5_MHZ_CMD:
                PR_INFO("camera clock 5mhz");
                camera_clock = 5 * 1000 * 1000;
                MXC_PT_Stop(MXC_F_PTG_ENABLE_PT0);
                MXC_PT_SqrWaveConfig(0, camera_clock);
                MXC_PT_Start(MXC_F_PTG_ENABLE_PT0);
                break;
            case QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_10_MHZ_CMD:
                PR_INFO("camera clock 10mhz");
                camera_clock = 10 * 1000 * 1000;
                MXC_PT_Stop(MXC_F_PTG_ENABLE_PT0);
                MXC_PT_SqrWaveConfig(0, camera_clock);
                MXC_PT_Start(MXC_F_PTG_ENABLE_PT0);
                break;
            case QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_15_MHZ_CMD:
                PR_INFO("camera clock 15mhz");
                camera_clock = 15 * 1000 * 1000;
                MXC_PT_Stop(MXC_F_PTG_ENABLE_PT0);
                MXC_PT_SqrWaveConfig(0, camera_clock);
                MXC_PT_Start(MXC_F_PTG_ENABLE_PT0);
                break;
            case QSPI_PACKET_TYPE_VIDEO_ENABLE_VFLIP_CMD:
                PR_INFO("flip enable");
                camera_vflip = 1;
                camera_set_vflip(camera_vflip);
                break;
            case QSPI_PACKET_TYPE_VIDEO_DISABLE_VFLIP_CMD:
                PR_INFO("flip disable");
                camera_vflip = 0;
                camera_set_vflip(camera_vflip);
                break;
            case QSPI_PACKET_TYPE_VIDEO_DEBUG_CMD:
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
            PR_INFO("button A pressed");

            flash_led ^= 1;
            if (flash_led) {
                GPIO_CLR(gpio_flash);
            } else {
                GPIO_SET(gpio_flash);
            }

            qspi_slave_send_packet(NULL, 0, QSPI_PACKET_TYPE_VIDEO_BUTTON_PRESS_RES);
        }

        if (!enable_video) {
            if (enable_sleep) {
                //MXC_LP_EnterSleepMode();
                __WFI();
            }
            continue;
        }

        if (camera_is_image_rcv()) { // Check whether image is ready
            capture_completed_time = GET_RTC_MS();

            send_img();

            qspi_completed_time = GET_RTC_MS();

            if (enable_cnn) {


            	PR_INFO("CNN_EN");
                run_cnn(0, 0);
            }
            else
            	PR_INFO("CNN_DIS");

            cnn_completed_time = GET_RTC_MS();

            if (time_counter % 10 == 0) {
                max78000_statistics.capture_duration_us = (capture_completed_time - capture_started_time) * 1000;
                max78000_statistics.communication_duration_us = (qspi_completed_time - capture_completed_time) * 1000;
                max78000_statistics.cnn_duration_us = cnn_time; //(cnn_completed_time - qspi_completed_time) * 1000;
                max78000_statistics.total_duration_us = (cnn_completed_time - capture_started_time) * 1000;

                PR_DEBUG("Capture : %lu", max78000_statistics.capture_duration_us);
                PR_DEBUG("CNN     : %lu", max78000_statistics.cnn_duration_us);
                PR_DEBUG("QSPI    : %lu", max78000_statistics.communication_duration_us);
                PR_DEBUG("Total   : %lu\n\n", max78000_statistics.total_duration_us);

                qspi_slave_send_packet((uint8_t *) &max78000_statistics, sizeof(max78000_statistics),
                        QSPI_PACKET_TYPE_VIDEO_STATISTICS_RES);
            }

            time_counter++;

            camera_start_capture_image();
            capture_started_time = GET_RTC_MS();

        }
    }
}

static void load_input_camera(int x_offset, int y_offset)
{

    uint8_t *data;
    uint8_t *raw;
    uint8_t ur, ug, ub;
    int8_t r, g, b;
    uint32_t number;
    uint32_t w, h;
    PR_DEBUG("load_input_camera");
    camera_get_image(&raw, &number, &w, &h);

	// Read 240x240 pixels, pick one out of 3 pixels with offset (9,9) to make 74x74 CNN input
	// CNN needs RGB888, pack into 32bit word

        for (int i = y_offset; i < CATS_DOGS_HEIGHT + y_offset; i++) {
            //PR_INFO("i=%d",i);
        data = raw + (((LCD_HEIGHT - CATS_DOGS_HEIGHT) / 2) + i) * LCD_WIDTH * LCD_BYTE_PER_PIXEL;  // down
        data += ((LCD_WIDTH - CATS_DOGS_WIDTH) / 2) * LCD_BYTE_PER_PIXEL;  // right
        //data += ((FACEDETECTION_HEIGHT - FACEDETECTION_WIDTH) / 2) * LCD_BYTE_PER_PIXEL;  // right

        for(int j = x_offset; j < CATS_DOGS_WIDTH + x_offset; j++) {
            //PR_INFO("j=%d",j);
            // RGB565, |RRRRRGGG|GGGBBBBB|
            ub = (data[j * LCD_BYTE_PER_PIXEL + 1] << 3);
            ug = ((data[j * LCD_BYTE_PER_PIXEL] << 5) | ((data[j * LCD_BYTE_PER_PIXEL + 1] & 0xE0) >> 3));
            ur = (data[j * LCD_BYTE_PER_PIXEL] & 0xF8);
            b = ub - 128;
            g = ug - 128;
            r = ur - 128;
            //PR_INFO("r=%d",r);
            // Loading data into the CNN fifo
            while (((*((volatile uint32_t *) 0x50000004) & 1)) != 0); // Wait for FIFO 0

            number = 0x00FFFFFF & ((((uint8_t)b) << 16) | (((uint8_t)g) << 8) | ((uint8_t) r));

            *((volatile uint32_t *) 0x50000008) = number; // Write FIFO 0
        }
    }
	//PR_INFO("Total samples: %d", cnt);
}

static void send_img(void)
{
    uint8_t   *raw;
    uint32_t  imgLen;
    uint32_t  w, h;

    // Get the details of the image from the camera driver.
    camera_get_image(&raw, &imgLen, &w, &h);

    qspi_slave_send_packet(raw, imgLen, QSPI_PACKET_TYPE_VIDEO_DATA_RES);
//    MXC_Delay(MXC_DELAY_MSEC(3)); // Yield SPI DMA RAM read
}

static void run_cnn(int x_offset, int y_offset)
{

    // Get the details of the image from the camera driver.
    //camera_get_image(&raw, &number, &w, &h);

#ifdef PRINT_TIME_CNN
    uint32_t pass_time = GET_RTC_MS();
#endif

    // Enable CNN clock
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

    cnn_init(); // Bring state machine into consistent state
   // cnn_load_weights(); // No need to reload kernels
    cnn_load_bias(); //load bias
    cnn_configure(); // Configure state machine

    cnn_start();

#ifdef PRINT_TIME_CNN
    PR_TIMER("CNN init : %d", GET_RTC_MS() - pass_time);
    pass_time = GET_RTC_MS();
#endif

    load_input_camera(0, 0);



#ifdef PRINT_TIME_CNN
    PR_TIMER("CNN load : %d", GET_RTC_MS() - pass_time);
    pass_time = GET_RTC_MS();
#endif

    while (cnn_time == 0)
        __WFI(); // Wait for CNN done

#ifdef PRINT_TIME_CNN
    PR_TIMER("CNN wait : %d", GET_RTC_MS() - pass_time);
    pass_time = GET_RTC_MS();
#endif

    MXC_TMR_GetTime(CNN_INFERENCE_TIMER, cnn_time, (uint32_t*) &cnn_time, &units);
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
	
	PR_INFO("load_inference_time: %d us", cnn_time);
    cnn_unload((uint32_t*) ml_data);

    cnn_stop();
    // Disable CNN clock to save power
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CNN);

#ifdef PRINT_TIME_CNN
    PR_TIMER("CNN unload : %d", GET_RTC_MS() - pass_time);
    pass_time = GET_RTC_MS();
#endif

    softmax_q17p14_q15((const q31_t*) ml_data, CNN_NUM_OUTPUTS, ml_softmax);

	int digs, tens, max_index=0;
	int max_result = result[0];
    for (int i = 0; i < CNN_NUM_OUTPUTS; i++) {
      digs = (1000 * ml_softmax[i] + 0x4000) >> 15;
      tens = digs % 10;
      digs = digs / 10;
      result[i] = digs;

      PR_INFO("[%7d] -> Class %d %8s: %d.%d%%\r\n", ml_data[i], i, classes[i], result[i], tens);
      if (result[i] > max_result)
      {
    	  max_result = result[i];
    	  max_index = i;
      }
    }

    // show classification + %
    classification_result_t classification_result = {0};

    char msg[14];

	if (max_result < 80)
	{
		classification_result.classification = CLASSIFICATION_UNKNOWN;
		sprintf(msg,"Unknown  ");
    }
	else
	{
		classification_result.classification = CLASSIFICATION_DETECTED;
		sprintf(msg,"%s (%d%%)",classes[max_index], max_result);
	}
	strncpy(classification_result.result, msg, sizeof(classification_result.result) - 1);
    qspi_slave_send_packet((uint8_t *) &classification_result, sizeof(classification_result),
            QSPI_PACKET_TYPE_VIDEO_CLASSIFICATION_RES);

    MXC_Delay(MXC_DELAY_MSEC(250));


#ifdef PRINT_TIME_CNN
    PR_TIMER("Embedding result : %d", GET_RTC_MS() - pass_time);
    pass_time = GET_RTC_MS();
#endif
}

