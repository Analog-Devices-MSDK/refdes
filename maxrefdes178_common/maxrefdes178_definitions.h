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

#ifndef _MAXREFDES178_DEFINTIIONS_H_
#define _MAXREFDES178_DEFINTIIONS_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdint.h>


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
/*** Common ***/
// Common SPI
#define QSPI_SPEED                         10000000UL  // 7-12
#define QSPI_START_SYMBOL                  0xAABBCCDD
#define QSPI_CS_ASSERT_WAIT                10         // us

#define SPI_TIMEOUT_CNT                    1000000  // loop counter

// Common I2C
#define I2C_SPEED                          100000  // hz
#define I2C_TIMEOUT_CNT                    1000000  // loop counter

#define I2C_ADDR_MAX20303_PMIC             0x50  // 0x50 for writes, 0x51 for reads
#define I2C_ADDR_MAX17048_FUEL_GAUGE       0x6C  // 0x6C for writes, 0x6D for reads
#define I2C_ADDR_MAX7325_PORTS             0xD2
#define I2C_ADDR_MAX7325_OUTPUTS           0xB2
#define I2C_ADDR_BMI160                    0xD0
#define I2C_ADDR_MAX34417                  0x20
#define I2C_ADDR_MAX9867                   0x30
#define I2C_ADDR_FT6236U                   0x70

// Common LCD
#define LCD_WIDTH                          240
#define LCD_HEIGHT                         240
#define LCD_BYTE_PER_PIXEL                 2  // RGB565
#define LCD_DATA_SIZE                      (LCD_WIDTH * LCD_HEIGHT * LCD_BYTE_PER_PIXEL)
#define LCD_NOTIFICATION_MAX_SIZE          40

#define LCD_CLASSIFICATION_DURATION        UINT32_C(1000)  // ms
#define LCD_NO_VIDEO_REFRESH_DURATION      UINT32_C(3000)  // ms
#define LCD_VIDEO_DISABLE_REFRESH_DURATION UINT32_C(100)  // ms
#define LCD_NOTIFICATION_DURATION          UINT32_C(7 * 1000)  // ms

#define LCD_START_BUTTON_X1                20
#define LCD_START_BUTTON_X2                220
#define LCD_START_BUTTON_Y1                150 // changed to fit the start up button to ADI logo
#define LCD_START_BUTTON_Y2                194 // changed to fit the start up button to ADI logo

// Common Camera
#define CAMERA_WIDTH                       LCD_WIDTH
#define CAMERA_HEIGHT                      LCD_HEIGHT
#define CAMERA_FORMAT                      PIXFORMAT_RGB565

// Common Classification
#define CLASSIFICATION_STRING_SIZE         15

// Common Demo Name
#define DEMO_STRING_SIZE                   15
#define FACEID_DEMO_NAME                   "FaceID"
#define UNET_DEMO_NAME                     "UNet"
#define WILDLIFE_DEMO_NAME                 "WildLife"
#define CATSDOGS_DEMO_NAME                 "CatsDogs"
#define IMAGECAPTURE_DEMO_NAME             "Image Capture"
#define VCAUDIO_DEMO_NAME                  "VC Audio"
#define ASL_DEMO_NAME                      "ASL"
#define DIGIT_DET_DEMO_NAME                "DigitDetect"
#define AIPORTRAIT_DEMO_NAME               "AIPortrait"
#define FACEDETECTION_DEMO_NAME            "FaceDetection"
#define FACIAL_RECOGNITION_DEMO_NAME       "Facial Rec"

// Common FaceID
#define FACEID_WIDTH                       112
#define FACEID_HEIGHT                      112

#define FACEID_RECTANGLE_X1                ((LCD_WIDTH - FACEID_WIDTH) / 2)
#define FACEID_RECTANGLE_Y1                ((LCD_HEIGHT - FACEID_HEIGHT) / 2)
#define FACEID_RECTANGLE_X2                (FACEID_RECTANGLE_X1 + FACEID_WIDTH)
#define FACEID_RECTANGLE_Y2                (FACEID_RECTANGLE_Y1 + FACEID_HEIGHT)

#define FACEID_MAX_SUBJECT                 6
#define FACEID_MAX_PHOTO_PER_SUBJECT       8
#define FACEID_MAX_SUBJECT_NAME_SIZE       CLASSIFICATION_STRING_SIZE
#define FACEID_EMBEDDING_SIZE              512
#define FACEID_MAX_EMBEDDINGS_HEADER_SIZE  (((FACEID_MAX_SUBJECT_NAME_SIZE + 1) * FACEID_MAX_SUBJECT) + 9)
#define FACEID_MAX_EMBEDDINGS_DATA_SIZE    ((FACEID_EMBEDDING_SIZE + 1) * FACEID_MAX_SUBJECT * FACEID_MAX_PHOTO_PER_SUBJECT)
#define FACEID_MAX_EMBEDDINGS_SIZE         (FACEID_MAX_EMBEDDINGS_HEADER_SIZE + FACEID_MAX_EMBEDDINGS_DATA_SIZE)

// Common FaceDetection
#define FACEDETECTION_WIDTH                168
#define FACEDETECTION_HEIGHT               224
#define FACEDETECTION_DATA_SIZE            (FACEDETECTION_WIDTH * FACEDETECTION_HEIGHT * LCD_BYTE_PER_PIXEL)

// Common CatsDogs
#define CATSDOGS_WIDTH                     128
#define CATSDOGS_HEIGHT                    128

#define CATSDOGS_RECTANGLE_X1              ((LCD_WIDTH - CATSDOGS_WIDTH) / 2)
#define CATSDOGS_RECTANGLE_Y1              ((LCD_HEIGHT - CATSDOGS_HEIGHT) / 2)
#define CATSDOGS_RECTANGLE_X2              (CATSDOGS_RECTANGLE_X1 + CATSDOGS_WIDTH)
#define CATSDOGS_RECTANGLE_Y2              (CATSDOGS_RECTANGLE_Y1 + CATSDOGS_HEIGHT)

