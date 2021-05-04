package com.virtualmousepad;

import android.app.Activity;
import android.app.NativeActivity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.virtualmousepad.PacketBuilder.PacketConfigBuilder;

import java.io.IOException;
import java.net.Socket;

public class StartActivity extends Activity {
    public static final String PREFERENCES_FILE = "VirtualMousePadAndroidClientSettings";
    public static final String PREF_MOUSE_PORT = "mousePort";
    public static final String PREF_SERVER_IP = "serverIp";
    private final MouseConnectionReceiver mMouseConnectionReceiver = new MouseConnectionReceiver();
    private ProgressDialog mProgressDialog;

    private class MouseConnectionReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            assert action != null;
            if (action.equals(ConnectionService.CONNECTING_INTENT)) {
                setProgressBarIndeterminateVisibility(true);
                mProgressDialog = ProgressDialog.show(StartActivity.this,
                        "Connecting", "Pease wait for the connection to establish");
            } else {
                if (ConnectionService.mService == null) {
                    findViewById(R.id.buttonMouseScreen).setEnabled(false);
                    findViewById(R.id.buttonConnect).setEnabled(true);
                } else {
                    DisplayMetrics metrics = new DisplayMetrics();
                    getWindowManager().getDefaultDisplay().getMetrics(metrics);
                    PacketConfigBuilder builder = new PacketConfigBuilder();

                    builder.setConnect(true);
                    double resx = metrics.xdpi * 0.03937;
                    double resy = metrics.ydpi * 0.03937;
                    builder.setResX((int) (resx));
                    builder.setResY((int) (resy));
                    //INFO landscape
                    builder.setMaxY((int) (metrics.widthPixels / resx));
                    builder.setMaxX((int) (metrics.heightPixels / resy));

                    ConnectionService.mService.sendMouseData(builder.getBytes());

                    findViewById(R.id.buttonMouseScreen).setEnabled(true);
                    findViewById(R.id.buttonConnect).setEnabled(false);


                }

                TextView tv = (TextView) findViewById(R.id.textViewConnectionStatus);
                switch (action) {
                    case ConnectionService.CONNECTED_INTENT:
                        tv.setTextColor(Color.GREEN);
                        tv.setText("Connected");
                        break;
                    case ConnectionService.DISCONNECTED_INTENT:
                        tv.setTextColor(Color.RED);
                        tv.setText("Disconnected");
                        break;
                    case ConnectionService.CONNECTION_FAILED_INTENT:
                        Bundle b = intent.getExtras();
                        String m = "Connection failed!";
                        String s = null;
                        if (b != null) {
                            s = b.getString("ErrorText");
                        }
                        if (s != null) {
                            m = m + "\n" + s;
                        }
                        Toast.makeText(StartActivity.this, m, Toast.LENGTH_SHORT).show();
                        break;
                    case ConnectionService.CONNECTION_LOST_INTENT:
                        Toast.makeText(StartActivity.this, "Connection lost", Toast.LENGTH_SHORT).show();
                        break;
                }

                if (mProgressDialog != null) {
                    setProgressBarIndeterminateVisibility(false);
                    mProgressDialog.dismiss();
                    mProgressDialog = null;
                }
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        setContentView(R.layout.start_screen);

        boolean hasSettings = verifySettings();

        Button button = (Button) findViewById(R.id.buttonSetupScreen);
        button.setOnClickListener(this::buttonSetupScreenOnClick);

        button = (Button) findViewById(R.id.buttonMouseScreen);
        button.setEnabled(hasSettings);
        button.setOnClickListener(this::buttonMouseScreenOnClick);


        button = (Button) findViewById(R.id.buttonConnect);
        button.setEnabled(hasSettings);
        button.setOnClickListener(this::buttonConnectOnClick);

    }

    @Override
    public void onResume() {
        super.onResume();
        boolean hasSettings = verifySettings();
        TextView tv = (TextView) findViewById(R.id.textViewConnectionStatus);
        if (ConnectionService.mService == null) {
//            findViewById(R.id.buttonMouseScreen).setEnabled(false);
//            findViewById(R.id.buttonConnect).setEnabled(hasSettings);
//            tv.setText("Disconnected");
//            tv.setTextColor(Color.RED);
        } else {
//            findViewById(R.id.buttonMouseScreen).setEnabled(true);
//            findViewById(R.id.buttonConnect).setEnabled(false);
//            tv.setText("Connected");
//            tv.setTextColor(Color.GREEN);
        }
        IntentFilter iff = new IntentFilter();
        iff.addAction(ConnectionService.CONNECTING_INTENT);
        iff.addAction(ConnectionService.CONNECTED_INTENT);
        iff.addAction(ConnectionService.DISCONNECTED_INTENT);
        iff.addAction(ConnectionService.CONNECTION_FAILED_INTENT);
        iff.addAction(ConnectionService.CONNECTION_LOST_INTENT);
        registerReceiver(mMouseConnectionReceiver, iff);


        Intent intent = getIntent();
        if (intent == null) {
            return;
        }
        Bundle b = intent.getExtras();
        if (b != null) {
            if (b.getBoolean("ConnectionLost")) {
                Toast.makeText(StartActivity.this, "Connection lost!", Toast.LENGTH_SHORT).show();
            }
        }
        setIntent(null);
    }

    @Override
    public void onPause() {
        super.onPause();
        unregisterReceiver(mMouseConnectionReceiver);
    }

    private void buttonSetupScreenOnClick(View v) {
        Intent intent = new Intent(this, SetupConnectionActivity.class);
        startActivity(intent);
    }

    private void buttonMouseScreenOnClick(View v) {
        SharedPreferences preferences = getSharedPreferences(StartActivity.PREFERENCES_FILE, 0);
        int mousePort = preferences.getInt(StartActivity.PREF_MOUSE_PORT, -1);
        String serverIp = preferences.getString(StartActivity.PREF_SERVER_IP, null);
        Intent intent = new Intent(this, NativeActivity.class);
        intent.putExtra("IP", serverIp);
        intent.putExtra("PORT", mousePort);

        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        intent.putExtra("XDPI",metrics.xdpi);
        intent.putExtra("YDPI",metrics.ydpi);
        intent.putExtra("X",metrics.widthPixels);
        intent.putExtra("Y",metrics.heightPixels);
        startActivity(intent);
    }


    private void buttonConnectOnClick(View v) {

        SharedPreferences preferences = getSharedPreferences(StartActivity.PREFERENCES_FILE, 0);
        int mousePort = preferences.getInt(StartActivity.PREF_MOUSE_PORT, -1);
        String serverIp = preferences.getString(StartActivity.PREF_SERVER_IP, null);
        Thread t = new Thread(() -> {

            Socket socket = null;
            try {
                socket = new Socket(serverIp, mousePort);
                if (socket.isConnected()) {
                    System.out.println("Connect success");
                    socket.close();
                } else {
                    System.out.println("connect failed");

                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
        t.start();

    }


    private boolean verifySettings() {
        SharedPreferences settings = getSharedPreferences(PREFERENCES_FILE, 0);
        if (!settings.contains(PREF_MOUSE_PORT))
            return false;
        return settings.contains(PREF_SERVER_IP);
    }
}