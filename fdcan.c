
void MX_FDCAN1_Init(void)
{
    FDCAN_FilterTypeDef filter;

    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;

    /* ===== 500 kbps @ 80 MHz kernel clock ===== */
    hfdcan1.Init.NominalPrescaler = 10;
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1 = 12;
    hfdcan1.Init.NominalTimeSeg2 = 2;

    hfdcan1.Init.StdFiltersNbr = 1;
    hfdcan1.Init.ExtFiltersNbr = 0;

    hfdcan1.Init.RxFifo0ElmtsNbr = 8;
    hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;

    hfdcan1.Init.TxFifoQueueElmtsNbr = 8;
    hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;

    HAL_FDCAN_Init(&hfdcan1);

    /* ===== FILTER ===== */
    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = 0;
    filter.FilterType = FDCAN_FILTER_MASK;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = 0x123 << 5;
    filter.FilterID2 = 0x7FF << 5;  //111 1111 1111  (11 bits) it's mask  Accept ONLY Standard ID = 0x123
	//checks (RECEIVED_ID & MASK) == (FILTER_ID & MASK)
	/*For standard 11-bit IDs, STM32 stores the ID in bits [15:5].
	So: 0x123 << 5 puts 0x123 into the correct bit position for the hardware filter.
	You must always shift standard IDs by 5 bits on STM32 FDCAN.*/

    HAL_FDCAN_ConfigFilter(&hfdcan1, &filter);

    /* ===== TX HEADER ===== */
    TxHeader.Identifier = 0x321;  //nodes having same message identifier will recieve the data.
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
}
