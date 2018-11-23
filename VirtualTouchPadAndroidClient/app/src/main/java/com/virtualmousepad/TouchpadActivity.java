
package com.virtualmousepad;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.PointF;
import android.os.Bundle;
import android.util.Log;
import android.util.SparseArray;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

import com.virtualmousepad.PacketBuilder.PacketHeadBUilder;
import com.virtualmousepad.PacketBuilder.PacketMotionBuilder;
import com.virtualmousepad.PacketBuilder.PacketStatusBuilder;

import java.util.Timer;
import java.util.TimerTask;

import static java.lang.Math.abs;

public class TouchpadActivity extends Activity {

    private int mCurrentFingerCount;
    private int mLastFingerCount = -1;
    private static int tapCount;

    private SparseArray<PointF> mActivePoints;
        private final Object mActivePointsLock = new Object();

        private ConnectionReceiver mMouseConnectionReceiver = new ConnectionReceiver();
        private boolean till = false;
        Timer timer = new Timer();
        Timer timerUp = new Timer();

        private class ConnectionReceiver extends BroadcastReceiver {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                assert action != null;
                if (action.equals(ConnectionService.DISCONNECTED_INTENT) ||
                        action.equals(ConnectionService.CONNECTION_LOST_INTENT)) {

                    Intent i = new Intent(context, StartActivity.class);
                    i.putExtra("ConnectionLost", true);
                    startActivity(i);
                }
            }

        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.touchpad_screen);

        mActivePoints = new SparseArray<PointF>();
        mCurrentFingerCount = 0;
        mLastFingerCount = -1;
        tapCount = 0;

        View v = findViewById(R.id.touchPad);

        v.setOnTouchListener(new OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return touchpadOnTouch(event);
            }
        });


    }

    @Override
    public void onResume() {
        super.onResume();
        if (ConnectionService.mService == null) {
            finish();
        }
        IntentFilter iff = new IntentFilter();
        iff.addAction(ConnectionService.DISCONNECTED_INTENT);
        iff.addAction(ConnectionService.CONNECTION_LOST_INTENT);
        registerReceiver(mMouseConnectionReceiver, iff);
    }

    @Override
    public void onPause() {
        super.onPause();
        unregisterReceiver(mMouseConnectionReceiver);
    }

//	@Override
//    public boolean onCreateOptionsMenu(Menu menu) {
//    	MenuInflater inflater = getMenuInflater();
//    	inflater.inflate(R.menu.chose_activity_menu, menu);
//    	return true;
//    }