// Common ASL 
#define ASL_WIDTH                     192
#define ASL_HEIGHT                    172

#define ASL_RECTANGLE_X1              ((LCD_WIDTH - ASL_WIDTH) / 2)
#define ASL_RECTANGLE_Y1              ((LCD_HEIGHT - ASL_HEIGHT) / 2)
#define ASL_RECTANGLE_X2              (ASL_RECTANGLE_X1 + ASL_WIDTH)
#define ASL_RECTANGLE_Y2              (ASL_RECTANGLE_Y1 + ASL_HEIGHT)

// Common UNet
#define UNET_WIDTH                         192
#define UNET_HEIGHT                        172

#define UNET_RECTANGLE_X1                  ((LCD_WIDTH - UNET_WIDTH) / 2)
#define UNET_RECTANGLE_Y1                  ((LCD_HEIGHT - UNET_HEIGHT) / 2)
#define UNET_RECTANGLE_X2                  (UNET_RECTANGLE_X1 + UNET_WIDTH)
#define UNET_RECTANGLE_Y2                  (UNET_RECTANGLE_Y1 + UNET_HEIGHT)

#define UNET_IMAGE_SIZE_X                  80
#define UNET_IMAGE_SIZE_Y                  80

// Common WILDLIFE
#define WILDLIFE_WIDTH                     192
#define WILDLIFE_HEIGHT                    172

#define WILDLIFE_RECTANGLE_X1              ((LCD_WIDTH - WILDLIFE_WIDTH) / 2)
#define WILDLIFE_RECTANGLE_Y1              ((LCD_HEIGHT - WILDLIFE_HEIGHT) / 2)
#define WILDLIFE_RECTANGLE_X2              (WILDLIFE_RECTANGLE_X1 + WILDLIFE_WIDTH)
#define WILDLIFE_RECTANGLE_Y2              (WILDLIFE_RECTANGLE_Y1 + WILDLIFE_HEIGHT)

// Common AIPortrait
#define AIPORTRAIT_WIDTH                   240
#define AIPORTRAIT_HEIGHT                  240

#define AIPORTRAIT_IMAGE_SIZE_X            352
#define AIPORTRAIT_IMAGE_SIZE_Y            352

#define AIPORTRAIT_RECTANGLE_X1            ((AIPORTRAIT_IMAGE_SIZE_X - AIPORTRAIT_WIDTH) / 2)
#define AIPORTRAIT_RECTANGLE_Y1            ((AIPORTRAIT_IMAGE_SIZE_Y - AIPORTRAIT_HEIGHT) / 2)
#define AIPORTRAIT_RECTANGLE_X2            (AIPORTRAIT_RECTANGLE_X1 + AIPORTRAIT_WIDTH)
#define AIPORTRAIT_RECTANGLE_Y2            (AIPORTRAIT_RECTANGLE_Y1 + AIPORTRAIT_HEIGHT)
#define AIPORTRAIT_INFER_SIZE              30976 // size of inference 32x88x88/8

// Common DigitDetection
#define DIGIT_DET_WIDTH                    240
#define DIGIT_DET_HEIGHT                   240
#define DIGIT_DET_RECTANGLE_X1             ((LCD_WIDTH - DIGIT_DET_WIDTH) / 2)
#define DIGIT_DET_RECTANGLE_Y1             ((LCD_HEIGHT - DIGIT_DET_HEIGHT) / 2)
#define DIGIT_DET_RECTANGLE_X2             (DIGIT_DET_RECTANGLE_X1 + DIGIT_DET_WIDTH)
#define DIGIT_DET_RECTANGLE_Y2             (DIGIT_DET_RECTANGLE_Y1 + DIGIT_DET_HEIGHT)

// Common BLE
#define BLE_MAX_MTU_SIZE                   256
#define BLE_MAX_MTU_REQUEST_SIZE           (BLE_MAX_MTU_SIZE - 4) //252
#define BLE_MAX_PACKET_SIZE                (BLE_MAX_MTU_REQUEST_SIZE - 3) //249

#define BLE_STATISTICS_INTERVAL            UINT32_C(1000)  // ms

// Inactivity
#define INACTIVITY_SHORT_DURATION          UINT32_C(60 * 1000)  // ms
#define INACTIVITY_LONG_DURATION           UINT32_C(2 * 60 * 1000)  // ms

// Common MAX78000s
#define MAX78000_SLEEP_DEFER_DURATION      30  // s

/*** MAX32666 ***/
// MAX32666 PINS
#define MAX32666_VIDEO_INT_PIN             {MXC_GPIO0, MXC_GPIO_PIN_30, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_VIDEO_INT_MODE            MXC_GPIO_INT_FALLING
#define MAX32666_VIDEO_CS_PIN              {MXC_GPIO0, MXC_GPIO_PIN_8, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX32666_AUDIO_INT_PIN             {MXC_GPIO0, MXC_GPIO_PIN_23, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_AUDIO_INT_MODE            MXC_GPIO_INT_FALLING
#define MAX32666_AUDIO_CS_PIN              {MXC_GPIO0, MXC_GPIO_PIN_14, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX32666_CORE1_SWD_PIN             {MXC_GPIO0, (MXC_GPIO_PIN_6 | MXC_GPIO_PIN_7), MXC_GPIO_FUNC_ALT3, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO mxc_pins

#define MAX32666_LCD_DC_PIN                {MXC_GPIO0, MXC_GPIO_PIN_31, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_LCD_CS_PIN                {MXC_GPIO0, MXC_GPIO_PIN_22, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_LCD_TOUCH_INT_PIN         {MXC_GPIO1, MXC_GPIO_PIN_10, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_LCD_TOUCH_INT_MODE        MXC_GPIO_INT_FALLING

#define MAX32666_SD_EN_PIN                 {MXC_GPIO1, MXC_GPIO_PIN_11, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX32666_CORE0_INT_PIN             {MXC_GPIO0, MXC_GPIO_PIN_16, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO
#define MAX32666_CORE0_INT_MODE            MXC_GPIO_INT_FALLING                                                                     // TODO
#define MAX32666_CORE1_INT_PIN             {MXC_GPIO0, MXC_GPIO_PIN_18, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO} // TODO

