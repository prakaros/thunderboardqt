#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <QtCore\QTimer>
#include <QtSerialPort\QSerialPortInfo>
#include "applicationtypes.hpp"
#include "connectionmanager.hpp"
#include "device.hpp"

//Need to declare interface with QObject, the name string must be unique 
//Andd using namespace and declaration must be done outside namespace
Q_DECLARE_INTERFACE(sensemon::ConnectionManager, "ConnectionManager/1.0")

namespace sensemon
{


    class ConnectionManagerVCOM : public QObject, public ConnectionManager
    {
        Q_OBJECT
            Q_INTERFACES(sensemon::ConnectionManager)
    public:
        explicit ConnectionManagerVCOM();
        virtual ~ConnectionManagerVCOM();
        virtual bool openDevicePort(const std::string&);
        virtual bool connectToDevice(const std::string& dev);
        virtual void disconnectFromDevice();
        virtual bool isConnected();
        virtual void getDeviceList(std::vector<std::string>&);
        void processData();
        std::shared_ptr<Device> getConnectedDevice() { return mComDev; }
        bool executeGetPeripheralList(const std::string& dev);
        bool executePeripheralDecp(const std::string& peripheral);
        PeripheralList getPeripheralList() { return mCurrentPList; }
        SignalDescpList_t getSignalDescList() { return mCurrentSignalList; }
        bool executeStartLog(int index);
        bool executeStopLog(int index);
        public slots:
        void getData();
    private:
        std::vector<std::string> mVCOMDeviceList;
        std::string mSelectedDevice;
        bool mDeviceConnected;
        bool mStartCommThread;
        std::shared_ptr<Device> mComDev;
        QSerialPortInfo mDevSerialPortInfo;
        QTimer *mTimer;
        std::vector<uint8_t> mDataBuffer;
        std::thread mCommThread;
        PeripheralList mCurrentPList;
        SignalDescpList_t mCurrentSignalList;
        SignalDescpList_t mCurrentLogSigalList;

        peripheral_t mPeriPheral;
    };
}

