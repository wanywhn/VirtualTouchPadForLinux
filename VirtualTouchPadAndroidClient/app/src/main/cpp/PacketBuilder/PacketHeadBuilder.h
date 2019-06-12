//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETHEADBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETHEADBUILDER_H

class PacketHeadBUilder {
public:
    PacketHeadBUilder() {
        initRes();
    }

    void setWidth(short width) {
        bytes[0] |=  width << 4;
    }

    void setPressure(int pressure) {
        bytes[1] |=  pressure & 0xf0;
        bytes[4] |= (pressure & 0xf) << 4;
    }

    void setX(float mx) {
        int x = (int) mx;
        bytes[1] |= (x >> 8) & 0xf;
        bytes[2] |= x & 0xff;

    }

    void setY(float my) {
        //TODO float
        int y = (int) my;
        bytes[4] |= (y >> 8) & 0xf;
        bytes[5] |= y & 0xff;
    }

    void setId(int id) {
        bytes[3] |= (id +1) << 5;
    }

    void clear() {
        initRes();
    }

    void initRes() {
        bytes = new unsigned char[6];
        bytes[0] |=  0x4;
        bytes[3] |=  0x11;
    }


    unsigned char *getBytes() {
        return bytes;
    }

private :
    unsigned char *bytes;
};

#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETHEADBUILDER_H
