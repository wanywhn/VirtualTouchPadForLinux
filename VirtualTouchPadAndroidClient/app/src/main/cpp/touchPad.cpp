//
// Created by tender on 19-6-12.
//

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <android/log.h>
#include <unistd.h>
#include "touchPad.h"
#include "PacketBuilder/PacketMotionBuilder.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

touchPad::touchPad() {
    mLastFingerCount = -1;

}

bool touchPad::touchpadOnTouch(AInputEvent *event) {


    auto action = AMotionEvent_getAction(event);

    switch (action & AMOTION_EVENT_ACTION_MASK) {
        //TODO ??
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN: {
            auto f = new PointF();
            auto idx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            f->x = AMotionEvent_getRawX(event, idx);
            f->y = AMotionEvent_getRawY(event, idx);
            auto pointerId = AMotionEvent_getPointerId(event, idx);
//            LOGI("ACTION_POINTER_DOWN,FingerID:%d,X:%f,Y:%f\n", pointerId, f->x, f->y);
            mActivatePoints[pointerId] = f;

            break;
        }

        case AMOTION_EVENT_ACTION_MOVE: {

            if (mLastFingerCount != mActivatePoints.size()) {
//                synchronized (mActivePointsLock) {
                SendStatusPacket();
                SendHeadPacket(event);
//                }
                mLastFingerCount = mActivatePoints.size();
            }


            if (mActivatePoints.size() == 1) {
                SendHeadPacketForOneFingerWhenMotion(event);
                break;
            }

//            if (!till) {
//                break;
//            }
            auto motionBuilder = new PacketMotionBuilder();
            bool clearFlag = true;

            int hiscount = AMotionEvent_getHistorySize(event);
            for (int hidx = 0; hidx != hiscount; ++hidx) {

                for (auto idx = 0; idx != AMotionEvent_getPointerCount(event); ++idx) {
                    auto pointerId = AMotionEvent_getPointerId(event, idx);
                    if(mActivatePoints.find(pointerId)==mActivatePoints.end()){
                        continue;
                    }
//                        auto pid = AMotionEvent_getPointerId(event, idx);
                    auto deltX = AMotionEvent_getHistoricalRawX(event, idx, hidx) -
                                 mActivatePoints.at(pointerId)->x;
                    auto deltY = AMotionEvent_getHistoricalRawY(event, idx, hidx) -
                                 mActivatePoints.at(pointerId)->y;
//                int deltX = (int) (event.getHistoricalX(pointer, hidx) - mActivePoints.valueAt(idx).x);
//                int deltY = (int) (event.getHistoricalY(pointer, hidx) - mActivePoints.valueAt(idx).y);
                    if (deltX == 0 && deltY == 0) {
                        continue;
                    }
                    mActivatePoints.at(pointerId)->x += deltX;
                    mActivatePoints.at(pointerId)->y += deltY;
//                mActivePoints.valueAt(idx).x = event.getHistoricalX(pointer, hidx);
//                mActivePoints.valueAt(idx).y = event.getHistoricalY(pointer, hidx);
                    if (clearFlag) {
                        clearFlag = false;
                        motionBuilder->clear();
                        motionBuilder->setId1(pointerId);
                        motionBuilder->setX1(deltX);
                        motionBuilder->setY1(deltY);
                        LOGI("ACTION_MOVE/MOTION,FingerID:%d,DELT_X1:%f,DELT_Y1:%f\n", pointerId,
                             deltX, deltY);

                    } else {
                        clearFlag = true;
                        motionBuilder->setId2(pointerId);
                        motionBuilder->setX2(deltX);
                        motionBuilder->setY2(deltY);
                        sendData(motionBuilder->getBytes(), 6);
                        LOGI("ACTION_MOVE/MOTION,FingerID:%d,DELT_X2:%f,DELT_Y2:%f\n", pointerId,
                             deltX, deltY);
                    }

                }
            }
            break;
        }
//    }


        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP: {
//            synchronized (mActivePointsLock) {
//                LOGI("ACTION_POINTER_UP,FingerID:%d\n", pointerId);
            auto ttcnt=AMotionEvent_getPointerCount(event);
            auto idx= AMotionEvent_getPointerId(event, (action &
                                                              AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
            auto pointerId=AMotionEvent_getPointerId(event,idx);
            mActivatePoints.erase(pointerId);

            if(ttcnt==1){
                mActivatePoints.clear();
            }

            if (mActivatePoints.empty()) {
                mLastFingerCount = -1;
            }
            SendStatusPacket();
            SendHeadPacket(event);
//    }
//                if (timerUp != null) {
//                    timerUp.cancel();
//                    timerUp = null;
//                }
//                timerUp = new Timer();
//                timerUp.schedule(new TimerTask()
//                {
//                    @Override
//                    public void run() {
//                        SendStatusPacket();
//                        SendHeadPacket(event);
//                    }
//                }, 30);

            break;
        }
        default: {
//                LOGI("unhanded case:pointer id:%d,action=%d\n",pointerId,action);

            break;
        };
    }

    return true;
}

void touchPad::SendHeadPacketForOneFingerWhenMotion(AInputEvent *event) {
//    assert(AMotionEvent_getPointerCount(event) == 1);

    auto historySize = AMotionEvent_getHistorySize(event);
    for (int i = 0; i < historySize; ++i) {
        auto headBUilder = new PacketHeadBUilder();
        //!!FIXME
        auto id = AMotionEvent_getPointerId(event, 0);
        if(mActivatePoints.find(id)==mActivatePoints.end()){
            continue;
        }
        auto currentx = AMotionEvent_getHistoricalRawX(event, 0, i);
        auto currenty = AMotionEvent_getHistoricalRawY(event, 0, i);
//        float currenty = event.getHistoricalY(i);
        //TODO touchMajor etc
        mActivatePoints.at(id)->x = currentx;
        mActivatePoints.at(id)->y = currenty;
        headBUilder->setId(id);
//        headBUilder->setWidth((short) (event.getHistoricalTouchMajor(i)));
        headBUilder->setWidth(AMotionEvent_getHistoricalTouchMajor(event, 0, i));
//        float press = event.getHistoricalPressure(i) * 100;
        headBUilder->setPressure(2);
        headBUilder->setX(currentx);
        headBUilder->setY(currenty);
//        LOGI("SingleHeadPacket: X:%f,Y:%f\n", currentx, currenty);
        sendData(headBUilder->getBytes(), 6);
    }

}

void touchPad::SendHeadPacket(AInputEvent *event) {
    auto headBUilder = new PacketHeadBUilder();

    //TODO cnt should use the number in event
    for (int idx = 0; idx < mActivatePoints.size(); ++idx) {
        if (idx >= AMotionEvent_getPointerCount(event) || idx >= mActivatePoints.size()) {
            continue;
        }
        headBUilder->clear();
        //FIXME idx sometime would out of range
        auto pid = AMotionEvent_getPointerId(event, idx);
        if (pid < 0) {
            continue;
        }
        if(mActivatePoints.find(pid)==mActivatePoints.end()){
            continue;
        }
        //!!FIXME
        headBUilder->setWidth((short) pid);
        headBUilder->setPressure(2);
//        auto x = mActivatePoints.at(pid)->x;
//        auto y = mActivatePoints.at(pid)->y;
        headBUilder->setX(mActivatePoints.at(pid)->x);
        headBUilder->setY(mActivatePoints.at(pid)->y);
        headBUilder->setId(pid);
        sendData(headBUilder->getBytes(), 6);
//        LOGI("ACTION_MOVE/HEAD:FingerID:%d,X:%f,Y:%f\n", pid, x, y);

    }
}

void touchPad::SendStatusPacket() {
    //prepare and send status packet;
    auto statusBuilder = new PacketStatusBuilder();

    //TODO use non-block send
    for (auto &mActivatePoint : mActivatePoints) {
        //FIXME id should be 0-4
        //TODO palm???
        statusBuilder->setFingerTouched(mActivatePoint.first);
//        LOGI("send status packet:activate: %d\n", mActivatePoint.first);
    }
    sendData(statusBuilder->getBytes(), 6);
}

bool touchPad::connectTo(const char *servaddr, int port) {
    struct sockaddr_in seraddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    inet_pton(AF_INET, servaddr, &seraddr.sin_addr);
    seraddr.sin_port = htons(port);

    if (connect(sockfd, reinterpret_cast<const sockaddr *>(&seraddr), sizeof(seraddr)) < 0) {
        printf("connect error:%s\n", strerror(errno));
        return false;
    }
    return true;
}

bool touchPad::sendData(const unsigned char *d, size_t size) {
    assert(write(sockfd, d, size) == size);
    return true;
}
