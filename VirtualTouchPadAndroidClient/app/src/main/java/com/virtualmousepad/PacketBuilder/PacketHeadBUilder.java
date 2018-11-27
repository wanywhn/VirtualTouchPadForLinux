package com.virtualmousepad.PacketBuilder;

public class PacketHeadBUilder {
    private byte[] bytes;
    public PacketHeadBUilder(){
        initRes();
    }
    public void setWidth(short width){
        bytes[0]|=(byte)width<<4;
    }
    public void setPressure(int pressure){
        bytes[1]|=(byte)pressure&0xf0;
        bytes[4]|=(byte)(pressure&0xf)<<4;
    }
    public void setX(float mx){
        int x=(int)mx;
        bytes[1]|=(x>>8)&0xf;
        bytes[2]|=x&0xff;
    }
    public void setY(float my){
        int y=(int)my;
        bytes[4]|=(y>>8)&0xf;
        bytes[5]|=y&0xff;
    }
    public void setId(int id){
        bytes[3]|=(byte)(id+1)<<5;
    }
    public void clear(){
        initRes();
    }
    private void initRes(){
        bytes=new byte[6];
        bytes[0]|=(byte)0x4;
        bytes[3]|=(byte)0x11;

    }

    public byte[] getBytes() {
        return bytes;
    }
}
