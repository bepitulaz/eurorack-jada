#include <stdlib.h>
#include <stdio.h>
#include "step_sequencer.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_CV_1_DUTY (4096) // Set duty to 100%. (2 ** 12) * 100%
#define GATE_1_GPIO (10)
#define CV_1_GPIO (23)

void step_sequencer_init()
{
  // Initialize step data. It's an array of StepData structs.
  // Create a sequence with 16 steps and random number of pitch, modulation (freq), and gate values (1 and 0).
  StepData sequence[16];
  float frequency_range_min = 65.41; // C2
  float frequency_range_max = 523.25; // C5
  for (int i = 0; i < 16; i++)
  {
    sequence[i].pitch = frequency_range_min + ((float)rand() / RAND_MAX) * (frequency_range_max - frequency_range_min);
    sequence[i].gate = rand() % 2;
  }

  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_12_BIT,
      .freq_hz = 5000,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_cv_channel_1 = {
      .channel = LEDC_CHANNEL_0,
      .duty = LEDC_CV_1_DUTY,
      .gpio_num = CV_1_GPIO,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_sel = LEDC_TIMER_0,
      .hpoint = 0,
      .flags.output_invert = 0,
  };

  ledc_channel_config(&ledc_cv_channel_1);

  gpio_config_t gate_conf_channel_1 = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << GATE_1_GPIO),
      .pull_down_en = 0,
      .pull_up_en = 0,
  };

  gpio_config(&gate_conf_channel_1);

  int tempo_bpm = 50; // It should be set by the user.
  int delay_time = 60000 / tempo_bpm; // Calculate delay time for each beat

  int sequence_length = sizeof(sequence) / sizeof(sequence[0]);
  int step = 0;
  while (1)
  {
    // Get the current step data, and make sure it will go back to the first step after the last step.
    StepData current_step = sequence[step % sequence_length];

    printf("1. Gate channel 1 set %d\n", current_step.gate);
    gpio_set_level(GATE_1_GPIO, current_step.gate);

    printf("2. CV channel 1 set to %f\n", current_step.pitch);
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, current_step.pitch);

    vTaskDelay(delay_time / portTICK_PERIOD_MS);

    step++;
  }
}
