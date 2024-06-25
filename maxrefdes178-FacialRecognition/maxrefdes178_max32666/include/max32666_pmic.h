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

#ifndef _MAX32666_PMIC_H_
#define _MAX32666_PMIC_H_


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function declarations
//-----------------------------------------------------------------------------
/**
 * @brief  Initialize MAX20303 PMIC. Before calling this function, I2C should
 * be initialized via i2c_init() function.
 * @retval     #E_NO_ERROR   Initialization successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_init(void);

/**
 * @brief  Worker function for MAX20303 PMIC. Before calling this function, I2C should
 * be initialized via i2c_init() function.
 * @retval     #E_NO_ERROR   Initialization successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_worker(void);

/**
 * @brief  Alert interrupt handler function for MAX20303 PMIC. Before calling this function,
 * I2C should be initialized via i2c_init() function.
 * @param    state  Interrupt pin state.
 * @retval   None
 */
void pmic_alert_int_handler(int state);

/**
 * @brief  Interrupt handler function for MAX20303 PMIC. Before calling this function, I2C should
 * be initialized via i2c_init() function.
 * @param    state  Interrupt pin state.
 * @retval   None
 */
void pmic_int_handler(int state);

/**
 * @brief  Setup for LDO1 of MAX20303 PMIC to enable or disable MAX78000 board 1.8V digital supply.
 * LDO1 is configured as load-switch. Before calling this function, PMIC should be initialized via
 * max20303_initialize() function
 * @param    on  Non-zero value configures LDO1 as load-switch but configures the state as "ON".
 * Value 0 still configures LDO1 as load-switch and sets the state as "OFF.
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_ldo1(int on);

/**
 * @brief  Setup for LDO2 of MAX20303 PMIC to enable or disable MAX78000 board 3.3V digital supply.
 * LDO2 is configured as an LDO. Before calling this function, PMIC should be initialized via
 * max20303_initialize() function
 * @param    on  Non-zero value configures LDO1 as load-switch but configures the state as "ON".
 * Value 0 still configures LDO2 as an LDO and sets the state as "OFF.
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_ldo2(int on);

/**
 * @brief  Setup for Buck1 of MAX20303 PMIC to enable or disable MAX78000 board 1.8V digital supply.
 * Before calling this function, PMIC should be initialized via max20303_initialize() function
 * @param  on   Non-zero value configures Buck1 voltage as 1.8V but configures the state as "ON".
 * Value 0 still configures LDO1 as load-switch and sets the state as "OFF.
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_buck1(int on);

/**
 * @brief  Setup for Buck2 of MAX20303 PMIC to enable or disable MAX78000 board 2.8V digital supply.
 * Before calling this function, PMIC should be initialized via max20303_initialize() function
 * @param  on    Non-zero value configures Buck2 voltage to 2.8V and sets the state as "ON".
 * Value 0 still configures Buck2 voltage as 2.8V but sets the state as "OFF"
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_buck2(int on);

/**
 * @brief  Setup for Boost Converter of MAX20303 PMIC to enable or disable LCD backlight.
 * Before calling this function, PMIC should be initialized via max20303_initialize() function
 * @param  on   Non-zero value enables boost converter
 * Value 0 still configures Boost voltage as 9.6V but configures the state as "OFF"
 * @param  boost_output_level Sets the voltage output level of the boost converter. Please
 * refer to MAX20303 PMIC datasheet for details.
 *
 * @returns    This function has no return value
 */
int pmic_boost(int on, uint8_t boost_output_level);

/**
 * @brief  Power off MAX20303.
 * Before calling this function, PMIC should be initialized via max20303_initialize() function
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_power_off(void);

/**
 * @brief  Hard reset MAX20303.
 * Before calling this function, PMIC should be initialized via max20303_initialize() function
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_hard_reset(void);

/**
 * @brief  Setup for BuckBoost Converter of MAX20303 PMIC.
 * Before calling this function, PMIC should be initialized via max20303_initialize() function
 * @param  on   Non-zero value enables buck-boost converter
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_buckboost(int on);

/**
 * @brief   Controls PMIC's RGB LED - Red Color. Before calling this function, PMIC should
 * be initialized via max20303_initialize() function
 * @param      on value 0 turns off the Red LED, non-zero value turns on the Red LED
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_led_red(int on);

/**
 * @brief   Controls PMIC's RGB LED - Green Color. Before calling this function, PMIC should
 * be initialized via max20303_initialize() function
 * @param      on value 0 turns off the Green LED, non-zero value turns on the Red LED
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_led_green(int on);

/**
 * @brief   Controls PMIC's RGB LED - Blue Color. Before calling this function, PMIC should
 * be initialized via max20303_initialize() function
 * @param      on value 0 turns off the Blue LED, non-zero value turns on the Red LED
 * @retval     #E_NO_ERROR   If successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int pmic_led_blue(int on);

#endif /* _MAX32666_PMIC_H_ */
