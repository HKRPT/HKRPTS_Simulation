/*
 * CAN module object for generic microcontroller.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author      Janez Paternoster
 * @copyright   2004 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
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
 */


#include "CO_driver.h"
#include "CO_Emergency.h"
#include "my_can.h"
#include "f28x_project.h"
/******************************************************************************/
void CO_CANsetConfigurationMode(void *CANdriverState){
    /* Put CAN module in configuration mode */
}


/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule){
    /* Put CAN module in normal mode */

    CANmodule->CANnormal = true;
}


/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        void                   *CANdriverState,
        CO_CANrx_t              rxArray[],
        uint16_t                rxSize,
        CO_CANtx_t              txArray[],
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
    uint16_t i;

    /* verify arguments */
    if(CANmodule==NULL || rxArray==NULL || txArray==NULL){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Configure object variables */
    CANmodule->CANdriverState = CANdriverState;
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->CANnormal = false;
    CANmodule->useCANrxFilters = (rxSize <= 32U) ? true : false;/* microcontroller dependent */
    CANmodule->bufferInhibitFlag = false;
    CANmodule->firstCANtxMessage = true;
    CANmodule->CANtxCount = 0U;
    CANmodule->errOld = 0U;
    CANmodule->em = NULL;

    for(i=0U; i<rxSize; i++){
        rxArray[i].ident = 0U;
        rxArray[i].mask = (uint16_t)0xFFFFFFFFU;
        rxArray[i].object = NULL;
        rxArray[i].pFunct = NULL;
    }
    for(i=0U; i<txSize; i++){
        txArray[i].bufferFull = false;
    }


    /* Configure CAN module registers */
    CAN_Init();

    /* Configure CAN timing */


    /* Configure CAN module hardware filters */
    if(CANmodule->useCANrxFilters){
        /* CAN module filters are used, they will be configured with */
        /* CO_CANrxBufferInit() functions, called by separate CANopen */
        /* init functions. */
        /* Configure all masks so, that received message must match filter */
    }
    else{
        /* CAN module filters are not used, all messages with standard 11-bit */
        /* identifier will be received */
        /* Configure mask 0 so, that all messages with standard identifier are accepted */
    }


    /* configure CAN interrupt registers */


    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule){
    /* turn off the module */
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(const CO_CANrxMsg_t *rxMsg){
    return (uint16_t) rxMsg->ident;
}


/******************************************************************************/
CO_ReturnError_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        bool_t                  rtr,
        void                   *object,
        void                  (*pFunct)(void *object, const CO_CANrxMsg_t *message))
{
    CO_ReturnError_t ret = CO_ERROR_NO;

    if((CANmodule!=NULL) && (object!=NULL) && (pFunct!=NULL) && (index < CANmodule->rxSize)){
        /* buffer, which will be configured */
        CO_CANrx_t *buffer = &CANmodule->rxArray[index];

        /* Configure object variables */
        buffer->object = object;
        buffer->pFunct = pFunct;

        /* CAN identifier and CAN mask, bit aligned with CAN module. Different on different microcontrollers. */
        buffer->ident = ident & 0x07FFU;
        if(rtr){
            buffer->ident |= 0x0800U;
        }
        buffer->mask = (mask & 0x07FFU) | 0x0800U;

        /* Set CAN hardware module filter and mask. */
        if(CANmodule->useCANrxFilters){

        }
    }
    else{
        ret = CO_ERROR_ILLEGAL_ARGUMENT;
    }

    return ret;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        bool_t                  rtr,
        uint8_t                 noOfBytes,
        bool_t                  syncFlag)
{
    CO_CANtx_t *buffer = NULL;

    if((CANmodule != NULL) && (index < CANmodule->txSize)){
        /* get specific buffer */
        buffer = &CANmodule->txArray[index];

        /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer.
         * Microcontroller specific. */
        buffer->ident = ((uint32_t)ident & 0x07FFU)
                      | ((uint32_t)(((uint32_t)noOfBytes & 0xFU) << 12U))
                      | ((uint32_t)(rtr ? 0x8000U : 0U));

        buffer->bufferFull = false;
        buffer->syncFlag = syncFlag;
    }

    return buffer;
}