#define MAX32666_VIDEO_IO_PIN              {MXC_GPIO0, MXC_GPIO_PIN_20, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_AUDIO_IO_PIN              {MXC_GPIO0, MXC_GPIO_PIN_21, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX32666_EXPANDER_INT_PIN          {MXC_GPIO0, MXC_GPIO_PIN_15, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_EXPANDER_INT_MODE         MXC_GPIO_INT_FALLING

#define MAX32666_BUTTON_X_INT_PIN          {MXC_GPIO1, MXC_GPIO_PIN_6, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_BUTTON_X_INT_MODE         MXC_GPIO_INT_FALLING

#define MAX32666_BUTTON_POWER_INT_PIN      {MXC_GPIO1, MXC_GPIO_PIN_9, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_BUTTON_POWER_INT_MODE     MXC_GPIO_INT_BOTH

#define MAX32666_SRAM_CS_PIN               {MXC_GPIO1, MXC_GPIO_PIN_8, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX32666_SRAM_HOLD_PIN             {MXC_GPIO0, MXC_GPIO_PIN_13, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};

#define MAX32666_HOST_BL_TX_PIN            {MXC_GPIO1, MXC_GPIO_PIN_13, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

// MAX32666 I2C
#define MAX32666_I2C                       MXC_I2C2_BUS0
#define MAX32666_I2C_GPIO_CFG              gpio_cfg_i2c2

// MAX32666 Host Bootloader UART
#define MAX32666_HOST_BL_UART              MXC_UART1  // TODO

// MAX32666 LCD
#define MAX32666_LCD_SPI                   MXC_SPI1
#define MAX32666_LCD_DMA_REQSEL_SPITX      MXC_DMA_REQUEST_SPI1TX
#define MAX32666_LCD_SPI_SPEED             16000000
#define MAX32666_LCD_BACKLIGHT_HIGH        0x10
#define MAX32666_LCD_BACKLIGHT_LOW         0x0B

#define MAX32666_LCD_DMA_CHANNEL           0
#define MAX32666_LCD_DMA_IRQ               DMA0_IRQn
#define MAX32666_LCD_DMA_IRQ_HAND          DMA0_IRQHandler
#define MAX32666_LCD_MAP                   MAP_A

// MAX32666 QSPI
#define MAX32666_QSPI                      MXC_SPI0
#define MAX32666_QSPI_DMA_REQSEL_SPITX     MXC_DMA_REQUEST_SPI0TX
#define MAX32666_QSPI_DMA_REQSEL_SPIRX     MXC_DMA_REQUEST_SPI0RX

#define MAX32666_QSPI_DMA_CHANNEL          1
#define MAX32666_QSPI_DMA_IRQ              DMA1_IRQn
#define MAX32666_QSPI_DMA_IRQ_HAND         DMA1_IRQHandler
#define MAX32666_QSPI_MAP                  MAP_B

// MAX32666 SD CARD
#define MAX32666_SD_BUS_VOLTAGE            MXC_SDHC_Bus_Voltage_3_3
#define MAX32666_SD_CLK_DIV                0x0b0  // Maximum divide ratio
#define MAX32666_SD_INIT_RETRY             10

// MAX32666 Hardware semaphores
#define MAX32666_SEMAPHORE_PRINT           1
#define MAX32666_SEMAPHORE_BLE_QUEUE       2

// MAX32666 Timers
#define MAX32666_TIMER_BLE                 MXC_TMR0  // TODO remove
#define MAX32666_TIMER_BLE_SLEEP           MXC_TMR1  // TODO remove
#define MAX32666_TIMER_MS                  MXC_TMR2
#define MAX32666_TIMER_BUTTON_POWER        MXC_TMR3

// MAX32666 BLE Communication buffer
#define MAX32666_BLE_QUEUE_SIZE            10
#define MAX32666_BLE_COMMAND_BUFFER_SIZE   FACEID_MAX_EMBEDDINGS_SIZE

// MAX32666 PMIC and Fuel Gauge
#define MAX32666_PMIC_INTERVAL             UINT32_C(10 * 1000)  // ms
#define MAX32666_BUTTON_POWER_OFF_INTERVAL 3  // s
#define MAX32666_SOC_WARNING_LEVEL         10

// MAX32666 Power Accumulator
#define MAX32666_POWMON_INTERVAL           UINT32_C(5000)  // ms

// MAX32666 LED
#define MAX32666_LED_INTERVAL              UINT32_C(1000)  // ms

/*** MAX78000 AUDIO ***/
// MAX78000 AUDIO PINS
#define MAX78000_AUDIO_HOST_CS_PIN         {MXC_GPIO0, MXC_GPIO_PIN_4, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_AUDIO_HOST_INT_PIN        {MXC_GPIO0, MXC_GPIO_PIN_12, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_AUDIO_HOST_IO_PIN         {MXC_GPIO1, MXC_GPIO_PIN_6, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_AUDIO_LED_RED_PIN         {MXC_GPIO2, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_AUDIO_LED_GREEN_PIN       {MXC_GPIO2, MXC_GPIO_PIN_1, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_AUDIO_LED_BLUE_PIN        {MXC_GPIO2, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_AUDIO_EXPANDER_OUT1_PIN   {MXC_GPIO1, MXC_GPIO_PIN_7, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO} // TODO
#define MAX78000_AUDIO_EXPANDER_OUT2_PIN   {MXC_GPIO3, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO} // TODO

#define MAX78000_AUDIO_BUTTON_INT_PIN      {MXC_GPIO0, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_AUDIO_BUTTON_INT_MODE     MXC_GPIO_INT_FALLING

#define MAX78000_AUDIO_CODEC_INT_PIN       {MXC_GPIO0, MXC_GPIO_PIN_13, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO
#define MAX78000_AUDIO_CODEC_INT_MODE      MXC_GPIO_INT_FALLING

