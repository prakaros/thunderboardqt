#include <QtSerialPort\QSerialPort>
#include <QtSerialPort\QSerialPortInfo>
#include <QtCore\QList>
#include <QtCore\QDebug>
#include <Windows.h>
#include "ConnectionManagerVCOM.hpp"
#include "ApplicationTypes.hpp"
#include "vcomdevice.hpp"
#include "cmdinterface.hpp"
#include "cmdresponse.hpp"
#include "dataplotter.hpp"


namespace sensemon
{

    ConnectionManagerVCOM::ConnectionManagerVCOM() : 
                            mDeviceConnected(false)
                            , mStartCommThread(false)
                            , mDataBuffer(1024)
                            , mComDev(0)
    {

    }

    ConnectionManagerVCOM::~ConnectionManagerVCOM()
    {

    }

    bool ConnectionManagerVCOM::openDevicePort(const std::string& dev)
    {
#if defined(SIMULATED_DEVICE)
        mDeviceConnected = false;
        mSelectedDevice = dev;
        qDebug() << "Connect to Device " << QString::fromStdString(mSelectedDevice);
        //mDevSerialPortInfo;
        mComDev = std::make_shared<VCOMDevice>(mDevSerialPortInfo);
        if (mSelectedDevice == "SimCOMDevice1")
        {
            mDeviceConnected = true;
        }
        else
        {
          
            if (mComDev->connectToChannel())
            {
                mDeviceConnected = true;
                //    mTimer = new QTimer(this);
               //     QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(getData()));
                 //   mTimer->start(500);
            }
        }
        QString result = mDeviceConnected ? QString("Connected") : QString("Not Connected");
        qDebug() << "Device Status " << result;
        return mDeviceConnected;
#else
        return true;
#endif
    }


    bool ConnectionManagerVCOM::connectToDevice(const std::string& dev)
    {
        bool bResult = false;
#if defined(SIMULATED_DEVICE)
        if (mDeviceConnected && (dev == "SimCOMDevice1"))
        {
            bResult = true;
            qDebug() << "Sim COM Device 1 Connected ";
        }
        else
        {
            if (mDeviceConnected && (mComDev->getDeviceState() == DeviceState::READY))
            {
                ConnectCmd connectCmd;
                connectCmd.setCommand(ConnectCmdId::CMD_CONNECT);
                connectCmd.sendDeviceName(dev);
                mComDev->sendCommand(connectCmd);
                ConnectRsp rspMsg;
                bResult = mComDev->receivedResponse(rspMsg);

                if (!bResult)
                    qDebug() << "Invalid Connect Resp ";
                return bResult;

                if (rspMsg.getResponseId() == ResponseId::RSP_ACK)
                {
                    bResult = true;
                }
                QString strResult = (rspMsg.getResponseId() == ResponseId::RSP_ACK) ? QString("Ack") : QString("Not Ack");
                qDebug() << "Connect Status " << strResult;
                qDebug() << "Board Name " << QString(rspMsg.getBoardName().c_str());
            }
        }
        return bResult;
#else
        return true;
#endif
    }

    void ConnectionManagerVCOM::disconnectFromDevice()
    {
#if defined(SIMULATED_DEVICE)
        if ((mComDev->getDeviceState() != DeviceState::IDLE)
            && (mComDev->getDeviceState() != DeviceState::DEV_ERROR))
        {
            ConnectCmd disconnectCmd;
            disconnectCmd.setCommand(ConnectCmdId::CMD_DISCONECT);
            disconnectCmd.sendDeviceName(mSelectedDevice);
            mComDev->sendCommand(disconnectCmd);
            ConnectRsp rspMsg;
            mComDev->receivedResponse(rspMsg);
            if (rspMsg.getResponseId() == ResponseId::RSP_ACK)
            {
                mSelectedDevice.clear();
                qDebug() << "Disconnect from Device ";
            }
        }
#else

#endif
    }

    bool ConnectionManagerVCOM::isConnected()
    {
        return mDeviceConnected;
    }

    void ConnectionManagerVCOM::getDeviceList(std::vector<std::string>& devList)
    {
        devList.push_back("SimCOMDevice1");
        devList.push_back("SimCOMDevice2");
        QList<QSerialPortInfo> serialPortList = QSerialPortInfo::availablePorts();
        for (const auto &itr : serialPortList)
        {
            qDebug() << "Serial Description" << itr.description();
            qDebug() << "Port Name " << itr.portName();
            devList.push_back(itr.description().toStdString());
            mDevSerialPortInfo = itr;
        }
    }

    void ConnectionManagerVCOM::getData()
    {
        if (mDeviceConnected)
        {
            //mDataBuffer.clear();
            mComDev->readData(reinterpret_cast<uint8_t*>(&mDataBuffer),100);
        }
    }

    void ConnectionManagerVCOM::processData()
    {
#if 0
        while (mStartCommThread)
        {
            waitForReadyRead(200);
            qint64 bytesAvail = bytesAvailable();
        }
#endif
    }

