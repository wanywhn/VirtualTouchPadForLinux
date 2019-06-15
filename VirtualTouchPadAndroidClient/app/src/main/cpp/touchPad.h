//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H
#define VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H

#include <vector>
#include <android/input.h>
#include <math.h>
#include <map>
#include <unordered_map>
#include <sys/socket.h>
#import "PacketBuilder/PacketStatusBuilder.h"
#include "PacketBuilder/PacketHeadBuilder.h"

class PointF {
public:
    float x;
    float y;

    bool equals(float ox, float oy) {
        return (fabs(ox - x) < 0.001) && (fabs(ox - y) < 0.001);

    };
};

class touchPad {
public:
    touchPad();

    bool connectTo(const char *servaddr, int port);

    bool touchpadOnTouch(AInputEvent *event);

    bool sendData(const unsigned char *,size_t size);
private:
    bool checkPosSame(int id, float currentx, float currenty) {
        return mActivatePoints.at(id)->equals(currentx, currenty);

    }

    void SendHeadPacketForOneFingerWhenMotion(AInputEvent *event);

    void SendHeadPacket(AInputEvent *event);

    void SendStatusPacket();


private:

    int mLastFingerCount = -1;
    std::unordered_map<int32_t, PointF *> mActivatePoints;
    int sockfd;

};


#endif //VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H
