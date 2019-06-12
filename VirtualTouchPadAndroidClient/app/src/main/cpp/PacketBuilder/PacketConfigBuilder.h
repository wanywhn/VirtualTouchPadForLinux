//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETCONFIGBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETCONFIGBUILDER_H



class PacketConfigBuilder {

public:

    PacketConfigBuilder() {
            initRes();
        }

    void setMaxX(int x) {
            bytes[2] |= x&0xff;
            bytes[5] |=(x&0xf00)>>4;
        }

    void setMaxY(int y) {
            bytes[4] |= y&0xff;
            bytes[5] |=(y&0xf00)>>4;
        }

    void setResX(int x) {
            bytes[0] |= x;
        }

    void setResY(int y) {
            bytes[1] |= y ;
        }

    void setConnect(bool c) {
            int tmp= c ?0x1:0x0;
            bytes[3] |= ((tmp) << 3);
        }

    void clear() {
            initRes();
        }

    void initRes() {
        bytes=new unsigned char[6];
        memset(bytes,0, 6);
            bytes[3] |= 0xB;

        }

    unsigned char * getBytes() {
            return bytes;
        }
private:

unsigned char * bytes;

};


#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETCONFIGBUILDER_H
