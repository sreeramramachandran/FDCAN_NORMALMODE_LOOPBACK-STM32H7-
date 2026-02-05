#include "main.h"
#include <stdio.h>

/* ===================== HANDLES ===================== */
FDCAN_HandleTypeDef hfdcan1;
UART_HandleTypeDef  huart3;

/* ===================== CAN STRUCTS ===================== */
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;

/* ===================== DATA ===================== */
uint8_t  can_rx_data[8];
uint8_t  can_tx_data[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};

volatile uint8_t can_rx_flag = 0;
volatile uint32_t can_error_count = 0;

/* ===================== RETARGET printf ===================== */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

/* ========================================================= */
/* ===================== CALLBACKS ========================== */
/* ========================================================= */

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                              uint32_t RxFifo0ITs)
{
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        HAL_FDCAN_GetRxMessage(hfdcan,
                               FDCAN_RX_FIFO0,
                               &RxHeader,
                               can_rx_data);
        can_rx_flag = 1;
    }
}

void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
    can_error_count++;
}

/* ========================================================= */
/* ===================== CAN SEND =========================== */
/* ========================================================= */

void CAN_Send(void)
{
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1,
                                     &TxHeader,
                                     can_tx_data) != HAL_OK)
    {
        can_error_count++;
    }
}

/* ========================================================= */
/* ===================== APPLICATION ======================== */
/* ========================================================= */

void Process_CAN_Message(void)
{
    uint16_t sensor1 = (can_rx_data[0] << 8) | can_rx_data[1];
    uint16_t sensor2 = (can_rx_data[2] << 8) | can_rx_data[3];
    uint16_t sensor3 = (can_rx_data[4] << 8) | can_rx_data[5];
    uint16_t sensor4 = (can_rx_data[6] << 8) | can_rx_data[7];

    printf("RX: %u %u %u %u\r\n",
           sensor1, sensor2, sensor3, sensor4);
}

/* ========================================================= */
/* ===================== MAIN =============================== */
/* ========================================================= */

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_FDCAN1_Init();

    /* ---------- START CAN ---------- */
    HAL_FDCAN_Start(&hfdcan1);

    HAL_FDCAN_ActivateNotification(&hfdcan1,
        FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_ERROR,
        0);

    printf("CAN production firmware started\r\n");

    while (1)
    {
        if (can_rx_flag)
        {
            can_rx_flag = 0;
            Process_CAN_Message();
        }

        CAN_Send();
        HAL_Delay(1000);
    }
}
