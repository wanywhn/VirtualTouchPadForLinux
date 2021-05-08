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
    PointF(double _x,double _y):x(_x),y(_y){};
    double x;
    double y;

    bool equals(double ox, double oy) {
        return (fabs(ox - x) < 0.001) && (fabs(oy - y) < 0.001);

    };
};

class TouchPad {
public:
    TouchPad();

    bool connectTo(const char *servaddr, int port);

    bool touchpadOnTouch(AInputEvent *event);

    bool sendData(const unsigned char *,size_t size);
    int currentTouchPoints(void)
    {
        return mActivatePoints.size();
    }

    void clearState(void);
    void addPoint(int id, PointF point)
    {
        mActivatePoints[id] = point;
    }
    void SendStatusPacket();
    void SendHeadPacketForOneFingerWhenMotion(AInputEvent *event);
    void SendHeadPacket(AInputEvent *event);
private:
    bool checkPosSame(int id, double currentx, double currenty) {
        return mActivatePoints.at(id).equals(currentx, currenty);

    }




private:

    int mLastFingerCount = -1;
    std::unordered_map<int, PointF> mActivatePoints;
    int sockfd;

};


#endif //VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H
