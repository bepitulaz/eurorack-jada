#include <stdlib.h>
#include <stdio.h>
#include "step_sequencer.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define TIMER_FREQUENCY (5000)
#define LEDC_CV_DUTY (8192) // Set duty to 100%. (2 ** 13) * 100%
#define GATE_1_GPIO (10)
#define GATE_2_GPIO (11)
#define CV_1_GPIO (23)
#define CV_2_GPIO (22)

/**
 * @brief Calculate the duty cycle count for a given frequency within a specified range.
 *
 * This function maps a frequency to a duty cycle count, where the minimum frequency corresponds
 * to a duty cycle count of 0, the maximum frequency corresponds to a duty cycle count of 8191,
 * and frequencies in between correspond to duty cycle counts in between.
 *
 * @param curr_freq The current frequency for which the duty cycle count is to be calculated.
 * @param min_freq The minimum frequency in the range.
 * @param max_freq The maximum frequency in the range.
 * @return The calculated duty cycle count as a 32-bit unsigned integer.
 */
uint32_t calculate_duty_cycle_count(float curr_freq, float min_freq, float max_freq)
{
  float duty_cycle = (curr_freq - min_freq) / (max_freq - min_freq);
  return (uint32_t)(duty_cycle * (1 << 13));
}

void step_sequencer_init()
{
  // C1 to C6 notes frequency range.
  float frequency_range_min = 32.70;   // C1 Note
  float frequency_range_max = 1046.50; // C6 Note

  // Initialize step data. It's an array of StepData structs.
  // Create a sequence with 16 steps and random number of pitch, modulation (freq), and gate values (1 and 0).
  StepData sequence_1[16];
  for (int i = 0; i < 16; i++)
  {
    sequence_1[i].pitch = frequency_range_min + ((float)rand() / RAND_MAX) * (frequency_range_max - frequency_range_min);
    sequence_1[i].gate = rand() % 2;
  }

  StepData sequence_2[32];
  for (int i = 0; i < 32; i++)
  {
    sequence_2[i].pitch = frequency_range_min + ((float)rand() / RAND_MAX) * (frequency_range_max - frequency_range_min);
    sequence_2[i].gate = rand() % 2;
  }

  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_13_BIT,
      .freq_hz = TIMER_FREQUENCY,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_cv_channel_1 = {
      .channel = LEDC_CHANNEL_0,
      .duty = LEDC_CV_DUTY,
      .gpio_num = CV_1_GPIO,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_sel = LEDC_TIMER_0,
      .hpoint = 0,
      .flags.output_invert = 0,
  };

  ledc_channel_config(&ledc_cv_channel_1);

  ledc_channel_config_t ledc_cv_channel_2 = {
      .channel = LEDC_CHANNEL_1,
      .duty = LEDC_CV_DUTY,
      .gpio_num = CV_2_GPIO,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_sel = LEDC_TIMER_0,
      .hpoint = 0,
      .flags.output_invert = 0,
  };

  ledc_channel_config(&ledc_cv_channel_2);

  gpio_config_t gate_conf_channel_1 = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << GATE_1_GPIO),
      .pull_down_en = 0,
      .pull_up_en = 0,
  };

  gpio_config(&gate_conf_channel_1);

  gpio_config_t gate_conf_channel_2 = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << GATE_2_GPIO),
      .pull_down_en = 0,
      .pull_up_en = 0,
  };

  gpio_config(&gate_conf_channel_2);

  int tempo_bpm = 100;                 // It should be set by the user.
  int delay_time = 60000 / tempo_bpm; // Calculate delay time for each beat

  int sequence_1_length = sizeof(sequence_1) / sizeof(sequence_1[0]);
  int sequence_2_length = sizeof(sequence_2) / sizeof(sequence_2[0]);
  int step = 0;
  while (1)
  {
    // Get the current step data, and make sure it will go back to the first step after the last step.
    StepData current_step_1 = sequence_1[step % sequence_1_length];
    StepData current_step_2 = sequence_2[step % sequence_2_length];

    float frequency_1 = current_step_1.pitch;
    float frequency_2 = current_step_2.pitch;

    // Calculate LEDC timer count based on the period of the frequency.
    uint32_t ledc_cv_count_channel_1 = calculate_duty_cycle_count(frequency_1, frequency_range_min, frequency_range_max);
    uint32_t ledc_cv_count_channel_2 = calculate_duty_cycle_count(frequency_2, frequency_range_min, frequency_range_max);

    printf("Gate channel 1 set %d\n", current_step_1.gate);
    gpio_set_level(GATE_1_GPIO, current_step_1.gate);

    printf("Gate channel 2 set %d\n", current_step_2.gate);
    gpio_set_level(GATE_2_GPIO, current_step_2.gate);

    printf("CV channel 1 duty to %lu\n", ledc_cv_count_channel_1);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, ledc_cv_count_channel_1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    printf("CV channel 2 duty to %lu\n", ledc_cv_count_channel_2);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, ledc_cv_count_channel_2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

    vTaskDelay(delay_time / portTICK_PERIOD_MS);

    step++;
  }
}
