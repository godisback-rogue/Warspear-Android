#pragma once

#include <android/log.h>

#define WS_LOG_TAG "WS_SDK"

#define ALOGV(fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, WS_LOG_TAG, fmt, ##__VA_ARGS__)
#define ALOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG,   WS_LOG_TAG, fmt, ##__VA_ARGS__)
#define ALOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO,    WS_LOG_TAG, fmt, ##__VA_ARGS__)
#define ALOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN,    WS_LOG_TAG, fmt, ##__VA_ARGS__)
#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR,   WS_LOG_TAG, fmt, ##__VA_ARGS__)
