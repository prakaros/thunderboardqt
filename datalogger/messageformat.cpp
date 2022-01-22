#include "messageformat.hpp"
#include <string>
#include <QtCore\QDebug>

namespace sensemon
{
    Frame::Frame()
    {
    
    }

    Frame::~Frame()
    {
    
    }

    Frame::Frame(const uint8_t *bytePointer,int cmd, int len)
    {
        setHeader(cmd, len);
        copyMessageContent(bytePointer, len);
        setFooter();
    }


    void Frame::setHeader(uint8_t cmd, uint16_t length)
    {
        qDebug() << "Frame Hader Cmd " << cmd;
        qDebug() << "Frame Hader Mssg Len " << length;
        frameBuffer.frameheader.start_of_frame = START_OF_FRAME; // (uint16_t)(START_OF_FRAME << 8) | ((START_OF_FRAME >> 8));
        frameBuffer.frameheader.cmd_byte = cmd;
        frameBuffer.frameheader.message_length = length;

    }
    void Frame::setFooter()
    {
        frameBuffer.framefooter.frame_crc = 0;
        frameBuffer.framefooter.end_of_frame = END_OF_FRAME; // (uint16_t)(END_OF_FRAME << 8) | ((END_OF_FRAME >> 8));;
    }

    void Frame::copyMessageContent(const uint8_t *data, uint16_t len)
    {
        memcpy(frameBuffer.msg_body, data, len);
    }

    bool Frame::createFrame(const uint8_t *byte, uint16_t len)
    {
        bool result = true;

        result = result && CHECK_START_OF_FRAME(byte);
     //   qDebug() << "Creat Frame Start Of Frame " << result;
        uint16_t length = GET_MESSAGE_LEN(byte);
  //      qDebug() << "Creat Frame Msg Len" << length;
        const uint8_t *footer = byte + sizeof(frame_header_t) + length;
        
        result = result && CHECK_END_OF_FRAME(footer);

        if (result)
        {
  //          qDebug() << "Valid Frame, Msg Length" << length;
            memcpy(&frameBuffer.frameheader, byte, sizeof(frame_header_t));
            memcpy(frameBuffer.msg_body, byte + sizeof(frame_header_t), length);
            memcpy(&frameBuffer.framefooter, byte + sizeof(frame_header_t) + length, sizeof(frame_footer_t));
        }
        else
        {
            qDebug() << "Create Frame Failed, INVALID";
        }
        return result;
    }

    void Frame::copyFrameRawData(uint8_t*data, uint16_t *len)
    {
        *len = 0;

        memcpy(data, &frameBuffer.frameheader, sizeof(frame_header_t));
        *len += sizeof(frame_header_t);
        qDebug() << "Copy Msg Frame header Len" << *len;
        memcpy(data + sizeof(frame_header_t), frameBuffer.msg_body, frameBuffer.frameheader.message_length);
        *len += frameBuffer.frameheader.message_length;
        qDebug() << "Copy Msg Frame Header + Msg Len" << *len;
        memcpy(data + sizeof(frame_header_t) + frameBuffer.frameheader.message_length, &frameBuffer.framefooter, sizeof(frame_footer_t));
        *len += sizeof(frame_footer_t);
        qDebug() << "Copy Msg Frame Header + Msg Len + Footer" << *len;
    }

}