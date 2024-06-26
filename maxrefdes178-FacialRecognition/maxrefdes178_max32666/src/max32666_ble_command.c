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
#include <core1.h>
#include <string.h>

#include "max32666_ble_command.h"
#include "max32666_ble_queue.h"
#include "max32666_data.h"
#include "max32666_debug.h"
#include "max32666_expander.h"
#include "max32666_fonts.h"
#include "max32666_lcd.h"
#include "max32666_qspi_master.h"
#include "max32666_pmic.h"
#include "max32666_timer_led_button.h"
#include "maxrefdes178_definitions.h"


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define S_MODULE_NAME   "comm"


//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef enum {
    BLE_COMMAND_STATE_IDLE = 0,
    BLE_COMMAND_STATE_RX_RUNNING,
    BLE_COMMAND_STATE_TX_RUNNING,

    BLE_COMMAND_STATE_LAST
} ble_command_state_e;

typedef struct {
    uint8_t command;
    ble_command_state_e command_state;
    uint32_t received_payload_size;
    uint32_t transmitted_payload_size;
    uint32_t total_payload_size;
    uint8_t total_payload_buffer[MAX32666_BLE_COMMAND_BUFFER_SIZE];
} ble_command_buffer_t;


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
static ble_packet_container_t tmp_container;
static ble_command_buffer_t ble_command_buffer;


//-----------------------------------------------------------------------------
// Local function declarations
//-----------------------------------------------------------------------------
static int ble_command_handle_rx(void);
//static int ble_command_handle_tx(void);


//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
// TODO: handle small MTU, multiple packet cases
int ble_command_send_single_packet(ble_command_e ble_command, uint32_t payload_size, uint8_t *payload)
{
    int ret;

    if (ble_command_buffer.command_state != BLE_COMMAND_STATE_IDLE) {
        PR_ERROR("command state is not idle");
        return E_BAD_STATE;
    }

    if (payload_size + sizeof(ble_command_packet_header_t) > device_status.ble_max_packet_size) {
        PR_ERROR("command is bigger than ble max packet size %d %d",
                payload_size + sizeof(ble_command_packet_header_t), device_status.ble_max_packet_size);
        return E_BAD_PARAM;
    }

    if (payload_size > sizeof(tmp_container.packet.command_packet.payload)) {
        PR_ERROR("invalid command %d payload size %d", ble_command, payload_size);
        return E_BAD_PARAM;
    }
    tmp_container.packet.command_packet.header.packet_info.type = BLE_PACKET_TYPE_COMMAND;
    tmp_container.packet.command_packet.header.packet_info.seq = device_status.ble_next_tx_seq;
    tmp_container.packet.command_packet.header.command = ble_command;
    tmp_container.packet.command_packet.header.total_payload_size = payload_size;
    memcpy(tmp_container.packet.command_packet.payload, payload, payload_size);
    tmp_container.size = payload_size + sizeof(ble_command_packet_header_t);

    ret = ble_queue_enq_tx(&tmp_container);
    if (ret != E_SUCCESS) {
        PR_ERROR("ble_queue_enq_tx failed %d", ret);
        return ret;
    }

    return ret;
}

