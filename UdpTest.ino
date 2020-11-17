/************************************************************
■description
UDP_Send_Receive.
oF	: key 0-9をpressすると、"Button|KeyId"をUDPで送ってくる。
Ard	: UDPを受信し、"Button[p]KeyId[p]Nth_Message"の形に変形してoFに送り返す。

■参考
	■イーサーネットシールド２
		https://ht-deko.com/arduino/shield_ethernet2.html
		
	■EthernetUDP
		https://garretlab.web.fc2.com/arduino_reference/libraries/standard_libraries/Ethernet/EthernetUDP/

■通信可能なIPについて検討した
	classA(10.0.0.0/8 = 10.0.0.0 - 10.255.255.255, subnet mask = 255.0.0.0)の場合、前8bitがグループアドレス、後 24bitがホストアドレス。
	
	グループアドレスが同じなら、Routerを介さずに通信が可能なので、例えば、
		10.0.0.5 - 10.0.1.10
	の通信も可能なはずである。
	実際、
		mac(10.0.0.5) - mac(10.0.1.10)
	のUDP通信は、問題なく動作した。
	
	But : 
		mac(10.0.0.5) - Arduino(10.0.1.10)
	とすると、mac -> Arduinoは、通ったが、逆のmessageが来なかった。
		mac(10.0.1.5) - Arduino(10.0.1.10)
	とすると、通った。
		mac(10.255.123.5) - Arduino(10.255.123.10)
	なども同様。
	
		IPAddress subnet(255, 0, 0, 0);
	として、
		Ethernet.begin(MyMacAddress, MyIP, gateway, subnet);
	というのもtryしてみたが、同じ結果であった。
	
	Arduinoがsendする方向に限り、上24bitがグループアドレスとして固定され、動作している様子。
	
	■考察と結論
		mac 1台に付き、maxで
			10.x.x.0 - 10.x.x.255
		例えば、
			10.0.1.0 - 10.0.1.255
		の256個しか、Arduinoを接続できないことになる。
		ちなみに、IPアドレス範囲のうち、先頭(10.0.0.0) = ネットワークアドレス、最後(10.255.255.255) = ブロードキャストアドレスで、これらは、LAN内の各機器に設定できないので注意。
		
		Artnetなど使う際は、さらに分け合う形になる。
		これで足りればOKだが、足りない場合は、
		macを複数台 準備し、これらをつないだ上で(macは、グループアドレスが同じであれば、OKなので、莫大な台数 つなげることが可能)
		それぞれのmacにArduinoをぶら下げてNetworkを構築することとなる。
************************************************************/
#include <SPI.h>			// needed for Arduino versions later than 0018
#include <Ethernet2.h>		//
#include <EthernetUdp2.h> 	// UDP library from: bjoern@cs.stanford.edu 12/30/2008

/************************************************************
************************************************************/
// Arduino
byte MyMacAddress[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x27, 0x26 };

IPAddress MyIP(10, 0, 0, 10);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 0, 0, 0);
unsigned int ReceivePort = 12345;

// mac
IPAddress IP_SendTo(10, 0, 0, 5);
unsigned int SendPort = 12346;

char Buf_for_Receive[UDP_TX_PACKET_MAX_SIZE/* 24 */];

EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP

enum{
	MAX_UDP_MSGS = 5, // separatorでsplitした結果の数
};
int counter = 0;


/************************************************************
************************************************************/
int SplitString(String src, char separater, String *dst);


/************************************************************
************************************************************/
/******************************
******************************/
void setup() {
	/********************
	********************/
	Ethernet.begin(MyMacAddress, MyIP);
	// Ethernet.begin(MyMacAddress, MyIP, gateway, subnet);
	Udp.begin(ReceivePort);
	
	/********************
	********************/
	Serial.begin(9600);
	
	/********************
	********************/
	Serial.println("> Started");
	Serial.print("> UDP_TX_PACKET_MAX_SIZE = ");
	Serial.println(UDP_TX_PACKET_MAX_SIZE);	// Result = 24.
	
	Serial.print("my IP : ");
	Serial.println(Ethernet.localIP());
}

/******************************
******************************/
void loop() {
	int packetSize = Udp.parsePacket();
	if (packetSize){
		/********************
		********************/
		Serial.print("> Received packet of size ");
		Serial.println(packetSize);
		Serial.print("From ");
		IPAddress remote = Udp.remoteIP();
		for (int i = 0; i < 4; i++){
			Serial.print(remote[i], DEC); // Serial.print(remote[i], HEX);
			if(i < 3) Serial.print(".");
		}
		Serial.print(", port ");
		Serial.println(Udp.remotePort());
		
		/********************
		********************/
		Udp.read(Buf_for_Receive, UDP_TX_PACKET_MAX_SIZE);
		Serial.print("Contents = ");
		Serial.println(Buf_for_Receive);
		
		String str_Message = Buf_for_Receive;
		String UdpMsgs[MAX_UDP_MSGS] = {"\0"}; // zero初期化の場合は、多分これで大丈夫だけど。。。
		for(int i = 0; i < MAX_UDP_MSGS; i++){
			UdpMsgs[i] = "\0"; // 一応、ちゃんと初期化.
		}
		
		SplitString(str_Message, '|', UdpMsgs);
		
		int KeyId = atoi(UdpMsgs[1].c_str()); // 一応、数字 取り出す例
		Serial.print("Pressed = ");
		Serial.println(KeyId);
		
		counter++;
		char buf[100];
		sprintf(buf, "%d", counter); // 数字を文字列化して 送信する例
		Udp.beginPacket(IP_SendTo, SendPort);
			String str_for_Send;
			str_for_Send = UdpMsgs[0];
			str_for_Send += "[p]";
			str_for_Send += UdpMsgs[1];
			str_for_Send += "[p]";
			str_for_Send += buf;
			Udp.write(str_for_Send.c_str());
		Udp.endPacket();
	}
	
	delay(10);
}

/******************************
description
	簡単のため、separaterは、一文字のみの対応となります。
	ofSplitString()より不便ですが、上手く使いこなしてください。
	
return
	Num of splitted messages.
	-1 means num of messages are larger than assumed.
******************************/
int SplitString(String src, char separater, String *dst){
    int index = 0;
    int src_length = src.length();
    for (int i = 0; i < src_length; i++) {
        char tmp = src.charAt(i);
        if ( tmp == separater ) {
			// dst[index] += "\0";
			
            index++;
            if(MAX_UDP_MSGS <= index) return -1; // Separateした結果が想定より多かった.
        }
        else{
			dst[index] += tmp;
		}
    }
	// dst[index] += "\0";
	
    return (index + 1);
}
