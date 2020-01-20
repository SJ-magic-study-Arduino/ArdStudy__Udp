/************************************************************
■description
UDP_Send_Receive.
oF	: key 0-9をpressすると、"Button|KeyId"をUDPで送ってくる。
Ard	: UDPを受信し、"Button[p]KeyId[p]Nth_Message"の形に変形してoFに送り返す。

■参考
	■イーサーネットシールド２
		https://ht-deko.com/arduino/shield_ethernet2.html
************************************************************/
#include <SPI.h>			// needed for Arduino versions later than 0018
#include <Ethernet2.h>		//
#include <EthernetUdp2.h> 	// UDP library from: bjoern@cs.stanford.edu 12/30/2008

/************************************************************
************************************************************/
// Arduino
byte MyMac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x27, 0x26 };
IPAddress MyIP(10, 0, 0, 10);
unsigned int ReceivePort = 12345;

// mac
IPAddress IP_SendTo(10, 0, 0, 5);
unsigned int SendPort = 12346;

char Buf_for_Receive[UDP_TX_PACKET_MAX_SIZE/* 24 */];

EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP

enum{
	MAX_UDP_MSGS = 5,
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
	Ethernet.begin(MyMac, MyIP);
	Udp.begin(ReceivePort);
	
	/********************
	********************/
	Serial.begin(9600);
	
	/********************
	********************/
	Serial.println("> Started");
	Serial.print("> UDP_TX_PACKET_MAX_SIZE = "); // Result = 24.
	Serial.println(UDP_TX_PACKET_MAX_SIZE);
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
			Serial.print(remote[i], DEC);
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
		String UdpMsgs[MAX_UDP_MSGS] = {"\0"};
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
            index++;
            if(MAX_UDP_MSGS <= index) return -1; // Separateした結果が想定より多かった.
        }
        else{
			dst[index] += tmp;
		}
    }
    return (index + 1);
}
