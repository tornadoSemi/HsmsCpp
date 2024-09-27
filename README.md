# GemDemo
Demo for SECS HSMS communication

# Example

通讯库支持SECS-I与HSMS两种通讯方式。区别在初始化时，使用不同的参数类型。
其它的函数是一样的。

## 创建通讯库对象
    GemPtr pGem = CGem::GetInstancePtr();
## 初始化连接
### Secs-I

	SecsSet sset;
	sset.sComName = "COM1";
	sset.uBaudrate = 9600;
	sset.uDevID = 1;
	sset.mode = CSECS_EQUIP;

    pGem->InitLink(sset);

### Secs-II (HSMS)

	HsmsSet set;
	set.uLocalPort = 5000;
	set.mode = CHSMS_PASSIVE;
	set.uDevID = 1;

    pGem->InitLink(set);

## 设置消息接收处理

    pGem->SetSecsRecvFun([this](const SecsMessage& msg) {
		return handleMessage(msg);
		});

## 启动连接

    pGem->Connect();

## 发送消息

    ItemPtr iSend = Item::L();
    iSend->Append(Item::B(6));
    iSend->Append(Item::U2(10));
    iSend->Append(Item::A("Fire Alarm"));

    SecsMessage smsg{ 5, 1, iSend };
    mid = pGem->Send(smsg);

## 回应消息

    ItemPtr item = Item::L();
    item->Append(Item::A("DemoEquip"));
    item->Append(Item::A("Rev 1.0"));

    SecsMessage rmsg{1, 2, item, msg.MID};
    m_pGem->Reply(rmsg);

## 联系方式
    QQ: 24164068


