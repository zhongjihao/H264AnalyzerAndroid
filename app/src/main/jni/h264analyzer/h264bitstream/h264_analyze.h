/*************************************************************************
    > File Name: h264_analyze.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2018年04月19日 星期四 11时20分10秒
 ************************************************************************/

#ifndef H264_ANALYZE_H
#define H264_ANALYZE_H

#include <vector>
#include <stdio.h>

using std::vector;

//一条Packet记录
typedef struct NALInfo {
    int data_offset;
    int data_lenth;
} NALInfo;


class H264Analyze
{
private:
    vector<NALInfo*> nal_infovector;
    char pH264bits[256];                //!< the bit stream file
public:
	H264Analyze();
	~H264Analyze();
    void setH264BitStreamFilePath(const char* filePath);
	//记录一条nalu在文件中的偏移量
	int appendNaLInfo(int data_lenth,int data_offset);
	//解析一条nalu
	const char* probe_nal_unit(int index);
};

#endif

