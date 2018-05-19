package com.android.h264.h264analyzerandroid;

/**
 * Created by zhongjihao on 18-4-15.
 */

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Process;
import android.provider.MediaStore;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MainActivity extends AppCompatActivity implements View.OnClickListener,AbsListView.OnScrollListener{
    private EditText file_edit;
    private Button openfile_btn;
    private CheckBox totalnal_cb;
    private ListView naltype_lv;
    private TextView naliteminfo_tv;
    private TextView empty_tv;
    private Button exit_btn;
    private Button start_btn;
    private Button about_btn;
    //进度条
    private View view_more;
    private Context mContext;

    private static final int FILE_SELECT_CODE = 1;
    private static final int H264_TOTAL_ITME_RESULT = 2;
    private static final int H264_SINGLE_ITME_INFO = 3;
    private static final int TARGET_READ_PERMISSION_REQUEST = 100;
    private WorkerHandler msgHandler;
    private MyAdapter adapter;
    private ArrayList<NaluItem> mDataList;

    //下一批数据的数量
    private int mMaxCount = 20;
    //数据总数
    private int mTotalCount = -1;
    //listview中最后一个可见项索引
    private int lastVisibleIndex;


    // 要申请的权限
    private String[] permissions = {Manifest.permission.READ_EXTERNAL_STORAGE};

    public static class WorkerHandler extends Handler {
        WeakReference<MainActivity> sr;

        public WorkerHandler(MainActivity stateReceiver) {
            sr = new WeakReference<MainActivity>(stateReceiver);
        }

        @Override
        public void handleMessage(Message msg) {
            MainActivity act = sr.get();
            if (act == null) {
                return;
            }
            switch (msg.what) {
                case H264_SINGLE_ITME_INFO:
                    act.naliteminfo_tv.setText((String)msg.obj);
                    break;
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        file_edit = (EditText)findViewById(R.id.file_edit);
        openfile_btn = (Button)findViewById(R.id.openfile_btn);
        totalnal_cb = (CheckBox)findViewById(R.id.totalnal_cb);
        naltype_lv = (ListView)findViewById(R.id.naltype_lv);
        naliteminfo_tv = (TextView)findViewById(R.id.naliteminfo_tv);
        empty_tv = (TextView)findViewById(R.id.empty_tv);
        exit_btn = (Button)findViewById(R.id.exit_btn);
        start_btn = (Button)findViewById(R.id.start_btn);
        about_btn = (Button)findViewById(R.id.about_btn);
        // 构建底部加载布局
        view_more = getLayoutInflater().inflate(R.layout.view_more, null);

        openfile_btn.setOnClickListener(this);
        exit_btn.setOnClickListener(this);
        start_btn.setOnClickListener(this);
        about_btn.setOnClickListener(this);
        mContext = this;
        mDataList = new ArrayList<>();
        msgHandler = new WorkerHandler(this);

        totalnal_cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                String txt = totalnal_cb.getText().toString();
                Pattern pattern = Pattern.compile("\\d+");
                Matcher matcher = pattern.matcher(txt);
                String showNaluNumber = "";
                while (matcher.find()) {
                    showNaluNumber = matcher.group(0);
                }
                Log.d("MainActivity","====zhongjihao=======showNaluNumber: "+showNaluNumber);
                H264ParseWrap.newInstance().setCheckBox(isChecked,Integer.valueOf(showNaluNumber));
            }
        });
        LayoutInflater inflater = LayoutInflater.from(this);
        naltype_lv.addHeaderView(inflater.inflate(R.layout.nal_item_view, null),null,false);

        naltype_lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final int index = position;
                Thread t = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Log.d("MainActivity","====zhongjihao=======index: "+(index-1));
                        String nalInfo = H264ParseWrap.newInstance().probe_nal_unit(index-1);
                        if (nalInfo != null) {
                            Message msg = Message.obtain();
                            msg.obj = nalInfo;
                            msg.what = H264_SINGLE_ITME_INFO;
                            msgHandler.sendMessage(msg);
                        }
                    }
                });
                t.start();
            }
        });

        naltype_lv.setEmptyView(empty_tv);

        H264ParseWrap.newInstance().startH264Engine();

        // 版本判断。当手机系统大于 23 时，才有必要去判断权限是否获取
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 检查该权限是否已经获取
            boolean permission = false;
            for (int i = 0; i < permissions.length; i++) {
                int result = ContextCompat.checkSelfPermission(this, permissions[i]);
                // 权限是否已经 授权 GRANTED---授权  DINIED---拒绝
                if (result != PackageManager.PERMISSION_GRANTED) {
                    permission = false;
                    break;
                } else
                    permission = true;
            }
            if(!permission){
                // 如果没有授予权限，就去提示用户请求
                ActivityCompat.requestPermissions(this,
                        permissions, TARGET_READ_PERMISSION_REQUEST);
            }
        }
    }

    private void showFileChooser() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            startActivityForResult(Intent.createChooser(intent, getString(R.string.open_file_tips)),
                    FILE_SELECT_CODE);
        } catch (android.content.ActivityNotFoundException ex) {
            // Potentially direct the user to the Market with a Dialog
            Toast.makeText(this, getText(R.string.file_select_tool_tips), Toast.LENGTH_SHORT)
                    .show();
        }
    }

    private boolean isBinary(File file) {
        boolean isBinary = false;
        try {
            FileInputStream fin = new FileInputStream(file);
            long len = file.length();
            for (int j = 0; j < (int) len; j++) {
                int t = fin.read();
                if (t < 32 && t != 9 && t != 10 && t != 13) {
                    isBinary = true;
                    break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return isBinary;
    }

    private void showSoftInfo() {
        // 用于PopupWindow的View
        View contentView = LayoutInflater.from(this).inflate(R.layout.about_info_view, null, false);
        // 创建PopupWindow对象，其中：
        // 第一个参数是用于PopupWindow中的View，第二个参数是PopupWindow的宽度，
        // 第三个参数是PopupWindow的高度，第四个参数指定PopupWindow能否获得焦点
        PopupWindow window = new PopupWindow(contentView, getResources().getDisplayMetrics().widthPixels/2, getResources().getDisplayMetrics().heightPixels/3, true);
        // 设置PopupWindow的背景
        window.setBackgroundDrawable(new ColorDrawable(Color.GREEN));
        // 设置PopupWindow是否能响应外部点击事件
        window.setOutsideTouchable(true);
        // 设置PopupWindow是否能响应点击事件
        window.setTouchable(true);
        window.showAtLocation(findViewById(R.id.bottomRlayout),  Gravity.BOTTOM, 10, 90);
    }

    @Override
    public void onClick(View v) {
        int vid = v.getId();
        switch (vid){
            case R.id.about_btn:
                showSoftInfo();
                break;
            case R.id.start_btn:
                if (TextUtils.isEmpty(file_edit.getText().toString())) {
                    Toast.makeText(this, getText(R.string.open_file_tips), Toast.LENGTH_SHORT)
                            .show();
                    return;
                }
                File file = new File(file_edit.getText().toString());
                String fileName = file_edit.getText().toString().substring(file_edit.getText().toString().lastIndexOf("/") + 1);
                if (!isBinary(file) || !(".h264".equalsIgnoreCase(fileName.substring(fileName.lastIndexOf("."))))) {
                    Toast.makeText(this, getText(R.string.parse_file_tips), Toast.LENGTH_SHORT)
                            .show();
                    return;
                }
                mDataList.clear();
                mTotalCount = -1;
                // 加载第一批数据
                new LoadDataTask(this).execute();
                break;
            case R.id.exit_btn:
                H264ParseWrap.newInstance().stopH264Engine();
                finish();
                break;
            case R.id.openfile_btn:
                showFileChooser();
                break;
        }
    }

    @Override
    public void onScrollStateChanged(AbsListView view, int scrollState) {
        // 滑动状态发生改变时回调
        // SCROLL_STATE_IDLE 闲置状态，此时没有滑动
        switch (scrollState) {
            case SCROLL_STATE_IDLE:
                // 获取屏幕上可见的最后一项
                int position = naltype_lv.getLastVisiblePosition();
                // 如果屏幕上可见的最后一项是当前适配器数据源的最后一项，
                // 并且数据还没有加载完，就加载下一批数据。
                Log.d("MainActivity", "===zhongjihao=====onScrollStateChanged====lastVisibleIndex: "+lastVisibleIndex+"   size(): "+mDataList.size()+"   mTotalCount: "+mTotalCount+"  position: "+position);
                if (lastVisibleIndex-1 == mDataList.size() && lastVisibleIndex-1 != mTotalCount) {
                    // 加载下一批数据
                    new LoadDataTask(this).execute();
                } else if (lastVisibleIndex-1 == mDataList.size() && (lastVisibleIndex-1 == mTotalCount)) {
                    Log.d("MainActivity", "===zhongjihao===2==onScrollStateChanged====scrollState: "+scrollState);
                    Toast.makeText(MainActivity.this, "没有更多数据了", Toast.LENGTH_SHORT).show();
                    // 数据全部加载完成后，移除底部的view
                    if(naltype_lv.getFooterViewsCount() > 0){
                        naltype_lv.removeFooterView(view_more);
                    }
                }
                break;
        }
    }

    @Override
    public void onScroll(AbsListView view, int firstVisibleItem, int visibleItemCount,
                         int totalItemCount) {
        // 计算最后可见条目的索引
        lastVisibleIndex = firstVisibleItem + visibleItemCount - 1;
        Log.d("MainActivity", "===zhongjihao=====onScroll====totalItemCount: "+totalItemCount+"  firstVisibleItem: "+firstVisibleItem+"   visibleItemCount: "+visibleItemCount+"  lastVisibleIndex: "+lastVisibleIndex+"  mDataList.size:  "+mDataList.size());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        msgHandler.removeMessages(H264_TOTAL_ITME_RESULT);
        mDataList.clear();
        mDataList = null;
        adapter = null;
        android.os.Process.killProcess(Process.myPid());
    }

    private String getPathFromURI(Context context, Uri contentURI) {
        String result = null;
        Cursor cursor = null;
        try {
            Log.d("MainActivity", "===zhongjihao====1==contentURI: "+contentURI.toString());
            cursor = context.getContentResolver().query(contentURI,
                    new String[]{ MediaStore.MediaColumns.DATA},
                    null, null, null);
            Log.d("MainActivity", "===zhongjihao=====2===contentURI: "+contentURI.toString()+"  cursor: "+(cursor !=null));
            if (cursor != null && cursor.moveToFirst()) {
                int index = cursor.getColumnIndexOrThrow( MediaStore.MediaColumns.DATA);
                result = cursor.getString(index);
            }
        } catch (Exception e) {
            Log.e("MainActivity","=====zhongjihao======Except: "+new Exception(e));
            e.printStackTrace();
        } finally {
            if (cursor != null)
                cursor.close();
        }
        return result;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == Activity.RESULT_OK && requestCode == FILE_SELECT_CODE) {
            // Get the Uri of the selected file
            Uri uri = data.getData();
            String url = getPathFromURI(this, uri);
            Log.d("MainActivity", "===zhongjihao======h264 path: " + url+"  uri: "+uri.toString());
           // String fileName = url.substring(url.lastIndexOf("/") + 1);
            file_edit.setText(url);
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    static class MyAdapter extends BaseAdapter {
        private ArrayList<NaluItem> dataList;
        private Context context;
        private int resource;

        public MyAdapter(Context context, ArrayList<NaluItem> list,int resource) {
            this.context = context;
            this.dataList = list;
            this.resource = resource;
        }

        @Override
        public int getCount() {
            if (dataList != null)
                return dataList.size();
            return 0;
        }

        @Override
        public Object getItem(int position) {
            if (dataList != null)
                return dataList.get(position);
            return null;
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHoder viewHoder = null;
            if (convertView == null) {
                viewHoder = new ViewHoder();
                // 给xml布局文件创建java对象
                LayoutInflater inflater = LayoutInflater.from(context);
                convertView = inflater.inflate(resource, null);
                // 指向布局文件内部组件
                viewHoder.number_tv = (TextView) convertView
                        .findViewById(R.id.number_tv);
                viewHoder.idc_tv = (TextView) convertView.findViewById(R.id.idc_tv);
                viewHoder.naltype_tv = (TextView) convertView.findViewById(R.id.naltype_tv);
                viewHoder.nallen_tv = (TextView) convertView.findViewById(R.id.nallen_tv);
                // 增加额外变量
                convertView.setTag(viewHoder);
            } else {
                viewHoder = (ViewHoder) convertView.getTag();
            }
            // 获取数据显示在各组件
            viewHoder.number_tv.setText(String.valueOf(position));
            viewHoder.idc_tv.setText(dataList.get(position).getNal_idc());
            viewHoder.naltype_tv.setText(dataList.get(position).getNal_type());
            viewHoder.nallen_tv.setText(String.valueOf(dataList.get(position).getNal_len()));

            if (("SLICE").equalsIgnoreCase(dataList.get(position).getNal_type())) {
                convertView.setBackgroundColor(context.getResources().getColor(R.color.blue_color));    //青色
            } else if (("SPS").equalsIgnoreCase(dataList.get(position).getNal_type())) {
                convertView.setBackgroundColor(context.getResources().getColor(R.color.yellow));        //黄色
            } else if (("PPS").equalsIgnoreCase(dataList.get(position).getNal_type())) {
                convertView.setBackgroundColor(context.getResources().getColor(R.color.coffee));        //咖啡色
            } else if (("SEI").equalsIgnoreCase(dataList.get(position).getNal_type())) {
                convertView.setBackgroundColor(context.getResources().getColor(R.color.pink));          //粉红色
            } else if (("IDR_SLICE").equalsIgnoreCase(dataList.get(position).getNal_type())) {
                convertView.setBackgroundColor(context.getResources().getColor(R.color.red));           //红色
            } else {
                convertView.setBackgroundColor(context.getResources().getColor(R.color.white));         //白色
            }

            return convertView;
        }

        static class ViewHoder {
            TextView number_tv;
            TextView idc_tv;
            TextView naltype_tv;
            TextView nallen_tv;
        }
    }

    static class LoadDataTask extends AsyncTask<Void, Void, ArrayList<NaluItem>> {

        WeakReference<MainActivity> sr;

        public LoadDataTask(MainActivity stateReceiver) {
            sr = new WeakReference<MainActivity>(stateReceiver);
        }

        @Override
        protected void onPreExecute() {
            MainActivity act = sr.get();
            if (act == null) {
                return;
            }
            Log.d("MainActivity", "===zhongjihao=====LoadDataTask===onPreExecute===");

            if (act.adapter != null && act.mTotalCount == -1) {
                if( act.naltype_lv.getFooterViewsCount() == 0){
                    // 添加底部加载布局
                    act.naltype_lv.addFooterView(act.view_more);
                }
            }
        }

        @Override
        protected ArrayList<NaluItem> doInBackground(Void... params) {
            MainActivity act = sr.get();
            if (act == null) {
                return null;
            }

            // 查询一共有多少数据
            if (act.mTotalCount == -1) {
                act.mTotalCount = H264ParseWrap.newInstance().getTotalNalItem(act.file_edit.getText().toString().trim());
            }

            // 分批加载
            return act.loadMore(act.mMaxCount);
        }

        @Override
        protected void onPostExecute(ArrayList<NaluItem> result) {
            MainActivity act = sr.get();
            if (act == null) {
                return;
            }
            Log.d("MainActivity", "===zhongjihao===LoadDataTask=====onPostExecute===");

            if (act.adapter == null) {
                if(act.mTotalCount > act.mMaxCount){
                    // 添加底部加载布局
                    act.naltype_lv.addFooterView(act.view_more);
                    //设置滑动监听
                    act.naltype_lv.setOnScrollListener(act);
                }
                act.adapter = new MyAdapter(act.mContext, act.mDataList, R.layout.nal_item_view);
                act.naltype_lv.setAdapter(act.adapter);
            } else {
                if(act.mTotalCount <= act.mMaxCount){
                    // 移除底部的加载布局
                    act.naltype_lv.removeFooterView(act.view_more);
                }
                act.adapter.notifyDataSetChanged();
            }
        }
    }

    private ArrayList<NaluItem> loadMore(int loadNalItemCount) {
        ArrayList<NaluItem> result = H264ParseWrap.newInstance().h264_nal_parse(loadNalItemCount, file_edit.getText().toString().trim());
        if (result != null && result.size() > 0)
            mDataList.addAll(result);
        Log.d("MainActivity", "===zhongjihao======dataList size: " + mDataList.size());
        return mDataList;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (ActivityCompat.checkSelfPermission(this, android.Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
            if(requestCode == TARGET_READ_PERMISSION_REQUEST){
                openfile_btn.setEnabled(true);
            }
        }else{
            openfile_btn.setEnabled(false);
            Toast.makeText(this, getText(R.string.file_read_permission_tips), Toast.LENGTH_SHORT)
                    .show();
        }
    }
}
