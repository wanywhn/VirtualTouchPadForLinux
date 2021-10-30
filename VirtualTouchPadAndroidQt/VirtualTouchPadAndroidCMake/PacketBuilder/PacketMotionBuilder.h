//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETMOTIONBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETMOTIONBUILDER_H


class PacketMotionBuilder {
public:
    PacketMotionBuilder() {
        initRes();
    }

    void setOverflows(bool b) {
        bytes[0] |= (b ? 1U : 0U) << 4U;
    }

    void setId1(unsigned int id) {
        bytes[0] |= ((id&0x7) << 5U) & 0xffU;
    }

    void setX1(int x) {
        bytes[1] |= x;
    }

    void setY1(int y) {
        bytes[2] |= y & 0xffU;
    }

    void setId2(int id) {
        bytes[3] |= ((id & 0x7) << 5U) & 0xffU;

    }

    void setX2(int x) {
        bytes[4] |=  x;
    }

    void setY2(int y) {
        bytes[5] |= y & 0xffU;
    }

    void clear() {
        initRes();
    }


    unsigned char *getBytes() {
        return bytes;
    }


    void initRes() {

        bytes = new unsigned char[6];
        memset(bytes, 0, 6);
        bytes[0] |=  0x4U;
        bytes[3] |=  0x12U;
    }


private:

    unsigned char *bytes{};
};

#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETMOTIONBUILDER_H
