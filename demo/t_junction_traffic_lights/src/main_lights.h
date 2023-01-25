#ifndef MAIN_LIGHTS_H
#define MAIN_LIGHTS_H

/**
 * \file main_lights.h
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

typedef struct side_lights_t side_lights_t;

typedef struct main_lights_t
{
  state_machine_t Machine;
  uint32_t Timer;
  side_lights_t* pSideLights;
  state_machine_event_logger event_logger;
  state_machine_result_logger result_logger;
} main_lights_t;

extern void init_main_lights(main_lights_t* const pMainLights,
                             side_lights_t* const pSideLights,
                             state_machine_event_logger event_logger,
                             state_machine_result_logger result_logger);

extern void stop_main_lights(main_lights_t* const pMainLights);

extern void start_main_lights(main_lights_t* const pMainLights);

extern void on_main_lights_timeout(main_lights_t* const pMainLights);

#endif // MAIN_LIGHTS_H
