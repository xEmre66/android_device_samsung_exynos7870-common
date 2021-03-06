#
# Copyright (C) 2017 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := device/samsung/exynos7870-common

# Include path
TARGET_SPECIFIC_HEADER_PATH := $(LOCAL_PATH)/include

# Release tools
TARGET_RELEASETOOLS_EXTENSIONS := $(LOCAL_PATH)

# Seccomp filters
BOARD_SECCOMP_POLICY := $(LOCAL_PATH)/seccomp

# SELinux
BOARD_SEPOLICY_DIRS := $(LOCAL_PATH)/sepolicy

# inherit the splitted configs
-include $(LOCAL_PATH)/board/*.mk
