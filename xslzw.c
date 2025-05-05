/////////////////////////////////////////////////////////////////////////////
//文件：lzw.cpp
//功能：lzw compress algrithm
//相关：lzw.h
/////////////////////////////////////////////////////////////////////////////

#include "xslzw.h"

xsushort LZW_m_InputCode;		// InputBits,Expand
xsbyte LZW_m_Error;			// InputBits,Expand							
xsbyte LZW_m_BitsRest;		// OutputBits,InputBits
xsbyte LZW_m_BitsShift;		// OutputBits,InputBits							
xsbyte LZW_m_Rack;			// 主要OutputBits,InputBits,还有Clear,Compress							
xsushort LZW_m_LzwOffset;		// 主要OutputBits,InputBits,还有Compress,Expand
xsushort LZW_m_NextCode;		// Compress,Expand
xsushort LZW_m_CurrentCodeBits;	// OutputBits,InputBits,Compress,Expand
xsushort LZW_m_NextBumpCode;	// Compress
xsbyte * LZW_m_pBitStream;	// OutputBits,InputBits,Compress,Expand
xsbyte LZW_m_DecodeStack[LZW_TABLE_SIZE];	// DecodeString和Expand
DICTIONARY	LZW_m_Dictionary[LZW_TABLE_SIZE+1];	// 基本都用到了
xsbyte xsLzwErrorCode;

void xsLzwClearDictionary()
{
	xsushort i;
	for(i=0;i<LZW_TABLE_SIZE;i++)
	{
		LZW_m_Dictionary[i].m_CurrentCode = LZW_MYUNUSED;	// 0xFFFF
	}
	//memset(LZW_m_Dictionary,0xFF,sizeof(DICTIONARY)*(LZW_TABLE_SIZE+1));

	LZW_m_NextCode		  = LZW_FIRST_CODE;				// 0x0101
	LZW_m_CurrentCodeBits = 9;							// 9bit~15bit
	LZW_m_NextBumpCode	  = 511;						// 0x01FF
}

void xsLzwClear()
{
	LZW_m_Rack = 0;
	// 初始化字典
	xsLzwClearDictionary();
}

int xsGetLastError()
{
	return xsLzwErrorCode;
}

// 向压缩流中写
void xsLzwOutputBits(xsushort Code)
{
	LZW_m_BitsShift = LZW_m_CurrentCodeBits - 8 + LZW_m_BitsRest;	// 右移位数：1~8~14

	*(LZW_m_pBitStream + LZW_m_LzwOffset) = LZW_m_Rack|(xsbyte)(Code>>LZW_m_BitsShift);
	LZW_m_LzwOffset ++;

	if(LZW_m_BitsShift > 8)
	{
		LZW_m_BitsShift -= 8;
		*(LZW_m_pBitStream + LZW_m_LzwOffset) = Code >> LZW_m_BitsShift;
		LZW_m_LzwOffset ++;

		// LZW_m_BitsShift 必然小于8了
	}

	LZW_m_BitsRest = LZW_m_BitsShift;

	LZW_m_BitsShift = 8 - LZW_m_BitsShift;
	LZW_m_Rack = Code << LZW_m_BitsShift;
}

// 从压缩流中取
xsushort xsLzwInputBits()
{
	// 处理上次的剩余
	if(LZW_m_BitsRest != 0)
	{
		LZW_m_BitsShift = 8 - LZW_m_BitsRest;	// 左移位数：
		LZW_m_Rack <<= LZW_m_BitsShift;			// 挤掉左边的东西
		LZW_m_InputCode = LZW_m_Rack >> LZW_m_BitsShift;
		LZW_m_BitsShift = LZW_m_CurrentCodeBits - LZW_m_BitsRest;	// 右移位数：2~14
	}
	else
	{
		LZW_m_InputCode = *(LZW_m_pBitStream + LZW_m_LzwOffset);
		if(LZW_m_InputCode == LZW_EOF){LZW_m_Error = 1;return 0;}
		LZW_m_LzwOffset ++;
		LZW_m_BitsShift = LZW_m_CurrentCodeBits - 8;
	}

	// 如果有希望
	if(LZW_m_BitsShift > 8)
	{		
		LZW_m_InputCode <<= 8;

		LZW_m_Rack = *(LZW_m_pBitStream + LZW_m_LzwOffset);
		if(LZW_m_Rack == LZW_EOF){LZW_m_Error = 1;return 0;}
		LZW_m_LzwOffset ++;

		LZW_m_InputCode |= LZW_m_Rack;

		LZW_m_BitsShift -= 8;			// 不可能再大于8
	}

	LZW_m_InputCode <<= LZW_m_BitsShift;	

	LZW_m_Rack = *(LZW_m_pBitStream + LZW_m_LzwOffset);
	if(LZW_m_Rack == LZW_EOF){LZW_m_Error = 1;return 0;}
	LZW_m_LzwOffset ++;

	LZW_m_BitsRest = 8 - LZW_m_BitsShift;
	
	LZW_m_InputCode |= LZW_m_Rack >> LZW_m_BitsRest;
	
	return LZW_m_InputCode;
}

