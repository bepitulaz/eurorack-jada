#include <stdlib.h>
#include <stdio.h>
#include "step_sequencer.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define TIMER_FREQUENCY (5000)
#define LEDC_CV_DUTY (8192)        // Set duty cycle to 100%. (2 ** 13) * 100%
#define POTENTIOMETER_GPIO (4)     // For setup the tempo.
#define CV_IN_1_GPIO (0)           // For external CV input. Modulating the sequence.
#define CV_IN_2_GPIO (1)           // For external CV input. Modulating the sequence.
#define CV_IN_3_GPIO (2)           // For external CV input. Modulating the sequence.
#define CV_IN_4_GPIO (3)           // For external CV input. Modulating the sequence.
#define CLOCK_IN_GPIO (6)          // For external clock input.
#define SWITCH_RANDOM_GPIO (5)     // For randomize the sequence.
#define SWITCH_REVERSE_GPIO (7)    // For reverse the sequence.
#define BUTTON_START_STOP_GPIO (8) // For start and stop the sequencer.
#define BUTTON_BANK_A_GPIO (9)     // For select pattern in bank A.
#define BUTTON_BANK_B_GPIO (12)    // For select pattern in bank B.
#define CV_OUT_1_GPIO (20)
#define CV_OUT_2_GPIO (21)
#define CV_OUT_3_GPIO (22)
#define CV_OUT_4_GPIO (23)
#define CV_OUT_5_GPIO (10)
#define CV_OUT_6_GPIO (11)
#define CV_OUT_7_GPIO (15)
#define CV_OUT_8_GPIO (18)
#define CV_OUT_9_GPIO (19)
#define CV_OUT_10_GPIO (13)
#define FREQUENCY_RANGE_MIN (27.50)   // A0 Note
#define FREQUENCY_RANGE_MAX (4186.01) // C8 Note

int tempo_bpm;  // It should be set by the user.
int delay_time; // Calculate delay time for each beat

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
  // Default tempo is 120 BPM.
  tempo_bpm = 120;
  delay_time = 60000 / tempo_bpm;

  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_13_BIT,
      .freq_hz = TIMER_FREQUENCY,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ledc_timer_config(&ledc_timer);

  // Setup the array of CV_OUT GPIOs.
  int cv_out_gpios[] = {
      CV_OUT_1_GPIO,
      CV_OUT_2_GPIO,
      CV_OUT_3_GPIO,
      CV_OUT_4_GPIO,
      CV_OUT_5_GPIO,
      CV_OUT_6_GPIO,
      CV_OUT_7_GPIO,
      CV_OUT_8_GPIO,
      CV_OUT_9_GPIO,
      CV_OUT_10_GPIO};

  int cv_out_gpios_size = sizeof(cv_out_gpios) / sizeof(cv_out_gpios[0]);

  // Initialize the configuration for the CV_OUT ledc channels.
  for (int i = 0; i < cv_out_gpios_size; i++)
  {
    ledc_channel_config_t ledc_cv_out_channel = {
        .channel = (i < 6) ? LEDC_CHANNEL_0 : LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = cv_out_gpios[i],
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
        .hpoint = 0,
        .flags.output_invert = 0,
    };

    ledc_channel_config(&ledc_cv_out_channel);
  }
}

void step_sequencer_play()
{
  int step = 0;

  while (1)
  {
    // TODO: Implement the step sequencer logic here.

    vTaskDelay(delay_time / portTICK_PERIOD_MS);
    step++;
  }
}

void step_sequencer_stop()
{
  // TODO: Implement the stop functionality here.
}