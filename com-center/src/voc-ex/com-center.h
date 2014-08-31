#ifndef COMCENTER_H_
#define COMCENTER_H_

#include <windows.h>
#include <vector>
#include <string>

using namespace std;

namespace vocex {

enum COMMANDS {
	PREPARE = 1, STOP = 2, START = 3
};

class ComCenter {

	SOCKET s;
	sockaddr_in serverAddr;
	int serverAddrSize;

	char buf0[1];
	char buf1[4096 + 2];

public:
	/*
	 * a0.a1.a2.a3 - IP адрес КУП
	 * Например: ComCenter c(192, 168, 0, 1);
	 */
	ComCenter(unsigned char a0, unsigned char a1, unsigned char a2,
			unsigned char a3) {
		WSAData data;
		WSAStartup(MAKEWORD(1, 2), &data);
		s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (s == INVALID_SOCKET) {
			throw "Error creating socket!";
		}
		memset((char*) &serverAddr, sizeof(serverAddr), 0);
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(3333);
		serverAddr.sin_addr.S_un.S_un_b.s_b1 = a0;
		serverAddr.sin_addr.S_un.S_un_b.s_b2 = a1;
		serverAddr.sin_addr.S_un.S_un_b.s_b3 = a2;
		serverAddr.sin_addr.S_un.S_un_b.s_b4 = a3;
		serverAddrSize = sizeof(serverAddr);
	}
	void sendPrepare() {
		send(PREPARE);
		serverAddrSize = sizeof(serverAddr);
		recvfrom(s, buf0, 1, 0, (sockaddr*) &serverAddr, &serverAddrSize);
	}
	void sendStart() {
		send(START);
	}
	void sendStop() {
		send(STOP);
	}
	void sendStop(vector<unsigned char>& rawPOPData, string& variant) {
		sendStop();
		serverAddrSize = sizeof(serverAddr);
		bool variantFlag = true;
		variant.clear();
		while (recvfrom(s, buf1, 4096 + 2, 0, (sockaddr*) &serverAddr,
				&serverAddrSize) != -1) {
			serverAddrSize = sizeof(serverAddr);
			long l = buf1[0] << 8 | buf1[1];
			if (l == 0) {
				break;
			}
			for (long i = 0; i < l; i++) {
				if (variantFlag) {
					variant += buf1[i + 2];
				} else {
					rawPOPData.push_back(buf1[i + 2]);
				}
			}
			variantFlag = false;
		}
	}
	void send(char cmd) {
		buf0[0] = cmd;
		serverAddrSize = sizeof(serverAddr);
		if (sendto(s, buf0, 1, 0, (sockaddr*) &serverAddr, serverAddrSize)
				== -1) {
			throw "Error transmitting data!";
		};
	}
	~ComCenter() {
		if (s != INVALID_SOCKET) {
			closesocket(s);
		}
		WSACleanup();
	}

};

}

#endif /*COMCENTER_H_*/
