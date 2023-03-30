/*
                                                                  *****FILE HEADER*****
File Name - main.ino

Author/s - Joe Button

Description - Main function's for Controll of ESP32-CAM

Hardware - A0.3 (ESP32-WROOM, 2xESP32-CAM)

Comments - See .cpp for comments 

Libraries - N/A


Repo - michaelgamston/MVP
Branch - main

*/
// #include <Arduino.h>
#include "camera_funcs.h"
#include "serial_comms.h"
#include "CNN.h"
#include <chrono>
#include <iostream>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <array>

const int IMAGE_SIZE = H * W;
CNN *nn;

// test neural net on input image
std::array<float, 2> inference(uint16_t *im) {
  for (int i = 0; i < IMAGE_SIZE; i++) {
    std::array<int, 2> indices = {i, IMAGE_SIZE + i};
    // testing performance on multiple input channels 
    // in practice channels would be raw pixel value and {0, 255} depending on background subtraction
    for (int j = 0; j < 2; j++) {
      // normalise pixel values by dividing by 255 
      // this is important for training so need to replicate at inference stage
      float pixel = (float) im[i] / 255.0;
      nn->getInputBuffer()[i + j] = nn->convert_float2int8(pixel);
    }
  }
  std::array<float, 2> results;
  for (int i = 0; i < 2; i++) {
    results[i] = nn->convert_int82float(nn->predict());
  }
  return results;
}

void setup() {
  setup_camera(FRAME_SIZE);
  // Neural network object
  nn = new CNN();
  setup_spi();
  for (int i = 0; i < DETECTION_ARR_SIZE; i++){
    // 170 = 10101010 
    HxW_PBR_byte_arr[i] = (uint8_t) 170;
  }
}

void loop() {
  // time the main loop
  auto start = std::chrono::high_resolution_clock::now();
  // copy previous frame for background subtraction
  update_frame();
  // take pic
  capture_still();
  if (motion_detect()) {
    printf("Motion detected\n");
  }
  // get cnn results from current frame 
  auto results = inference(current_frame);
  // calculate duration
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // print out duration and results 
  printf("result 1 = %.3f, result 2 = %.3f, took %lld ms \n", results[0], results[1], duration.count());
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  send_image();
}

extern "C" void app_main() {
    setup();
    while (true) {
        loop();
    }
}
