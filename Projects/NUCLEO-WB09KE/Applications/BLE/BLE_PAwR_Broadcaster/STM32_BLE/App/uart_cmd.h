
#ifndef UART_CMD_H
#define UART_CMD_H

void UART_CMD_DataReceived(uint8_t * pRxDataBuff, uint16_t nDataSize);

void UART_CMD_ProcessRequestCB(void);

void UART_CMD_Process(void);


#endif /* UART_CMD_H */
