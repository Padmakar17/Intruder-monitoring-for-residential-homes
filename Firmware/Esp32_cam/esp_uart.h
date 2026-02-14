// esp_uart.h

// #include "driver/uart.h"
// #include "esp_log.h"

// #define UART_PORT_NUM      UART_NUM_1   // Use UART1
// #define UART_BAUD_RATE     115200
// #define UART_TX_PIN        14           // GPIO14 as TX
// #define UART_RX_PIN        1            // GPIO1 as RX
// #define BUF_SIZE           1024

//static const char *UART_TAG = "uart_comm";

void uart_init(void);

void uart_send_data(const char *data);

void uart_receive_task(void *arg);
