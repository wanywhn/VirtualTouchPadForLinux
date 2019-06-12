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
        bytes[0] |= (b ? 1 : 0) << 4;
    }

    void setId1(int id) {
        bytes[0] |= (id + 1) << 5;
    }

    void setX1(int x) {
        bytes[1] |= x;
    }

    void setY1(int y) {
        bytes[2] |= y & 0xff;
    }

    void setId2(int id) {
        bytes[3] |= (id + 1) << 5;

    }

    void setX2(int x) {
        bytes[4] |=  x;
    }

    void setY2(int y) {
        bytes[5] |= y & 0xff;
    }

    void clear() {
        initRes();
    }


    unsigned char *getBytes() {
        return bytes;
    }


    void initRes() {

        bytes = new unsigned char[6];
        bytes[0] |=  0x4;
        bytes[3] |=  0x12;
    }


private:

    unsigned char *bytes;
};

#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETMOTIONBUILDER_H
