//
// Created by tender on 19-6-12.
//

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <QDebug>
#include <PacketBuilder/PacketStatusBuilder.h>
#include "touchPad.h"
#include "PacketBuilder/PacketMotionBuilder.h"

TouchPad::TouchPad() {
    mLastFingerCount = -1;

}

void TouchPad::SendHeadPacketForOneFingerWhenMotion() {
    static PacketHeadBuilder headBuilder;
    headBuilder.initRes();
    //TODO touchMajor etc
    auto id = mActivatePoints.begin()->first;
    auto point = mActivatePoints.begin()->second;
    qDebug()<< "num:"<< mActivatePoints.size()<< " id:" << id << " x:" << point.x << " y:" << point.y;
    headBuilder.setId(id);
    headBuilder.setPressure(2);
    headBuilder.setX(point.x);
    headBuilder.setY(point.y);
    sendData(headBuilder.getBytes(), 6);

}

void TouchPad::SendHeadPacket() {
    static PacketHeadBuilder packetHeadBuilder;
    packetHeadBuilder.initRes();

    //TODO cnt should use the number in event
    for (auto item: mActivatePoints) {
        auto pid = item.first;
        //!!FIXME
        packetHeadBuilder.setWidth((short) pid);
        // TODO get pressure
        packetHeadBuilder.setPressure(2);
        packetHeadBuilder.setX(item.second.x);
        packetHeadBuilder.setY(item.second.y);
        packetHeadBuilder.setId(pid);
        sendData(packetHeadBuilder.getBytes(), 6);
    }
}

void TouchPad::SendStatusPacket() {
    //prepare and send status packet;
    static PacketStatusBuilder statusBuilder;
    statusBuilder.initRes();

    //TODO use non-block send
    for (auto &mActivatePoint: mActivatePoints) {
        //FIXME id should be 0-4
        //TODO palm???
        statusBuilder.setFingerTouched(mActivatePoint.first);
//        LOGI("send status packet:activate: %d\n", mActivatePoint.first);
    }
    sendData(statusBuilder.getBytes(), 6);
}

bool TouchPad::connectTo(const char *servaddr, int port) {
    struct sockaddr_in seraddr{};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    seraddr.sin_family = AF_INET;
    inet_pton(AF_INET, servaddr, &seraddr.sin_addr);
    seraddr.sin_port = htons(port);

    if (connect(sockfd, reinterpret_cast<const sockaddr *>(&seraddr), sizeof(seraddr)) < 0) {
        return false;
    }
    return true;
}

bool TouchPad::sendData(const unsigned char *d, size_t size) const {
    assert(write(sockfd, d, size) == size);
    return true;
}

int TouchPad::updatePointMotion() {
    static PacketMotionBuilder motionBuilder;
    motionBuilder.initRes();
    static bool clearFlag = true;

    for (auto item: mActivatePoints) {
        auto point = item.second;
        if (point.x == point.px && point.y == point.py) {
            continue;
        }

        if (clearFlag) {
            clearFlag = false;
            motionBuilder.clear();
            motionBuilder.setId1(item.first);
            motionBuilder.setX1(point.x - point.px);
            motionBuilder.setY1(point.y - point.py);
            qDebug()<< "id"<< item.first<<"x1-px:"<<point.x - point.px << " y1-px:"<< point.y-point.py;
        } else {
            clearFlag = true;
            motionBuilder.setId2(item.first);
            motionBuilder.setX2(point.x - point.px);
            motionBuilder.setY2(point.y - point.py);
            sendData(motionBuilder.getBytes(), 6);
            qDebug()<< "id"<< item.first<<"x2-px:"<<point.x - point.px << " y2-px:"<< point.y-point.py;
        }
    }
    return 0;

}
