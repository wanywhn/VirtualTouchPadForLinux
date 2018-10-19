
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
    private int mLastFingerCount;

    private SparseArray<PointF> mActivePoints;
    private final Object mActivePointsLock = new Object();
    private static int pcount=0;

    private ConnectionReceiver mMouseConnectionReceiver = new ConnectionReceiver();

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
        mLastFingerCount = 0;

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
                synchronized (mActivePointsLock) {
                    mActivePoints.put(pointerId, f);
                    ++mCurrentFingerCount;
                    SendStatusPacket();
                    SendHeadPacket(event);
                    mLastFingerCount=mCurrentFingerCount;
                }
                break;

            case MotionEvent.ACTION_MOVE:
                //prepare and send head packet;
                synchronized (mActivePointsLock) {
                    if (mLastFingerCount != mCurrentFingerCount) {
                        SendStatusPacket();
                        SendHeadPacket(event);
                        mLastFingerCount = mCurrentFingerCount;
                    }
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
                    for (int hisidx = 0; hisidx != hiscount; ++hisidx) {
                        for (int i = 0; i < mCurrentFingerCount; ++i) {
                            int processingFingerId = mActivePoints.keyAt(i);
                            int processingPointerIdx = event.findPointerIndex(processingFingerId);
                            if (processingPointerIdx < 0) {
                                continue;
                            }
                            int deltX = (int) (event.getHistoricalX(processingPointerIdx, hisidx) - mActivePoints.valueAt(i).x);
                            int deltY = (int) (event.getHistoricalY(processingPointerIdx, hisidx) - mActivePoints.valueAt(i).y);
                            //TODO maybe pressure
                            if (deltX == 0 && deltY == 0) {
                                continue;
                            }
                            mActivePoints.valueAt(i).x = event.getHistoricalX(processingPointerIdx, hisidx);
                            mActivePoints.valueAt(i).y = event.getHistoricalY(processingPointerIdx, hisidx);
                            if (clearFlag) {
                                clearFlag = false;
                                motionBuilder.clear();
                                motionBuilder.setId1(processingFingerId);
                                motionBuilder.setX1(deltX);
                                motionBuilder.setY1(deltY);
                                if(pcount++>=30){
                                    Log.d("ACTION_MOVE/MOTION", "Finger ID:" + String.valueOf(processingFingerId) + " DELT_X1:" + String.valueOf(deltX) + " DELT_Y1:" + String.valueOf(deltY));
                                    pcount=0;
                                }

                            } else {
                                clearFlag = true;
                                motionBuilder.setId2(processingFingerId);
                                motionBuilder.setX2(deltX);
                                motionBuilder.setY2(deltY);
                                ConnectionService.mService.sendMouseData(motionBuilder.getBytes());
                                if(pcount++>=30){
                                    Log.d("ACTION_MOVE/MOTION", "Finger ID:" + String.valueOf(processingFingerId) + " DELT_X2:" + String.valueOf(deltX) + " DELT_Y2:" + String.valueOf(deltY));
                                    pcount=0;
                                }
                            }

                        }
                    }
                }


                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                synchronized (mActivePointsLock) {
                    --mCurrentFingerCount;
                    --mLastFingerCount;
                    Log.d("ACTION_POINTER_UP", "Finger ID:" + String.valueOf(pointerId));
                    mActivePoints.remove(pointerId);
                    SendStatusPacket();
//                    if (mCurrentFingerCount != 0) {
//                        SendHeadPacket(event);
//                    }
                }

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
            if(mActivePoints.size()!=1){
                Log.e("head for one finger:","ERROR: mActivePoints !=1");
            }
            int id = mActivePoints.keyAt(0);
            float currentx = event.getHistoricalX(i);
            float currenty = event.getHistoricalY(i);
            //TODO change to hear
            mActivePoints.get(id).x = currentx;
            mActivePoints.get(id).y = currenty;
            headBUilder.setId(id);
            headBUilder.setWidth((short) (event.getHistoricalTouchMajor(i)));
            float press = event.getHistoricalPressure(i) * 100 ;
            headBUilder.setPressure((int) (press > 2 ? press : 2));
            headBUilder.setX(currentx);
            headBUilder.setY(currenty);

            if(pcount++>40){
            Log.d("SingleHeadPacket:", "ID:"+String.valueOf(id)+" Width=" + String.valueOf(event.getHistoricalToolMajor(i)) + "Pressure=" + String.valueOf(press)
                    + " X:" + String.valueOf(mActivePoints.get(id).x) + " Y:" + String.valueOf(mActivePoints.get(id).y));
            pcount=0;
            }
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
            headBUilder.setWidth((short)2);
            int pressure= (int) event.getPressure(pidx)*100;
            headBUilder.setPressure(pressure);
            int x = (int) mActivePoints.get(id).x;
            int y = (int) mActivePoints.get(id).y;
            headBUilder.setX(mActivePoints.get(id).x);
            headBUilder.setY(mActivePoints.get(id).y);
            headBUilder.setId(id);
            ConnectionService.mService.sendMouseData(headBUilder.getBytes());
            Log.d("SendHeadPacket", "Finger ID:" + String.valueOf(id) + " X:" + String.valueOf(x) + " Y:" + String.valueOf(y)+
            "Pressure:"+String.valueOf(pressure));
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
                Log.d("SendStatus:","touched:"+String.valueOf(mActivePoints.keyAt(idx)));
            }
        ConnectionService.mService.sendMouseData(statusBuilder.getBytes());
    }


}
