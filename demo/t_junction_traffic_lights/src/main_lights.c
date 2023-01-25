/**
 * \file main_lights.c
 * \brief
 *
 * \author  Greg Brooks (greg.brooks@gregox.com)
 * \date    25 January 2023
 *
 *  Copyright (c) Greg Brooks 2023
 *
 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

#include <stdio.h>

#include "side_lights.h"
#include "main_lights.h"

typedef enum
{
  NO_EVENT = 0,
  STOP,
  START,
  TIMEOUT,
} main_lights_event_t;

#define ALL_PROCESS_STATE \
 ADD_STATE(GREEN_STATE, green_handler, green_entry_handler, green_exit_handler) \
 ADD_STATE(YELLOW_STATE, yellow_handler, yellow_entry_handler, yellow_exit_handler) \
 ADD_STATE(RED_STATE, red_handler, red_entry_handler, red_exit_handler) \
 ADD_STATE(RED_YELLOW_STATE, red_yellow_handler, red_yellow_entry_handler, red_yellow_exit_handler)

#define ADD_STATE(name, ...)  name,
typedef enum
{
  ALL_PROCESS_STATE
}main_lights_state_t;
#undef ADD_STATE

#define SIZE_ONE    (1)
#define TIMEOUT     (5)
#define TIMER_RESET (0)

static state_machine_result_t green_handler(state_machine_t* const State);
static state_machine_result_t green_entry_handler(state_machine_t* const State);
static state_machine_result_t green_exit_handler(state_machine_t* const State);

static state_machine_result_t yellow_handler(state_machine_t* const State);
static state_machine_result_t yellow_entry_handler(state_machine_t* const State);
static state_machine_result_t yellow_exit_handler(state_machine_t* const State);

static state_machine_result_t red_handler(state_machine_t* const State);
static state_machine_result_t red_entry_handler(state_machine_t* const State);
static state_machine_result_t red_exit_handler(state_machine_t* const State);

static state_machine_result_t red_yellow_handler(state_machine_t* const State);
static state_machine_result_t red_yellow_entry_handler(state_machine_t* const State);
static state_machine_result_t red_yellow_exit_handler(state_machine_t* const State);

static void dispatch(main_lights_t* const pMainLights, const uint32_t event);

#define ADD_STATE(name, state_handler, entry_handler, exit_handler) \
[name] = {                  \
  .Handler = state_handler, \
  .Entry   = entry_handler, \
  .Exit    = exit_handler,  \
  .Id      = name,          \
},

static const state_t Main_Lights_States[] =
{
  ALL_PROCESS_STATE
};

#undef ADD_STATE

void init_main_lights(main_lights_t* const pMainLights,
                      side_lights_t* const pSideLights,
                      state_machine_event_logger event_logger,
                      state_machine_result_logger result_logger)
{
  pMainLights->Machine.State = &Main_Lights_States[GREEN_STATE];
  pMainLights->Machine.Event = NO_EVENT;
  pMainLights->Timer = TIMER_RESET;
  pMainLights->pSideLights = pSideLights;
  pMainLights->event_logger = event_logger;
  pMainLights->result_logger = result_logger;

  (void)green_entry_handler((state_machine_t *)pMainLights);
}

static state_machine_result_t green_entry_handler(state_machine_t* const pState)
{
  (void)(pState);
  printf("Main Lights: GREEN\n");
  return EVENT_HANDLED;
}

static state_machine_result_t green_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case STOP:
    return switch_state(pState, &Main_Lights_States[YELLOW_STATE]);

  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t green_exit_handler(state_machine_t* const pState)
{
  (void)(pState);
  return EVENT_HANDLED;
}

static state_machine_result_t yellow_entry_handler(state_machine_t* const pState)
{
  printf("Main Lights: YELLOW\n");
  main_lights_t* pMainLights = (main_lights_t*)pState;
  pMainLights->Timer = TIMEOUT;
  return EVENT_HANDLED;
}

static state_machine_result_t yellow_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case TIMEOUT:
    return switch_state(pState, &Main_Lights_States[RED_STATE]);
  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t yellow_exit_handler(state_machine_t* const pState)
{
  main_lights_t* pMainLights = (main_lights_t*)pState;
  pMainLights->Timer = TIMER_RESET;
  return EVENT_HANDLED;
}

static state_machine_result_t red_entry_handler(state_machine_t* const pState)
{
  printf("Main Lights: RED\n");
  main_lights_t* pMainLights = (main_lights_t*)pState;
  signal_side_lights(pMainLights->pSideLights);
  return EVENT_HANDLED;
}

static state_machine_result_t red_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case START:
    return switch_state(pState, &Main_Lights_States[RED_YELLOW_STATE]);

  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t red_exit_handler(state_machine_t* const pState)
{
  (void)(pState);
  return EVENT_HANDLED;
}

static state_machine_result_t red_yellow_entry_handler(state_machine_t* const pState)
{
  printf("Main Lights: RED_YELLOW\n");
  main_lights_t* pMainLights = (main_lights_t*)pState;
  pMainLights->Timer = TIMEOUT;
  return EVENT_HANDLED;
}

static state_machine_result_t red_yellow_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case TIMEOUT:
    return switch_state(pState, &Main_Lights_States[GREEN_STATE]);

  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t red_yellow_exit_handler(state_machine_t* const pState)
{
  main_lights_t* pMainLights = (main_lights_t*)pState;
  pMainLights->Timer = TIMER_RESET;
  return EVENT_HANDLED;
}

static void dispatch(main_lights_t* const pMainLights, const uint32_t event)
{
  pMainLights->Machine.Event = event;

  if (dispatch_event((state_machine_t* const*)&pMainLights,
                     SIZE_ONE,
                     pMainLights->event_logger,
                     pMainLights->result_logger) == EVENT_UN_HANDLED)
  {
    printf("invalid event entered\n");
  }
}

void stop_main_lights(main_lights_t* const pMainLights)
{
  dispatch(pMainLights, STOP);
}

void start_main_lights(main_lights_t* const pMainLights)
{
  dispatch(pMainLights, START);
}

void on_main_lights_timeout(main_lights_t* const pMainLights)
{
  dispatch(pMainLights, TIMEOUT);
}
