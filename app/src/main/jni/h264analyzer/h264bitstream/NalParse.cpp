/*************************************************************************
    > File Name: NalParse.cpp
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2018年04月19日 星期四 15时46分38秒
 ************************************************************************/

#define LOG_TAG "NalParse"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "NalParse.h"
#include "../../log.h"


int NalParse::info2 = 0;
int NalParse::info3 = 0;

NalParse::NalParse() : checkBoxNal(false),showNalCount(0)
{
    LOGD("%s: ====zhongjihao=========",__FUNCTION__);
    nalItems.clear();
    pH264Analyze = new H264Analyze;

    int buffersize = 800000;
    pNalu = (NALU_t*)calloc(1, sizeof(NALU_t));
    pNalu->max_size = buffersize;
    pNalu->buf = (char*)calloc(buffersize, sizeof(char));
}

NalParse::~NalParse()
{
    LOGD("%s: ====zhongjihao=========",__FUNCTION__);
	if(pNalu)
	{
		if(pNalu->buf){
			free(pNalu->buf);
			pNalu->buf = NULL;
		}
		free(pNalu);
	}

    for(vector<NALU_ITEM*>::iterator it  = nalItems.begin(); it != nalItems.end();)
    {
        //释放空间
        delete *it;
        //从容器中删除
        it = nalItems.erase(it);
    }
    if(pH264Analyze)
        delete pH264Analyze;
    pH264Analyze = NULL;
}

int NalParse::FindStartCode2(unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) 
		return 0;
	else 
		return 1; //判断是否为0x000001，如果是返回1
}

int NalParse::FindStartCode3(unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1)
		return 0;
	else
		return 1; //判断是否为0x00000001，如果是返回1
}

void NalParse::setCheckBox(bool isCheck,int showNalCount)
{
    this->showNalCount = showNalCount;
    this->checkBoxNal = isCheck;
}

//这个函数输入为一个NALU的结构体，主要功能为得到一个完整的NALU并保存在NALU_t的buf中，获取它的长度，填充F、IDC、TYPE位
//并且返回两个相邻的起始码之间的字节数，即包含有前缀的NALU的长度
int NalParse::GetAnnexbNALU(FILE* fp,NALU_t *nalu)
{
	int pos = 0;
	int StartCodeFound, rewind;
	unsigned char *Buf;
	if((Buf = (unsigned char*)calloc(nalu->max_size , sizeof(char))) == NULL){
		LOGE("%s: ====zhongjihao====Could not allocate Buf memory========",__FUNCTION__);
		return 0;
	}

	nalu->startcodeprefix_len = 3;//初始化碼流序列的开始字符为3字节
	
	if(3 != fread(Buf, 1, 3, fp))//从碼流中读取3字节
	{
		free(Buf);
		return 0;
	}

	info2 = FindStartCode2(Buf);//判断是否为0x000001
	if(info2 != 1)
	{
		//如果不是再读取一个字节
		if(1 != fread(Buf+3, 1, 1, fp))//读取一个字节
		{
			free(Buf);
			return 0;
		}
		
		info3 = FindStartCode3(Buf);//判断是否为0x00000001
		if(info3 != 1)//如果不是返回-1
		{ 
			free(Buf);
			return -1;
		}
		else
		{
			//如果是0x00000001,得到开始前缀为4字节
			pos = 4;
			nalu->startcodeprefix_len = 4;
		}
	}
	else
	{
		//如果是0x000001,得到开始前缀为3字节
		nalu->startcodeprefix_len = 3;
		pos = 3;
	}

	//查找下一个开始字符的标志位
	StartCodeFound = 0;
	info2 = 0;
	info3 = 0;
	
	while(!StartCodeFound)
	{
		if(feof(fp))//判断是否到了文件尾
		{
			nalu->len = (pos-1)-nalu->startcodeprefix_len;
			memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
			nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
			nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
			nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
			free(Buf);
			return pos-1;
		}
		Buf[pos++] = fgetc(fp);//读取一个字节到Buf中
		info3 = FindStartCode3(&Buf[pos-4]);//判断是否为0x00000001
		if(info3 != 1)
			info2 = FindStartCode2(&Buf[pos-3]);//判断是否为0x000001
		StartCodeFound = (info2 == 1 || info3 == 1);
	}

	// Here, we have found another start code (and read length of startcode bytes more than we should
	// have.  Hence, go back in the file
	rewind = (info3 == 1)? -4 : -3;

	if(0 != fseek(fp, rewind, SEEK_CUR))//把文件指针指向前一个NALU的末尾
	{
		free(Buf);
		LOGE("%s: =====zhongjihao====Cannot fseek in the bit stream file===",__FUNCTION__);
	}

	// Here the Start code, the complete NALU, and the next start code is in the Buf.
	// The size of Buf is pos, pos+rewind are the number of bytes excluding the next
	// start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code
	
	nalu->len = (pos+rewind)-nalu->startcodeprefix_len; //NALU的大小，包括一个字节NALU头和EBSP数据
	memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//拷贝一个完整的NALU，不拷贝起始码
	nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
	nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
	nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
	free(Buf);

	return (pos+rewind);//返回两个起始码之间的字节数，即包含了起始码的Nalu的长度
}

