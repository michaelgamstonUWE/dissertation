#include "cam.h"
#include <bitset>
#ifndef NATIVE
#include "camera_pins.h"
#endif

Cam::Cam(float block_diff_threshold, float image_diff_threshold) {
    Cam::block_diff_threshold = block_diff_threshold;
    Cam::image_diff_threshold = image_diff_threshold;
}

#ifndef NATIVE
bool Cam::setup_camera(framesize_t frameSize) {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = frameSize;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  bool ok = esp_camera_init(&config) == ESP_OK;

  sensor_t *sensor = esp_camera_sensor_get();
  sensor->set_framesize(sensor, frameSize);

  return ok;
}

bool Cam::capture_still() {
  camera_fb_t *frame_buffer = esp_camera_fb_get(); 

  if (!frame_buffer)
      return false;

  // set all 0s in current frame
  for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
          current_frame[y*W + x] = 0;
  uint16_t x, y, current;
  uint8_t block_x, block_y, pixel;
  // down-sample image in blocks 
  for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
      // grab x and y coordinates 
      x = i % WIDTH;
      y = floor(i / WIDTH);
      block_x = floor(x / BLOCK_SIZE);
      block_y = floor(y / BLOCK_SIZE);
      pixel = frame_buffer->buf[i]; // this accesses the pixels in the raw image (an array of HxW) 
      current = current_frame[block_y*W + block_x];
      // average pixels in block (accumulate)
      current_frame[block_y*W +block_x] += pixel;
  }
  // average pixels in block (rescale)
  for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
          current_frame[y*W +x] /= BLOCK_SIZE * BLOCK_SIZE;

  esp_camera_fb_return(frame_buffer);
  return true;
}
#endif

// bool Cam::motion_detect() {
//   uint16_t changes = 0; // number of changes
//   const uint16_t blocks = (WIDTH * HEIGHT) / (BLOCK_SIZE * BLOCK_SIZE); // number of blocks
//   // loop over pixels
//   //variables for HxW pixel to bit representation
//   uint8_t HxW_PBR_byte_element = 0U;
//   std::bitset<8> manipulated_byte;
//   manipulated_byte.set(false); 
//   int bit_index = 7;
//   int byte_index = 0;
// 
//   for (int y = 0; y < H; y++) {
//       for (int x = 0; x < W; x++) {
//           // compare pixel at x,y to previous frame pixel at x,y
//           float current = current_frame[y*W +x];
//           float prev = previous_frame[y*W +x];
//           float delta = abs(current - prev) / prev; // magnitude of change wrt to previous value
//           // print coords of pixel change if threshold met
//           if (delta >= block_diff_threshold) {
//               // keep track of number of pixels changed for each frame
//               changes += 1;
//               //if detection change the corrisponding bit to 1 
//               manipulated_byte[bit_index] = 1; 
//           }
//         //bit count needs to count down because bitset is LSB so highest indexed bit is on the left, not the right 
//         bit_index -= 1;
//         //check if loops are on last iteration
//         //if they are add the byte into the array with the remaining bits left at 0 
//         if (y == H - 1 && x == W - 1){
//          HxW_PBR_byte_arr[byte_index] = HxW_PBR_byte_element | manipulated_byte.to_ulong();
//          manipulated_byte.set(false);
//         }
//         else if (bit_index < 0){
//         // add byte to HxW byte array 
//         // reset manipulated_byte bit's to back to 0
//         HxW_PBR_byte_arr[byte_index] = HxW_PBR_byte_element | manipulated_byte.to_ulong();
//         manipulated_byte.set(false);
//         //reset bit index
//         bit_index = 7;
//         //increase byte_index
//         byte_index += 1;
//         }
//       }
//   }
//   // compare average change to threshold 
//   return (1.0 * changes / blocks) > image_diff_threshold;
// }

/**
 * Copy current frame to previous
 */
void Cam::update_frame() {
    for (int i = 0; i < H * W; i++) {
        previous_frame[i] = (uint8_t) current_frame[i];
    }
}

std::array<uint16_t, H * W> &Cam::get_current_frame() {
    return current_frame;
}
// void set_current_frame(const std::vector<)
