/*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright (C) 2017 Jesse Chan <cjx123@outlook.com>
 * Copyright (C) 2017 Lukas Berger <mail@lukasberger.at>
 * Copyright (C) 2018 Siddhant Naik <siddhantnaik17@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <hardware/hardware.h>
#include <hardware/power.h>

using namespace std;

#ifndef EXYNOS5_POWER_HAL_H_INCLUDED
#define EXYNOS5_POWER_HAL_H_INCLUDED

/*
 * Macros
 */
#define INPUT_STATE_DISABLE    0
#define INPUT_STATE_ENABLE     1

#define POWER_TOUCHKEYS_ENABLED       "/sys/class/input/input1/enabled"
#define POWER_TOUCHSCREEN_ENABLED     "/sys/class/input/input6/enabled"
#define POWER_TOUCHKEYS_BRIGTHNESS    "/sys/class/sec/sec_touchkey/brightness"

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device);
static void power_init(struct power_module __unused * module);

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data);

/***********************************
 * Inputs
 */
static void power_input_device_state(int state);
static void power_set_interactive(struct power_module __unused * module, int on);

/***********************************
 * Utilities
 */
// C++ I/O
static bool pfwrite(string path, string str);
static bool pfwrite(string path, bool flag);
static bool pfwrite(string path, int value);
static bool pfread(string path, int *v);

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
