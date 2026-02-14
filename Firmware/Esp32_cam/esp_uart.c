// esp_uart.c

#include "driver/uart.h"
#include "esp_log.h"
#include "esp_uart.h"


#define UART_PORT_NUM      UART_NUM_1   // Use UART1
#define UART_BAUD_RATE     115200
#define UART_TX_PIN        14           // GPIO14 as TX
#define UART_RX_PIN        1            // GPIO1 as RX
#define BUF_SIZE           1024

static const char *UART_TAG = "uart_comm";

void uart_init(void) {
    // Configure UART parameters
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(UART_TAG, "UART initialized on TX=%d, RX=%d", UART_TX_PIN, UART_RX_PIN);
}

void uart_send_data(const char *data) {
    uart_write_bytes(UART_PORT_NUM, data, strlen(data));
    ESP_LOGI(UART_TAG, "Sent: %s", data);
}

void uart_receive_task(void *arg) {
    //uint8_t rx_data[BUF_SIZE];
	char rx_data[BUF_SIZE];
	int counter = 0;
	int send_obj_det = 0;
	static int once = 0;
    while (1) {
        // --- Send data periodically ---
        char tx_msg[64];
        snprintf(tx_msg, sizeof(tx_msg), "Hello STM32! Count=%d\r\n", counter++);
		if(once)
		{
        uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
		once++;
		}

        // --- Receive data from STM32 ---
        int len = uart_read_bytes(UART_PORT_NUM, rx_data, BUF_SIZE - 1,
                                  20 / portTICK_PERIOD_MS);
	    ESP_LOGI(UART_TAG, "Received: %s", (char *)rx_data);
        if (len > 0) {
            rx_data[len] = '\0'; // Null-terminate						
            ESP_LOGI(UART_TAG, "Received: %s", (char *)rx_data);
        }

        snprintf(tx_msg, sizeof(tx_msg), "OBJ:DET\n");
        uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
		 ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);
		vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

// void uart_receive_task(void *arg) {
    // char rx_data[BUF_SIZE];
    // int send_obj_det = 0;
    // static int once = 0;

    // while (1) {
        // char tx_msg[64];

        // // Print once at startup
        // if (once == 0) {
            // ESP_LOGI(UART_TAG, "Started communication with STM32");
            // once++;
        // }

        // // --- Receive data from STM32 ---
        // int len = uart_read_bytes(UART_PORT_NUM, rx_data, BUF_SIZE - 1,
                                  // 20 / portTICK_PERIOD_MS);

        // if (len > 0) {
            // rx_data[len] = '\0'; // Null-terminate
            // ESP_LOGI(UART_TAG, "Received: %s", (char *)rx_data);

            // // Respond to "Hello ESP32!"
            // if (strncmp(rx_data, "Hello ESP32!", 12) == 0) {
                // snprintf(tx_msg, sizeof(tx_msg), "Hello STM32!");
                // uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
                // ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);
            // }

            // // Respond to "Start live streaming"
            // if (strncmp(rx_data, "Start live streaming", 20) == 0) {
                // snprintf(tx_msg, sizeof(tx_msg), "Live stream started");
                // uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
                // ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);

                // // Enable periodic OBJ:DET sending
                // send_obj_det = 1;
            // }
        // }

        // // --- Periodic OBJ:DET sending ---
        // if (send_obj_det == 1) {
            // vTaskDelay(pdMS_TO_TICKS(15000)); // 15 seconds
            // snprintf(tx_msg, sizeof(tx_msg), "OBJ:DET");
            // uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
            // ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);
        // } else {
            // // Yield to other tasks
            // vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second
        // }
    // }
// }

// #include "driver/uart.h"
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #define UART_PORT_NUM      UART_NUM_1
// #define UART_BAUD_RATE     115200
// #define UART_TX_PIN        14
// #define UART_RX_PIN        1
// #define BUF_SIZE           1024

//static const char *UART_TAG = "uart_comm";

// typedef enum {
    // WAIT_GREETING,
    // WAIT_STREAM_CMD,
    // STREAMING
// } uart_state_t;

// void uart_receive_task(void *arg) {
    // char rx_data[BUF_SIZE];
    // char tx_msg[64];
    // uart_state_t state = WAIT_GREETING;

    // ESP_LOGI(UART_TAG, "UART task started, waiting for STM32 greeting...");

    // while (1) {
        // // --- Receive data from STM32 ---
        // int len = uart_read_bytes(UART_PORT_NUM, rx_data, BUF_SIZE - 1,
                                  // 20 / portTICK_PERIOD_MS);

        // if (len > 0) {
            // rx_data[len] = '\0'; // Null-terminate
            // ESP_LOGI(UART_TAG, "Received: %s", rx_data);

            // switch (state) {
                // case WAIT_GREETING:
                    // if (strncmp(rx_data, "Hello ESP32!", 12) == 0) {
                        // snprintf(tx_msg, sizeof(tx_msg), "Hello STM32!");
                        // uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
                        // ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);
                        // state = WAIT_STREAM_CMD;
                        // ESP_LOGI(UART_TAG, "Now waiting for 'Start live stream'...");
                    // }
                    // break;

                // case WAIT_STREAM_CMD:
                    // if (strncmp(rx_data, "Start live stream", 17) == 0) {
                        // snprintf(tx_msg, sizeof(tx_msg), "Live stream started");
                        // uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
                        // ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);
                        // state = STREAMING;
                        // ESP_LOGI(UART_TAG, "Entering streaming mode: will send OBJ:DET every 15s");
                    // }
                    // break;

                // case STREAMING:
                    // // In streaming mode we ignore incoming messages for now
                    // break;
            // }
        // }

        // // --- Periodic OBJ:DET sending ---
        // if (state == STREAMING) {
            // vTaskDelay(pdMS_TO_TICKS(15000)); // 15 seconds
            // snprintf(tx_msg, sizeof(tx_msg), "OBJ:DET");
            // uart_write_bytes(UART_PORT_NUM, tx_msg, strlen(tx_msg));
            // ESP_LOGI(UART_TAG, "Sent: %s", tx_msg);
        // } else {
            // // Yield to other tasks
            // vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second
        // }
    // }
// }
