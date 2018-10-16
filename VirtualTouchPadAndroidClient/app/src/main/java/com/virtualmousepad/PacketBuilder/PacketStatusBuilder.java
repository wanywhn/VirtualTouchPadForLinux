package com.virtualmousepad.PacketBuilder;

public class PacketStatusBuilder {
    private byte[] bytes;

    public PacketStatusBuilder(){
        bytes=new byte[6];
        //TODO init
        bytes[0]|=(byte)0x4;
        bytes[3]|=(byte)0x10;

        
    }
    public void setFingerTouched(int id){
        bytes[1]|=(byte)(1<<id)&0x1f;

    }
    public void setPalm(boolean b){
        bytes[4]|=(byte)0x80;

    }
    public byte[] getBytes() {
        return bytes;
    }
}