xsushort xsLzwDecodeString(xsushort Count, xsushort Code)
{
	if(Code >= LZW_MAX_CODE)
	{
		return LZW_NOT_POSSIBLE_NODE;
	}
	// 用不着建立HUSH表了，顺藤摸瓜
	while(Code > 255)
	{
		LZW_m_DecodeStack[Count] = LZW_m_Dictionary[Code].m_ByteValue;
		Count ++;

		if(Count >= LZW_TABLE_SIZE)
			return LZW_NOT_POSSIBLE_NODE;

		Code = LZW_m_Dictionary[Code].m_PrecedingCode;
		if(Code > LZW_MAX_CODE)
			return LZW_NOT_POSSIBLE_NODE;
	}
	
	LZW_m_DecodeStack[Count] = (xsbyte)Code;
	Count ++;
	return Count;
}

xsushort xsLzwFindChildNode(xsushort PreCode, xsushort CurCode)
{
	register xsushort Index;	
	register xsushort JumpOff;	

	// Hush算法主体
	Index = (CurCode << (LZW_BITS - 8)) ^ PreCode;
	
	// Hush算法跳跃间隔
	if(Index == 0)	
		JumpOff = 1;
	else					
		JumpOff = LZW_TABLE_SIZE - Index;

	for(;;)
	{
		if(Index >= LZW_TABLE_SIZE)
			return LZW_NOT_POSSIBLE_NODE;

		if(LZW_m_Dictionary[Index].m_CurrentCode == LZW_MYUNUSED)
			return Index;

		if((LZW_m_Dictionary[Index].m_PrecedingCode == PreCode)
			&&(LZW_m_Dictionary[Index].m_ByteValue == (xsbyte)CurCode))
			return Index;

		// Hush算法重叠时进行跳跃
		if(Index >= JumpOff)
			Index -= JumpOff;
		else						
			Index += (LZW_TABLE_SIZE - JumpOff);
	}
}


xsbyte xs_compress(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort *OutputSize)
{
	xsushort CurCode, PreCode;
	xsushort Index;
	xsushort BytesDealed = 0;
	// 初始化
	LZW_m_BitsRest = 0;	// 0~7
	// 标明压缩数据流
	LZW_m_pBitStream = pDestiny;

	// 原数据量过小，不压缩
	if(SourceSize < LZW_MIN_SIZE)
		return LZW_NOT_NEED_COMPRESS;
	// 压缩标记
	memmove((void*)pDestiny, (const void *)"JZIP2", 5);
	LZW_m_LzwOffset = 5;
	// 环境清理
	xsLzwClear();
	// 读一个字节
	PreCode = *(pSource+BytesDealed);
	BytesDealed ++;
	// 作用于每个原数据xsbyte
	while(BytesDealed < SourceSize)
	{
		// 压缩算法失效
		if(LZW_m_LzwOffset > SourceSize - 5)
			return LZW_NOT_NEED_COMPRESS;
		// 压缩算法输出将超过缓冲区容量
		if(LZW_m_LzwOffset > DestinySize - 5)
			return LZW_OUT_OF_BUFFER;
		// 读一个字节
		CurCode = *(pSource+BytesDealed);
		BytesDealed ++;
		// 按照HUSH算法寻找对应字典条目。正确结果：UNUSED或找到
		Index = xsLzwFindChildNode(PreCode, CurCode);
		// 寻找时出错
		if(Index == LZW_NOT_POSSIBLE_NODE)
			return LZW_COMPRESS_ERROR;
		if(LZW_m_Dictionary[Index].m_CurrentCode != LZW_MYUNUSED)
		{
			// 找到了的话，就进行编码；
			PreCode = LZW_m_Dictionary[Index].m_CurrentCode;
		}
		else
		{
			// 否则在字典中该位置填入该词条
			LZW_m_Dictionary[Index].m_CurrentCode	= LZW_m_NextCode;
			LZW_m_Dictionary[Index].m_PrecedingCode	= PreCode;
			LZW_m_Dictionary[Index].m_ByteValue	= (xsbyte)CurCode;
			LZW_m_NextCode ++;

			// 然后输出此字节
			xsLzwOutputBits(PreCode);
			PreCode = CurCode;

			// 调整字典
			if(LZW_m_NextCode > LZW_MAX_CODE)			// 重建字典	// 0x7FFFF
			{
				xsLzwOutputBits(LZW_FLUSH_CODE);			// 258
				xsLzwClearDictionary();
			}
			else if(LZW_m_NextCode > LZW_m_NextBumpCode)	// 码元升位
			{
				xsLzwOutputBits(LZW_BUMP_CODE);			// 257
				LZW_m_CurrentCodeBits ++;
				LZW_m_NextBumpCode <<= 1;
				LZW_m_NextBumpCode |= 1;
			}
		}
	}
	// 输出最后字节，并输出结束标志
	xsLzwOutputBits(PreCode);
	xsLzwOutputBits(LZW_END_OF_STREAM);					// 256

	// 如果还有些bit没写完，就一股脑儿写出
	if(LZW_m_BitsRest != 0)
	{
		*(pDestiny+LZW_m_LzwOffset) = LZW_m_Rack;	// ??
		LZW_m_LzwOffset ++;
	}


	// 正常返回
	*OutputSize = LZW_m_LzwOffset;
	return LZW_ACTION_OK;
}

