################################################################################
# Android test_smaf makefile                                                   #
################################################################################
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_CFLAGS += -Wall

LOCAL_SRC_FILES += tests/test_smaf.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib \

LOCAL_SHARED_LIBRARIES := libsmaf
LOCAL_MODULE := test_smaf
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# build smaf lib
include $(LOCAL_PATH)/lib/Android.mk
