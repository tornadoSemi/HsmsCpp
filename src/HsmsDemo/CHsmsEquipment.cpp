#include "CHsmsEquipment.h"
#include <thread>
#include <iostream>

using namespace std;

CHsmsEquipment::CHsmsEquipment()
{
	m_pGem = CGem::GetInstancePtr(CGem::LinkMode::HSMS_SS);
}

CHsmsEquipment::~CHsmsEquipment()
{
}

int CHsmsEquipment::Start()
{
	srand(time(nullptr));
	int nRand = rand();

	HsmsSet set;
	set.uLocalPort = 5000;
	set.mode = CHSMS_PASSIVE;
	set.uDevID = 0;
	set.cRemoteIP = "127.0.0.1";
	set.uRemotePort = 5000;

	SecsSet sset;
	sset.sComName = "COM1";
	sset.uBaudrate = 9600;
	sset.uDevID = 1;
	sset.mode = CSECS_EQUIP;

	m_pGem->SetSmlLog("../log", 7);
	m_pGem->InitLink(set);

	m_pGem->SetSecsRecvFun([this](const SecsMessage& msg)
		{
			return handleMessage(msg);
		});

	m_pGem->Connect();

	ItemTest();

	return 0;
}

int CHsmsEquipment::Stop()
{
	m_pGem->Disconnect();

	return 0;
}

int CHsmsEquipment::handleMessage(const SecsMessage& msg)
{
	int nSF = SF(msg.S, msg.F);

	switch (nSF)
	{
	case 1 << 8 | 1:
	{
		ItemPtr item = Item::L();
		item->Append(Item::A("DemoEquip"));
		item->Append(Item::A("Rev 1.0"));

		SecsMessage rmsg{ 1, 2, item, msg.MID };
		m_pGem->Reply(rmsg);

		std::thread t([this]()
			{
				SecsMessage smsgHello{ 1, 1, nullptr };
				m_pGem->Send(smsgHello);

				std::this_thread::sleep_for(std::chrono::seconds(1));
				ItemPtr iSend = Item::L();
				iSend->Append(Item::B(6));
				iSend->Append(Item::U2(10));
				iSend->Append(Item::A("Fire Alarm"));
				iSend->Append(Item::F4(NAN));

				SecsMessage smsg{ 5, 1, iSend };
				m_pGem->Send(smsg);
			});
		t.detach();
	}
	break;
	case 1 << 8 | 3:
	{
		ItemPtr item = Item::L();
		for (int i = 0; i < 1000; i++)
		{
			item->Append(Item::U4(i));
		}

		SecsMessage rmsg{ 1, 4, item, msg.MID };
		m_pGem->Reply(rmsg);
	}
	break;
	default:
	{
		if (msg.F % 2 != 0)
		{
			SecsMessage rmsg{ msg.S, (byte)(msg.F + 1), Item::L(), msg.MID };
			m_pGem->Reply(rmsg);
		}
	}
	break;
	}

	string sml = Item::GetSML(msg.item);
	cout << "recv" << "S" << (int)msg.S << "F" << (int)msg.F << " MID" << msg.MID << endl;
	cout << sml.c_str() << endl;

	return 0;
}

int CHsmsEquipment::ItemTest()
{
	auto item = Item::U4(vector<u4>{2, 3, 4});
	auto count = item->getCount();
	auto itemcount = item->getItemCount();

	return 0;
}