static int ble_command_execute_rx_command(void)
{
    PR_INFO("exec %d %d", ble_command_buffer.command,
            ble_command_buffer.total_payload_size);
//    for (int i = 0; i < ble_command_buffer.total_payload_size; i++) {
//        PR("%02hhX ", ble_command_buffer.total_payload_buffer[i]);
//    }
//    PR("\n");

    switch (ble_command_buffer.command) {
    case BLE_COMMAND_GET_VERSION_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        ble_command_send_single_packet(BLE_COMMAND_GET_VERSION_RES,
            sizeof(device_info.device_version), (uint8_t *) &device_info.device_version);
        break;
    case BLE_COMMAND_GET_SERIAL_NUM_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        ble_command_send_single_packet(BLE_COMMAND_GET_SERIAL_NUM_RES,
            sizeof(device_info.device_serial_num), (uint8_t *) &device_info.device_serial_num);
        break;
    case BLE_COMMAND_FACEID_EMBED_UPDATE_CMD:
        if ((ble_command_buffer.total_payload_size < FACEID_EMBEDDING_SIZE) ||
            (ble_command_buffer.total_payload_size > FACEID_MAX_EMBEDDINGS_SIZE)) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(ble_command_buffer.total_payload_buffer, ble_command_buffer.total_payload_size,
                QSPI_PACKET_TYPE_VIDEO_FACEID_EMBED_UPDATE_CMD);
        break;
    case BLE_COMMAND_DISABLE_BLE_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_ble = 0;
        lcd_notification(BLUE, "BLE disabled");
        break;
    case BLE_COMMAND_SHUT_DOWN_DEVICE_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        PR_INFO("shut down device");
        device_settings.enable_ble = 0;
        for (int i = 0; (i < 10000000) && !device_status.ble_running_status_changed; i++) {}
        Core1_Stop();
        pmic_power_off();
        break;
    case BLE_COMMAND_RESTART_DEVICE_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        PR_INFO("restart device");
        device_settings.enable_ble = 0;
        for (int i = 0; (i < 10000000) && !device_status.ble_running_status_changed; i++) {}
        Core1_Stop();
        pmic_hard_reset();
        break;
    case BLE_COMMAND_ENABLE_MAX78000_AUDIO_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_audio(NULL, 0, QSPI_PACKET_TYPE_AUDIO_ENABLE_CMD);
        device_settings.enable_max78000_audio = 1;
        lcd_notification(MAGENTA, "Audio enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_AUDIO_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_audio(NULL, 0, QSPI_PACKET_TYPE_AUDIO_DISABLE_CMD);
        device_settings.enable_max78000_audio = 0;
        lcd_notification(MAGENTA, "Audio disabled");
        break;
    case BLE_COMMAND_ENABLE_MAX78000_VIDEO_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_ENABLE_CMD);
        device_settings.enable_max78000_video = 1;
        lcd_notification(MAGENTA, "Video enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_VIDEO_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_DISABLE_CMD);
        device_settings.enable_max78000_video = 0;
        lcd_notification(MAGENTA, "Video disabled");
        break;
    case BLE_COMMAND_ENABLE_MAX78000_VIDEO_CNN_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_ENABLE_CNN_CMD);
        device_settings.enable_max78000_video_cnn = 1;
        lcd_notification(MAGENTA, "FaceID enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_VIDEO_CNN_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_DISABLE_CNN_CMD);
        device_settings.enable_max78000_video_cnn = 0;
        lcd_notification(MAGENTA, "FaceID disabled");
        break;
    case BLE_COMMAND_MAX78000_VIDEO_CAMERA_CLOCK_CMD:
        if (ble_command_buffer.total_payload_size != 1) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }

        switch(ble_command_buffer.total_payload_buffer[0]) {
        case CAMERA_CLOCK_5_MHZ:
            qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_5_MHZ_CMD);
            lcd_notification(MAGENTA, "Camera clock 5 MHz");
            break;
        case CAMERA_CLOCK_10_MHZ:
            qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_10_MHZ_CMD);
            lcd_notification(MAGENTA, "Camera clock 10 MHz");
            break;
        case CAMERA_CLOCK_15_MHZ:
            qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_CAMERA_CLOCK_15_MHZ_CMD);
            lcd_notification(MAGENTA, "Camera clock 15 MHz");
            break;
        }

        break;
    case BLE_COMMAND_ENABLE_MAX78000_AUDIO_CNN_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_audio(NULL, 0, QSPI_PACKET_TYPE_AUDIO_ENABLE_CNN_CMD);
        device_settings.enable_max78000_audio_cnn = 1;
        lcd_notification(MAGENTA, "Audio CNN enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_AUDIO_CNN_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_audio(NULL, 0, QSPI_PACKET_TYPE_AUDIO_DISABLE_CNN_CMD);
        device_settings.enable_max78000_audio_cnn = 0;
        lcd_notification(MAGENTA, "Audio CNN disabled");
        break;
    case BLE_COMMAND_ENABLE_MAX78000_VIDEO_FLASH_LED_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_ENABLE_FLASH_LED_CMD);
        device_settings.enable_max78000_video_flash_led = 1;
        lcd_notification(MAGENTA, "Video flash LED enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_VIDEO_FLASH_LED_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_DISABLE_FLASH_LED_CMD);
        device_settings.enable_max78000_video_flash_led = 0;
        lcd_notification(MAGENTA, "Video flash LED disabled");
        break;
    case BLE_COMMAND_ENABLE_MAX78000_VIDEO_AUDIO_POWER:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        pmic_ldo1(1);
        pmic_buck2(1);
        device_settings.enable_max78000_video_and_audio_power = 1;
        lcd_notification(MAGENTA, "Video Audio power enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_VIDEO_AUDIO_POWER:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        pmic_buck2(0);
        pmic_ldo1(0);
        device_settings.enable_max78000_video_and_audio_power = 0;
        lcd_notification(MAGENTA, "Video Audio power disabled");
        break;
    case BLE_COMMAND_ENABLE_LCD_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        if (device_settings.enable_max78000_video) {
            qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_ENABLE_CMD);
        }
        lcd_backlight(1, MAX32666_LCD_BACKLIGHT_HIGH);
        device_settings.enable_lcd = 1;
        break;
    case BLE_COMMAND_DISABLE_LCD_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        lcd_backlight(0, 0);
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_DISABLE_CMD);
        device_settings.enable_lcd = 0;
        break;
    case BLE_COMMAND_ENABLE_LCD_STATISCTICS_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_lcd_statistics = 1;
        timestamps.faceid_subject_names_received = timer_ms_tick;
        lcd_notification(MAGENTA, "LCD statistics enabled");
        break;
    case BLE_COMMAND_DISABLE_LCD_STATISCTICS_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_lcd_statistics = 0;
        lcd_notification(MAGENTA, "LCD statistics disabled");
        break;
    case BLE_COMMAND_ENABLE_LCD_PROBABILITY_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_lcd_probabilty = 1;
        lcd_notification(MAGENTA, "LCD probability enabled");
        break;
    case BLE_COMMAND_DISABLE_LCD_PROBABILITY_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_lcd_probabilty = 0;
        lcd_notification(MAGENTA, "LCD probability disabled");
        break;
    case BLE_COMMAND_ENABLE_SEND_STATISTICS_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_ble_send_statistics = 1;
        break;
    case BLE_COMMAND_DISABLE_SEND_STATISTICS_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_ble_send_statistics = 0;
        break;
    case BLE_COMMAND_ENABLE_SEND_CLASSIFICATION_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_ble_send_classification = 1;
        break;
    case BLE_COMMAND_DISABLE_SEND_CLASSIFICATION_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_ble_send_classification = 0;
        break;
    case BLE_COMMAND_ENABLE_INACTIVITY_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_inactivity = 1;
        lcd_notification(MAGENTA, "Inactivity timer enabled");
        break;
    case BLE_COMMAND_DISABLE_INACTIVITY_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        device_settings.enable_inactivity = 0;
        lcd_notification(MAGENTA, "Inactivity timer disabled");
        break;
    case BLE_COMMAND_ENABLE_MAX78000_VIDEO_VFLIP_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_ENABLE_VFLIP_CMD);
        device_settings.enable_max78000_video_vflip = 1;
        lcd_notification(MAGENTA, "Vertical flip enabled");
        break;
    case BLE_COMMAND_DISABLE_MAX78000_VIDEO_VFLIP_CMD:
        if (ble_command_buffer.total_payload_size != 0) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        qspi_master_send_video(NULL, 0, QSPI_PACKET_TYPE_VIDEO_DISABLE_VFLIP_CMD);
        device_settings.enable_max78000_video_vflip = 0;
        lcd_notification(MAGENTA, "Vertical flip disabled");
        break;
    case BLE_COMMAND_SET_DEBUGGER_CMD:
        if (ble_command_buffer.total_payload_size != 1) {
            PR_ERROR("invalid total payload size %d", ble_command_buffer.total_payload_size);
            return E_BAD_PARAM;
        }
        expander_select_debugger((debugger_select_e)ble_command_buffer.total_payload_buffer[0]);
        break;
    default:
        PR_ERROR("Unknown command");
        break;
    }

    timestamps.activity_detected = timer_ms_tick;

    return E_SUCCESS;
}