//    @Override
//    public boolean onOptionsItemSelected(MenuItem item) {
//    	Intent intent;
//        switch (item.getItemId()) {
//            case R.id.touchpadScreen:
//                return true;
//                /*
//            case R.id.startScreen:
//                intent = new Intent(this, StartActivity.class);
//                startActivity(intent);
//                return true;
//                */
//            case R.id.keyboardScreen:
//            	intent = new Intent(this, KeyboardActivity.class);
//            	startActivity(intent);
//            	return true;
//
//            default:
//                return super.onOptionsItemSelected(item);
//        }
//    }

    private boolean touchpadOnTouch(final MotionEvent event) {


        int pointerIdx = event.getActionIndex();
        final int pointerId = event.getPointerId(pointerIdx);

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                PointF f = new PointF();
                f.x = event.getX(pointerIdx);
                f.y = event.getY(pointerIdx);
                Log.d("ACTION_POINTER_DOWN", "Finger ID:" + String.valueOf(pointerId) + " X:" + String.valueOf(f.x) + " Y:" + String.valueOf(f.y));
                mActivePoints.put(pointerId, f);
                ++mCurrentFingerCount;


                if (timer != null) {
                    timer.cancel();
                    timer = null;
                }
                timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        till = true;
                    }
                }, 30);
                till = false;


                break;

            case MotionEvent.ACTION_MOVE:

                if (!till) {
                    break;
                }

                //prepare and send head packet;
                if (mLastFingerCount != mCurrentFingerCount) {
                    synchronized (mActivePointsLock) {
                        SendStatusPacket();
                        SendHeadPacket(event);
                    }
                    mLastFingerCount = mCurrentFingerCount;
                }


                if (mCurrentFingerCount == 1) {
                    SendHeadPacketForOneFingerWhenMotion(event);
                    break;
                }
                //TODO send move;
                synchronized (mActivePointsLock) {
                    PacketMotionBuilder motionBuilder = new PacketMotionBuilder();
                    boolean clearFlag = true;
                    int hiscount = event.getHistorySize();
                    for (int hidx = 0; hidx != hiscount; ++hidx) {
                        for (int idx = 0; idx < mCurrentFingerCount; ++idx) {
                            int id = mActivePoints.keyAt(idx);
                            int pointer = event.findPointerIndex(id);
                            if (pointer < 0) {
                                continue;
                            }
                            int deltX = (int) (event.getHistoricalX(pointer, hidx) - mActivePoints.valueAt(idx).x);
                            int deltY = (int) (event.getHistoricalY(pointer, hidx) - mActivePoints.valueAt(idx).y);
                            if (deltX == 0 && deltY == 0) {
                                continue;
                            }
//                            if(mCurrentFingerCount==3||mCurrentFingerCount==4||mCurrentFingerCount==5){
//
//                                if(abs(deltX)<2&&abs(deltY)<2){
//                                    continue;
//                                }
//                                else if(abs(deltX)<2){
//                                    deltY=(deltY+deltY<0?-4:4)*mCurrentFingerCount*mCurrentFingerCount;
//                                    deltX=0;
//                                }else if(abs(deltY) < 2){
//                                    deltX=(deltX+deltX<0?-4:4)*mCurrentFingerCount*mCurrentFingerCount;
//                                    deltY=0;
//                                }
//                            }
                            mActivePoints.valueAt(idx).x = event.getHistoricalX(pointer, hidx);
                            mActivePoints.valueAt(idx).y = event.getHistoricalY(pointer, hidx);
                            if (clearFlag) {
                                clearFlag = false;
                                motionBuilder.clear();
                                motionBuilder.setId1(id);
                                motionBuilder.setX1(deltX);
                                motionBuilder.setY1(deltY);
                                Log.d("ACTION_MOVE/MOTION", "Finger ID:" + String.valueOf(id) + " DELT_X1:" + String.valueOf(deltX) + " DELT_Y1:" + String.valueOf(deltY));

                            } else {
                                clearFlag = true;
                                motionBuilder.setId2(id);
                                motionBuilder.setX2(deltX);
                                motionBuilder.setY2(deltY);
                                ConnectionService.mService.sendMouseData(motionBuilder.getBytes());
                                Log.d("ACTION_MOVE/MOTION", "Finger ID:" + String.valueOf(id) + " DELT_X2:" + String.valueOf(deltX) + " DELT_Y2:" + String.valueOf(deltY));
                            }

                        }
                    }
                }


                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                synchronized (mActivePointsLock) {
                    Log.d("ACTION_POINTER_UP", "Finger ID:" + String.valueOf(pointerId));
                    mActivePoints.remove(pointerId);
                    --mCurrentFingerCount;
                    if (mCurrentFingerCount == 0) {
                        mLastFingerCount = -1;
                    }
                }
                if (timerUp != null) {
                    timerUp.cancel();
                    timerUp = null;
                }
                timerUp = new Timer();
                timerUp.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        SendStatusPacket();
                        SendHeadPacket(event);
                    }
                }, 30);

                break;
        }
        return true;
    }

    private boolean checkPosSame(int id, float currentx, float currenty) {
        return mActivePoints.get(id).equals(currentx, currenty);

    }

    private void SendHeadPacketForOneFingerWhenMotion(MotionEvent event) {
        float historySize = event.getHistorySize();
        for (int i = 0; i < historySize; ++i) {
            PacketHeadBUilder headBUilder = new PacketHeadBUilder();
            int id = mActivePoints.keyAt(0);
            float currentx = event.getHistoricalX(i);
            float currenty = event.getHistoricalY(i);
            //TODO touchMajor etc
            mActivePoints.get(id).x = currentx;
            mActivePoints.get(id).y = currenty;
            headBUilder.setId(id);
            headBUilder.setWidth((short) (event.getHistoricalTouchMajor(i)));
            float press = event.getHistoricalPressure(i) * 100;
            headBUilder.setPressure((int) (press > 2 ? press : 2));
            headBUilder.setX(currentx);
            headBUilder.setY(currenty);
            Log.d("SingleHeadPacket:", " Width=" + String.valueOf(event.getHistoricalToolMajor(i)) + "Pressure=" + String.valueOf(press)
                    + " X:" + String.valueOf(mActivePoints.get(id).x) + " Y:" + String.valueOf(mActivePoints.get(id).y));
            ConnectionService.mService.sendMouseData(headBUilder.getBytes());
        }

    }

    private void SendHeadPacket(MotionEvent event) {
        PacketHeadBUilder headBUilder = new PacketHeadBUilder();

        for (int idx = 0; idx < mCurrentFingerCount; ++idx) {
            if (idx >= event.getPointerCount() || idx >= mActivePoints.size()) {
                continue;
            }
            headBUilder.clear();
            int id = mActivePoints.keyAt(idx);
            //FIXME idx sometime would out of range
            int pidx = event.findPointerIndex(id);
            if (pidx < 0) {
                continue;
            }
            headBUilder.setWidth((short) pidx);
            headBUilder.setPressure((int) (100 * event.getPressure(pidx)));
            int x = (int) mActivePoints.get(id).x;
            int y = (int) mActivePoints.get(id).y;
            headBUilder.setX(mActivePoints.get(id).x);
            headBUilder.setY(mActivePoints.get(id).y);
            headBUilder.setId(id);
            ConnectionService.mService.sendMouseData(headBUilder.getBytes());
            Log.d("ACTION_MOVE/HEAD", "Finger ID:" + String.valueOf(id) + " X:" + String.valueOf(x) + " Y:" + String.valueOf(y));
        }
    }

    private void SendStatusPacket() {
        //prepare and send status packet;
        PacketStatusBuilder statusBuilder = new PacketStatusBuilder();

        //TODO use non-block send
        for (int idx = 0; idx < mCurrentFingerCount; ++idx) {
            //FIXME id should be 0-4
            //TODO palm???
            statusBuilder.setFingerTouched(mActivePoints.keyAt(idx));
        }
        ConnectionService.mService.sendMouseData(statusBuilder.getBytes());
    }


}
