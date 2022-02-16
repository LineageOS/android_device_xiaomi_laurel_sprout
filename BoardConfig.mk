#
# Copyright (C) 2021 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from sm6125-common
include device/xiaomi/sm6125-common/BoardConfigCommon.mk

DEVICE_PATH := device/xiaomi/laurel_sprout

# A/B
AB_OTA_PARTITIONS += \
    boot \
    dtbo \
    system \
    vbmeta \
    vendor

BOARD_USES_RECOVERY_AS_BOOT := true
TARGET_NO_RECOVERY := true

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := laurel_sprout

# Display
TARGET_SCREEN_DENSITY := 320

# Fingerprint
TARGET_USES_FOD_ZPOS := true

# Kernel
TARGET_KERNEL_CONFIG += vendor/laurel_sprout.config
BOARD_KERNEL_CMDLINE += androidboot.boot_devices=soc/4804000.ufshc

# Partitions
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 3221225472
BOARD_VENDORIMAGE_PARTITION_SIZE := 1073741824

# Properties
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# Recovery
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/rootdir/etc/fstab.qcom

# Security patch level - V12.0.26.0.RFQMIXM
BOOT_SECURITY_PATCH := 2022-08-01
VENDOR_SECURITY_PATCH := $(BOOT_SECURITY_PATCH)

# Inherit from the proprietary version
include vendor/xiaomi/laurel_sprout/BoardConfigVendor.mk
