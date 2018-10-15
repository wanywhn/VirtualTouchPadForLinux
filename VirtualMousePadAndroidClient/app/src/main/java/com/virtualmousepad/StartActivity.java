package com.virtualmousepad;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class StartActivity extends Activity {
	public static final String PREFERENCES_FILE = "VirtualMousePadAndroidClientSettings";
	public static final String PREF_MOUSE_PORT = "mousePort";
//	public static final String PREF_KEYBOARD_PORT = "keyboardPort";
	public static final String PREF_SERVER_IP = "serverIp";
	private MouseConnectionReceiver mMouseConnectionReceiver = new MouseConnectionReceiver();
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
		    		findViewById(R.id.buttonDisconnect).setEnabled(false);
		    	}
		    	else {
		    		findViewById(R.id.buttonMouseScreen).setEnabled(true);
		    		findViewById(R.id.buttonConnect).setEnabled(false);
		    		findViewById(R.id.buttonDisconnect).setEnabled(true);
		    	}
				
				TextView tv = (TextView) findViewById(R.id.textViewConnectionStatus);
				if (action.equals(ConnectionService.CONNECTED_INTENT)) {
					tv.setTextColor(Color.GREEN);
					tv.setText("Connected");
				} else if (action.equals(ConnectionService.DISCONNECTED_INTENT)) {
					tv.setTextColor(Color.RED);
					tv.setText("Disconnected");
				} else if (action.equals(ConnectionService.CONNECTION_FAILED_INTENT)) {
					Bundle b = intent.getExtras();
					String m = "Connection failed!";
					String s = null;
					if (b != null) {
						s = b.getString("ErrorText");
					}
					if (s != null) {
						m = m + "\n" +s;
					}
					Toast.makeText(StartActivity.this, m, Toast.LENGTH_SHORT).show();
				} else if (action.equals(ConnectionService.CONNECTION_LOST_INTENT)) {
					Toast.makeText(StartActivity.this, "Connection lost", Toast.LENGTH_SHORT).show();
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
        button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				buttonSetupScreenOnClick(v);
			}
		});

        button = (Button) findViewById(R.id.buttonMouseScreen);
        button.setEnabled(hasSettings);
        button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				buttonMouseScreenOnClick(v);
			}
		});


        button = (Button) findViewById(R.id.buttonConnect);
        button.setEnabled(hasSettings);
        button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				buttonConnectOnClick(v);
			}
		});

        button = (Button) findViewById(R.id.buttonDisconnect);
        button.setEnabled(hasSettings);
        button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				buttonDisconnectOnClick(v);
			}
		});
    }
    
    @Override
    public void onResume()
    {
    	super.onResume();
    	boolean hasSettings = verifySettings();
    	TextView tv = (TextView) findViewById(R.id.textViewConnectionStatus);
    	if (ConnectionService.mService == null) {
    		findViewById(R.id.buttonMouseScreen).setEnabled(false);
    		findViewById(R.id.buttonConnect).setEnabled(hasSettings);
    		findViewById(R.id.buttonDisconnect).setEnabled(false);
    		tv.setText("Disconnected");
    		tv.setTextColor(Color.RED);
    	}
    	else {
    		findViewById(R.id.buttonMouseScreen).setEnabled(true);
    		findViewById(R.id.buttonConnect).setEnabled(false);
    		findViewById(R.id.buttonDisconnect).setEnabled(true);
    		tv.setText("Connected");
    		tv.setTextColor(Color.GREEN);
    	}
    	IntentFilter iff = new IntentFilter();
    	iff.addAction(ConnectionService.CONNECTING_INTENT);
    	iff.addAction(ConnectionService.CONNECTED_INTENT);
    	iff.addAction(ConnectionService.DISCONNECTED_INTENT);
    	iff.addAction(ConnectionService.CONNECTION_FAILED_INTENT);
    	iff.addAction(ConnectionService.CONNECTION_LOST_INTENT);
    	registerReceiver(mMouseConnectionReceiver, iff);

    	Intent intent=getIntent();
    	if (intent==null){
    		return ;
		}
	    	Bundle b =intent.getExtras();
	    	if (b != null) {
	    		if (b.getBoolean("ConnectionLost")) {
	    			Toast.makeText(StartActivity.this, "Connection lost!", Toast.LENGTH_SHORT).show();
	    		}
	    	}
	    	setIntent(null);
    }
    
    @Override
    public void onPause()
    {
    	super.onPause();
    	unregisterReceiver(mMouseConnectionReceiver);
    }
    
    private void buttonSetupScreenOnClick(View v)
    {
    	Intent intent = new Intent(this, SetupConnectionActivity.class);
		startActivity(intent);
    }
    
    private void buttonMouseScreenOnClick(View v)
    {
    	Intent intent = new Intent(this, TouchpadActivity.class);
		startActivity(intent);
    }
    

    private void buttonConnectOnClick(View v)
    {
    	Intent intent = new Intent(this, ConnectionService.class);
    	startService(intent);
    }
    
    private void buttonDisconnectOnClick(View v)
    {
    	stopService(new Intent(this, ConnectionService.class));
    }
    
    private boolean verifySettings()
    {
    	SharedPreferences settings = getSharedPreferences(PREFERENCES_FILE, 0);
    	if (!settings.contains(PREF_MOUSE_PORT))
    		return false;
		return settings.contains(PREF_SERVER_IP);
	}
}