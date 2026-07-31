#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixup_remove,
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)
namespace_imports = [
    'hardware/qcom-caf/sm8150',
    'hardware/qcom-caf/wlan',
    'vendor/qcom/opensource/display',
    'vendor/xiaomi/sm6125-common',
]
lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
}

blob_fixups: blob_fixups_user_type = {
    'vendor/lib/miwatermark.so': blob_fixup()
        .add_needed('libpiex_shim.so'),
    # get_force_path() stores "/dev/input" without a terminating NUL.
    'vendor/lib64/hw/fingerprint.goodix_fod.default.so': blob_fixup()
        .sig_replace('e9 d3 00 79', 'e9 6b 00 b9'),
    'vendor/lib64/libvendor.goodix.hardware.biometrics.fingerprint@2.1.so': blob_fixup()
        .remove_needed('libhidlbase.so')
        .replace_needed('libhidltransport.so', 'libhidlbase-v32.so'),
    'vendor/lib/libalLDC.so': blob_fixup()
        .clear_symbol_version('AHardwareBuffer_allocate')
        .clear_symbol_version('AHardwareBuffer_describe')
        .clear_symbol_version('AHardwareBuffer_lock')
        .clear_symbol_version('AHardwareBuffer_release')
        .clear_symbol_version('AHardwareBuffer_unlock'),
    'vendor/lib/libmmcamera_faceproc.so': blob_fixup()
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),
    'vendor/lib/libVDSuperPhotoAPI.so': blob_fixup()
        .clear_symbol_version('remote_handle_close')
        .clear_symbol_version('remote_handle_invoke')
        .clear_symbol_version('remote_handle_open'),
}  # fmt: skip

module = ExtractUtilsModule(
    'laurel_sprout',
    'xiaomi',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
    add_firmware_proprietary_file=True,
)

if __name__ == '__main__':
    utils = ExtractUtils.device_with_common(
        module, 'sm6125-common', module.vendor
    )
    utils.run()
