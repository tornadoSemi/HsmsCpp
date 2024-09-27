#pragma once

#include <boost/statechart/event.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/deferral.hpp>
#include <boost/statechart/state.hpp>
#include <boost/mpl/list.hpp>

//#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <ctime>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

namespace SECS_E4
{
	// define event of Processing
	class EvENQ : public sc::event< EvENQ > {};   //
	class EvEOT : public sc::event< EvEOT > {};   //
	class EvACK : public sc::event< EvACK > {};   //
	class EvNoACK : public sc::event< EvACK > {};   //
	class EvSendRequest : public sc::event< EvSendRequest > {};   //
	class EvT1 : public sc::event< EvT1 > 
	{
	public:
		EvT1();
	};   //
	class EvT2 : public sc::event< EvT2 >
	{
	public:
		EvT2();
	};   //
	class EvOverRetry : public sc::event< EvOverRetry > {};   //
	class EvRetry : public sc::event< EvRetry > {};   //
	class EvLenErr : public sc::event< EvLenErr > 
	{
	public:
		EvLenErr();
	};   //
	class EvCheckSumErr : public sc::event< EvCheckSumErr > 
	{
	public:
		EvCheckSumErr();
	};   //
	class EvIdle : public sc::event< EvIdle > {};   //
	class EvNext : public sc::event< EvNext > {};   //
}

namespace SECS_E4
{
	// define state of Processing
	class Idle;
	class LineRecv;
	class LineSend;
	class Retry;
	class SendData;
	class BlockSended;
	class GetLen;
	class GetData;
	class CheckSum;
	class BlockReceived;
	class FailedSend;
	class TimeOut;
	class Eat;
	class RecvOK;
}

namespace SECS_E4
{
	enum class ErrorType
	{
		TmoT1,
		TmoT2,
		LenErr,
		CheckSumErr,
		SendFailed,
		NoAck,
		Contention,
	};
}

namespace SECS_E4
{
	class BlockState
		: public sc::state_machine<BlockState, Idle>
	{
	public:
		BlockState(void);
		~BlockState(void);

		bool Slave() const { return m_bSlave; }
		void Slave(bool val) { m_bSlave = val; }

		bool IsCanRetry();
		void ResetRetry();
		void SetRetryCount(int nRetry)
		{
			m_nTotalRetry = nRetry;
		}

	public:
		virtual int OnIdle() = 0;
		virtual int OnLineRecv() = 0;
		virtual int OnGetLen() = 0;
		virtual int OnGetData() = 0;
		virtual int OnTimeOut() = 0;
		virtual int OnEat() = 0;
		virtual int OnCheckSum() = 0;
		virtual int OnBlockRecv() = 0;
		virtual int OnRecvOK() = 0;

		virtual int OnLineSend() = 0;
		virtual int OnSendData() = 0;
		virtual int OnBlockSend() = 0;
		virtual int OnFailedSend() = 0;
		virtual int OnError(ErrorType err) = 0;

	private:
		bool m_bSlave;
		int m_nRetry;
		int m_nTotalRetry;
	};

//
//-----------------------------------------------------------------------------
//
	class Idle
		: public sc::state< Idle, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::transition< EvENQ, LineRecv >,
			sc::transition< EvSendRequest, LineSend >
			> reactions;

		Idle(my_context ctx);
		~Idle();
	};

//
//-----------------------------------------------------------------------------
//
	class LineRecv
		: public sc::state< LineRecv, BlockState >
	{
	public:
		typedef sc::transition< EvNext, GetLen > reactions; 

		LineRecv(my_context ctx);
		~LineRecv();
	};

//
//-----------------------------------------------------------------------------
//
	class LineSend
		: public sc::state< LineSend, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::transition< EvT2, Retry >,
			sc::transition< EvEOT, SendData >,
			sc::custom_reaction< EvENQ >
			> reactions;

		LineSend(my_context ctx);
		~LineSend();

		sc::result react(const EvENQ& ev);
	};

//
//-----------------------------------------------------------------------------
//
	class Retry
		: public sc::state< Retry, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::custom_reaction< EvRetry >
			> reactions;

		Retry(my_context ctx);
		~Retry();

		sc::result react(const EvRetry& ev);
	};

//
//-----------------------------------------------------------------------------
//
	class SendData
		: public sc::state< SendData, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::transition< EvT2, Retry >,
			sc::transition< EvACK, BlockSended >,
			sc::transition< EvNoACK, Retry >
			> reactions;

		SendData(my_context ctx);
		~SendData();
	};

//
//-----------------------------------------------------------------------------
//
	class BlockSended
		: public sc::state< BlockSended, BlockState >
	{
	public:
		typedef sc::transition< EvNext, Idle > reactions;

		BlockSended(my_context ctx);
		~BlockSended();
	};

//
//-----------------------------------------------------------------------------
//
	class GetLen
		: public sc::state< GetLen, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::transition< EvT2, TimeOut >,
			sc::transition< EvLenErr, Eat >,
			sc::transition< EvNext, GetData >
			> reactions;

		GetLen(my_context ctx);
		~GetLen();
	};

//
//-----------------------------------------------------------------------------
//
	class GetData
		: public sc::state< GetData, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::transition< EvT1, TimeOut >,
			sc::transition< EvNext, CheckSum >
			> reactions;

		GetData(my_context ctx);
		~GetData();
	};

//
//-----------------------------------------------------------------------------
//
	class CheckSum
		: public sc::state< CheckSum, BlockState >
	{
	public:
		typedef mpl::list
			<
			sc::transition< EvCheckSumErr, Eat >,
			sc::transition< EvNext, BlockReceived >
			> reactions;

		CheckSum(my_context ctx);
		~CheckSum();
	};

//
//-----------------------------------------------------------------------------
//
	class BlockReceived
		: public sc::state< BlockReceived, BlockState >
	{
	public:
		typedef sc::transition< EvNext, RecvOK > reactions;

		BlockReceived(my_context ctx);
		~BlockReceived();
	};

//
//-----------------------------------------------------------------------------
//
	class RecvOK
		: public sc::state< RecvOK, BlockState >
	{
	public:
		typedef sc::transition< EvNext, Idle > reactions;

		RecvOK(my_context ctx);
		~RecvOK();
	};

//
//-----------------------------------------------------------------------------
//
	class FailedSend
		: public sc::state< FailedSend, BlockState >
	{
	public:
		typedef sc::transition< EvNext, Idle > reactions;

		FailedSend(my_context ctx);
		~FailedSend();
	};

//
//-----------------------------------------------------------------------------
//
	class TimeOut
		: public sc::state< TimeOut, BlockState >
	{
	public:
		typedef sc::transition< EvNext, Idle > reactions;

		TimeOut(my_context ctx);
		~TimeOut();
	};

//
//-----------------------------------------------------------------------------
//
	class Eat
		: public sc::state< Eat, BlockState >
	{
	public:
		typedef sc::transition< EvT1, TimeOut > reactions;

		Eat(my_context ctx);
		~Eat();
	};



//
//-----------------------------------------------------------------------------
//
}




