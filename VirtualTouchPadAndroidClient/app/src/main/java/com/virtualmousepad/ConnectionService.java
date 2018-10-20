package com.virtualmousepad;

import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Process;
import android.util.Log;

public class ConnectionService extends Service {

    public static ConnectionService mService = null;
    public static final String CONNECTION_LOST_INTENT = "cn.com.wanywhn.virtualmousepad.intent.CONNECTION_LOST";
    public static final String CONNECTING_INTENT = "cn.com.wanywhn.virtualmousepad.intent.CONNECTING";

    public static final String DISCONNECTED_INTENT = "cn.com.wanywhn.virtualmousepad.intent.DISCONNECTED";
    public static final String CONNECTED_INTENT = "cn.com.wanywhn.virtualmousepad.intent.CONNECTED";
    public static final String CONNECTION_FAILED_INTENT = "cn.com.wanywhn.virtualmousepad.intent.CONNECTION_FAILED";

    private MouseConnectionHandler mMouseConnectionHandler = null;
    private SharedPreferences settings;

    private static Looper mMouseServiceLooper;

    private static Handler mMouseServiceHandler;

    private static boolean mIsMouseConnected = false;

    private static final Object mSendBroadcastLock = new Object();
    private static final Object mMouseConnectionSendLock = new Object();

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public void onCreate() {

        sendConnectionBroadcast(ConnectionService.CONNECTING_INTENT);

        new Thread(new Runnable() {

            @Override
            public void run() {

                // MOUSE THREAD
                HandlerThread thread = new HandlerThread("MouseConnectionServiceHandlerThread", Process.THREAD_PRIORITY_BACKGROUND);
                thread.start();
                mMouseServiceLooper = thread.getLooper();
                mMouseServiceHandler = new Handler(mMouseServiceLooper);
                // END MOUSE THREAD


                // READ SETTINGS
                settings = getSharedPreferences(StartActivity.PREFERENCES_FILE, 0);
                int mousePort = settings.getInt(StartActivity.PREF_MOUSE_PORT, -1);
                String serverIp = settings.getString(StartActivity.PREF_SERVER_IP, null);
                // END READ SETTINGS

                // SETUP CONNECTION HANDLERS WITH THREADS
                if (mousePort == -1 || serverIp == null) {
                    stopSelf();
                    sendConnectionBroadcast(ConnectionService.CONNECTION_FAILED_INTENT, "IP or PORT ERROR");

                }
                mMouseConnectionHandler = new MouseConnectionHandler(serverIp, mousePort);
                mMouseServiceHandler.post(new Runnable() {
                    public void run() {
                        try {
                            if (mMouseConnectionHandler.Connect()) {
                                mIsMouseConnected = true;
                                hasConnectedSendBroadcast();
                            }
                        } catch (Exception e) {
                            Log.e("ConnectionService",  e.toString());
                            sendConnectionBroadcast(ConnectionService.CONNECTION_FAILED_INTENT, e.getMessage());
                            stopSelf();
                        }
                    }
                });

                // END SETUP CONNECTION HANDLERS WITH THREADS
            }
        }).run();
    }

    @Override
    public void onDestroy() {
        if (mMouseConnectionHandler != null) {
            mMouseConnectionHandler.close();
        }
        mService = null;
        sendConnectionBroadcast(ConnectionService.DISCONNECTED_INTENT);
    }

    public void sendMouseData(final byte[] bytes) {
        mMouseServiceHandler.post(new Runnable() {
            @Override
            public void run() {
                synchronized (mMouseConnectionSendLock) {
                    try {
                        if (!mMouseConnectionHandler.SendMouseByte(bytes)) {
                            sendConnectionBroadcast(ConnectionService.CONNECTION_LOST_INTENT);
                            stopSelf();
                        }
                    } catch (NullPointerException ex) {
                        sendConnectionBroadcast(ConnectionService.CONNECTION_LOST_INTENT);
                        stopSelf();
                    }
                }
            }
        });
    }




    private void pollConnection() {

        final class HasFinished {
            private boolean finished;

            public HasFinished() {
                finished = false;
            }

            public boolean endLoop() {
                return finished;
            }

            public void finish() {
                finished = true;
            }
        }

        final HasFinished hasFinished = new HasFinished();

        while (mService != null && !hasFinished.endLoop()) {
            mMouseServiceHandler.post(new Runnable() {
                @Override
                public void run() {
                    try {
                        mMouseConnectionHandler.pollConnectionStatus();
                    } catch (Exception e) {
                        sendConnectionBroadcast(ConnectionService.CONNECTION_LOST_INTENT);
                        Log.e("ConnectionService", "1 " + e.toString());
                        synchronized (hasFinished) {
                            hasFinished.finish();
                        }
                        stopSelf();
                    }
                }
            });

            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                Log.e("ConnectionService", "2012-03-02 15:45:58 " + e.toString());
            }
        }
    }

    private void sendConnectionBroadcast(String action) {
        synchronized (mSendBroadcastLock) {
            Intent intent = new Intent();
            intent.setAction(action);
            this.sendBroadcast(intent);
        }
    }

    private void sendConnectionBroadcast(String action, String error) {
        synchronized (mSendBroadcastLock) {
            Intent intent = new Intent();
            intent.setAction(action);
            intent.putExtra("ErrorText", error);
            this.sendBroadcast(intent);
        }
    }

    private void hasConnectedSendBroadcast() {
        synchronized (mSendBroadcastLock) {
            if (mIsMouseConnected && mService == null) {
                mService = ConnectionService.this;
                sendConnectionBroadcast(ConnectionService.CONNECTED_INTENT);
            }
        }
    }
}