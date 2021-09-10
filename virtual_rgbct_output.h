#include "esphome.h"
#include <algorithm>
using namespace esphome;

enum virtual_set {VRGB, VBRIGHT, VTEMP};

void calculate_light(esphome::light::LightState *int_light, esphome::light::LightState *ext_light, virtual_set mode) {
  float new_red, new_green, new_blue, new_white, new_temp=0.0, new_bright;

  auto call = int_light->make_call();
  //this->current_values.as_rgbw(red, green, blue, white, this->gamma_correct_, false);
  //
  //   void current_values_as_rgbct(float *red, float *green, float *blue, float *color_temperature,
//                                float *white_brightness);
//   this->current_values.as_rgbct(traits.get_min_mireds(), traits.get_max_mireds(), red, green, blue, color_temperature,
//                                 white_brightness, this->gamma_correct_);

  auto traits = ext_light->get_traits();
  ext_light->remote_values.as_rgbct(traits.get_min_mireds(), traits.get_max_mireds(), 
                                    &new_red, &new_green, &new_blue, &new_temp, &new_white, ext_light->get_gamma_correct());

  ext_light->remote_values.as_brightness(&new_bright);
  call.set_brightness(new_bright);

  float blue_white = new_white * (1.0 - new_temp);
  float red_white  = new_white * new_temp;
  float green_white = new_white * std::min(1.0f-new_temp, new_temp);

  float color_brightness = ext_light->remote_values.get_color_brightness();
  new_red *= color_brightness;
  new_blue *= color_brightness;
  new_green *= color_brightness;

  call.set_white(new_white);
  call.set_rgb(new_red+red_white, new_green+green_white, new_blue+blue_white); // color in RGB order, this example is purple

  // set parameters (optional)
  call.set_transition_length(1000); // in ms
  call.perform();
}