H264Analyze* & NalParse::getH264Analyze()
{
    return pH264Analyze;
}

std::vector<NALU_ITEM*>& NalParse::h264_nal_parse(const char *fileurl)
{
    FILE* fp = fopen(fileurl, "r");
	if(fp == NULL){
		LOGE("%s: ===zhongjihao====Error open file==",__FUNCTION__);
		return nalItems;
	}
	LOGD("%s: =====zhongjihao====open file OK===",__FUNCTION__);
    for(vector<NALU_ITEM*>::iterator it  = nalItems.begin(); it != nalItems.end();)
    {
        //释放空间
        delete *it;
        //从容器中删除
        it = nalItems.erase(it);
    }
    memset(pNalu->buf,0, pNalu->max_size);
    pH264Analyze->setH264BitStreamFilePath(fileurl);
	int data_offset = 0;
	int nal_num = 0;
	LOGD("-----+-------- NALU Table ------+--------");
	LOGD(" NUM |    POS   | IDC | TYPE |   LEN    |");
	LOGD("-----+----------+-----+------+-----------");

//	printf("-----+-------- NALU Table ------+--------\n");
//	printf(" NUM |    POS   | IDC | TYPE |   LEN    |\n");
//	printf("-----+----------+-----+------+-----------\n");
	
	while(!feof(fp))
	{
		int data_lenth;
		data_lenth = GetAnnexbNALU(fp,pNalu);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码
        pNalu->data_offset = data_offset;
		//输出NALU的长度和类型
		int nal_reference_idc = pNalu->nal_reference_idc >> 5;
		LOGD("%5d%9d   %2d %6d   %5d\n",nal_num,pNalu->data_offset,nal_reference_idc,pNalu->nal_unit_type,pNalu->len);

        char type_str[10] = {0};
        switch(pNalu->nal_unit_type)
        {
            case NALU_TYPE_SLICE:
                sprintf(type_str,"SLICE");
                break;
            case NALU_TYPE_DPA:
                sprintf(type_str,"DPA");
                break;
            case NALU_TYPE_DPB:
                sprintf(type_str,"DPB");
                break;
            case NALU_TYPE_DPC:
                sprintf(type_str,"DPC");
                break;
            case NALU_TYPE_IDR:
                sprintf(type_str,"IDR_SLICE");
                break;
            case NALU_TYPE_SEI:
                sprintf(type_str,"SEI");
                break;
            case NALU_TYPE_SPS:
                sprintf(type_str,"SPS");
                break;
            case NALU_TYPE_PPS:
                sprintf(type_str,"PPS");
                break;
            case NALU_TYPE_AUD:
                sprintf(type_str,"AUD");
                break;
            case NALU_TYPE_EOSEQ:
                sprintf(type_str,"EOSEQ");
                break;
            case NALU_TYPE_EOSTREAM:
                sprintf(type_str,"EOSTREAM");
                break;
            case NALU_TYPE_FILL:
                sprintf(type_str,"FILL");
                break;
            default:
                sprintf(type_str,"Other");
                break;
        }

        char idc_str[10] = {0};
        switch(nal_reference_idc)
        {
            case NALU_PRIORITY_DISPOSABLE:
                sprintf(idc_str,"DISPOS");
                break;
            case NALU_PRIRITY_LOW:
                sprintf(idc_str,"LOW");
                break;
            case NALU_PRIORITY_HIGH:
                sprintf(idc_str,"HIGH");
                break;
            case NALU_PRIORITY_HIGHEST:
                sprintf(idc_str,"HIGHEST");
                break;
            default:
                sprintf(idc_str,"Other");
                break;
        }
        NALU_ITEM* nalItem = new NALU_ITEM;
        memset(nalItem,0,sizeof(NALU_ITEM));
        nalItem->nal_index = nal_num;
        nalItem->nal_len = pNalu->len;
        memcpy(nalItem->nal_idc,idc_str,strlen(idc_str));
        memcpy(nalItem->nal_type,type_str,strlen(type_str));
        nalItems.push_back(nalItem);

        pH264Analyze->appendNaLInfo(pNalu->len+pNalu->startcodeprefix_len,pNalu->data_offset);
	//	printf("%5d%9d   %2d %6d   %5d\n",nal_num,pNalu->data_offset,nal_reference_idc,pNalu->nal_unit_type,pNalu->len);
		//判断是否选中了"只输出300条"，如果选择了就不在分析了
		if(checkBoxNal && nal_num>=showNalCount-1){
			break;
		}
		nal_num++;
		data_offset = data_offset + data_lenth;
	}
    if(fp){
        fclose(fp);
    }
	return nalItems;
}

