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

#define LOG_TAG "power.universal7870"
#define LOG_NDEBUG 0

#include <atomic>
#include <cutils/properties.h>
#include <fcntl.h>
#include <fstream>
#include <future>
#include <hardware/hardware.h>
#include <hardware/power.h>
#include <iostream>
#include <linux/stat.h>
#include <math.h>
#include <pwd.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>

#include "power.h"

using namespace std;

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int input_state_touchkeys = 1;

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device) {
	int retval = 0; // 0 is ok; -1 is error

	ALOGD("%s: enter; name=%s", __func__, name);

	if (strcmp(name, POWER_HARDWARE_MODULE_ID) == 0) {
		power_module_t *dev = (power_module_t *)calloc(1, sizeof(power_module_t));

		if (dev) {
			// Common hw_device_t fields
			dev->common.tag = HARDWARE_DEVICE_TAG;
			dev->common.module_api_version = POWER_MODULE_API_VERSION_0_5;
			dev->common.hal_api_version = HARDWARE_HAL_API_VERSION;

			dev->init = power_init;
			dev->powerHint = power_hint;
			dev->setInteractive = power_set_interactive;

			*device = (hw_device_t *)dev;
		} else {
			retval = -ENOMEM;
		}
	} else {
		retval = -EINVAL;
	}

	ALOGD("%s: exit %d", __func__, retval);
	return retval;
}

static void power_init(struct power_module __unused * module) {
	// initialize all input-devices
	power_input_device_state(1);
}

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data) {
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);
	int value = (data ? *((intptr_t *)data) : 0);

	pthread_mutex_lock(&sec->lock);

	switch (hint) {
		case POWER_HINT_DISABLE_TOUCH:
			ALOGI("%s: hint(POWER_HINT_DISABLE_TOUCH, %d, %llu)", __func__, value, (unsigned long long)data);
			power_input_device_state(value ? 0 : 1);
			break;
		default: break;
	}

	pthread_mutex_unlock(&sec->lock);
}

static void power_input_device_state(int state) {
#if LOG_NDEBUG
	ALOGD("%s: state = %d", __func__, state);
#endif

	switch (state) {
		case INPUT_STATE_DISABLE:
			// save to current state to prevent enabling
			pfread(POWER_TOUCHKEYS_ENABLED, &input_state_touchkeys);
			pfwrite(POWER_TOUCHSCREEN_ENABLED, false);
			pfwrite(POWER_TOUCHKEYS_ENABLED, false);
			pfwrite(POWER_TOUCHKEYS_BRIGTHNESS, 0);
			break;

		case INPUT_STATE_ENABLE:
			pfwrite(POWER_TOUCHSCREEN_ENABLED, true);
			if (input_state_touchkeys) {
				pfwrite(POWER_TOUCHKEYS_ENABLED, true);
				pfwrite(POWER_TOUCHKEYS_BRIGTHNESS, 255);
			}
			break;
	}
	// give hw some milliseconds to properly boot up
	usleep(100 * 1000); // 100ms
}

static void power_set_interactive(struct power_module __unused * module, int on) {
	int screen_is_on = (on != 0);
	power_input_device_state(screen_is_on);
}

/***********************************
 * Utilities
 */
// C++ I/O
static bool pfwrite(string path, string str) {
	ofstream file;

	file.open(path);
	if (!file.is_open()) {
#if LOG_NDEBUG
		ALOGE("%s: failed to open %s", __func__, path.c_str());
#endif
		return false;
	}

#if LOG_NDEBUG
	ALOGI("%s: store \"%s\" to %s", __func__, str.c_str(), path.c_str());
#endif
	file << str;
	file.close();
	return true;
}

static bool pfwrite(string path, bool flag) {
	return pfwrite(path, flag ? 1 : 0);
}

static bool pfwrite(string path, int value) {
	return pfwrite(path, to_string(value));
}

static bool pfread(string path, int *v) {
	ifstream file;

	file.open(path);
	if (!file.is_open()) {
#if LOG_NDEBUG
		ALOGE("%s: failed to open %s", __func__, path.c_str());
#endif
		return false;
	}

	file >> *v;
#if LOG_NDEBUG
	ALOGI("%s: File read value is %d", __func__, *v);
#endif
	file.close();
	return true;
}

static struct hw_module_methods_t power_module_methods = {
	.open = power_open,
};

struct sec_power_module HAL_MODULE_INFO_SYM = {
	.base = {
		.common = {
			.tag = HARDWARE_MODULE_TAG,
			.module_api_version = POWER_MODULE_API_VERSION_0_5,
			.hal_api_version = HARDWARE_HAL_API_VERSION,
			.id = POWER_HARDWARE_MODULE_ID,
			.name = "Power HAL for Exynos 7870 SoCs",
			.author = "Siddhant Naik <siddhantnaik17@gmail.com>",
			.methods = &power_module_methods,
		},

		.init = power_init,
		.powerHint = power_hint,
		.setInteractive = power_set_interactive,
	},

	.lock = PTHREAD_MUTEX_INITIALIZER
};
