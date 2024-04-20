#include <stdlib.h>
#include <stdio.h>
#include "step_sequencer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TEST_DUTY (4000)
#define LEDC_TEST_FADE_TIME (3000)

/*
 * This callback function will be called when fade operation has ended
 * Use callback only if you are aware it is being called inside an ISR
 * Otherwise, you can use a semaphore to unblock tasks
 */
static IRAM_ATTR bool cb_ledc_fade_end_event(const ledc_cb_param_t *param, void *user_arg)
{
  BaseType_t taskAwoken = pdFALSE;

  if (param->event == LEDC_FADE_END_EVT)
  {
    SemaphoreHandle_t counting_sem = (SemaphoreHandle_t)user_arg;
    xSemaphoreGiveFromISR(counting_sem, &taskAwoken);
  }

  return (taskAwoken == pdTRUE);
}

void step_sequencer_init()
{
  // Initialize dummy step data. It's an array of StepData structs.
  // Create a dummy sequence with 16 steps and random number of pitch, modulation (freq), and gate values (1 and 0).
  StepData dummy_sequence[16];
  for (int i = 0; i < 16; i++)
  {
    dummy_sequence[i].step = i;
    dummy_sequence[i].pitch = 440.0 + (rand() % 1000) / 100.0;
    dummy_sequence[i].gate = rand() % 2;
  }

  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_13_BIT,
      .freq_hz = 4000,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel_1 = {
      .channel = LEDC_CHANNEL_0,
      .duty = 0,
      .gpio_num = 15,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_sel = LEDC_TIMER_0,
      .hpoint = 0,
      .flags.output_invert = 0,
  };

  ledc_channel_config(&ledc_channel_1);

  ledc_fade_func_install(0);

  ledc_cbs_t callbacks = {
      .fade_cb = cb_ledc_fade_end_event};

  SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(1, 0);

  ledc_cb_register(ledc_channel_1.speed_mode, ledc_channel_1.channel, &callbacks, (void *)counting_sem);

  while (1)
  {
    printf("1. LEDC fade up to duty = %d\n", LEDC_TEST_DUTY);

    ledc_set_fade_with_time(ledc_channel_1.speed_mode,
                            ledc_channel_1.channel, LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel_1.speed_mode,
                    ledc_channel_1.channel, LEDC_FADE_NO_WAIT);

    xSemaphoreTake(counting_sem, portMAX_DELAY);

    printf("2. LEDC fade down to duty = 0\n");
    ledc_set_fade_with_time(ledc_channel_1.speed_mode,
                            ledc_channel_1.channel, 0, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel_1.speed_mode,
                    ledc_channel_1.channel, LEDC_FADE_NO_WAIT);

    xSemaphoreTake(counting_sem, portMAX_DELAY);

    printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY);
    ledc_set_duty(ledc_channel_1.speed_mode, ledc_channel_1.channel, LEDC_TEST_DUTY);
    ledc_update_duty(ledc_channel_1.speed_mode, ledc_channel_1.channel);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("4. LEDC set duty = 0 without fade\n");
    ledc_set_duty(ledc_channel_1.speed_mode, ledc_channel_1.channel, 0);
    ledc_update_duty(ledc_channel_1.speed_mode, ledc_channel_1.channel);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
