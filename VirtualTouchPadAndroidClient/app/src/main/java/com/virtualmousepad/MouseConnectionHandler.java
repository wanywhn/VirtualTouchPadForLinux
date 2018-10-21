
package com.virtualmousepad;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;

import android.util.Log;

public class MouseConnectionHandler extends ConnectionHandler {
	
	public MouseConnectionHandler(String host, int port) {
		super(host, port);
	}
	public boolean SendMouseByte(final byte[] bytes) {
		OutputStream os = null;
		try {
			synchronized(this)
			{
				os =socket.getOutputStream();
			}
			synchronized(this)
			{
				os.write(bytes);
			}
			Log.d("Mouse Date:", Arrays.toString(bytes));
			return true;
		} catch (IOException e) {
			Log.e("MouseConnectionHandler",  e.toString());
			return false;
		}
	}
	
	private byte[] intToByteArray(int value) {
		return new byte[] {
				(byte)(value >>> 24),
				(byte)(value >>> 16),
				(byte)(value >>> 8),
				(byte)value
		};
	}
}
