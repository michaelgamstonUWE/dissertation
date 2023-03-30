/*
                                                                  *****FILE HEADER*****
File Name - serial_comms.cpp

Author/s - Joe Button

Description - Functions for SPI comms between FiPy and esp32-cam. Header file for serial_comms.cpp

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for updated comments

Repo - michaelgamston/MVP
Branch - main

*/
#include "serial_comms.h"
#include "camera_funcs.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"


/*
This uses the standard SPI pins (MISO, MOSI, SCLK, CS) to
transmit data over in a full-duplex fashion, that is, while the master puts data on the MOSI pin, the slave puts its own
data on the MISO pin.

This example uses one extra pin: GPIO_HANDSHAKE is used as a handshake pin. After a transmission has been set up and we're
ready to send/receive data, this code uses a callback to set the handshake pin high. The sender will detect this and start
sending a transaction. As soon as the transaction is done, the line gets set low again.
*/

/*
Pins in use. The SPI Master can use the GPIO mux, so feel free to change these if needed.
*/
#define GPIO_HANDSHAKE GPIO_NUM_2
#define GPIO_MOSI GPIO_NUM_12
#define GPIO_MISO GPIO_NUM_13
#define GPIO_SCLK GPIO_NUM_15
#define GPIO_CS GPIO_NUM_14
#define RCV_HOST    HSPI_HOST

const uint32_t BUFFER_SIZE = 9216;
const uint32_t IMAGE_SIZE = H * W;
uint8_t* spi_slave_tx_buf;
uint8_t* spi_slave_rx_buf;
bool is_setup = false;
bool is_activated = false;
esp_err_t ret;
spi_slave_transaction_t t;
//const uint8_t TRP = 2;

//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
    gpio_set_level(GPIO_HANDSHAKE, 1);
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
    gpio_set_level(GPIO_HANDSHAKE, 0);
}

void setup_spi() {
  //Configuration for the SPI bus
  spi_bus_config_t buscfg = {
      .mosi_io_num = GPIO_MOSI,
      .miso_io_num = GPIO_MISO,
      .sclk_io_num = GPIO_SCLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };
  //Configuration for the SPI slave interface
  spi_slave_interface_config_t slvcfg={
      .spics_io_num=GPIO_CS,
      .flags=0,
      .queue_size=1,
      .mode=1,
      .post_setup_cb=my_post_setup_cb,
      .post_trans_cb=my_post_trans_cb

  };
  //Configuration for the handshake line
  gpio_config_t io_conf={
      .pin_bit_mask=(1<<GPIO_HANDSHAKE),
      .mode=GPIO_MODE_OUTPUT,
      .intr_type=GPIO_INTR_DISABLE,
  };
  //Configure handshake line as output
  gpio_config(&io_conf);
  //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
  gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);
  //Initialize SPI slave interface
  ret=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
  assert(ret==ESP_OK);
  memset(&t, 0, sizeof(t));
  spi_slave_tx_buf = (uint8_t*) malloc(BUFFER_SIZE);
  spi_slave_rx_buf = (uint8_t*) malloc(BUFFER_SIZE);
  memset(spi_slave_rx_buf, 0, BUFFER_SIZE);
  memset(spi_slave_tx_buf, 0, BUFFER_SIZE);
  is_setup = true;
}

//change these 
void addActivationByte() {
  is_activated = true;
  spi_slave_tx_buf[8191] = 1;
  //digitalWrite(TRP, LOW);
}

void deactivateByte() {
  spi_slave_tx_buf[8191] = 0;
}

void copy_to_buffer() {
  if (!is_setup) {
    setup_spi();
  }
  for (int i = 1; i <= IMAGE_SIZE; i++)
  {
    spi_slave_tx_buf[i] = (uint8_t) current_frame[i-1];

  }

  //add detection metadata 
  int x = 0; 
  for (int i = 7600; i < 8538; i++){
    spi_slave_tx_buf[i] = HxW_PBR_byte_arr[x];
    x++;

  }
  memset(HxW_PBR_byte_arr, 0, sizeof(uint8_t)*DETECTION_ARR_SIZE);
}

bool send_image() {
  copy_to_buffer();
  //Set up a transaction of BUFFER_SIZE bytes to send/receive
  t.length=BUFFER_SIZE;
  t.tx_buffer = spi_slave_tx_buf;
  t.rx_buffer = spi_slave_rx_buf;
  /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
  initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
  by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
  .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
  data.
  */
  ret = spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);
  //spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
  //received data from the master. 
  return true;
}
