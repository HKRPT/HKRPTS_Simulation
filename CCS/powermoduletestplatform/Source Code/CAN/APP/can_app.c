#include "can_app.h"

// 控制中断开关
void __disable_irq(void)
{
    DINT;
    DRTM;
}
void __enable_irq(void)
{
    EINT;
    ERTM;
}
void __set_PRIMASK(unsigned char state)
{
    if(state == 1)
    {
        __disable_irq();
    }
    else if(state == 0)
    {
        __enable_irq();
    }
    else
    {
        return;
    }
}

// 跳转到BOOT程序
void CAN_BOOT_JumpToApplication(uint32_t Addr)
{
    (*((void(*)(void))(Addr)))();
}

// 处理CAN消息函数
void CAN_BOOT_ExecutiveCommand(CanRxMsg *pRxMessage)
{
    extern void CAN_Tx_Msg(CanTxMsg *can_tx_msg);  //发送消息

    CanTxMsg TxMessage;
    uint8_t PCI = pRxMessage->CAN_Rx_msg_data.msg_byte.data[1];
    uint8_t SID=0xFF;
    uint8_t CMD=0xFF;
    //根据消息获取SID和CMD
    if((PCI&0xF0)==0x00){
      SID = pRxMessage->CAN_Rx_msg_data.msg_byte.data[2];
      CMD = pRxMessage->CAN_Rx_msg_data.msg_byte.data[3];
    }else if((PCI&0xF0)==0x10){
      SID = pRxMessage->CAN_Rx_msg_data.msg_byte.data[3];
      CMD = pRxMessage->CAN_Rx_msg_data.msg_byte.data[4];
    }
    //准备发送的数据
    TxMessage.CAN_num = CANA;
    TxMessage.DLC = 8;
    TxMessage.MBox_num = 0x01;
    TxMessage.Tx_timeout_cnt = 0x00;
    TxMessage.SAE_J1939_Flag = 0;
    if(MSG_ID_TYPE == CAN_Id_Standard){
      TxMessage.StdId.bit.StdId = MSG_SEND_ID;
      TxMessage.IDE = CAN_Id_Standard;
      TxMessage.StdId.bit.resved=0;
    }else{
      TxMessage.ExtId.bit.ExtId = MSG_SEND_ID;
      TxMessage.IDE = CAN_Id_Extended;
      TxMessage.ExtId.bit.resved=0;
    }
    TxMessage.CAN_Tx_msg_data.msg_byte.data[0] = GetNAD();//填充NAD
    //解析命令
    switch(CMD)
    {
      case CMD_GET_FW_INFO:
          TxMessage.CAN_Tx_msg_data.msg_byte.data[1] = 0x06;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[2] = SID+0x40;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[3] = FW_TYPE;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[4] = 1;//固件版本号 Major
          TxMessage.CAN_Tx_msg_data.msg_byte.data[5] = 0;//固件版本号 Minor
          TxMessage.CAN_Tx_msg_data.msg_byte.data[6] = 0;//固件版本号 Revision
          TxMessage.CAN_Tx_msg_data.msg_byte.data[7] = 3;//固件版本号 Build
          CAN_Tx_Msg(&TxMessage);
          break;
      case CMD_ENTER_BOOT:
          if(FW_TYPE == FW_TYPE_APP){
              CAN_BOOT_JumpToApplication(0x080000);//跳转地址参考boot的cmd文件的BEGIN定义
          }
          break;
      case CMD_ERASE_APP:
          //APP中不能执行擦除APP的操作
          TxMessage.CAN_Tx_msg_data.msg_byte.data[1] = 0x06;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[2] = SID+0x40;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[3] = CAN_BOOT_ERR_ERASE_IN_APP;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[4] = 0xFF;TxMessage.CAN_Tx_msg_data.msg_byte.data[5] = 0xFF;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[6] = 0xFF;TxMessage.CAN_Tx_msg_data.msg_byte.data[7] = 0xFF;
          CAN_Tx_Msg(&TxMessage);
          break;
      case CMD_SET_ADDR_OFFSET:
          TxMessage.CAN_Tx_msg_data.msg_byte.data[1] = 0x06;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[2] = SID+0x40;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[3] = CAN_BOOT_ERR_WRITE_IN_APP;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[4] = 0xFF;TxMessage.CAN_Tx_msg_data.msg_byte.data[5] = 0xFF;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[6] = 0xFF;TxMessage.CAN_Tx_msg_data.msg_byte.data[7] = 0xFF;
          CAN_Tx_Msg(&TxMessage);
          break;
      case CMD_TRAN_DATA:
          break;
      case CMD_WRITE_DATA:
          TxMessage.CAN_Tx_msg_data.msg_byte.data[1] = 0x06;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[2] = SID+0x40;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[3] = CAN_BOOT_ERR_WRITE_IN_APP;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[4] = 0xFF;TxMessage.CAN_Tx_msg_data.msg_byte.data[5] = 0xFF;
          TxMessage.CAN_Tx_msg_data.msg_byte.data[6] = 0xFF;TxMessage.CAN_Tx_msg_data.msg_byte.data[7] = 0xFF;
          CAN_Tx_Msg(&TxMessage);
          break;
      case CMD_ENTER_APP:
          break;
      default:
          break;
    }
}











