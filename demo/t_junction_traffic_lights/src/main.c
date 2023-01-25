/**
 * @file main.c
 * @author Greg Brooks (greg.brooks@gregox.com)
 * @brief
 * @date 2023-01-25
 *
 * Copyright (c) Gregory Brooks 2023
 *
 * Distributed under the MIT License, (See accompanying
 * file LICENSE or copy at https://mit-license.org/)
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "hsm.h"
#include "main_lights.h"
#include "side_lights.h"

main_lights_t MainLights;
side_lights_t SideLights;

char input_char;
pthread_mutex_t mtx;

void event_logger(uint32_t stateMachine, uint32_t state, uint32_t event)
{
  (void)stateMachine;
  (void)state;
  (void)event;
  // printf("State Machine: %d, State: %d, Event: %d\n", stateMachine, state, event);
}

void result_logger(uint32_t state, state_machine_result_t result)
{
  (void)state;
  (void)result;
  // printf("Result: %d, New State: %d\n", result, state);
}

void* console(void* vargp)
{
  (void)(vargp);
  while(1)
  {
    // Get input from console
    char input  = getchar();

    // ignore new line input
    if((input == '\n' ) ||(input == '\r'))
    {
      continue;
    }

    pthread_mutex_lock(&mtx);
    input_char = input;
    pthread_mutex_unlock(&mtx);

  }
}

int main(void)
{
  init_main_lights(&MainLights, &SideLights, event_logger, result_logger);
  init_side_lights(&SideLights, &MainLights, event_logger, result_logger);

  pthread_t console_thread;
  pthread_create(&console_thread, NULL, console, NULL);

  while(1)
  {
    sleep(1);

    pthread_mutex_lock(&mtx);
    char input = input_char;
    input_char = 0;
    pthread_mutex_unlock(&mtx);

    if (input != 0)
    {
      trigger_side_lights(&SideLights);
      printf("\n");
    }

    if(MainLights.Timer > 0)
    {
      MainLights.Timer--;

      if(MainLights.Timer == 0)
      {
        on_main_lights_timeout(&MainLights);  // Generate the timeout event
        printf("\n");
      }
    }

    if(SideLights.Timer > 0)
    {
      SideLights.Timer--;

      if(SideLights.Timer == 0)
      {
        on_side_lights_timeout(&SideLights);
        printf("\n");
      }
    }
  }
  return 0;
}