#define MAX78000_AUDIO_CNN_BOOST_PIN       {MXC_GPIO2, MXC_GPIO_PIN_5, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_AUDIO_AUDIO_OSC_PIN       {MXC_GPIO0, MXC_GPIO_PIN_31, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_AUDIO_MIC_EN_PIN          {MXC_GPIO0, MXC_GPIO_PIN_15, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_AUDIO_MIC_SEL_PIN         {MXC_GPIO0, MXC_GPIO_PIN_30, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_AUDIO_DEBUG_SEL_PIN       {MXC_GPIO2, MXC_GPIO_PIN_3, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_AUDIO_VIDEO_INT_PIN       {MXC_GPIO2, MXC_GPIO_PIN_4, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_AUDIO_SRAM_CS_PIN         {MXC_GPIO3, MXC_GPIO_PIN_1, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_AUDIO_I2C_PINS            {MXC_GPIO0, (MXC_GPIO_PIN_10 | MXC_GPIO_PIN_11), MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO mxc_pins

// MAX78000 AUDIO QSPI
#define MAX78000_AUDIO_QSPI                MXC_SPI0
#define MAX78000_AUDIO_QSPI_DMA_CHANNEL    1
#define MAX78000_AUDIO_QSPI_DMA_IRQ        DMA1_IRQn
#define MAX78000_AUDIO_QSPI_DMA_IRQ_HAND   DMA1_IRQHandler

// MAX78000 AUDIO I2C SLAVE
#define MAX78000_AUDIO_I2C                 MXC_I2C0  // TODO

// MAX78000 AUDIO UART
#define MAX78000_AUDIO_HOST_UART           MXC_UART3  // TODO

// MAX78000 AUDIO SLEEP DEFER TIMER
#define MAX78000_AUDIO_SLEEP_DEFER_TMR     MXC_TMR2

/*** MAX78000 VIDEO ***/
// MAX78000 VIDEO PINS
#define MAX78000_VIDEO_HOST_CS_PIN         {MXC_GPIO0, MXC_GPIO_PIN_4, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_VIDEO_HOST_INT_PIN        {MXC_GPIO0, MXC_GPIO_PIN_12, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_VIDEO_HOST_IO_PIN         {MXC_GPIO1, MXC_GPIO_PIN_6, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_VIDEO_LED_RED_PIN         {MXC_GPIO2, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_VIDEO_LED_GREEN_PIN       {MXC_GPIO2, MXC_GPIO_PIN_1, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_VIDEO_LED_BLUE_PIN        {MXC_GPIO2, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_VIDEO_EXPANDER_IO_PIN     {MXC_GPIO1, MXC_GPIO_PIN_7, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO} // TODO
#define MAX78000_VIDEO_EXPANDER_OUT_PIN    {MXC_GPIO3, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO} // TODO

#define MAX78000_VIDEO_BUTTON_INT_PIN      {MXC_GPIO0, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}
#define MAX78000_VIDEO_BUTTON_INT_MODE     MXC_GPIO_INT_FALLING

#define MAX78000_VIDEO_CNN_BOOST_PIN       {MXC_GPIO2, MXC_GPIO_PIN_5, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_VIDEO_FLASH_LED_PIN       {MXC_GPIO0, MXC_GPIO_PIN_19, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH}

#define MAX78000_VIDEO_CAMERA_PIN          {MXC_GPIO0, MXC_GPIO_PIN_3, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}

#define MAX78000_VIDEO_DEBUG_SEL_PIN       {MXC_GPIO2, MXC_GPIO_PIN_3, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_VIDEO_AUDIO_INT_PIN       {MXC_GPIO2, MXC_GPIO_PIN_4, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_VIDEO_SRAM_CS_PIN         {MXC_GPIO3, MXC_GPIO_PIN_1, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO

#define MAX78000_VIDEO_I2C_PINS            {MXC_GPIO0, (MXC_GPIO_PIN_10 | MXC_GPIO_PIN_11), MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO}  // TODO mxc_pins

// MAX78000 VIDEO QSPI
#define MAX78000_VIDEO_QSPI                MXC_SPI0
#define MAX78000_VIDEO_QSPI_DMA_CHANNEL    1
#define MAX78000_VIDEO_QSPI_DMA_IRQ        DMA1_IRQn
#define MAX78000_VIDEO_QSPI_DMA_IRQ_HAND   DMA1_IRQHandler

// MAX78000 VIDEO CAMERA
#define MAX78000_VIDEO_CAMERA_DMA_CHANNEL  0
#define MAX78000_VIDEO_CAMERA_DMA_IRQ      DMA0_IRQn
#define MAX78000_VIDEO_CAMERA_DMA_IRQ_HAND DMA0_IRQHandler

// MAX78000 VIDEO I2C SLAVE
#define MAX78000_VIDEO_I2C                 MXC_I2C0  // TODO

// MAX78000 VIDEO UART
#define MAX78000_VIDEO_HOST_UART           MXC_UART3  // TODO

// MAX78000 VIDEO SLEEP DEFER TIMER
#define MAX78000_VIDEO_SLEEP_DEFER_TMR     MXC_TMR2

/*** MAX32666 BOOTLOADER ***/
#define MAX32666_BL_MAX_DIR_LEN            (34 + 1)     // +1 for NULL terminator
#define MAX32666_BL_MAX_DIR_NUMBER         22
#define MAX32666_BL_MAX_FW_PATH_LEN        100
#define MAX32666_BL_MAX32666_FW_NAME       "maxrefdes178_max32666"
#define MAX32666_BL_MAX78000_VIDEO_FW_NAME "maxrefdes178_max78000_video"
#define MAX32666_BL_MAX78000_AUDIO_FW_NAME "maxrefdes178_max78000_audio"
#define MAX32666_BL_MAX32666_FW_EXTENSION  "msbl"


