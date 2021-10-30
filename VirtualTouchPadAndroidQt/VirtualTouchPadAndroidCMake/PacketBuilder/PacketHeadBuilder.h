//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETHEADBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETHEADBUILDER_H

class PacketHeadBuilder {
public:
    PacketHeadBuilder() {
        initRes();
    }

    void setWidth(unsigned short width) {
        bytes[0] |=  width << 4U;
    }

    void setPressure(unsigned int pressure) {
        bytes[1] |=  pressure & 0xf0U;
        bytes[4] |= (pressure & 0xfU) << 4U;
    }

    void setX(float mx) {
        auto x = (unsigned int) mx;
        bytes[1] |= (x >> 8U) & 0xfU;
        bytes[2] |= x & 0xffU;

    }

    void setY(float my) {
        //TODO float
        auto y = (unsigned int) my;
        bytes[4] |= (y >> 8U) & 0xfU;
        bytes[5] |= y & 0xffU;
    }

    void setId(unsigned int id) {
        bytes[3] |= ((id & 0x7) << 5U) & 0xffU;
    }

    void clear() {
        initRes();
    }

    void initRes() {
        bytes = new unsigned char[6];
        memset(bytes, 0, 6);
        bytes[0] |=  0x4U;
        bytes[3] |=  0x11U;
    }

    unsigned char *getBytes() {
        return bytes;
    }

private :
    unsigned char *bytes{};
};

#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETHEADBUILDER_H
