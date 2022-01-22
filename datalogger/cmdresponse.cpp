#include "cmdresponse.hpp"
#include <memory>
#include <QtCore\QDebug>

namespace sensemon
{
    uint16_t ConnectCmd::getMessageLength() const
    { 
        size_t len = MAX_DEV_NAME;
        len += sizeof(devnamelen);
        qDebug() << "Devive Name Lenght" << len;
        len += sizeof(static_cast<uint8_t>(mCmd));
        
        qDebug() << "Final Lenght " << len;
        return static_cast<uint16_t>(len);
    }

    void ConnectCmd::copyCommandData(uint8_t *msg) const
    {
        memcpy(msg, (const uint8_t*)&mCmd, sizeof(static_cast<uint8_t>(mCmd)));
        memcpy(msg + sizeof(static_cast<uint8_t>(mCmd)), (const uint8_t*)&devnamelen, sizeof(devnamelen)); //1 Byte for device name lenght
        memset(msg + sizeof(static_cast<uint8_t>(mCmd)) + sizeof(devnamelen), 0, MAX_DEV_NAME);
        memcpy(msg + sizeof(static_cast<uint8_t>(mCmd)) + sizeof(devnamelen), (const uint8_t*)mDeviceName.c_str(), devnamelen);
    }

    uint16_t ConnectRsp::getMessageLength() const
    {
        return static_cast<uint16_t>(sizeof(static_cast<uint8_t>(mRsp)) + sizeof(boardnamelen) + boardnamelen);
    }

    void ConnectRsp::copyResponseData(const uint8_t *rsp)
    {
        mRsp = static_cast<ResponseId>(rsp[0]);
        boardnamelen = rsp[1];
        mBoardName.append((const char *)&rsp[2], boardnamelen);
    }

    uint16_t PeripheralListRsp::getMessageLength() const
    { 
        return static_cast<uint16_t>(sizeof(static_cast<uint8_t>(mNumPeripherals)) +  sizeof(peripheral_t)*mNumPeripherals);
    }

    void PeripheralListRsp::copyResponseData(const uint8_t *rsp)
    {
        uint16_t j = 0;
        mNumPeripherals = rsp[j++];
        for (uint8_t i = 0; i < mNumPeripherals; i++)
        {
            peripheral_t peripheral;
            peripheral.periAvailable = rsp[j++];
            peripheral.periId = rsp[j++];
            uint8_t nameLen = rsp[j++];
            peripheral.peripheralName.append((const char*)&rsp[j], nameLen);
            mPeripheralList.push_back(peripheral);
            j += 32;
        }
    }

    void PeripheralListRsp::printList() const
    {
        for (auto pitr : mPeripheralList)
        {
            qDebug() << "Peripheral Available" << pitr.periAvailable;
            qDebug() << "Peripheral Id" << pitr.periId;
            qDebug() << "Peripheral NameLen" << pitr.periNameLen;
            qDebug() << "Peripheral Name" << QString::fromStdString(pitr.peripheralName);
        }
    }

    uint16_t PeripheralDescCmd::getMessageLength() const
    { 
        return 1;
    }

    void PeripheralDescCmd::copyCommandData(uint8_t *msg) const
    {
        msg[0] = static_cast<uint8_t>(mPeriPheralId);
    }

    uint16_t PeripheralDescRsp::getMessageLength() const
    {
        return sizeof(mPeriPheralId) + sizeof(mPeripheralAvail) + sizeof(mCaptureCmd) + 
                static_cast<uint16_t>(sizeof(static_cast<uint8_t>(mNumSignals))
                + sizeof(signaldescp_t)*mNumSignals);
    }

    void PeripheralDescRsp::copyResponseData(const uint8_t *rsp)
    {
        uint16_t j = 0;
        mPeriPheralId = static_cast<PeripheralId>(rsp[j++]);
        mPeripheralAvail = rsp[j++];
        mCaptureCmd = rsp[j++];
        mNumSignals = rsp[j++];
        for (uint8_t i = 0; i < mNumSignals; i++)
        {
            signaldescp_t signalDescp;
            signalDescp.signalid = rsp[j++];
            signalDescp.signalMinVal = *(int16_t*)&rsp[j];
            j += 2;
            signalDescp.signalMaxVal = *(int16_t*)&rsp[j];
            j += 2;
            signalDescp.signalsamplingrate = *(uint16_t*)&rsp[j];
            j += 2;
            signalDescp.operatingmode = rsp[j++];
            signalDescp.signalnamelen = rsp[j++];
            signalDescp.signalname.append((const char*)&rsp[j], signalDescp.signalnamelen);
            j += 24;
            signalDescp.signalunitnamelen = rsp[j++];
            signalDescp.signalunitname.append((const char*)&rsp[j], signalDescp.signalunitnamelen);
            j += 8;
            mPeriSignalList.push_back(signalDescp);
        }
    }

    void PeripheralDescRsp::printSignalList() const
    {
        qDebug() << "Peripheral Id" << static_cast<uint8_t>(mPeriPheralId);
        qDebug() << "Num of Signals" << mNumSignals;
        for (auto pitr : mPeriSignalList)
        {
            qDebug() << "Signal Id" << pitr.signalid;
            qDebug() << "Signal Name" << QString::fromStdString(pitr.signalname);
            qDebug() << "Signal Unit" << QString::fromStdString(pitr.signalunitname);
            qDebug() << "Signal Min Val" << pitr.signalMinVal;
            qDebug() << "Signal Max Val" << pitr.signalMaxVal;
            qDebug() << "Signal Sampling rate" << pitr.signalsamplingrate;
            qDebug() << "Signal Operating Mode" << pitr.operatingmode;
        }
    }

    uint16_t PeripheralCaptureCmd::getMessageLength() const 
    { return 3; }

    void PeripheralCaptureCmd::copyCommandData(uint8_t *msg) const
    {
        msg[0] = mPeripheralId;
        msg[1] = mPeripheralParam;
        msg[2] = mPeripheralCapCmd;
    }

    void PeripheralCaptureRsp::copyResponseData(const uint8_t *data)
    {
        //data[0] is dummy byte
        mPeripheralId = data[1];
        mCaptureDataLen = data[2];
        memcpy(mCaptureData, &data[3], mCaptureDataLen);
    }
}