// 发送单个消息帧
uint8_t My_CAN_SendSingleFrame(CO_CANtx_t *buffer)
{

    CAN_TX_CMD_FrameFunc = NULL;    // 优先级最高操作

    CAN_TX_CMD_FrameFuncID = buffer->ident & 0xFFF;

    CAN_TX_CMD_FrameFuncTXBuff[0] = buffer->data[0];
    CAN_TX_CMD_FrameFuncTXBuff[1] = buffer->data[1];
    CAN_TX_CMD_FrameFuncTXBuff[2] = buffer->data[2];
    CAN_TX_CMD_FrameFuncTXBuff[3] = buffer->data[3];
    CAN_TX_CMD_FrameFuncTXBuff[4] = buffer->data[4];
    CAN_TX_CMD_FrameFuncTXBuff[5] = buffer->data[5];
    CAN_TX_CMD_FrameFuncTXBuff[6] = buffer->data[6];
    CAN_TX_CMD_FrameFuncTXBuff[7] = buffer->data[7];


    return My_CAN_SendCANopenSingleFrame(0,CAN_TX_CMD_FrameFuncID,(buffer->ident >> 12)&0xFF,CAN_TX_CMD_FrameFuncTXBuff);
}

/******************************************************************************/
CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer){
    CO_ReturnError_t err = CO_ERROR_NO;

    /* Verify overflow */
    if(buffer->bufferFull){
        if(!CANmodule->firstCANtxMessage){
            /* don't set error, if bootup message is still on buffers */
            CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_CAN_TX_OVERFLOW, CO_EMC_CAN_OVERRUN, buffer->ident);
        }
        err = CO_ERROR_TX_OVERFLOW;
    }

    CO_LOCK_CAN_SEND();
    bool status = My_CAN_SendSingleFrame(buffer);
    /* if CAN TX buffer is free, copy message to it */
    if(status == false){
        CANmodule->bufferInhibitFlag = buffer->syncFlag;
        /* copy message and txRequest */
    }
    /* if no buffer is free, message will be sent by interrupt */
    else{
        buffer->bufferFull = true;
        CANmodule->CANtxCount++;
    }
    CO_UNLOCK_CAN_SEND();

    return err;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){
    uint32_t tpdoDeleted = 0U;

    CO_LOCK_CAN_SEND();
    /* Abort message from CAN module, if there is synchronous TPDO.
     * Take special care with this functionality. */
    if(/*messageIsOnCanBuffer && */CANmodule->bufferInhibitFlag){
        /* clear TXREQ */
        CANmodule->bufferInhibitFlag = false;
        tpdoDeleted = 1U;
    }
    /* delete also pending synchronous TPDOs in TX buffers */
    if(CANmodule->CANtxCount != 0U){
        uint16_t i;
        CO_CANtx_t *buffer = &CANmodule->txArray[0];
        for(i = CANmodule->txSize; i > 0U; i--){
            if(buffer->bufferFull){
                if(buffer->syncFlag){
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;
                    tpdoDeleted = 2U;
                }
            }
            buffer++;
        }
    }
    CO_UNLOCK_CAN_SEND();


    if(tpdoDeleted != 0U){
        CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_TPDO_OUTSIDE_WINDOW, CO_EMC_COMMUNICATION, tpdoDeleted);
    }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
    uint16_t rxErrors, txErrors, overflow;
    CO_EM_t* em = (CO_EM_t*)CANmodule->em;
    uint32_t err;
    uint8_t port = 0;

    /* get error counters from module. Id possible, function may use different way to
     * determine errors. */
    rxErrors = CAN_R[port]->CAN_ERRC.bit.REC;
    txErrors = CAN_R[port]->CAN_ERRC.bit.TEC;
    overflow = 0;

    err = ((uint32_t)txErrors << 16) | ((uint32_t)rxErrors << 8) | overflow;

    if(CANmodule->errOld != err){
        CANmodule->errOld = err;

        if(txErrors >= 256U){                               /* bus off */
            CO_errorReport(em, CO_EM_CAN_TX_BUS_OFF, CO_EMC_BUS_OFF_RECOVERED, err);
        }
        else{                                               /* not bus off */
            CO_errorReset(em, CO_EM_CAN_TX_BUS_OFF, err);

            if((rxErrors >= 96U) || (txErrors >= 96U)){     /* bus warning */
                CO_errorReport(em, CO_EM_CAN_BUS_WARNING, CO_EMC_NO_ERROR, err);
            }

            if(rxErrors >= 128U){                           /* RX bus passive */
                CO_errorReport(em, CO_EM_CAN_RX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
            }
            else{
                CO_errorReset(em, CO_EM_CAN_RX_BUS_PASSIVE, err);
            }

            if(txErrors >= 128U){                           /* TX bus passive */
                if(!CANmodule->firstCANtxMessage){
                    CO_errorReport(em, CO_EM_CAN_TX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
                }
            }
            else{
                bool_t isError = CO_isError(em, CO_EM_CAN_TX_BUS_PASSIVE);
                if(isError){
                    CO_errorReset(em, CO_EM_CAN_TX_BUS_PASSIVE, err);
                    CO_errorReset(em, CO_EM_CAN_TX_OVERFLOW, err);
                }
            }

            if((rxErrors < 96U) && (txErrors < 96U)){       /* no error */
                CO_errorReset(em, CO_EM_CAN_BUS_WARNING, err);
            }
        }

        if(overflow != 0U){                                 /* CAN RX bus overflow */
            CO_errorReport(em, CO_EM_CAN_RXB_OVERFLOW, CO_EMC_CAN_OVERRUN, err);
        }
    }
}
// CAN 控制数组
CO_CANrxMsg_t rcvMsgBuff[2];
uint16_t    CanTxlen[2];
uint16_t    CanTxBuff[2][8];
uint16_t    CanExTxFlag[2];
uint16_t    CanExRxFlag[2];
void (*CAN_RX_FrameResolutionFunc[2])(modbusMasterData_t* ) = {NULL,NULL};

void CAN_RX_ArrayInit(uint16_t port)
{
    rcvMsgBuff[port].DLC        = 0x00;
    rcvMsgBuff[port].data[0]    = 0x00;
    rcvMsgBuff[port].ident      = 0x00;
}
// CAN modbus 构建函数
void CAN_modbusConstruction(CO_CANrxMsg_t *rcvMsg,uint16_t port)
{
    modbusMasterData[20 + port].type         = MD_CAN;                       // 消息类型
    modbusMasterData[20 + port].port         = 20 + port;                    // 端口
    modbusMasterData[20 + port].Rxlen        = (uint16_t *)&rcvMsg->DLC;     // 接收缓存区长度
    modbusMasterData[20 + port].Rxbuff       = rcvMsg->data;                 // 接收缓存区
    modbusMasterData[20 + port].Txlen        = &CanTxlen[port];              // 发送缓存区长度
    modbusMasterData[20 + port].Txbuff       = CanTxBuff[port] ;             // 发送缓存区
    modbusMasterData[20 + port].RX_ArrayInit = CAN_RX_ArrayInit;             // 数组清除函数
    modbusMasterData[20 + port].exTxFlag     = &CanExTxFlag[port];           // 发送标志位
    modbusMasterData[20 + port].exRxFlag     = &CanExRxFlag[port];           // 接收标志位

    modbusMasterData[20 + port].RxOverFlag   = 0x01;                         // 接收标志位
}
/******************************************************************************/
// 固定回复帧数为0x3D
void CAN_FrameSend(uint16_t port)
{
    /*  发送数据    */

    if(CanExTxFlag[port] == 0x01)       return ;        // 发送未触发

    My_CAN_SendStdCMD(port, 0x3D, CanTxBuff[port]);

    CanExTxFlag[port] = 0x01;       // 发送完毕
}

void CAN_CCC(void)
{
    uint16_t port;
    for (port = 0; port < 2; port++)
    {
        if (CAN_RX_FrameResolutionFunc[port] != NULL)
        {
            CAN_RX_FrameResolutionFunc[port](&modbusMasterData[20 +port]);   // 帧解析
            CAN_RX_FrameResolutionFunc[port] = NULL;
        }
        CAN_FrameSend(port);        // 发送函数
    }
}

void CO_CANinterrupt(CO_CANmodule_t *CANmodule){

    uint8_t port = 0;
    CAN_CCC();
    for(port = 0; port < 2; port++)
    {
        /* receive interrupt */
        if(CAN_R[port]->CAN_IF3OBS.bit.IF3Upd){
            CO_CANrxMsg_t *rcvMsg;      /* pointer to received message in CAN module */
               /* pointer to received message in CAN module */

            extern uint32_t CAN_CMD_ID;
            extern uint32_t CAN_CMD_DeviceID;
            extern uint32_t CAN_CMD_ModuleAddr;
            extern uint8_t CAN_RXBuff[8];

            uint16_t index;             /* index of received message */
            uint32_t rcvMsgIdent;       /* identifier of the received message */
            CO_CANrx_t *buffer = NULL;  /* receive message buffer from CO_CANmodule_t object. */
            bool_t msgMatched = false;

            /* get message from module here */
            rcvMsg = &rcvMsgBuff[port];

            rcvMsg->ident   = (CAN_R[port]->CAN_IF3ARB.bit.ID >> 18) & 0x7FFF;   // 命令ID, 标准帧
            rcvMsg->DLC     = CAN_R[port]->CAN_IF3MCTL.bit.DLC;
            rcvMsg->data[0] = CAN_R[port]->CAN_IF3DATA.bit.Data_0;
            rcvMsg->data[1] = CAN_R[port]->CAN_IF3DATA.bit.Data_1;
            rcvMsg->data[2] = CAN_R[port]->CAN_IF3DATA.bit.Data_2;
            rcvMsg->data[3] = CAN_R[port]->CAN_IF3DATA.bit.Data_3;
            rcvMsg->data[4] = CAN_R[port]->CAN_IF3DATB.bit.Data_4;
            rcvMsg->data[5] = CAN_R[port]->CAN_IF3DATB.bit.Data_5;
            rcvMsg->data[6] = CAN_R[port]->CAN_IF3DATB.bit.Data_6;
            rcvMsg->data[7] = CAN_R[port]->CAN_IF3DATB.bit.Data_7;

            CAN_R[port]->CAN_IF3OBS.all |= 0x0000001F; // 告知接口3可以读下一个邮箱

            rcvMsgIdent = rcvMsg->ident;
            if(CAN_R[port]->CAN_IF3ARB.bit.Xtd)
            {
                // 扩展帧
                CAN_CMD_ID = rcvMsg->ident;             // 扩展帧
                CAN_CMD_ModuleAddr = 0;                 // 模块地址为0

                CAN_RXBuff[0] = rcvMsg->data[0];
                CAN_RXBuff[1] = rcvMsg->data[1];
                CAN_RXBuff[2] = rcvMsg->data[2];
                CAN_RXBuff[3] = rcvMsg->data[3];
                CAN_RXBuff[4] = rcvMsg->data[4];
                CAN_RXBuff[5] = rcvMsg->data[5];
                CAN_RXBuff[6] = rcvMsg->data[6];
                CAN_RXBuff[7] = rcvMsg->data[7];

                Huawei_Control();
                return;
            }
            if(rcvMsgIdent == 0x3C)
            {
                // bootLoader在线解析函数
                extern void BootLoaderFrameFunc(modbusMasterData_t* data);
                extern uint16_t (*modbusMasterReturnFrame[MODBUS_NUM])(modbusMasterData_t* );
                // 提交到接口层
                modbusMasterData[20 + port].RxOverFlag = 1;
                // 处理modbus报文
                CAN_modbusConstruction(rcvMsg, port);                           // CAN构建函数

                CAN_RX_FrameResolutionFunc[port] = BootLoaderFrameFunc;          // 异步帧解析

                return;
            }
            /* CAN module filters are not used, message with any standard 11-bit identifier */
                /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
            buffer = &CANmodule->rxArray[0];
            for(index = CANmodule->rxSize; index > 0U; index--){
                if(((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U){
                    msgMatched = true;
                    break;
                }
                buffer++;
            }


                /* Call specific function, which will process the message */
                if(msgMatched && (buffer != NULL) && (buffer->pFunct != NULL)){
                    buffer->pFunct(buffer->object, rcvMsg);
                }
                    /* Clear interrupt flag */
        }
        /* transmit interrupt */
        else if(CAN_R[port]->CAN_IF2CMD.bit.Busy != 0x01 && (CAN_R[port]->CAN_TXRQ_21 & (1 << (7-1))) != 0x01)
        {
            /* Clear interrupt flag */

            /* First CAN message (bootup) was sent successfully */
            CANmodule->firstCANtxMessage = false;
            /* clear flag from previous message */
            CANmodule->bufferInhibitFlag = false;
            /* Are there any new messages waiting to be send */
            if(CANmodule->CANtxCount > 0U){
                uint16_t i;             /* index of transmitting message */

                /* first buffer */
                CO_CANtx_t *buffer = &CANmodule->txArray[0];
                /* search through whole array of pointers to transmit message buffers. */
                for(i = CANmodule->txSize; i > 0U; i--){
                    /* if message buffer is full, send it. */
                    if(buffer->bufferFull){
                        buffer->bufferFull = false;
                        CANmodule->CANtxCount--;

                        /* Copy message to CAN buffer */
                        CANmodule->bufferInhibitFlag = buffer->syncFlag;
                        /* canSend... */
                        CO_CANsend(CANmodule, buffer);
                        break;                      /* exit for loop */
                    }
                    buffer++;
                }/* end of for loop */

                /* Clear counter if no more messages */
                if(i == 0U){
                    CANmodule->CANtxCount = 0U;
                }
            }
        }
        else{
            /* some other interrupt reason */
        }
    }
}
