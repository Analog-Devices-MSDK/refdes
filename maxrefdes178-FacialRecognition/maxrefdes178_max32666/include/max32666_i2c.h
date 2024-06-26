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

#ifndef _MAX32666_I2C_H_
#define _MAX32666_I2C_H_


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <i2c_regs.h>


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function declarations
//-----------------------------------------------------------------------------

/**
 * @brief   Writes a single I2C byte. Before calling this function, I2C should
 * be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      value is the value to be written
 * @returns    This function has no return value.
 */
int i2c_master_byte_write(uint8_t addr, uint8_t val);

/**
 * @brief   Writes a single I2C register. Before calling this function, I2C should
 * be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      reg is the offset of the register
 * @param      value is the register value to be written
 * @returns    This function has no return value.
 */
int i2c_master_reg_write(uint8_t addr, uint8_t reg, uint8_t val);

/**
 * @brief   Writes len sequential bytes from buf starting at reg. Before calling
 * this function, I2C should be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      reg is the offset of the register
 * @param      buf is the buffer to be written
 * @param      len is size of the buffer
 * @returns    This function has no return value.
 */
int i2c_master_reg_write_buf(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len);

/**
 * @brief   Read a single I2C byte. Before calling this function, I2C should
 * be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      buf is the register value to be read
 * @returns    This function has no return value.
 */
int i2c_master_byte_read(uint8_t addr, uint8_t *buf);

/**
 * @brief   Read a single I2C byte. Before calling this function, I2C should
 * be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      buf is the register value to be read
 * @param      len is size of the buffer
 * @returns    This function has no return value.
 */
int i2c_master_byte_read_buf(uint8_t addr, uint8_t *buf, uint8_t len);

/**
 * @brief   Read a single I2C register. Before calling this function, I2C should
 * be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      reg is the offset of the register
 * @param      buf is the register value to be read
 * @returns    This function has no return value.
 */
int i2c_master_reg_read(uint8_t addr, uint8_t reg, uint8_t *buf);

/**
 * @brief   Read a multiple I2C register. Before calling this function, I2C should
 * be initialized via i2c_init() function
 * @param      addr is the address of the I2C slave
 * @param      reg is the offset of the register
 * @param      value is the register value to be read
 * @param      len is size of the buffer
 * @param      buf is the register value to be read
 * @returns    This function has no return value.
 */
int i2c_master_reg_read_buf(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);

/**
 * @brief  Initialize I2C peripheral
 * @retval     #E_NO_ERROR   Initialization successful.
 * @retval     "Error Code"  @ref MXC_Error_Codes "Error Code" if unsuccessful.
 */
int i2c_master_init(void);

#endif /* _MAX32666_I2C_H_ */
