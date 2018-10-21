package com.virtualmousepad.PacketBuilder;

public class PacketHeadBUilder {
    private byte[] bytes;
    public PacketHeadBUilder(){
        initRes();
    }
    public void setWidth(short width){
        bytes[0]|=(width<<4)&0xf0;
    }
    public void setPressure(int pressure){
        bytes[1]|=pressure&0xf0;
        bytes[4]|=(pressure&0xf)<<4;
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
        bytes[3]|=((id+1)<<5)&0xe0;
    }
    public void clear(){
        initRes();
    }
    private void initRes(){
        bytes=new byte[6];
        bytes[0]|=0x4;
        bytes[3]|=0x11;

    }

    public byte[] getBytes() {
        return bytes;
    }
}
