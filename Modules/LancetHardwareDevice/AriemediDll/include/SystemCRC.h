#ifndef SYSTEM_CRC_HPP
#define SYSTEM_CRC_HPP

class SystemCRC
{
public:
	SystemCRC();
	virtual ~SystemCRC() {};

	//calculate CRC16 of device's reply
	unsigned int calculateCRC16(const char* reply, int replyLength) const;

private:
	unsigned int calcValue(unsigned int crc, int data) const;

	static unsigned int crcTable_[256];
};
#endif // SYSTEM_CRC_HPP