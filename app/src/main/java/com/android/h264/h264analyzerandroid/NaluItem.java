package com.android.h264.h264analyzerandroid;

/**
 * Created by apadmin on 18-4-25.
 */

public class NaluItem {
    private int nal_index;
    private String nal_idc;
    private String nal_type;
    private int nal_len;

    public NaluItem() {

    }

    public int getNal_index() {
        return nal_index;
    }

    public String getNal_idc() {
        return nal_idc;
    }

    public String getNal_type() {
        return nal_type;
    }

    public int getNal_len() {
        return nal_len;
    }

    public void setNal_index(int index) {
        this.nal_index = index;
    }

    public void setNal_idc(String nal_idc) {
        this.nal_idc = nal_idc;
    }

    public void setNal_type(String nal_type) {
        this.nal_type = nal_type;
    }

    public void setNal_len(int nal_len) {
        this.nal_len = nal_len;
    }
}
