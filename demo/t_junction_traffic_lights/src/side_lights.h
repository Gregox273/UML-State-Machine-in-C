#ifndef SIDE_LIGHTS_H
#define SIDE_LIGHTS_H

/**
 * \file side_lights.h
 *
 * \author  Greg Brooks (greg.brooks@gregox.com)
 * \date    25 January 2023
 *
 *  Copyright (c) Greg Brooks 2023
 *
 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 *
 */

#include <stdint.h>

#include "hsm.h"

typedef struct main_lights_t main_lights_t;

typedef struct side_lights_t
{
  state_machine_t Machine;
  uint32_t Timer;
  main_lights_t* pMainLights;
  state_machine_event_logger event_logger;
  state_machine_result_logger result_logger;
} side_lights_t;

extern void init_side_lights(side_lights_t* const pSideLights,
                             main_lights_t* const pMainLights,
                             state_machine_event_logger event_logger,
                             state_machine_result_logger result_logger);

extern void trigger_side_lights(side_lights_t* const pSideLights);

extern void signal_side_lights(side_lights_t* const pSideLights);

extern void on_side_lights_timeout(side_lights_t* const pSideLights);

#endif // SIDE_LIGHTS_H
