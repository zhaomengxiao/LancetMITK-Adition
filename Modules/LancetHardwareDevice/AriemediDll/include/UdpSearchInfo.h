#ifndef UDP_SEARCHINFO_HPP
#define UDP_SEARCHINFO_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <winsock2.h> 
#include <iphlpapi.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")

struct dnsHeader
{
	unsigned short id;
	unsigned short flags;
	unsigned short questions;
	unsigned short answers;
	unsigned short authority;
	unsigned short additional;
};

struct dnsQuestion
{
	int length;
	unsigned short qtype;
	unsigned short qclass;
	char* name;
};

typedef struct dnsItem
{
	char* domain;
	char* ip;
}dnsItem;

typedef struct deviceInfo
{
	std::string hostname;
	std::string ipv4;
}deviceInfo;

class UdpSearchInfo
{
public:
	UdpSearchInfo();
	~UdpSearchInfo();

	//get net information of device
	std::map<std::string, std::vector<deviceInfo>> getHostname(std::string Feature_name);

private:
	char hostName[255];

	//DNS transmission
	int DNSCreateHeader(dnsHeader* header);
	int DNSCreateQuestion(dnsQuestion* question, const char* hostname);
	int DNSBuildRequestion(dnsHeader* header, dnsQuestion* question, char* request, int rlen);
	void DNSParseName(unsigned char* chunk, unsigned char* ptr, char* out, int* len);
	std::vector<std::string> DNSParseResponse(char* buffer, struct dns_item** domains);
	void MDNSQuestionReponse(std::string Feature_name, std::vector<std::string>& hosttemp, std::string Local_IP, std::vector<std::string>& hostipv4);

	//get hostname, IP and MAC address
	void getLocalHostname();
	void getTargetIP(char TargetIPv4[22], std::string hostName);
	std::string getDiviceMAC(std::string Ipv4);

	//IP check
	int pointerCheck(int in);
};
#endif