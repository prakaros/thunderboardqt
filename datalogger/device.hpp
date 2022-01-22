#pragma once

#include <string>
#include <vector>
#include "cmdinterface.hpp"

namespace sensemon
{
    enum class DeviceState
    {
        IDLE,
        READY,
        BUSY,
        DEV_ERROR
    };

    class Device
    {
    public:
        Device() : mDevState(DeviceState::IDLE)
        {}
        virtual ~Device() {};
        std::string  getDeviceName() const { return mDeviceName; }
        DeviceState  getDeviceState() const { return mDevState; }
        virtual bool connectToChannel() = 0;
        virtual void disconnectFromChannel() = 0;
        virtual int  readData(uint8_t*, int readtimeout) = 0;
        virtual int  writeData(const uint8_t*, int len) = 0;
        virtual bool sendCommand(const CmdInterface& cmdId) = 0;
        virtual bool receivedResponse(CmdInterface& cmdId) = 0;
    protected:
        std::string mDeviceName;
        DeviceState mDevState;
        std::vector<uint8_t> mBuffer;
    };

}