static int ble_command_handle_rx(void)
{
    uint8_t packet_payload_size;

    // BLE RX, check new packet on ble rx queue
    if (ble_queue_deq_rx(&tmp_container) != E_SUCCESS) {
        return E_NO_ERROR;
    }

    // Check sequence number of the packet
    if (device_status.ble_expected_rx_seq != tmp_container.packet.packet_info.seq) {
//        PR_ERROR("Incorrect seq expected %d received %d", device_status.ble_expected_rx_seq,
//                tmp_container.packet.packet_info.seq);
//        return E_BAD_STATE;
    }

//    PR_INFO("BLE RX %d", tmp_container.size);
    if (tmp_container.packet.packet_info.type == BLE_PACKET_TYPE_COMMAND) {
        packet_payload_size = tmp_container.size - sizeof(ble_command_packet_header_t);
        PR_INFO("C %d %d", tmp_container.packet.command_packet.header.command,
                tmp_container.packet.command_packet.header.total_payload_size);
//        PR_INFO("Payload: %d", packet_payload_size);
//        for (int i = 0; i < packet_payload_size; i++) {
//            PR("%02hhX ", tmp_container.packet.command_packet.payload[i]);
//        }
//        PR("\n");

        // Check abort command
        if (tmp_container.packet.command_packet.header.command == BLE_COMMAND_ABORT_CMD) {
            PR_INFO("reset command state");
            ble_command_reset();
            ble_queue_flush();
            return E_SUCCESS;
        }

        // Check state
        if (ble_command_buffer.command_state != BLE_COMMAND_STATE_IDLE) {
            PR_ERROR("command packet is not expected");
            return E_BAD_STATE;
        }

        // Check size
        if (packet_payload_size > tmp_container.packet.command_packet.header.total_payload_size) {
            PR_ERROR("packet payload size is larger than total payload size");
            return E_BAD_PARAM;
        }

        // TODO: handle big files
        // Check max size
        if (tmp_container.packet.command_packet.header.total_payload_size > MAX32666_BLE_COMMAND_BUFFER_SIZE) {
            PR_ERROR("total payload size is too big");
            return E_BAD_PARAM;
        }

        ble_command_buffer.command = tmp_container.packet.command_packet.header.command;
        ble_command_buffer.total_payload_size = tmp_container.packet.command_packet.header.total_payload_size;
        ble_command_buffer.received_payload_size = packet_payload_size;
        ble_command_buffer.command_state = BLE_COMMAND_STATE_RX_RUNNING;
        memcpy(ble_command_buffer.total_payload_buffer, tmp_container.packet.command_packet.payload,
                packet_payload_size);

        // Check if single packet command
        if (ble_command_buffer.total_payload_size <= ble_command_buffer.received_payload_size) {
            ble_command_buffer.command_state = BLE_COMMAND_STATE_IDLE;
            ble_command_execute_rx_command();
        }

        return E_SUCCESS;
    } else if (tmp_container.packet.packet_info.type == BLE_PACKET_TYPE_PAYLOAD) {
        packet_payload_size = tmp_container.size - sizeof(ble_payload_packet_header_t);
        PR_INFO("P %d (%d/%d): ", packet_payload_size, ble_command_buffer.received_payload_size,
                ble_command_buffer.total_payload_size);
//        for (int i = 0; i < packet_payload_size; i++) {
//            PR("%02hhX ", tmp_container.packet.payload_packet.payload[i]);
//        }
//        PR("\n");

        // Check state
        if (ble_command_buffer.command_state != BLE_COMMAND_STATE_RX_RUNNING) {
            PR_ERROR("payload packet is not expected");
            return E_BAD_STATE;
        }

        if (ble_command_buffer.received_payload_size + packet_payload_size > MAX32666_BLE_COMMAND_BUFFER_SIZE) {
            PR_ERROR("payload overflow");
            return E_OVERFLOW;
        }

        memcpy(&ble_command_buffer.total_payload_buffer[ble_command_buffer.received_payload_size],
                tmp_container.packet.payload_packet.payload, packet_payload_size);

        ble_command_buffer.received_payload_size += packet_payload_size;

        // Check if payload receive is completed
        if (ble_command_buffer.total_payload_size <= ble_command_buffer.received_payload_size) {
            ble_command_buffer.command_state = BLE_COMMAND_STATE_IDLE;
            ble_command_execute_rx_command();
        }

        return E_SUCCESS;
    }

    return E_SUCCESS;
}

//static int ble_command_handle_tx(void)
//{
//    // BLE TX, check new packet to enqueue ble tx queue
//    if (ble_command_buffer.command_state != BLE_COMMAND_STATE_TX_RUNNING) {
//        return E_NO_ERROR;
//    }
//    // TODO
//
//    return E_SUCCESS;
//}

int ble_command_reset(void)
{
    ble_command_buffer.command_state = BLE_COMMAND_STATE_IDLE;

    // TODO close open files

    return E_NO_ERROR;
}

int ble_command_worker(void)
{
    ble_command_handle_rx();
//    ble_command_handle_tx();

    return E_SUCCESS;
}

int ble_command_init(void)
{
    ble_command_reset();

    return E_NO_ERROR;
}
