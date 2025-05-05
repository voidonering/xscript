#ifndef XS_LZWH
#define XS_LZWH

#include "xsdef.h"


#ifdef __cplusplus
extern "C" {
#endif


// 压缩参量
#define LZW_MIN_SIZE			50						// 可压缩源尺寸下限

#define LZW_BITS				15						// 压缩代码最大长度
#define LZW_MAX_CODE			((1 << LZW_BITS) - 1)	// 0x7FFF
#define LZW_TABLE_SIZE			35023L					// 字典空间尺寸
#define LZW_NOT_POSSIBLE_NODE	(LZW_TABLE_SIZE + 1)	// 最小无效字典条目序号

#define LZW_END_OF_STREAM		256						// LZW压缩流结束标记
#define LZW_BUMP_CODE			257						// LZW压缩流码元升位标记
#define LZW_FLUSH_CODE			258						// LZW压缩流重建字典标记
#define LZW_FIRST_CODE			259						// 字典条目序号起始值
#define LZW_MYUNUSED			0xFFFF					// 字典条目无效序号
#define LZW_EOF					0xFFFF					// 流结束???????

// 字典参数结构
typedef struct
{
	xsushort	m_CurrentCode;								// 当前码值
	xsushort	m_PrecedingCode;							// 前续码值
	xsbyte	m_ByteValue;								// 原字节值
	xsbyte	m_Pad[3];									// 提高速度?
}DICTIONARY;

/////////////////////////////////////////////////////////////////////
// 工作类
void xsLzwClear();
void xsLzwClearDictionary();

void xsLzwOutputBits(xsushort Code);
xsushort xsLzwInputBits();
xsushort xsLzwDecodeString(xsushort Count, xsushort Code);
xsushort xsLzwFindChildNode(xsushort PreCode, xsushort CurCode);
extern xsbyte xsLzwErrorCode;

// 下列函数可直接调用
xsbyte xs_compress(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort * OutputSize);
				// LZW压缩，返回执行情况代码，将LZW_NOT_NEED_COMPRESS视为错误
xsbyte xs_decompress(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort* OutputSize);
				// LZW解压，返回执行情况代码，将LZW_NOT_COMPRESS_BUFFER视为错误
xsbyte xs_lzwSafeCompress(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort* OutputSize);
				// LZW压缩，返回是否操作失败，不将LZW_NOT_NEED_COMPRESS视为错误
xsbyte xs_lzwSafeExpand(xsbyte * pSource, xsushort SourceSize,xsbyte * pDestiny,xsushort DestinySize, xsushort* OutputSize);
				// LZW解压，返回是否操作失败，不将LZW_NOT_NEED_COMPRESS视为错误


int xsGetLastError();

//////////////////////////////////////////////////////////////////////////
// 以上为本类内部细节
//////////////////////////////////////////////////////////////////////////

// 压缩/解压操作返回值
#define LZW_ACTION_OK			0						// 操作正常
#define LZW_OUT_OF_BUFFER       201						// 输出缓冲区尺寸不够
#define LZW_NOT_NEED_COMPRESS   202						// 输入数据流不必压缩
#define LZW_NOT_COMPRESS_BUFFER 203						// 输入数据流未压缩
#define LZW_COMPRESS_ERROR		205						// 压缩出现错误
#define LZW_EXPAND_ERROR		206						// 解压出现错误

//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
#endif
//======END of lzw.h File======