    bool ConnectionManagerVCOM::executeGetPeripheralList(const std::string& dev)
    {
        bool result = false;
#if defined(SIMULATED_DEVICE)
        if (dev == "SimCOMDevice1")
        {
            PeripheralList simPeriList;
            peripheral_t simPeripheral;
            simPeripheral.periAvailable = true;
            simPeripheral.periId = 3;
            simPeripheral.periNameLen = 8;
            simPeripheral.peripheralName = "SimPeripheral1";
            simPeriList.push_back(simPeripheral);

            simPeripheral.periAvailable = true;
            simPeripheral.periId = 4;
            simPeripheral.periNameLen = 8;
            simPeripheral.peripheralName = "SimPeripheral2";
            simPeriList.push_back(simPeripheral);

            simPeripheral.periAvailable = false;
            simPeripheral.periId = 5;
            simPeripheral.periNameLen = 8;
            simPeripheral.peripheralName = "SimPeripheral3";
            simPeriList.push_back(simPeripheral);
            mCurrentPList = simPeriList;
            result = true;
        }
        else if ((mComDev->getDeviceState() == DeviceState::READY) && mDeviceConnected)
        {
            qDebug() << "Comm Mgr::Execute GetPeripheralList";
            PeripheralListCmd pListCmd;

            mComDev->sendCommand(pListCmd);
            PeripheralListRsp rspMsg;
            mComDev->receivedResponse(rspMsg);
            if (rspMsg.getNumPeripherals() > 0)
            {
                rspMsg.printList();
                mCurrentPList = rspMsg.getperipheralList();
                result = true;
            }
        }
        else
        {

        }
#else

#endif
        return result;
    }

    bool ConnectionManagerVCOM::executePeripheralDecp(const std::string& peripheral)
    {
        bool result = false;
#if defined(SIMULATED_DEVICE)
        if (/*(mComDev->getDeviceState() == DeviceState::READY) &&*/ mDeviceConnected)
        {
            peripheral_t periPheral;
            for (auto listitr : mCurrentPList)
            {
                if (listitr.peripheralName == peripheral)
                {
                    result = true;
                    periPheral = listitr;
                    qDebug() << "Found Peripheral";
                    break;
                }
            }
            if (result && !periPheral.periAvailable)
            {
               // result = false;
            }
            if (result)
            {
                mPeriPheral = periPheral;
                if (mPeriPheral.peripheralName == "SimPeripheral1")
                {
                    SignalDescpList_t simSignalList;
                    signaldescp_t simsignal;
                    simsignal.signalid = 1;
                    simsignal.signalnamelen = 8;
                    simsignal.signalname = "SimSignal1";
                    simsignal.signalMinVal = 0;
                    simsignal.signalMaxVal = 1234;
                    simsignal.signalsamplingrate = 50;
                    simSignalList.push_back(simsignal);

                    simsignal.signalid = 2;
                    simsignal.signalnamelen = 8;
                    simsignal.signalname = "SimSignal2";
                    simsignal.signalMinVal = 0;
                    simsignal.signalMaxVal = 234;
                    simsignal.signalsamplingrate = 50;
                    simSignalList.push_back(simsignal);

                    simsignal.signalid = 3;
                    simsignal.signalnamelen = 16;
                    simsignal.signalname = "SimSignal3";
                    simsignal.signalMinVal = 0;
                    simsignal.signalMaxVal = 2567;
                    simsignal.signalsamplingrate = 50;
                    simSignalList.push_back(simsignal);

                    mCurrentSignalList = simSignalList;
                    DataPlotterScene::getDataPlotterSceneInstance()->setSignalList(mCurrentSignalList);
                    result = true;
                }
                else
                {
                    PeripheralDescCmd pCmd;
                    pCmd.setPeripheralId(periPheral.periId);
                    mComDev->sendCommand(pCmd);
                    PeripheralDescRsp rspMsg;
                    mComDev->receivedResponse(rspMsg);
                    if (rspMsg.getNumSignals() > 0)
                    {
                        rspMsg.printSignalList();
                        mCurrentSignalList = rspMsg.getSignalList();
                        DataPlotterScene::getDataPlotterSceneInstance()->setSignalList(mCurrentSignalList);
                        result = true;
                    }
                    VCOMDevice *vComDev = static_cast<VCOMDevice*>(mComDev.get());
                    vComDev->stopCapture();

                }
            }
        }
#else
#endif
        return result;
    }

    bool ConnectionManagerVCOM::executeStartLog(int index)
    {
        bool result = false;

        if (mDeviceConnected)
        {
            if (mPeriPheral.peripheralName == "SimPeripheral1")
            {
                qDebug() << "SimPeripheral Starting Cap Thread";
                VCOMDevice *vComDev = static_cast<VCOMDevice*>(mComDev.get());
                vComDev->startSimCapture(index);
                mStartCommThread = true;
                result = true;
            }
            else
            {
                PeripheralCaptureCmd pCmd;
                pCmd.setPeripheralCmdData(mPeriPheral.periId, 0, static_cast<uint8_t>(PeripheralCapCmd::START_CAPTURE));
                mComDev->sendCommand(pCmd);
                VCOMDevice *vComDev = static_cast<VCOMDevice*>(mComDev.get());
                vComDev->startCapture(index);
                mStartCommThread = true;
                result = true;
            }

        }

        return result;
    }
    
    bool ConnectionManagerVCOM::executeStopLog(int index)
    {
        bool result = false;
        if ( mDeviceConnected /*&& (mComDev->getDeviceState() == DeviceState::READY)*/)
        {
            if (mPeriPheral.peripheralName == "SimPeripheral1")
            {
                qDebug() << "SimPeripheral Starting Cap Thread";
                VCOMDevice *vComDev = static_cast<VCOMDevice*>(mComDev.get());
                vComDev->stopSimCapture(index);
                mStartCommThread = false;
                result = true;
            }
            else
            {
                
                VCOMDevice *vComDev = static_cast<VCOMDevice*>(mComDev.get());
                if (!vComDev->stopLog(index))
                {
                    vComDev->sendStopCommand();
                   // Sleep(1000);
                    vComDev->stopCapture();
                }
            }
        }
        return result;
    }

}