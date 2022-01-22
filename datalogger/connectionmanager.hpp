#pragma once

#include <string>
#include <vector>

namespace sensemon
{
    class ConnectionManager
    {
    public:
        explicit ConnectionManager() :mConnected(false) {};
        virtual ~ConnectionManager() {};
        virtual bool openDevicePort(const std::string&) = 0;
        virtual bool connectToDevice(const std::string&) = 0;
        virtual void disconnectFromDevice() = 0;
        virtual bool isConnected() = 0;
        virtual void getDeviceList(std::vector<std::string>&) = 0;

    private:
        bool mConnected;
    };
}