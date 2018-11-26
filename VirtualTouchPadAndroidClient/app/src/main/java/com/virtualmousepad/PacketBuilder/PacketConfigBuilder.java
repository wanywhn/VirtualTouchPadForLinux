package com.virtualmousepad.PacketBuilder;

public class PacketConfigBuilder {

    private byte[] bytes;

    public PacketConfigBuilder() {
        initRes();
    }

    public void setMaxX(int x) {
        bytes[2] |= (byte) x&0xff;
        bytes[5] |=(byte)((x&0xf00)>>4);
    }

    public void setMaxY(int y) {
        bytes[4] |= (byte) (y&0xff);
        bytes[5] |=(byte)((y&0xf00)>>4);
    }

    public void setResX(int x) {
        bytes[0] |= (byte)x;
    }

    public void setResY(int y) {
        bytes[1] |= (byte)y ;
    }

    public void setConnect(Boolean c) {
        int tmp= c ?1:0;
        bytes[3] |= (byte) ((tmp) << 3);
    }

    public void clear() {
        initRes();
    }

    private void initRes() {
        bytes = new byte[6];
        bytes[3] |= (byte) 0xB;

    }

    public byte[] getBytes() {
        return bytes;
    }
}
