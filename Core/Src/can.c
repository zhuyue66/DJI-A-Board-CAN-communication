/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
extern UART_HandleTypeDef huart8;

CAN_HandleTypeDef hcan1;
moto_info_t motor_info[MOTOR_MAX_NUM];
uint8_t can_cnt = 0;

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */
	__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */
    
  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void CAN_FliterInit(CAN_HandleTypeDef *hcan){
	
	CAN_FilterTypeDef can_fliterinit;
	
	can_fliterinit.FilterMode = CAN_FILTERMODE_IDMASK;//掩码模式
	can_fliterinit.FilterScale = CAN_FILTERSCALE_32BIT;//32位
	can_fliterinit.FilterIdHigh = 0x0000; //32位ID
	can_fliterinit.FilterIdLow = 0x0000; 
	can_fliterinit.FilterMaskIdHigh = 0x0000;
	can_fliterinit.FilterMaskIdLow = 0x0000;
	can_fliterinit.FilterBank = 0;//过滤器号0
	can_fliterinit.FilterFIFOAssignment = CAN_RX_FIFO0;//用FIFO接收信息
	can_fliterinit.FilterActivation = ENABLE; //激活过滤器0
	can_fliterinit.SlaveStartFilterBank = 14;	//仅双机通讯有效
	
	if(HAL_CAN_ConfigFilter(hcan,&can_fliterinit) != HAL_OK){
		Error_Handler();
		printf("过滤器设置失败");
	}
	if(HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK){
		Error_Handler();
		printf("激活失败");
	}
	if(HAL_CAN_Start(hcan) != HAL_OK){
		printf("CAN 开启失败");
		Error_Handler();
	}
}

//接收回调函数
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
	
	CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
	
	if(hcan->Instance == CAN1){
		HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&rx_header,rx_data);
	}
	uint8_t index = rx_header.StdId - FEEDBACK_ID_BASE;  // get motor index by can_id
	switch(rx_header.StdId){
		case 0x205:
			can_cnt++;
			motor_info[index].rotor_angle    = ((rx_data[0] << 8) | rx_data[1]);
			motor_info[index].torque_current = ((rx_data[2] << 8) | rx_data[3]);
			motor_info[index].torque         = ((rx_data[4] << 8) | rx_data[5]);
			motor_info[index].temp           = ((rx_data[6] << 8) | rx_data[7]);
//			printf("  6623转角  %2d ",    motor_info[index].rotor_angle);
//			printf("  实际转矩电流测量值 %2d ",    motor_info[index].torque_current);
//			printf("  给定转矩电流测量值 %2d ",    motor_info[index].torque);
//			printf("  空字节            %2d  \r\n",motor_info[index].temp);
			break;
		case 0x208:
			can_cnt++;
			motor_info[index].rotor_angle    = ((rx_data[0] << 8) | rx_data[1]);
			motor_info[index].torque_current = ((rx_data[2] << 8) | rx_data[3]);
			motor_info[index].torque         = ((rx_data[4] << 8) | rx_data[5]);
			motor_info[index].temp           = ((rx_data[6] << 8) | rx_data[7]);
//			printf("  6020转角  %2d  ",    motor_info[index].rotor_angle);
//			printf("  实际转矩电流测量值  %2d  ",    motor_info[index].torque_current);
//			printf("  给定转矩电流测量值  %2d  ",    motor_info[index].torque);
//			printf("  空字节             %2d   \r\n",motor_info[index].temp);
			break;
		//电机2006
		case 0x201:
			can_cnt++;
			motor_info[index].rotor_angle    = ((rx_data[0] << 8) | rx_data[1]);
			motor_info[index].torque_current = ((rx_data[2] << 8) | rx_data[3]);
			motor_info[index].torque         = ((rx_data[4] << 8) | rx_data[5]);
			motor_info[index].temp           = ((rx_data[6] << 8) | rx_data[7]);
//			printf("  2006转角  %2d ",    motor_info[index].rotor_angle);
//			printf("  实际转矩电流测量值 %2d ",    motor_info[index].torque_current);
//			printf("  给定转矩电流测量值 %2d ",    motor_info[index].torque);
//			printf("  空字节            %2d  \r\n",motor_info[index].temp);
			break;
	}
	if(can_cnt == 200){
		can_cnt = 0;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);
		HAL_Delay(100);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);
	}
}


// v1,v2,v3,v4分别表示四个电机，一条CAN消息一共可以控制8个电机，0x1FF(Yaw/Pitch/Roll/Resv)四个、0x2FF(EX1/EX2/EX3/EX4)四个(扩展)
void set_motor_voltage(int16_t id, int16_t v1, int16_t v2, int16_t v3, int16_t v4)
{
  CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
    
  tx_header.StdId = id;
  tx_header.IDE   = CAN_ID_STD;
  tx_header.RTR   = CAN_RTR_DATA;
  tx_header.DLC   = 8;

  tx_data[0] = (v1>>8)&0xff;
  tx_data[1] =    (v1)&0xff;
  tx_data[2] = (v2>>8)&0xff;
  tx_data[3] =    (v2)&0xff;
  tx_data[4] = (v3>>8)&0xff;
  tx_data[5] =    (v3)&0xff;
  tx_data[6] = (v4>>8)&0xff;
  tx_data[7] =    (v4)&0xff;
	
  HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data,(uint32_t*)CAN_TX_MAILBOX0); 
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
