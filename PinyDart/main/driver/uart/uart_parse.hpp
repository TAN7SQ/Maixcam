#pragma once
#include "_basic.hpp"

#pragma pack(push, 1)
struct Frame
{
    uint8_t type;
    uint16_t length;
    uint8_t payload[256];
};
#pragma pack(pop)

class UartFrameParser
{
public:
    static constexpr uint8_t SOF = 0xAA;

    UartFrameParser()
    {
        reset();
    }

    bool input(uint8_t byte, Frame &frame)
    {
        switch (state) {
        case WAIT_SOF:
            if (byte == SOF) {
                reset();
                buffer[index++] = byte;
                state = READ_TYPE;
            }
            break;
        case READ_TYPE:
            frame.type = byte;
            buffer[index++] = byte;
            state = READ_LEN1;
            break;
        case READ_LEN1:
            length = byte;
            buffer[index++] = byte;
            state = READ_LEN2;
            break;
        case READ_LEN2:
            length |= (byte << 8);
            buffer[index++] = byte;
            if (length > 256) {
                reset();
                break;
            }
            payloadIndex = 0;
            state = READ_PAYLOAD;
            break;
        case READ_PAYLOAD:
            buffer[index++] = byte;
            frame.payload[payloadIndex++] = byte;
            if (payloadIndex >= length)
                state = READ_CRC1;
            break;
        case READ_CRC1:
            crc = byte;
            state = READ_CRC2;
            break;
        case READ_CRC2: {
            crc |= (byte << 8);
            uint16_t calc = Tools::crc16_ccitt(buffer, index);
            if (calc == crc) {
                frame.length = length;
                reset();
                return true;
            }
            reset();
        } break;
        }
        return false;
    }

private:
    enum State
    {
        WAIT_SOF,
        READ_TYPE,
        READ_LEN1,
        READ_LEN2,
        READ_PAYLOAD,
        READ_CRC1,
        READ_CRC2
    };
    State state;

    uint8_t buffer[256];
    uint16_t index;
    uint16_t length;
    uint16_t payloadIndex;
    uint16_t crc;
    void reset()
    {
        state = WAIT_SOF;
        index = 0;
        length = 0;
        payloadIndex = 0;
        crc = 0;
    }
};