//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
// Typedef Enums
// QSPI packet types
typedef enum {
//  QSPI Packet                                 QSPI Packet Payload Description
    QSPI_PACKET_TYPE_NOTHING = 0,            // None
    QSPI_PACKET_TYPE_ACKNOWLEDGE,            // qspi_packet_type_e

    QSPI_PACKET_TYPE_VIDEO_VERSION_CMD,      // None
    QSPI_PACKET_TYPE_VIDEO_VERSION_RES,      // version_t

    QSPI_PACKET_TYPE_AUDIO_VERSION_CMD,      // None
    QSPI_PACKET_TYPE_AUDIO_VERSION_RES,      // version_t

    QSPI_PACKET_TYPE_VIDEO_SERIAL_CMD,       // None
    QSPI_PACKET_TYPE_VIDEO_SERIAL_RES,       // serial_num_t

    QSPI_PACKET_TYPE_AUDIO_SERIAL_CMD,       // None
    QSPI_PACKET_TYPE_AUDIO_SERIAL_RES,       // serial_num_t

    QSPI_PACKET_TYPE_VIDEO_DATA_RES,                // 240x240 RGB565 Image
    QSPI_PACKET_TYPE_VIDEO_CLASSIFICATION_RES,      // classification_result_t
    QSPI_PACKET_TYPE_VIDEO_STATISTICS_RES,          // max78000_statistics_t

    QSPI_PACKET_TYPE_AUDIO_DATA_RES,                // TODO
    QSPI_PACKET_TYPE_AUDIO_CLASSIFICATION_RES,      // classification_result_t
    QSPI_PACKET_TYPE_AUDIO_STATISTICS_RES,          // max78000_statistics_t

    QSPI_PACKET_TYPE_VIDEO_FACEID_EMBED_UPDATE_CMD, // FaceID embeddings binary
    QSPI_PACKET_TYPE_VIDEO_FACEID_EMBED_UPDATE_RES, // faceid_embed_update_status_e

    QSPI_PACKET_TYPE_VIDEO_FACEID_SUBJECTS_CMD,     // None
    QSPI_PACKET_TYPE_VIDEO_FACEID_SUBJECTS_RES,     // Subject names string list

    QSPI_PACKET_TYPE_VIDEO_ENABLE_CMD,       // None
    QSPI_PACKET_TYPE_VIDEO_DISABLE_CMD,      // None
    QSPI_PACKET_TYPE_AUDIO_ENABLE_CMD,       // None
    QSPI_PACKET_TYPE_AUDIO_DISABLE_CMD,      // None

    QSPI_PACKET_TYPE_VIDEO_ENABLE_CNN_CMD,   // None
    QSPI_PACKET_TYPE_VIDEO_DISABLE_CNN_CMD,  // None
    QSPI_PACKET_TYPE_AUDIO_ENABLE_CNN_CMD,   // None
    QSPI_PACKET_TYPE_AUDIO_DISABLE_CNN_CMD,  // None

    QSPI_PACKET_TYPE_VIDEO_ENABLE_FLASH_LED_CMD,  // None
    QSPI_PACKET_TYPE_VIDEO_DISABLE_FLASH_LED_CMD, // None

    QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_5_MHZ_CMD, // None
    QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_10_MHZ_CMD, // None
    QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_15_MHZ_CMD, // None

    QSPI_PACKET_TYPE_VIDEO_BUTTON_PRESS_RES, // None
    QSPI_PACKET_TYPE_AUDIO_BUTTON_PRESS_RES, // None

    QSPI_PACKET_TYPE_VIDEO_ENABLE_VFLIP_CMD, // None
    QSPI_PACKET_TYPE_VIDEO_DISABLE_VFLIP_CMD, // None

    QSPI_PACKET_TYPE_VIDEO_DEBUG_CMD, // None
    QSPI_PACKET_TYPE_AUDIO_DEBUG_CMD, // None

    QSPI_PACKET_TYPE_TEST, // None
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS0,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS1,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS2,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS3,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS4,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS5,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS6,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS7,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS8,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS9,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS10,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS11,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS12,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS13,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS14,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS15,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS16,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS17,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS18,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS19,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS20,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS21,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS22,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS23,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS24,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS25,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS26,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS27,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS28,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS29,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS30,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS31,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS32,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS33,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS34,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS35,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS36,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS37,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS38,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS39,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS40,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS41,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS42,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS43,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS44,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS45,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS46,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS47,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS48,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS49,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS50,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS51,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS52,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS53,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS54,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS55,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS56,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS57,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS58,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS59,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS60,                
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS61,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS62,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS63,
    QSPI_PACKET_TYPE_WEIGHTS_KERNELS64,
    QSPI_PACKET_TYPE_VIDEO_RECORD_EN,
    QSPI_PACKET_TYPE_VIDEO_RECORD_DSB,
    QSPI_PACKET_TYPE_VIDEO_RECORD_MODE_FLAG,
    QSPI_PACKET_TYPE_VIDEO_WAIT,
    QSPI_PACKET_TYPE_VIDEO_CONTINUE,
    QSPI_PACKET_TYPE_VIDEO_RECORD_ACK,
    QSPI_PACKET_TYPE_VIDEO_CAPTURE_EN,
    QSPI_PACKET_TYPE_GETTING_NAME_EN,
    QSPI_PACKET_TYPE_GETTING_NAME_DSB,
    QSPI_PACKET_TYPE_VIDEO_EMBEDDING_VALUES,
    QSPI_PACKET_TYPE_VIDEO_ML_RES, // 80x80 ml result
    QSPI_PACKET_TYPE_VIDEO_SENDING_EMBEDDINGS_FLAG,
    QSPI_PACKET_TYPE_DOT_PRODUCT_WEIGHTS_0,
    QSPI_PACKET_TYPE_DOT_PRODUCT_WEIGHTS_1,
    QSPI_PACKET_TYPE_DOT_PRODUCT_WEIGHTS_2,
    QSPI_PACKET_TYPE_DOT_PRODUCT_NAMES,
    QSPI_PACKET_TYPE_VIDEO_CAPTURE_ACCEPT,
    QSPI_PACKET_TYPE_VIDEO_CAPTURE_DISCARD,
    QSPI_PACKET_TYPE_FACEDET_VIDEO_DATA_RES_FACEID,
    QSPI_PACKET_TYPE_FACEDET_VIDEO_DATA_BOX_DATA,
    QSPI_PACKET_TYPE_VIDEO_DEMO_NAME_CMD,      // None
    QSPI_PACKET_TYPE_VIDEO_DEMO_NAME_RES,      // Demo string

    QSPI_PACKET_TYPE_AUDIO_DEMO_NAME_CMD,      // None
    QSPI_PACKET_TYPE_AUDIO_DEMO_NAME_RES,      // Demo string

    QSPI_PACKET_TYPE_FACEDET_VIDEO_DATA_RES, // 224x168 RGB565 Image
    
	QSPI_PACKET_TYPE_AUDIO_LED_ON_CMD,   // None
	QSPI_PACKET_TYPE_AUDIO_LED_OFF_CMD,   // None

    QSPI_PACKET_TYPE_LAST
} qspi_packet_type_e;

