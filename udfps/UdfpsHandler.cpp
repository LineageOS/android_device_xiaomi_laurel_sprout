/*
 * Copyright (C) 2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "UdfpsHandler.laurel_sprout"

#include "UdfpsHandler.h"

#include <aidl/android/hardware/biometrics/fingerprint/BnFingerprint.h>
#include <android-base/logging.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <fstream>
#include <thread>

#define COMMAND_NIT 10
#define PARAM_NIT_FOD 1
#define PARAM_NIT_NONE 0

using ::aidl::android::hardware::biometrics::fingerprint::AcquiredInfo;

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

static const char* kFodUiPaths[] = {
        "/sys/devices/platform/soc/soc:qcom,dsi-display-primary/fod_ui",
        "/sys/devices/platform/soc/soc:qcom,dsi-display/fod_ui",
};

static const char* kFodStatusPath = "/sys/class/touch/tp_dev/fod_status";

static bool readBool(int fd) {
    char c;
    int rc;

    rc = lseek(fd, 0, SEEK_SET);
    if (rc) {
        LOG(ERROR) << "failed to seek fd, err: " << rc;
        return false;
    }

    rc = read(fd, &c, sizeof(char));
    if (rc != 1) {
        LOG(ERROR) << "failed to read bool from fd, err: " << rc;
        return false;
    }

    return c != '0';
}

class LaurelSproutUdfpsHandler : public UdfpsHandler {
  public:
    void init(fingerprint_device_t* device) {
        mDevice = device;

        std::thread([this]() {
            int fd;
            for (auto& path : kFodUiPaths) {
                fd = open(path, O_RDONLY);
                if (fd >= 0) {
                    break;
                }
            }

            if (fd < 0) {
                LOG(ERROR) << "failed to open fod_ui, err: " << fd;
                return;
            }

            int fodStatusFd = open(kFodStatusPath, O_RDWR);
            if (fodStatusFd < 0) {
                LOG(ERROR) << "failed to open fod_status, err: " << fd;
                return;
            }

            struct pollfd fodUiPoll = {
                    .fd = fd,
                    .events = POLLERR | POLLPRI,
                    .revents = 0,
            };

            while (true) {
                int rc = poll(&fodUiPoll, 1, -1);
                if (rc < 0) {
                    LOG(ERROR) << "failed to poll fd, err: " << rc;
                    continue;
                }

                bool fodUiEnabled = readBool(fd);
                mDevice->extCmd(mDevice, COMMAND_NIT,
                                fodUiEnabled ? PARAM_NIT_FOD : PARAM_NIT_NONE);
                if (fodStatusFd >= 0) {
                    write(fodStatusFd, fodUiEnabled ? "1" : "0", 1);
                }
            }
        }).detach();
    }

    void onFingerDown(uint32_t /*x*/, uint32_t /*y*/, float /*minor*/, float /*major*/) {
        // nothing
    }

    void onFingerUp() {
        // nothing
    }

    void onAcquired(int32_t result, int32_t vendorCode) {
        if (static_cast<AcquiredInfo>(result) == AcquiredInfo::VENDOR) {
            if (vendorCode == 22) {
                /*
                 * vendorCode = 22 onFingerDown
                 */
                mDevice->extCmd(mDevice, COMMAND_NIT, PARAM_NIT_FOD);
            } else if (vendorCode == 23) {
                /*
                 * vendorCode = 23 onFingerUp
                 */
                mDevice->extCmd(mDevice, COMMAND_NIT, PARAM_NIT_NONE);
            }
        }
    }

    void onAuthenticationSucceeded() override {
        mDevice->extCmd(mDevice, COMMAND_NIT, PARAM_NIT_NONE);
    }

    void onAuthenticationFailed() override {
        mDevice->extCmd(mDevice, COMMAND_NIT, PARAM_NIT_NONE);
    }

    void cancel() { mDevice->extCmd(mDevice, COMMAND_NIT, PARAM_NIT_NONE); }

  private:
    fingerprint_device_t* mDevice;
};

static UdfpsHandler* create() {
    return new LaurelSproutUdfpsHandler();
}

static void destroy(UdfpsHandler* handler) {
    delete handler;
}

extern "C" UdfpsHandlerFactory UDFPS_HANDLER_FACTORY = {
        .create = create,
        .destroy = destroy,
};
