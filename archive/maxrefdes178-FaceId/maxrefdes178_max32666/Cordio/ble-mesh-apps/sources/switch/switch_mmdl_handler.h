/*************************************************************************************************/
/*!
 *  \file   switch_mmdl_handler.h
 *
 *  \brief  Mesh Model Handler for the Switch App.
 *
 *  Copyright (c) 2015-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#ifndef SWITCH_MMDL_HANDLER_H
#define SWITCH_MMDL_HANDLER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief     WSF event handler for Mesh Models used by the Switch application.
 *
 *  \param[in] event  WSF event mask.
 *  \param[in] pMsg   WSF message.
 *
 *  \return    None.
 */
/*************************************************************************************************/
void SwitchMmdlHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

#ifdef __cplusplus
}
#endif

#endif /* SWITCH_MMDL_HANDLER_H */
