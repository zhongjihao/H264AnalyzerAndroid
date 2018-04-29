/*************************************************************************
    > File Name: NalParse.h
    > Author: zhongjihao
    > Mail: zhongjihao100@163.com 
    > Created Time: 2018年04月19日 星期四 14时46分38秒
 ************************************************************************/

#ifndef NAL_PARSE_H
#define NAL_PARSE_H

#include <stdio.h>
#include <vector>
#include "h264_analyze.h"

typedef enum{
	NALU_TYPE_SLICE    = 1,      //不分区、非IDR的片
	NALU_TYPE_DPA      = 2,      //片分区A
	NALU_TYPE_DPB      = 3,      //片分区B
	NALU_TYPE_DPC      = 4,      //片分区C
	NALU_TYPE_IDR      = 5,      //IDR图像中的片
	NALU_TYPE_SEI      = 6,      //补充增强信息单元(SEI)
	NALU_TYPE_SPS      = 7,      //序列参数集 (SPS)
	NALU_TYPE_PPS      = 8,      //图像参数集 (PPS)
	NALU_TYPE_AUD      = 9,      //分界符
	NALU_TYPE_EOSEQ    = 10,     //序列结束
	NALU_TYPE_EOSTREAM = 11,     //碼流结束
	NALU_TYPE_FILL     = 12      //填充
}NaluType;

typedef enum{
	NALU_PRIORITY_DISPOSABLE = 0,
	NALU_PRIRITY_LOW         = 1,
	NALU_PRIORITY_HIGH       = 2,
	NALU_PRIORITY_HIGHEST    = 3
}NaluPriority;

typedef struct {
	int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned int len;             //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned int max_size;        //! Nal Unit Buffer size
	int forbidden_bit;            //! should be always FALSE
	int nal_reference_idc;        //! NALU_PRIORITY_xxxx
	int nal_unit_type;            //! NALU_TYPE_xxxx
	char *buf;                    //! contains the first byte followed by the EBSP
	unsigned short lost_packets;  //! true, if packet loss is detected
	int data_offset;
} NALU_t;

typedef struct {
	int nal_index;
	char nal_idc[10];
	char nal_type[10];
	int nal_len;
} NALU_ITEM;

class NalParse
{
private:
	static int info2;
	static int info3;
	NALU_t* pNalu;
	bool checkBoxNal;
	int showNalCount;
	std::vector<NALU_ITEM*> nalItems;
	H264Analyze* pH264Analyze;
private:
	NalParse(const NalParse&);
	NalParse& operator=(const NalParse&);
	int GetAnnexbNALU(FILE* fp,NALU_t *nalu);
	static int FindStartCode2(unsigned char *Buf);
	static int FindStartCode3(unsigned char *Buf);
public:
	NalParse();
	~NalParse();
	void setCheckBox(bool isCheck,int showNalCount);
	H264Analyze* & getH264Analyze();
	std::vector<NALU_ITEM*>& h264_nal_parse(const char *fileurl);
};

#endif

