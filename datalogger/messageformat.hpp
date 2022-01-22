#pragma once
#include <stdint.h>
namespace sensemon
{
#define MSG_BODY_LEN 512

#pragma pack(push,1)
    typedef struct frame_header
    {
        uint16_t start_of_frame;
        uint16_t message_length;
        uint8_t  cmd_byte;
    }frame_header_t;
#pragma pack(pop)

#pragma pack(push,1)
    typedef struct frame_footer
    {
        uint16_t frame_crc;
        uint16_t end_of_frame;
    }frame_footer_t;
#pragma pack(pop)

#pragma pack(push,1)
    typedef struct frame_format
    {
        frame_header_t frameheader;
        uint8_t msg_body[MSG_BODY_LEN];
        frame_footer_t framefooter;
    }frame_format_t;
#pragma pack(pop)

    //Given message boday length size of message = Message body length + Header length + Footer length 
#define MESSAGE_LEN(msgLength) (msgLength + sizeof(frame_header_t) + sizeof(frame_footer_t))

//Given length of full frame message body length = frame length - header length - footer length
#define MESSAGE_BODY_LEN(frameLength) (frameLength - sizeof(frame_header_t) - sizeof(frame_footer_t))

#define START_OF_FRAME 0xAABB
#define END_OF_FRAME   0xCCDD

#define CHECK_START_OF_FRAME(byte)          (((byte[1] << 8 )| byte[0]) == START_OF_FRAME)
#define GET_MESSAGE_LEN(byte)               ((byte[3] << 8) | byte[2])
#define CHECK_END_OF_FRAME(footbyte)      (((footbyte[3] << 8 )| footbyte[2]) == END_OF_FRAME)


    class Frame
    {
    public:
        Frame();
        Frame(const uint8_t *bytePointer, int cmd, int len);
        ~Frame();
        int getLength() const { return frameBuffer.frameheader.message_length; }
        uint8_t getCmd() const { return frameBuffer.frameheader.cmd_byte; }
        const void *getMessagePointer() { return frameBuffer.msg_body; }
        void setHeader(uint8_t cmd, uint16_t length);
        void setFooter();
        void copyMessageContent(const uint8_t *data, uint16_t len);
        bool createFrame(const uint8_t* byte, uint16_t len);
        void copyFrameRawData(uint8_t*data, uint16_t *len);
    private:
        frame_format_t frameBuffer;
    };

}
