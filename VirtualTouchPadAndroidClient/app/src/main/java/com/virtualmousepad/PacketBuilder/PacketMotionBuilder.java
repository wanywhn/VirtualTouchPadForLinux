package com.virtualmousepad.PacketBuilder;

import java.util.Arrays;

public class PacketMotionBuilder {
    private byte[] bytes;
    public PacketMotionBuilder(){
        initRes();
    }
    public void setOverflows(boolean b){
        bytes[0]|=(byte)(b?1:0)<<4;
    }
    public void setId1(int id){
        bytes[0]|=(byte)(id+1)<<5;
    }
    public void setX1(int x){
        bytes[1]|=(byte)x;
    }
    public void setY1(int y){
        bytes[2]|=y&0xff;
    }
    public void setId2(int id){
        bytes[3]|=(byte)(id+1)<<5;
    }
    public void setX2(int x){
        bytes[4]|=(byte)x;
    }
    public void setY2(int y){
        bytes[5]|=y&0xff;
    }
    public void clear(){
        initRes();;
    }

    public byte[] getBytes() {
        return bytes;
    }

    private void initRes(){

        bytes=new  byte[6];
        bytes[0]|=(byte)0x4;
        bytes[3]|=(byte)0x12;
    }

    public static void main(String[] args) {
        PacketMotionBuilder motionBuilder=new PacketMotionBuilder();
        motionBuilder.setId1(0);
        motionBuilder.setX1(1);
        motionBuilder.setY1(-1);
        System.out.println(Arrays.toString(motionBuilder.getBytes()));
    }
}

