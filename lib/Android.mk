################################################################################
# Android libsmaf makefile                                                     #
################################################################################
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_CFLAGS += -Wall

LOCAL_SRC_FILES += libsmaf.c

LOCAL_C_INCLUDES := $(LOCAL_PATH) \

LOCAL_MODULE := libsmaf
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
