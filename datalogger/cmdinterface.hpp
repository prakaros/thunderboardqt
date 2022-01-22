#pragma once
#include <stdint.h>
namespace sensemon
{
    enum class CommandId
    {
        CMD_CONNECTTODEVICE = 1,
        CMD_CONNECTTODEVICEACK,
        CMD_PERIPHERALLIST,
        CMD_PERIPHERALLISTACK,
        CMD_PERIPHERALDESC,
        CMD_PERIPHERALDESCACK,
        CMD_PERIPHERALCONFIG,
        CMD_PERIPHERALCONFIGACK,
        CMD_PERIPHERALCAPTURECMD,
        CMD_PERIPHERALCAPTURERSP,
        CMD_LAST
    };

    enum class ResponseId
    {
        RSP_ACK = 1,
        RSP_NACK,
        RSP_UNKNOWN
    };

    enum class ConnectCmdId
    {
        CMD_CONNECT = 1,
        CMD_DISCONECT,
        CMD_UNKNOWN
    };


    class CmdInterface
    {
    public:
        CmdInterface(CommandId cmdId) : mCmdId(cmdId)
        {}
        ~CmdInterface() {}
        virtual void handleCommand() = 0;
        virtual void sendResponse() = 0;
        CommandId getCommandId() const { return mCmdId; }
        virtual uint16_t getMessageLength() const = 0;
        virtual void copyCommandData(uint8_t *msg) const = 0;
        virtual void copyResponseData(const uint8_t *rsp) = 0;
    protected:
        CommandId mCmdId;
        uint8_t mMsgLen;
    };

}
