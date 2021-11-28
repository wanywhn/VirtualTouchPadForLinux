//
// Created by tender on 19-6-12.
//


#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETCONFIGBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETCONFIGBUILDER_H

#include <string.h>
class PacketConfigBuilder {

public:

    PacketConfigBuilder() {
        initRes();
    }

    void setMaxX(unsigned int x) {
        bytes[2] |= x & 0xffU;
        bytes[5] |= (x & 0xf00U) >> 4;
    }

    void setMaxY(unsigned int y) {
        bytes[4] |= y & 0xffU;
        bytes[5] |= (y & 0xf00U) >> 8U;
    }

    void setResX(unsigned int x) {
        bytes[0] |= x;
    }

    void setResY(unsigned int y) {
        bytes[1] |= y;
    }

    void setConnect(bool c) {
        unsigned int tmp = c ? 0x1 : 0x0;
        bytes[3] |= ((tmp) << 0x3U);
    }

    void clear() {
        initRes();
    }

    void initRes() {
        bytes = new unsigned char[6];
        memset(bytes, 0, 6);
        bytes[3] |= 0xBU;

    }

    unsigned char *getBytes() {
        return bytes;
    }

    virtual ~PacketConfigBuilder() {
        delete bytes;
    }

private:
    unsigned char *bytes{};

};


#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETCONFIGBUILDER_H