xsbyte xs_decompress(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort *OutputSize)
{
	xsushort CurCode, PreCode;
	xsushort Count;
	xsbyte CurByte;
	// 初始化
	LZW_m_BitsRest = 0;	// 0~7
	LZW_m_Error = 0;

	// 标明压缩数据流
	LZW_m_pBitStream = pSource;


	// 检查是否有压缩标记并初始化
	if(memcmp(pSource,"JZIP2",5) != 0)
		return LZW_NOT_COMPRESS_BUFFER;
	LZW_m_LzwOffset = 5;
	*OutputSize = 0;

	// 环境清理
	xsLzwClear();
	
	// 作用于每个原数据xsbyte
	while(LZW_m_LzwOffset < SourceSize)
	{
		// 准备重建字典
		xsLzwClearDictionary();

		// 从压缩流中读取一个字（不止一个字节(>=9bit)）
		PreCode = xsLzwInputBits();

		// 判断是否读取合法
		if(LZW_m_Error == 1)
			return LZW_NOT_COMPRESS_BUFFER;
		
		// 判断是否读取完毕；完全不必，因为压缩流必须足够大
		if(PreCode == LZW_END_OF_STREAM)
			return LZW_ACTION_OK;

		// 第一个字节总是这样的
		CurByte = (xsbyte)PreCode;

		// 输出第一个字节。
		*(pDestiny + *OutputSize) = CurByte;
		OutputSize ++;

		// 作用于每个原数据xsbyte
		while(LZW_m_LzwOffset < SourceSize)
		{
			// 如果输出缓冲区不够了
			if(*OutputSize > DestinySize - 5)
				return LZW_OUT_OF_BUFFER;
			
			// 从压缩流中读取一个字（不止一个字节(>=9bit)）
			CurCode = xsLzwInputBits();
			
			// 判断是否读取合法
			if(LZW_m_Error == 1)
				return LZW_NOT_COMPRESS_BUFFER;

			// 根据读取内容判断：是否读取完毕；
			if(CurCode == LZW_END_OF_STREAM)	//正常出口
			{
				return LZW_ACTION_OK;
			}
			else if(CurCode == LZW_FLUSH_CODE)	//字典重建 
			{
				break;
			}
			else if(CurCode == LZW_BUMP_CODE)	//码元升位
			{
				LZW_m_CurrentCodeBits++;
				continue;
			}
			
			// 判断字典中是否有此码元
			if(CurCode >= LZW_m_NextCode)
			{
				// 尚未
				LZW_m_DecodeStack[0] = CurByte;
				Count = xsLzwDecodeString(1, PreCode);
			}
			else
			{
				// 查出
				Count = xsLzwDecodeString(0, CurCode);
			}

			// 检查是否查询字典出错
			if(Count == LZW_NOT_POSSIBLE_NODE)
				return LZW_EXPAND_ERROR;

			// 
			CurByte = LZW_m_DecodeStack[Count - 1];	//??
			
			// 将查到的东西逐个字节输出
			while(Count > 0)
			{
				Count --;
				*(pDestiny + *OutputSize) = LZW_m_DecodeStack[Count];
				OutputSize ++;
			}
			
			// 总是字典中增加条目
			LZW_m_Dictionary[LZW_m_NextCode].m_PrecedingCode = PreCode;
			LZW_m_Dictionary[LZW_m_NextCode].m_ByteValue = CurByte;		
			LZW_m_NextCode ++;
			
			// 轮到下一个了
			PreCode = CurCode;
		}
	}

	// 正常返回
	return LZW_ACTION_OK;
}


xsbyte xs_lzwSafeCompress(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort *OutputSize)
{
	xsbyte Return = xs_compress(pSource,SourceSize,pDestiny,DestinySize,OutputSize);
    xsLzwErrorCode=Return;
	if(Return == LZW_ACTION_OK)
	{
		return 1;
	}
	else if(Return == LZW_NOT_NEED_COMPRESS)
	{
		if(DestinySize < SourceSize)
		{
			return 0;
		}
		else
		{
			memcpy(pDestiny,pSource,SourceSize);
			*OutputSize = SourceSize;
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

xsbyte xs_lzwSafeExpand(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort* OutputSize)
{
	xsbyte Return = xs_decompress(pSource,SourceSize,pDestiny,DestinySize,OutputSize);
    xsLzwErrorCode=Return;
	if(Return == LZW_ACTION_OK)
	{
		return 1;
	}
	else if(Return == LZW_NOT_COMPRESS_BUFFER)
	{
		if(DestinySize < SourceSize)
		{
			return 0;
		}
		else
		{
			memcpy(pDestiny,pSource,SourceSize);
			*OutputSize = SourceSize;
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

//======END of lzw.cpp File======