// QSPI states
typedef enum {
    QSPI_STATE_IDLE = 0,

    QSPI_STATE_STARTED,
    QSPI_STATE_CS_ASSERTED_HEADER,
    QSPI_STATE_CS_DEASSERTED_HEADER,
    QSPI_STATE_CS_ASSERTED_DATA,
    QSPI_STATE_COMPLETED,

    QSPI_STATE_LAST
} qspi_state_e;

// BLE packet types
typedef enum {
    BLE_PACKET_TYPE_COMMAND = 0,
    BLE_PACKET_TYPE_PAYLOAD
} ble_packet_type_e;

// BLE command types
typedef enum {
//  Command                             Command Payload Description
    //// v0.4 commands

    // Communication
    BLE_COMMAND_ABORT_CMD = 0,           // None
    BLE_COMMAND_INVALID_RES,             // None
    BLE_COMMAND_NOP_CMD,                 // None
    BLE_COMMAND_MTU_CHANGE_RES,          // uint16_t MTU

    // Version
    BLE_COMMAND_GET_VERSION_CMD,         // None
    BLE_COMMAND_GET_VERSION_RES,         // device_version_t
    BLE_COMMAND_GET_SERIAL_NUM_CMD,      // None
    BLE_COMMAND_GET_SERIAL_NUM_RES,      // device_serial_num_t

    // SD Card
    BLE_COMMAND_GET_SD_INSERTED_CMD,     // None
    BLE_COMMAND_GET_SD_INSERTED_RES,     // sd_status_e
    BLE_COMMAND_WRITE_SD_FILE_CMD,       // File name size (uint8_t) + File name string + File content
    BLE_COMMAND_WRITE_SD_FILE_RES,       // sd_status_e
    BLE_COMMAND_READ_SD_FILE_CMD,        // File name string
    BLE_COMMAND_READ_SD_FILE_RES,        // sd_status_e + File content
    BLE_COMMAND_GET_SD_CONTENT_CMD,      // None
    BLE_COMMAND_GET_SD_CONTENT_RES,      // (file_info_header_t + File name string) * Number of files
    BLE_COMMAND_GET_SD_FREE_SPACE_CMD,   // None
    BLE_COMMAND_GET_SD_FREE_SPACE_RES,   // Free space size (uint32_t)
    BLE_COMMAND_DELETE_SD_FILE_CMD,      // File name string
    BLE_COMMAND_DELETE_SD_FILE_RES,      // sd_status_e
    BLE_COMMAND_FORMAT_SD_CMD,           // None
    BLE_COMMAND_FORMAT_SD_RES,           // sd_status_e

    // External Flash
    BLE_COMMAND_WRITE_EXT_FILE_CMD,      // File name size (uint8_t) + File name string + File content
    BLE_COMMAND_WRITE_EXT_FILE_RES,      // ext_status_e
    BLE_COMMAND_READ_EXT_FILE_CMD,       // File name string
    BLE_COMMAND_READ_EXT_FILE_RES,       // ext_status_e + File content
    BLE_COMMAND_GET_EXT_CONTENT_CMD,     // None
    BLE_COMMAND_GET_EXT_CONTENT_RES,     // (file_info_header_t + File name string) * Number of files
    BLE_COMMAND_GET_EXT_FREE_SPACE_CMD,  // None
    BLE_COMMAND_GET_EXT_FREE_SPACE_RES,  // Free space size (uint32_t)
    BLE_COMMAND_DELETE_EXT_FILE_CMD,     // File name string
    BLE_COMMAND_DELETE_EXT_FILE_RES,     // ext_status_e
    BLE_COMMAND_FORMAT_EXT_CMD,          // None
    BLE_COMMAND_FORMAT_EXT_RES,          // ext_status_e

    // Firmware Update from SD Card
    BLE_COMMAND_FW_UPDATE_MAX32666_SD_CMD,        // File name string
    BLE_COMMAND_FW_UPDATE_MAX32666_SD_RES,        // fw_update_status_e
    BLE_COMMAND_FW_UPDATE_MAX78000_SD_VIDEO_CMD,  // File name string
    BLE_COMMAND_FW_UPDATE_MAX78000_SD_VIDEO_RES,  // fw_update_status_e
    BLE_COMMAND_FW_UPDATE_MAX78000_SD_AUDIO_CMD,  // File name string
    BLE_COMMAND_FW_UPDATE_MAX78000_SD_AUDIO_RES,  // fw_update_status_e
    BLE_COMMAND_FW_UPDATE_COMBINED_SD_CMD,        // File name string
    BLE_COMMAND_FW_UPDATE_COMBINED_SD_RES,        // fw_update_status_e

    // Firmware Update from External Flash
    BLE_COMMAND_FW_UPDATE_MAX32666_EXT_CMD,       // File name string
    BLE_COMMAND_FW_UPDATE_MAX32666_EXT_RES,       // fw_update_status_e
    BLE_COMMAND_FW_UPDATE_MAX78000_EXT_VIDEO_CMD, // File name string
    BLE_COMMAND_FW_UPDATE_MAX78000_EXT_VIDEO_RES, // fw_update_status_e
    BLE_COMMAND_FW_UPDATE_MAX78000_EXT_AUDIO_CMD, // File name string
    BLE_COMMAND_FW_UPDATE_MAX78000_EXT_AUDIO_RES, // fw_update_status_e
    BLE_COMMAND_FW_UPDATE_COMBINED_EXT_CMD,       // File name string
    BLE_COMMAND_FW_UPDATE_COMBINED_EXT_RES,       // fw_update_status_e

    // FaceID Embeddings Update
    BLE_COMMAND_FACEID_EMBED_UPDATE_CMD,          // FaceID embeddings binary content
    BLE_COMMAND_FACEID_EMBED_UPDATE_RES,          // faceid_embed_update_status_e
    BLE_COMMAND_FACEID_EMBED_UPDATE_SD_CMD,       // File name string
    BLE_COMMAND_FACEID_EMBED_UPDATE_SD_RES,       // faceid_embed_update_status_e
    BLE_COMMAND_FACEID_EMBED_UPDATE_EXT_CMD,      // File name string
    BLE_COMMAND_FACEID_EMBED_UPDATE_EXT_RES,      // faceid_embed_update_status_e

    // Device Settings
    BLE_COMMAND_DISABLE_BLE_CMD,                      // None
    BLE_COMMAND_SHUT_DOWN_DEVICE_CMD,                 // None
    BLE_COMMAND_RESTART_DEVICE_CMD,                   // None
    BLE_COMMAND_ENABLE_MAX78000_AUDIO_CMD,            // None
    BLE_COMMAND_DISABLE_MAX78000_AUDIO_CMD,           // None
    BLE_COMMAND_ENABLE_MAX78000_VIDEO_CMD,            // None
    BLE_COMMAND_DISABLE_MAX78000_VIDEO_CMD,           // None
    BLE_COMMAND_ENABLE_MAX78000_VIDEO_CNN_CMD,        // None
    BLE_COMMAND_DISABLE_MAX78000_VIDEO_CNN_CMD,       // None
    BLE_COMMAND_ENABLE_MAX78000_AUDIO_CNN_CMD,        // None
    BLE_COMMAND_DISABLE_MAX78000_AUDIO_CNN_CMD,       // None
    BLE_COMMAND_ENABLE_MAX78000_VIDEO_FLASH_LED_CMD,  // None
    BLE_COMMAND_DISABLE_MAX78000_VIDEO_FLASH_LED_CMD, // None
    BLE_COMMAND_ENABLE_MAX78000_VIDEO_AUDIO_POWER,    // None
    BLE_COMMAND_DISABLE_MAX78000_VIDEO_AUDIO_POWER,   // None
    BLE_COMMAND_ENABLE_LCD_CMD,                       // None
    BLE_COMMAND_DISABLE_LCD_CMD,                      // None
    BLE_COMMAND_ENABLE_LCD_STATISCTICS_CMD,           // None
    BLE_COMMAND_DISABLE_LCD_STATISCTICS_CMD,          // None
    BLE_COMMAND_ENABLE_LCD_PROBABILITY_CMD,           // None
    BLE_COMMAND_DISABLE_LCD_PROBABILITY_CMD,          // None
    BLE_COMMAND_ENABLE_SEND_STATISTICS_CMD,           // None
    BLE_COMMAND_DISABLE_SEND_STATISTICS_CMD,          // None
    BLE_COMMAND_ENABLE_SEND_CLASSIFICATION_CMD,       // None
    BLE_COMMAND_DISABLE_SEND_CLASSIFICATION_CMD,      // None
    BLE_COMMAND_ENABLE_INACTIVITY_CMD,                // None
    BLE_COMMAND_DISABLE_INACTIVITY_CMD,               // None

    // Statistics
    BLE_COMMAND_GET_STATISTICS_RES,      // device_statistics_t

    // Classification
    BLE_COMMAND_GET_MAX78000_VIDEO_CLASSIFICATION_RES, // classification_result_t
    BLE_COMMAND_GET_MAX78000_AUDIO_CLASSIFICATION_RES, // classification_result_t

    // Debugger Selection
    BLE_COMMAND_SET_DEBUGGER_CMD,        // debugger_select_e


    //// v0.7 commands
    BLE_COMMAND_MAX78000_VIDEO_CAMERA_CLOCK_CMD,   // camera_clock_e


    //// v0.8 commands
    BLE_COMMAND_ENABLE_MAX78000_VIDEO_VFLIP_CMD,    // None
    BLE_COMMAND_DISABLE_MAX78000_VIDEO_VFLIP_CMD,   // None

    //// v1.1 commands
    BLE_COMMAND_GET_DEMO_NAME_CMD,         // None
    BLE_COMMAND_GET_DEMO_NAME_RES,         // Demo string

    BLE_COMMAND_LAST
} ble_command_e;

