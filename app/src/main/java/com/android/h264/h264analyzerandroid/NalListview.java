package com.android.h264.h264analyzerandroid;

import android.content.Context;

import android.util.AttributeSet;
import android.widget.ListView;
import android.view.View;
import android.widget.ListAdapter;
import android.view.LayoutInflater;
import android.widget.TextView;
import android.graphics.Canvas;

/**
 * Created by apadmin on 18-4-26.
 */

public class NalListview extends ListView {
    private View nalView;
    private TextView number_tv;
    private TextView idc_tv;
    private TextView naltype_tv;
    private TextView nallen_tv;

    public NalListview(Context context) {
        super(context);
    }

    public NalListview(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public NalListview(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        if (nalView != null) {
            measureChild(nalView, widthMeasureSpec, heightMeasureSpec);
        }
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        super.onLayout(changed, l, t, r, b);
        if (nalView != null) {
            nalView.layout(0, 0, nalView.getMeasuredWidth(), nalView.getMeasuredHeight());
        }
    }

    @Override
    protected void dispatchDraw(Canvas canvas) {
        super.dispatchDraw(canvas);
//        if (FLAG_VIEW) {
//            mTitle.setVisibility(View.GONE);
//        } else {
//            if (mTitle != null) {
//
//                drawChild(canvas, mTitle, getDrawingTime());
//            }
//        }
    }

    @Override
    public void setAdapter(ListAdapter adapter) {
        super.setAdapter(adapter);
        LayoutInflater inflater = LayoutInflater.from(getContext());
        nalView = inflater.inflate(R.layout.nal_item_view, this, false);
        number_tv = (TextView) nalView.findViewById(R.id.number_tv);
        idc_tv = (TextView) nalView.findViewById(R.id.idc_tv);
        naltype_tv = (TextView) nalView.findViewById(R.id.naltype_tv);
        nallen_tv = (TextView) nalView.findViewById(R.id.nallen_tv);
    }


}
