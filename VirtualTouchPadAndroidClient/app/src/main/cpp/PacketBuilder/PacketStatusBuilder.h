//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_PACKETSTATUSBUILDER_H
#define VIRTUALTOUCHPADANDROIDCLIENT_PACKETSTATUSBUILDER_H
class PacketStatusBuilder {
public:

PacketStatusBuilder(){
        bytes=new  unsigned char [6];
        //TODO init
        bytes[0]|=0x4;
        bytes[3]|=0x10;


    }
void setFingerTouched(int id){
        bytes[1]|=(1<<id)&0x1f;

    }
void setPalm(bool b){
        bytes[4]|=0x80;
    }
unsigned char *getBytes() {
        return bytes;
    }
private :
    unsigned char *bytes;
};

#endif //VIRTUALTOUCHPADANDROIDCLIENT_PACKETSTATUSBUILDER_H