// SD card commands status codes
typedef enum {
    SD_STATUS_SUCCESS = 0,
    SD_STATUS_ERROR_DISK_ERR,
    SD_STATUS_ERROR_INT_ERR,
    SD_STATUS_ERROR_NOT_READY,
    SD_STATUS_ERROR_NO_FILE,
    SD_STATUS_ERROR_NO_PATH,
    SD_STATUS_ERROR_INVALID_NAME,
    SD_STATUS_ERROR_DENIED,
    SD_STATUS_ERROR_EXIST,
    SD_STATUS_ERROR_INVALID_OBJECT,
    SD_STATUS_ERROR_NOT_ENABLED,
    SD_STATUS_ERROR_NO_FILESYSTEM,
    SD_STATUS_ERROR_MKFS_ABORTED,
    SD_STATUS_ERROR_TIMEOUT,
    SD_STATUS_ERROR_LOCKED,
    SD_STATUS_ERROR_NOT_ENOUGH_CORE,
    SD_STATUS_ERROR_TOO_MANY_OPEN_FILES,
    SD_STATUS_ERROR_INVALID_PARAMETER,

    SD_STATUS_LAST
} sd_status_e;

// External flash commands status codes
typedef enum {
    EXT_STATUS_OK = 0,
    //TODO

    EXT_STATUS_LAST
} ext_status_e;

