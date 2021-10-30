//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETSTATUSBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETSTATUSBUILDER_H

class PacketStatusBuilder {
public:

    PacketStatusBuilder() {
        bytes = new unsigned char[6];
        memset(bytes, 0, 6);
        //TODO init
        bytes[0] |= 0x4U;
        bytes[3] |= 0x10U;

    }

    void setFingerTouched(unsigned int id) {
        bytes[1] |= (1U << (id & 0x7)) & 0x1fU;

    }

    void setPalm(bool b) {
        bytes[4] |= 0x80U;
    }

    unsigned char *getBytes() {
        return bytes;
    }

private :
    unsigned char *bytes;
};

#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETSTATUSBUILDER_H
