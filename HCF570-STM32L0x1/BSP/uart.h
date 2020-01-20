#ifndef __uart_H
#define __uart_H

#define 	debug_log

#ifdef debug_log

			#define		log_info(...)	printf(__VA_ARGS__)

#else
			#define		log_info(...)
#endif


void MX_LPUART1_UART_Init(void);

#endif