// Firmware update commands status codes
typedef enum {
    FW_UPDATE_STATUS_SUCCESS = 0,
    FW_UPDATE_STATUS_ERROR_NO_FILE,
    FW_UPDATE_STATUS_ERROR_MAX32666,
    FW_UPDATE_STATUS_ERROR_MAX78000_VIDEO,
    FW_UPDATE_STATUS_ERROR_MAX78000_AUDIO,
    FW_UPDATE_STATUS_ERROR_COMBINED,

    FW_UPDATE_STATUS_LAST
} fw_update_status_e;

// FaceID embedding update commands status codes
typedef enum {
    FACEID_EMBED_UPDATE_STATUS_SUCCESS = 0,
    FACEID_EMBED_UPDATE_STATUS_ERROR_NO_FILE,
    FACEID_EMBED_UPDATE_STATUS_ERROR_UNKNOWN,

    FACEID_EMBED_UPDATE_STATUS_LAST
} faceid_embed_update_status_e;

// Debugger select command types
typedef enum {
    DEBUGGER_SELECT_MAX32666_CORE1 = 0,
    DEBUGGER_SELECT_MAX78000_VIDEO,
    DEBUGGER_SELECT_MAX78000_AUDIO,

    DEBUGGER_SELECT_LAST
} debugger_select_e;

// Camera clock types
typedef enum {
    CAMERA_CLOCK_5_MHZ = 0,
    CAMERA_CLOCK_10_MHZ,
    CAMERA_CLOCK_15_MHZ,

    CAMERA_CLOCK_LAST
} camera_clock_e;

// Classification command response classification codes
typedef enum {
    CLASSIFICATION_NOTHING = 0,
    CLASSIFICATION_UNKNOWN,
    CLASSIFICATION_LOW_CONFIDENCE,
    CLASSIFICATION_DETECTED,

    CLASSIFICATION_LAST
} classification_e;

// LCD rotation
typedef enum {
    LCD_ROTATION_UP = 0,
    LCD_ROTATION_RIGHT,
    LCD_ROTATION_DOWN,
    LCD_ROTATION_LEFT,

    LCD_ROTATION_LAST
} lcd_rotation_e;

// Typedef Structs
// QSPI packet info
typedef struct __attribute__((packed)) {
    uint32_t packet_size;
    uint16_t packet_type;
} qspi_packet_header_info_t;

// QSPI packet header
typedef struct __attribute__((packed)) {
    uint32_t start_dummy;
    uint32_t start_symbol;
    uint16_t header_crc16;
    uint16_t payload_crc16;
    qspi_packet_header_info_t info;
    uint32_t stop_dummy;
} qspi_packet_header_t;

// BLE packet info field
#define BLE_PACKET_SEQ_MASK   0x7F
typedef struct __attribute__((packed)) {
    uint8_t type : 1;
    uint8_t seq  : 7;
} ble_packet_info_t;

// BLE command packet header
typedef struct __attribute__((packed)) {
    ble_packet_info_t packet_info;
    uint8_t command;
    uint32_t total_payload_size;
} ble_command_packet_header_t;

// BLE payload packet header
typedef struct __attribute__((packed)) {
    ble_packet_info_t packet_info;
} ble_payload_packet_header_t;

// BLE command packet
#define BLE_COMMAND_PACKET_MAX_PAYLOAD_SIZE    BLE_MAX_PACKET_SIZE - sizeof(ble_command_packet_header_t) //243
typedef struct __attribute__((packed)) {
    ble_command_packet_header_t header;
    uint8_t payload[BLE_COMMAND_PACKET_MAX_PAYLOAD_SIZE];
} ble_command_packet_t;

// BLE payload packet
#define BLE_PAYLOAD_PACKET_MAX_PAYLOAD_SIZE    BLE_MAX_PACKET_SIZE - sizeof(ble_payload_packet_header_t)
typedef struct __attribute__((packed)) {
    ble_payload_packet_header_t header;
    uint8_t payload[BLE_PAYLOAD_PACKET_MAX_PAYLOAD_SIZE];
} ble_payload_packet_t;

// BLE packet container
typedef struct __attribute__((packed)) {
    uint8_t size;
    union {
        ble_packet_info_t packet_info;
        ble_command_packet_t command_packet;
        ble_payload_packet_t payload_packet;
    } packet;
} ble_packet_container_t;

// Version info field
typedef struct __attribute__((packed)) {
    uint8_t major;
    uint8_t minor;
    uint32_t build;
} version_t;

// Version command response
typedef struct __attribute__((packed)) {
    version_t max32666;
    version_t max78000_video;
    version_t max78000_audio;
} device_version_t;

// Serial number command response
typedef uint8_t serial_num_t[13];
typedef struct __attribute__((packed)) {
    serial_num_t max32666;
    serial_num_t max78000_video;
    serial_num_t max78000_audio;
} device_serial_num_t;

// MAX78000 statistics field
typedef struct __attribute__((packed)) {
    uint32_t cnn_duration_us;
    uint32_t capture_duration_us;
    uint32_t communication_duration_us;
    uint32_t total_duration_us;
} max78000_statistics_t;

// Statistics command response
typedef struct __attribute__((packed)) {
    max78000_statistics_t max78000_video;
    max78000_statistics_t max78000_audio;
    float lcd_fps;
    uint8_t battery_soc;
    uint32_t max78000_video_power_mw;
    uint32_t max78000_audio_power_mw;
} device_statistics_t;

// Classification command response
typedef struct __attribute__((packed)) {
    float probability;
    classification_e classification;
    int max_embed_index;
    char result[CLASSIFICATION_STRING_SIZE];
} classification_result_t;

// File operation commands file info field
typedef struct __attribute__((packed)) {
    uint8_t file_name_size;
    uint32_t file_size;
    // char [] file_name
} file_info_header_t;

#endif /* _MAXREFDES178_DEFINTIIONS_H_ */
