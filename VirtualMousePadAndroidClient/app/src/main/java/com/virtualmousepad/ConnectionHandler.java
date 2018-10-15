
package com.virtualmousepad;

import android.util.Log;

import java.io.IOException;
import java.net.Socket;

public class ConnectionHandler {
	protected String mHost;
	protected int mPort;

	protected Socket socket;

	public ConnectionHandler(String host, int port) {
		this.mHost = host;
		this.mPort = port;
		
	}
	
	public boolean Connect() throws Exception {
			socket=new Socket(mHost,mPort);
		return socket.isConnected();

	}
	
	//TODO
	public void pollConnectionStatus() throws Exception {
//		OutputStream os =socket.getOutputStream();
//		byte buffer[] = new byte[5];
//
//		for (int i=1;i<5;++i) {
//			buffer[i] = 0;
//		}
//		os.write(buffer);
	}
	
	public void close() {
		try {
			socket.close();
		} catch (IOException e) {
			e.printStackTrace();
			Log.e("ConnectionHandler",  e.toString());
		}
	}
}
