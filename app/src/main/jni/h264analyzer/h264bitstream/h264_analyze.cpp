/*************************************************************************
    > File Name: h264_analyze.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2018年04月20日 星期五 10时10分10秒
 ************************************************************************/

#define LOG_TAG "H264-Analyze"

#include "h264_stream.h"
#include "h264_analyze.h"
#include "../../log.h"
#include "NalParse.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

H264Analyze::H264Analyze()
{
    LOGD("%s: ====zhongjihao=========",__FUNCTION__);
	nal_infovector.clear();
    memset(pH264bits,0,sizeof(pH264bits));
}

H264Analyze::~H264Analyze()
{
    LOGD("%s: ====zhongjihao=========",__FUNCTION__);
    for(vector<NALInfo*>::iterator it  = nal_infovector.begin(); it != nal_infovector.end();)
    {
        //释放空间
        delete *it;
        //从容器中删除
        it = nal_infovector.erase(it);
    }
}

void H264Analyze::setH264BitStreamFilePath(const char* filePath)
{
    memset(pH264bits,0,sizeof(pH264bits));
    memcpy(pH264bits,filePath,strlen(filePath));
    for(vector<NALInfo*>::iterator it  = nal_infovector.begin(); it != nal_infovector.end();)
    {
        //释放空间
        delete *it;
        //从容器中删除
        it = nal_infovector.erase(it);
    }
}

//data_lenth 是包含起始码前缀的NALU长度
//data_offset nal在文件中的偏移量
int H264Analyze::appendNaLInfo(int data_lenth,int data_offset)
{
	//这个vector记录了nalu的位置信息，使用它我们可以获取到nalu的详细信息
	//我们要存储包含起始码的长度，起始码原本不是NALU的一部分
	NALInfo* nalinfo = new NALInfo;
	nalinfo->data_lenth = data_lenth;
	nalinfo->data_offset = data_offset;
	nal_infovector.push_back(nalinfo);
	LOGD("%s: ====zhongjihao=========data_lenth: %d, data_offset: %d",__FUNCTION__,data_lenth,data_offset);
	return 0;
}

//存放解析出来的字符串
char tempstr[1000]={0};
char outputstr[100000]={'\0'};

const char* H264Analyze::probe_nal_unit(int index)
{
	memset(outputstr,'\0',sizeof(outputstr));
    memset(tempstr,'\0',sizeof(tempstr));
	int data_lenth = nal_infovector[index]->data_lenth;
	int data_offset = nal_infovector[index]->data_offset;
	LOGD("%s: ====zhongjihao====index: %d,  data_lenth: %d, data_offset: %d",__FUNCTION__,index,data_lenth,data_offset);
	//用于存放NALU，包含起始码
	uint8_t *nal_temp = (uint8_t *)malloc(data_lenth);
	//从文件读取
	FILE *fp = fopen(pH264bits,"r");
	fseek(fp,data_offset,SEEK_SET);
	fread(nal_temp,data_lenth,1,fp);
	// read some H264 data into buf
	int nal_start,nal_end;
	h264_stream_t* h = h264_new();
	find_nal_unit(nal_temp, data_lenth, &nal_start, &nal_end);
	read_nal_unit(h, &nal_temp[nal_start], nal_end - nal_start);
	debug_nal(h,h->nal);
	free(nal_temp);
	fclose(fp);
	h264_free(h);

	return outputstr;
}
