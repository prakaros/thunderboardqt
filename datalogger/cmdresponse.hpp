#pragma once

#include "cmdinterface.hpp"
#include <stdint.h>
#include <string>
#include <list>
#include "applicationtypes.hpp"

namespace sensemon
{
    #define MAX_DEV_NAME 32
    #define MAX_CAP_DATA 128

    class ConnectCmd : public CmdInterface
    {
    public:
        ConnectCmd():CmdInterface(CommandId::CMD_CONNECTTODEVICE)
                     , devnamelen(0)
                     , mDeviceName("UnKnown")
                     , mCmd(ConnectCmdId::CMD_UNKNOWN)
        {
        }
        virtual ~ConnectCmd() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const;
        virtual void copyCommandData(uint8_t *msg) const;
        virtual void copyResponseData(const uint8_t *rsp) {}
        virtual void sendDeviceName(const std::string& devName) { mDeviceName = devName; devnamelen = static_cast<uint8_t>(mDeviceName.length()); }
        virtual void setCommand(ConnectCmdId cmd) { mCmd = cmd; }
    private:
        ConnectCmdId mCmd;
        uint8_t devnamelen;
        std::string mDeviceName;

    };

    class ConnectRsp : public CmdInterface
    {
    public:
        ConnectRsp() :
            CmdInterface(CommandId::CMD_CONNECTTODEVICEACK)
            , mRsp(ResponseId::RSP_UNKNOWN)
            , boardnamelen(0)
        {}
        virtual ~ConnectRsp() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const;
        virtual void copyCommandData(uint8_t *msg)const {}
        virtual void copyResponseData(const uint8_t *rsp);
        ResponseId getResponseId() const { return mRsp; }
        std::string getBoardName() const { return mBoardName; }
    private:
        ResponseId mRsp;
        uint8_t boardnamelen;
        std::string mBoardName;

    };

    class PeripheralListCmd : public CmdInterface
    {
    public:
        PeripheralListCmd() : CmdInterface(CommandId::CMD_PERIPHERALLIST)
        {}
        virtual ~PeripheralListCmd() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const { return 0; }
        virtual void copyCommandData(uint8_t *msg) const {}
        virtual void copyResponseData(const uint8_t *rsp) {}

    };

    class PeripheralListRsp : public CmdInterface
    {
    public:
        PeripheralListRsp() : CmdInterface(CommandId::CMD_PERIPHERALLISTACK)
        {}
        virtual ~PeripheralListRsp() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const;
        virtual void copyCommandData(uint8_t *msg) const {}
        virtual void copyResponseData(const uint8_t *rsp);
        PeripheralList getperipheralList() { return mPeripheralList; }
        uint8_t getNumPeripherals() const { return mNumPeripherals; }
        void printList() const;
    private:
        uint8_t mNumPeripherals;
        PeripheralList mPeripheralList;
    };


    class PeripheralDescCmd : public CmdInterface
    {
    public:
        PeripheralDescCmd() : CmdInterface(CommandId::CMD_PERIPHERALDESC)
        {}
        virtual ~PeripheralDescCmd() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const;
        virtual void copyCommandData(uint8_t *msg) const;
        virtual void copyResponseData(const uint8_t *rsp) {}
        void setPeripheralId(uint8_t perId) { mPeriPheralId = perId; }
    private:
        uint8_t mPeriPheralId;
    };

    class PeripheralDescRsp : public CmdInterface
    {
    public:
        PeripheralDescRsp() : CmdInterface(CommandId::CMD_PERIPHERALDESCACK)
        {}
        virtual ~PeripheralDescRsp() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const;
        virtual void copyCommandData(uint8_t *msg) const {};
        virtual void copyResponseData(const uint8_t *rsp);
        SignalDescpList_t getSignalList() { return mPeriSignalList; }
        uint8_t getNumSignals() const { return mNumSignals; }
        PeripheralId getPeripheralId() const { return mPeriPheralId; }
        bool getPeripheralAvail() const { return mPeripheralAvail; }
        uint8_t getCaptureCmd() const { return mCaptureCmd; }
        void printSignalList() const;
    private:
        PeripheralId mPeriPheralId;
        bool mPeripheralAvail;
        uint8_t mCaptureCmd;
        uint8_t mNumSignals;
        SignalDescpList_t mPeriSignalList;
    };

    class PeripheralCaptureCmd : public CmdInterface
    {
    public:
        PeripheralCaptureCmd() : CmdInterface(CommandId::CMD_PERIPHERALCAPTURECMD)
        {}
        virtual ~PeripheralCaptureCmd() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const;
        virtual void copyCommandData(uint8_t *msg) const;
        virtual void copyResponseData(const uint8_t *rsp) {}
        void setPeripheralCmdData(uint8_t perId, uint8_t param, uint8_t capCmd)
        {
            mPeripheralId = perId;
            mPeripheralParam = param;
            mPeripheralCapCmd = capCmd;
        }
    private:
        uint8_t mPeripheralId;
        uint8_t mPeripheralParam;
        uint8_t mPeripheralCapCmd;
    };

    class PeripheralCaptureRsp : public CmdInterface
    {
    public:
        PeripheralCaptureRsp() : CmdInterface(CommandId::CMD_PERIPHERALCAPTURERSP)
        {}
        virtual ~PeripheralCaptureRsp() {}
        virtual void handleCommand() {}
        virtual void sendResponse() {}
        virtual uint16_t getMessageLength() const { return (2 + mCaptureDataLen); }
        virtual void copyCommandData(uint8_t *msg) const {}
        virtual void copyResponseData(const uint8_t *rsp);
        uint8_t getPeripheralId() const { return mPeripheralId; }
        uint8_t getCaptureDataLen() const { return mCaptureDataLen; }
        const uint8_t*getCaptureDataPtr() const { return mCaptureData; }
    private:
        uint8_t mPeripheralId;
        uint8_t mCaptureDataLen;
        uint8_t mCaptureData[MAX_CAP_DATA];
    };

}