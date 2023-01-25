/**
 * \file side_lights.c
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

#include "main_lights.h"
#include "side_lights.h"

typedef enum
{
  NO_EVENT = 0,
  TRIGGER,
  SIGNAL,
  TIMEOUT,
} side_lights_event_t;

#define ALL_PROCESS_STATE \
 ADD_STATE(RED_STATE, red_handler, red_entry_handler, red_exit_handler) \
 ADD_STATE(WAIT_STATE, wait_handler, wait_entry_handler, wait_exit_handler) \
 ADD_STATE(RED_YELLOW_STATE, red_yellow_handler, red_yellow_entry_handler, red_yellow_exit_handler) \
 ADD_STATE(GREEN_STATE, green_handler, green_entry_handler, green_exit_handler) \
 ADD_STATE(YELLOW_STATE, yellow_handler, yellow_entry_handler, yellow_exit_handler)

#define ADD_STATE(name, ...)  name,
typedef enum
{
  ALL_PROCESS_STATE
}side_lights_state_t;
#undef ADD_STATE

#define SIZE_ONE    (1)
#define TIMEOUT     (5)
#define TIMER_RESET (0)

static state_machine_result_t red_handler(state_machine_t* const State);
static state_machine_result_t red_entry_handler(state_machine_t* const State);
static state_machine_result_t red_exit_handler(state_machine_t* const State);

static state_machine_result_t wait_handler(state_machine_t* const State);
static state_machine_result_t wait_entry_handler(state_machine_t* const State);
static state_machine_result_t wait_exit_handler(state_machine_t* const State);

static state_machine_result_t red_yellow_handler(state_machine_t* const State);
static state_machine_result_t red_yellow_entry_handler(state_machine_t* const State);
static state_machine_result_t red_yellow_exit_handler(state_machine_t* const State);

static state_machine_result_t green_handler(state_machine_t* const State);
static state_machine_result_t green_entry_handler(state_machine_t* const State);
static state_machine_result_t green_exit_handler(state_machine_t* const State);

static state_machine_result_t yellow_handler(state_machine_t* const State);
static state_machine_result_t yellow_entry_handler(state_machine_t* const State);
static state_machine_result_t yellow_exit_handler(state_machine_t* const State);

static void dispatch(side_lights_t* const pSideLights, const uint32_t event);

#define ADD_STATE(name, state_handler, entry_handler, exit_handler) \
[name] = {                  \
  .Handler = state_handler, \
  .Entry   = entry_handler, \
  .Exit    = exit_handler,  \
  .Id      = name,          \
},

static const state_t Side_Lights_States[] =
{
  ALL_PROCESS_STATE
};

#undef ADD_STATE

void init_side_lights(side_lights_t* const pSideLights,
                      main_lights_t* const pMainLights,
                      state_machine_event_logger event_logger,
                      state_machine_result_logger result_logger)
{
  pSideLights->Machine.State = &Side_Lights_States[RED_STATE];
  pSideLights->Machine.Event = NO_EVENT;
  pSideLights->Timer = TIMER_RESET;
  pSideLights->pMainLights = pMainLights;
  pSideLights->event_logger = event_logger;
  pSideLights->result_logger = result_logger;

  (void)red_entry_handler((state_machine_t *)pSideLights);
}

static state_machine_result_t red_entry_handler(state_machine_t* const pState)
{
  (void)(pState);
  printf("Side Lights: RED\n");
  return EVENT_HANDLED;
}

static state_machine_result_t red_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case TRIGGER:
    return switch_state(pState, &Side_Lights_States[WAIT_STATE]);
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

static state_machine_result_t wait_entry_handler(state_machine_t* const pState)
{
  printf("Side Lights: WAITING (RED)\n");
  side_lights_t* pSideLights = (side_lights_t*)pState;
  stop_main_lights(pSideLights->pMainLights);
  return EVENT_HANDLED;
}

static state_machine_result_t wait_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case SIGNAL:
    return switch_state(pState, &Side_Lights_States[RED_YELLOW_STATE]);
  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t wait_exit_handler(state_machine_t* const pState)
{
  (void)(pState);
  return EVENT_HANDLED;
}

static state_machine_result_t red_yellow_entry_handler(state_machine_t* const pState)
{
  printf("Side Lights: RED_YELLOW\n");
  side_lights_t* pSideLights = (side_lights_t*)pState;
  pSideLights->Timer = TIMEOUT;
  return EVENT_HANDLED;
}

static state_machine_result_t red_yellow_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case TIMEOUT:
    return switch_state(pState, &Side_Lights_States[GREEN_STATE]);
  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t red_yellow_exit_handler(state_machine_t* const pState)
{
  side_lights_t* pSideLights = (side_lights_t*)pState;
  pSideLights->Timer = TIMER_RESET;
  return EVENT_HANDLED;
}

static state_machine_result_t green_entry_handler(state_machine_t* const pState)
{
  printf("Side Lights: GREEN\n");
  side_lights_t* pSideLights = (side_lights_t*)pState;
  pSideLights->Timer = TIMEOUT;
  return EVENT_HANDLED;
}

static state_machine_result_t green_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case TIMEOUT:
    return switch_state(pState, &Side_Lights_States[YELLOW_STATE]);
  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t green_exit_handler(state_machine_t* const pState)
{
  side_lights_t* pSideLights = (side_lights_t*)pState;
  pSideLights->Timer = TIMER_RESET;
  return EVENT_HANDLED;
}

static state_machine_result_t yellow_entry_handler(state_machine_t* const pState)
{
  printf("Side Lights: YELLOW\n");
  side_lights_t* pSideLights = (side_lights_t*)pState;
  pSideLights->Timer = TIMEOUT;
  return EVENT_HANDLED;
}

static state_machine_result_t yellow_handler(state_machine_t* const pState)
{
  switch(pState->Event)
  {
  case TIMEOUT:
    return switch_state(pState, &Side_Lights_States[RED_STATE]);
  default:
    return EVENT_UN_HANDLED;
  }
  return EVENT_HANDLED;
}

static state_machine_result_t yellow_exit_handler(state_machine_t* const pState)
{
  side_lights_t* pSideLights = (side_lights_t*)pState;
  pSideLights->Timer = TIMER_RESET;
  start_main_lights(pSideLights->pMainLights);
  return EVENT_HANDLED;
}

static void dispatch(side_lights_t* const pSideLights, const uint32_t event)
{
  pSideLights->Machine.Event = event;

  if (dispatch_event((state_machine_t* const*)&pSideLights,
                     SIZE_ONE,
                     pSideLights->event_logger,
                     pSideLights->result_logger) == EVENT_UN_HANDLED)
  {
    printf("invalid event entered\n");
  }
}

void trigger_side_lights(side_lights_t* const pSideLights)
{
  dispatch(pSideLights, TRIGGER);
}

void signal_side_lights(side_lights_t* const pSideLights)
{
  dispatch(pSideLights, SIGNAL);
}

void on_side_lights_timeout(side_lights_t* const pSideLights)
{
  dispatch(pSideLights, TIMEOUT);
}
