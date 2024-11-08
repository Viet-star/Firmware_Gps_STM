/** @file sim808.h
 *  @brief Prototypes of functions that communicate with the SIM808 module and control it and definition of related parameters.
 *
 *  @author xuanviet
 *  @bug issue #9
 */
#ifndef SIM808_H
#define SIM808_H

#include <stdint.h>
#include "main.h"

#define AT_uart huart1
#define debug_uart huart2

#define DEBUG_MODE 1
/* Error codes */
#define FAIL 0
#define SUCCESS 1
#define TRUE 1
#define FALSE 0

#define GREEN_LED GPIO_PIN_13


#define RX_WAIT 200 /* After sending AT command, wait RX_WAIT ms  to ensure that the reply is receeived in the buffer */
#define RX_TIMEOUT 1000
#define TX_TIMEOUT 100
#define BAUD_RATE 38400 /* BAUD_RATE=38400 => it take 26 ms to send 100 bytes */
#define RX_BUFFER_LENGTH 256
#define SIM_UART huart2
#define DEBUG_UART huart1
#define TCP_CONNECT_TIMEOUT 5 /* value in second */
#define GPS_COORDINATES_LENGTH 23 





/** 
 * @brief is used to abstract the SIM808 hardware interface. 
 * It will decouple the functions from the hardware (uart+pins) used to interface with the module.
 * The UART peripheral used to send AT commands to the module and the UART used to send Debug messages
 * and the pins used to power on, reset and check status of the module are combined together in this struct.
 * This struct will be passed to all the functions that perform actions on the module.
 */

typedef struct {
	USART_TypeDef * AT_uart_instance; /*send AT commands through this UART.*/
	USART_TypeDef * debug_uart_instance;
	GPIO_TypeDef  * power_on_gpio;
	uint16_t power_on_pin;
	GPIO_TypeDef  * reset_gpio;
	uint16_t reset_pin;
	GPIO_TypeDef  * status_gpio;
	uint16_t status_pin;
} SIM808_typedef;



 /**
 * @brief it initialises the SIM808_typedef struct members and powers on the module.
 * initializes the UARTs and the GPIOs used to power on, reset and check the status of the module.
 * Which UART peripheral is used for what is specified in the parameter SIM808_typedef members: AT_uart and debug_uart.
 * @param sim is the definition of the sim808 hardware
 * @return SUCCESS if module is ready for use, FAIL otherwise
 */	
uint8_t sim_init(SIM808_typedef * sim);

 /**
 * @brief power_off the module.
 * @param sim is the definition of the sim808 hardware
 * @return SUCCESS if module is ready for use, FAIL otherwise
 */	
uint8_t sim_power_off(SIM808_typedef * sim);

 /**
 * @brief power_off the module and reset the MCU
 * @param sim is the definition of the sim808 hardware
 */	
void system_reset(SIM808_typedef * sim);


 /**
 * @brief sends a cmd to the module, if the parameter save_reply == 1 then it copies the reply into the parameter cmd_reply.
 * @param cmd is the AT command to be sent
 * @param expected_reply is used to determine if the outcome of the function is SUCCESS or FAIL.
 * @param save_reply if set to 1, then the reply from the module gets copied into cmd_reply.
 * @param cmd_reply is an array where to modules reply is copied.
 * @param rx_timeout the maximum amount of time the function will wait for the module to receive a reply.  
 * @returns SUCCESS if the module replies withing timeout and the reply includes the expected_reply, FAIL otherwise.
 */
uint8_t send_AT_cmd(const char * cmd, const char * expected_reply, uint8_t save_reply, char * cmd_reply, uint32_t rx_timeout);

/**
 * @brief sends raw serial data to the SIM808 module through the AT_uart peripheral. Then it deletes the receive buffer.
 * @param cmd contains the command 
 * @param rx_wait waiting time before exit. To make sure the reply is received.
 */
uint8_t send_serial_data(uint8_t * data, uint8_t length, char * cmd_reply, uint32_t rx_wait);

 /**
 * @brief sends a debug message through the debug UART
 * @param debug_message is the debug message.
 */	
void send_debug(const char * debug_msg);


void  send_raw_debug(uint8_t * debug_dump,uint8_t length);
uint8_t is_subarray_present(const uint8_t *array, size_t array_len, const uint8_t *subarray, size_t subarray_len);
#endif
