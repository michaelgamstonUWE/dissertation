/*
                                                                  *****FILE HEADER*****
File Name - serial_comms.cpp

Author/s - Joe Button

Description - Functions for SPI comms between FiPy and esp32-cam. Header file for serial_comms.cpp

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for updated comments

Libraries - Uses esp32 dma SPI library: https://github.com/hideakitai/ESP32DMASPI

Repo - michaelgamston/MVP
Branch - main

*/
#include "serial_comms.h"
#include <ESP32DMASPISlave.h>
// #include <Arduino.h>
#include "camera_funcs.h"

ESP32DMASPI::Slave slave;

const uint32_t BUFFER_SIZE = 9216;
const uint32_t IMAGE_SIZE = H * W;
uint8_t* spi_slave_tx_buf;
uint8_t* spi_slave_rx_buf;
// Pins
const uint8_t ERROR_GPIO_PIN = 2;
const uint8_t CS_GPIO_PIN = 15;
bool is_setup = false;
bool is_activated = false;
//const uint8_t TRP = 2;

void setup_spi()
{
  // to use DMA buffer, use these methods to allocate buffer
  spi_slave_tx_buf = slave.allocDMABuffer(BUFFER_SIZE);
  spi_slave_rx_buf = slave.allocDMABuffer(BUFFER_SIZE);
  // pins 
  // pinMode(CS_GPIO_PIN, INPUT);
  //pinMode(TRP, OUTPUT);
  // set buffer data...
  memset(spi_slave_tx_buf, 0, BUFFER_SIZE);
  memset(spi_slave_rx_buf, 0, BUFFER_SIZE);
  spi_slave_tx_buf[0] = 5; // set checkbyte to 5
  //spi_slave_tx_buf[8191] = 0;
  // slave device configuration
  slave.setDataMode(SPI_MODE0);
  slave.setMaxTransferSize(BUFFER_SIZE);
  // begin() after setting
  slave.begin();  // HSPI = CS: 15, CLK: 14, MOSI: 13, MISO: 12 -> default
                  // VSPI (CS:  5, CLK: 18, MOSI: 23, MISO: 19)
  is_setup = true;
}

//change these 
void addActivationByte(){
  is_activated = true;
  spi_slave_tx_buf[8191] = 1;
  //digitalWrite(TRP, LOW);
}

void deactivateByte(){
  spi_slave_tx_buf[8191] = 0;
}

void copy_to_buffer()
{
  if (!is_setup)
  {
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

bool send_image()
{
  bool result = false;
  copy_to_buffer();
  // if there is no transaction in queue, add transaction
  if (slave.remained() == 0) 
  {
    // this adds buffers to queue 
    slave.queue(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);
    is_activated = false;
    //deactivateByte();
  }
  // if transaction has completed from master,
  // available() returns size of results of transaction,
  // and buffer is automatically updated
  // truthiness of slave.available is false if there is a transaction in progress, otherwise true

  while (slave.available()) 
  {
    // do something with received data: spi_slave_rx_buf
    slave.pop();
    // Serial.println("transferring");
    //digitalWrite(TRP, HIGH);
    result = true;
  }

  return result;
  
}
