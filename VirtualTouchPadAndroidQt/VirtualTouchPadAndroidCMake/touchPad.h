//
// Created by tender on 19-6-12.
//

#ifndef VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H
#define VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H

#include <unistd.h>
#include <vector>
#include <math.h>
#include <map>
#include <unordered_map>
#include <sys/socket.h>
#include <PacketBuilder/PacketMotionBuilder.h>
#include "PacketBuilder/PacketHeadBuilder.h"

class PointF {
public:
    PointF(int _x, int _y) : x(_x), px(_x), y(_y), py(_y) {};

    PointF(PointF const &p) = default;

    int x;
    int y;
    int px;
    int py;

    bool equals(int ox, int oy) const {
        return (fabs(ox - x) < 0.001) && (fabs(oy - y) < 0.001);
    };

    void setPoint(PointF &p) {
        this->x = p.x;
        this->y = p.y;
        this->px = p.x;
        this->py = p.y;
    };
};

class TouchPad {
public:
    TouchPad();

    bool connectTo(const char *servaddr, int port);

    int currentTouchPoints() {
        return mActivatePoints.size();
    }

    void clearState() {
        this->mActivatePoints.clear();
        this->mLastFingerCount = 0;
    }

    void addPoint(int id, PointF point) {
        auto item = mActivatePoints.find(id);
        if (item == mActivatePoints.cend()) {
            mActivatePoints.emplace(id, point);
        } else {
            item->second.px = item->second.x;
            item->second.py = item->second.y;
            item->second.x = point.x;
            item->second.y = point.y;
        }
    }

    int PointCounts() {
        return mActivatePoints.size();
    }

    void removePoint(int id) {
        mActivatePoints.erase(id);
    }

    int updatePointMotion();

    void SendStatusPacket();

    void SendHeadPacketForOneFingerWhenMotion();

    void SendHeadPacket();
    bool isConnected(void) {
            return this->connected;
    }

private:
    bool checkPosSame(int id, double currentx, double currenty) {
        return mActivatePoints.at(id).equals(currentx, currenty);

    }

public:
    bool sendData(const unsigned char *, size_t size) const;
    bool disconn(void) {
            this->connected = false;
            shutdown(sockfd, SHUT_RDWR);
            printf("shutdown sockfd\r\n");
            return close(sockfd);
    }

    int mLastFingerCount = -1;

private:

    bool connected = false;
    std::unordered_map<int, PointF> mActivatePoints;
    int sockfd{};

};


#endif //VIRTUALTOUCHPADANDROIDCLIENT_TOUCHPAD_H
