// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 HSMS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// HSMS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef HSMS_EXPORTS
#define HSMS_API __declspec(dllexport)
#else
#define HSMS_API __declspec(dllimport)

#if _WIN64
#pragma comment(lib, "hsms.lib")
#else
#ifdef _DEBUG
#pragma comment(lib, "hsmsd.lib")
#else
#pragma comment(lib, "hsms.lib")
#endif
#endif

#endif

//#define  HSMS_API

#pragma once
//#pragma comment(lib, "hsms.lib")

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <map>
#include <deque>
#include <mutex>
#include <iomanip>
typedef unsigned char byte;
typedef unsigned long ulong;
typedef unsigned short ushort;

#ifdef _WIN32

typedef __int64		i8;
typedef __int8			i1;
typedef __int16		i2;
typedef __int32		i4;
typedef unsigned __int64		u8;
typedef unsigned __int8		u1;
typedef unsigned __int16		u2;
typedef unsigned __int32		u4;
typedef double				f8;
typedef float				f4;
typedef bool					sboolean;

typedef unsigned __int16   __uint16_t;
typedef unsigned __int32   __uint32_t;
typedef unsigned __int64		__uint64_t;

#else

typedef __int64_t		i8;
typedef __int8_t			i1;
typedef __int16_t		i2;
typedef __int32_t		i4;
typedef __uint64_t		u8;
typedef __uint8_t		u1;
typedef __uint16_t		u2;
typedef __uint32_t		u4;
typedef double				f8;
typedef float				f4;
typedef bool					sboolean;

#endif // WIN32

using VEC = std::vector<byte>;

enum class SecsFormat
{
	List = 0,					// 000000 00
	Binary = 0x20,		// 001000 00
	Boolean = 0x24,		// 001001 00
	ASCII = 0x40,		// 010000 00
	JIS8 = 0x44,			// 010001 00
	I8 = 0x60,				// 011000 00
	I1 = 0x64,				// 011001 00
	I2 = 0x68,				// 011010 00
	I4 = 0x70,				// 011100 00
	F8 = 0x80,				// 100000 00
	F4 = 0x90,				// 100100 00
	U8 = 0xA0,				// 101000 00
	U1 = 0xA4,				// 101001 00
	U2 = 0xA8,				// 101010 00
	U4 = 0xB0,				// 101100 00
	NONE = 0xFF
};

enum  HSMSMODE
{
	CHSMS_PASSIVE,
	CHSMS_ACTIVE
};

enum  SECSMODE
{
	CSECS_EQUIP = 0,
	CSECS_HOST = 1,
};

enum HSMSMSGTYPE
{
	CHSMS_RESPOND, //reply
	CHSMS_REQUEST, //
	CHSMS_RECV,
};

struct HsmsSet
{
	unsigned short uDevID = 1;
	int nT3 = 45;
	int nT5 = 10;
	int nT6 = 5;
	int nT7 = 10;
	int nT8 = 5;
	HSMSMODE mode = CHSMS_PASSIVE;
	unsigned short uLocalPort;
	unsigned short uRemotePort;
	std::string cLocalIP;
	std::string cRemoteIP;
};

struct SecsSet
{
	unsigned short uDevID;
	int nT1;
	int nT2;
	int nT3;
	int nT4;
	int nRty;
	SECSMODE mode;
	ushort uBaudrate;
	std::string sComName;
	bool bIgnoreDevID;
	int nDelaySend;
};

HSMS_API u2 SF(byte uS, byte uF);

class Item;
using ItemPtr = std::shared_ptr<Item>;
using VecItem = std::vector<ItemPtr>;
class  HSMS_API Item
{
public:
	static ItemPtr L(void);
	static ItemPtr B(byte data);
	static ItemPtr Boolean(sboolean data);
	static ItemPtr U1(u1 data);
	static ItemPtr U2(u2 data);
	static ItemPtr U4(u4 data);
	static ItemPtr U8(u8 data);
	static ItemPtr I1(i1 data);
	static ItemPtr I2(i2 data);
	static ItemPtr I4(i4 data);
	static ItemPtr I8(i8 data);
	static ItemPtr F4(f4 data);
	static ItemPtr F8(f8 data);

	static ItemPtr A(const std::string& str);
	static ItemPtr L(const std::vector<ItemPtr>& vData);
	static ItemPtr B(const VEC& Bin);
	static ItemPtr Boolean(const std::vector<sboolean>& vData);
	static ItemPtr U1(const std::vector<u1>& vData);
	static ItemPtr U2(const std::vector<u2>& vData);
	static ItemPtr U4(const std::vector<u4>& vData);
	static ItemPtr U8(const std::vector<u8>& vData);
	static ItemPtr I1(const std::vector<i1>& vData);
	static ItemPtr I2(const std::vector<i2>& vData);
	static ItemPtr I4(const std::vector<i4>& vData);
	static ItemPtr I8(const std::vector<i8>& vData);
	static ItemPtr F4(const std::vector<f4>& vData);
	static ItemPtr F8(const std::vector<f8>& vData);

	static ItemPtr fromStream(const VEC& vData, int nIndex);

	virtual void Append(const ItemPtr& it) = 0;

	virtual SecsFormat getFormat() const = 0;
	virtual size_t getCount() const = 0;
	virtual size_t getItemCount() const = 0;
	virtual ItemPtr getItem(int nIndex) const = 0;
	virtual std::vector<ItemPtr> GetList() const = 0;
	virtual VEC& getValueBytes() = 0;

	static std::string GetSML(const ItemPtr& pItem);

	virtual std::string getString() const = 0;
	virtual VEC getStream() = 0;

	template<typename T>
	T getValue() const;

	template<typename T>
	std::vector<T> getValueList() const;
};

using vecU4 = std::vector<u4>;

struct HSMS_API SecsMessage
{
	u8 Addr;
	ulong MID;
	byte S;
	byte F;
	ItemPtr item;
	HSMSMSGTYPE type;
	SYSTEMTIME timeStamp;

	SecsMessage();
	SecsMessage(u8 uAddr, ulong uMID, byte uS, byte uF, const ItemPtr& data, HSMSMSGTYPE tp);
	SecsMessage(byte uS, byte uF, const ItemPtr& data);
	SecsMessage(byte uS, byte uF, const ItemPtr& data, ulong uMID);
	VEC getStream(void) const;
	void fromStream(const VEC& vData);
	SecsMessage(const VEC& vData);
};

using funSecsMessageRecv = std::function<int(const SecsMessage& msg)>;
using funConnChage = std::function<void(int)>;
using funTimeOut = std::function<void(int, int)>;

class HSMS_API CGem
{
public:
	CGem();
	virtual ~CGem();

	enum class LinkMode
	{
		SECS_I,
		HSMS_SS,
		HSMS_MS,
	};

	static std::shared_ptr<CGem> GetInstancePtr(LinkMode mode);

public:
	virtual int InitLink(const HsmsSet& set) = 0;
	virtual int InitLink(const SecsSet& set) = 0;

	virtual int Connect() = 0;
	virtual int Disconnect() = 0;

	virtual unsigned long Send(const SecsMessage& msg) = 0;
	virtual int Reply(const SecsMessage& msg) = 0;

	virtual int SetSecsRecvFun(funSecsMessageRecv fun) = 0;
	virtual int SetConnChange(funConnChage) = 0;
	virtual int GetChangeState() = 0;
	virtual int SetTimeOut(funTimeOut) = 0;

	virtual void SetSmlLog(const std::string& path, int nDay, int nLogType = 0) = 0;
};

using GemPtr = std::shared_ptr<CGem>;
