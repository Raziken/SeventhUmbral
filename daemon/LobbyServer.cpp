#include "LobbyServer.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <WinSock2.h>
#include "Types.h"
#include "MemStream.h"
#include "Base64.h"
#include "../common/BLOWFISH.H"

static const uint8 g_secureConnectionAcknowledgment[0x2A0] =
{
	0x00, 0x00, 0x00, 0x00, 0xA0, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x90, 0x02, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0C, 0x69, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xD0, 0xED, 0x45, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0xC0, 0xED, 0xDF, 0xFF, 0xAF, 0xF7, 0xF7, 0xAF, 0x10, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 
	0x42, 0x82, 0x63, 0x52, 0x01, 0x00, 0x00, 0x00, 0x10, 0xEF, 0xDF, 0xFF, 0x53, 0x61, 0x6D, 0x70, 
	0x6C, 0x65, 0x20, 0x53, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x20, 0x52, 0x75, 0x6E, 0x52, 0x75, 0x6E, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0xF7, 0xAF, 0xAF, 0xF7, 0x00, 0x00, 0xB8, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 0x40, 0x2C, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x43, 0x61, 0x6C, 0x6C, 0x62, 0x61, 0x63, 0x6B, 0x4F, 0x62, 
	0x6A, 0x65, 0x63, 0x74, 0x2E, 0x2E, 0x2E, 0x5B, 0x36, 0x36, 0x2E, 0x31, 0x33, 0x30, 0x2E, 0x39, 
	0x39, 0x2E, 0x38, 0x32, 0x3A, 0x36, 0x33, 0x34, 0x30, 0x37, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x70, 0xEE, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0x6C, 0x4E, 0x38, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x32, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0xAF, 0xF7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xC0, 0xEE, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0xFE, 0x4E, 0x38, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x0B, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x20, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 
	0x00, 0x01, 0xCC, 0xCC, 0x0C, 0x69, 0x00, 0xE0, 0xD0, 0x58, 0x33, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x80, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 
	0xC0, 0xEE, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0xD0, 0xED, 0x45, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0xF0, 0xEE, 0xDF, 0xFF, 0xAF, 0xF7, 0xF7, 0xAF, 0x20, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 
	0x0C, 0x69, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x34, 0x30, 0x37, 0x00, 0x00, 0x00, 0x00, 
	0x90, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0x18, 0xBE, 0x34, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0xD8, 0x32, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x32, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0xF7, 0xAF, 0x42, 0x82, 0x63, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x36, 0x36, 0x2E, 0x31, 0x33, 0x30, 0x2E, 0x39, 0x39, 0x2E, 0x38, 0x32, 0x00, 0x00, 
	0x00, 0x00, 0x36, 0x36, 0x2E, 0x31, 0x33, 0x30, 0x2E, 0x39, 0x39, 0x2E, 0x38, 0x32, 0x00, 0xFF, 
	0x90, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0x24, 0xCF, 0x76, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 0x70, 0x7A, 0xB7, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x90, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0xD1, 0xF3, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x32, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0xC0, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0xE8, 0x3E, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x70, 0x99, 0xAA, 0x01, 0x0C, 0x69, 0x00, 0xE0, 0xA0, 0x32, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x58, 0x59, 0x33, 0x02, 0x00, 0x00, 0x00, 0x00, 0x10, 0x6C, 0x4D, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0xE0, 0xEF, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0x05, 0x3F, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x0C, 0x69, 0x00, 0xE0, 0x0C, 0x69, 0x00, 0xE0, 0xA0, 0x32, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xF0, 0xDF, 0xFF, 0x7F, 0xFD, 0xFF, 0xFF, 0x23, 0x3F, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0xC0, 0x5A, 0x33, 0x02, 0x0C, 0x69, 0x00, 0xE0, 0xA0, 0x32, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 
};

static const uint8 g_loginAcknowledgment[0x280] = 
{
	0x01, 0x00, 0x00, 0x00, 0x80, 0x02, 0x01, 0x00, 0x3F, 0xC7, 0x8A, 0xEE, 0x3B, 0x01, 0x00, 0x00, 
	0x70, 0x02, 0x03, 0x00, 0x68, 0x68, 0x00, 0xE0, 0x68, 0x68, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x99, 0x00, 0x00, 0x00, 0x00, 
	0x42, 0x9A, 0x5F, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x49, 0x4E, 0x41, 0x4C, 0x20, 0x46, 0x41, 
	0x4E, 0x54, 0x41, 0x53, 0x59, 0x20, 0x58, 0x49, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static const uint8 g_characterListPacket[0xC00] =
{
	0x01, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x05, 0x00, 0xEA, 0xCE, 0x8A, 0xEE, 0x3B, 0x01, 0x00, 0x00, 

	0x10, 0x02, 0x03, 0x00, 0x68, 0x68, 0x00, 0xE0, 0x68, 0x68, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,			//(9) 0x06 -> Item Count
	0x02, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x44, 0x75, 0x72, 0x61, 0x6E, 0x64, 0x61, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0x01, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x48, 0x79, 0x70, 0x65, 0x72, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x04, 0x00, 0x02, 0x00, 0x5E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x4D, 0x61, 0x73, 0x61, 0x6D, 0x75, 0x6E, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x05, 0x00, 0x03, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x47, 0x75, 0x6E, 0x67, 0x6E, 0x69, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0x00, 0x04, 0x00, 0x5C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x65, 0x67, 0x69, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0A, 0x00, 0x05, 0x00, 0x5E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x53, 0x61, 0x72, 0x67, 0x61, 0x74, 0x61, 0x6E, 0x61, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	
	0x10, 0x02, 0x03, 0x00, 0x68, 0x68, 0x00, 0xE0, 0x68, 0x68, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0B, 0x00, 0x06, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x42, 0x61, 0x6C, 0x6D, 0x75, 0x6E, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0C, 0x00, 0x07, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x52, 0x69, 0x64, 0x69, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x00, 0x08, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x45, 0x78, 0x63, 0x61, 0x6C, 0x69, 0x62, 0x75, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x09, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x52, 0x61, 0x67, 0x6E, 0x61, 0x72, 0x6F, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	
	0x10, 0x02, 0x03, 0x00, 0x68, 0x68, 0x00, 0xE0, 0x68, 0x68, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

	0xF0, 0x01, 0x03, 0x00, 0x68, 0x68, 0x00, 0xE0, 0x68, 0x68, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0xF5, 0xA5, 0xE0, 
	0x09, 0x79, 0xC1, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x6F, 0x72, 0x6E,			//(C) "Cornelius" Retainer Name
	0x65, 0x6C, 0x69, 0x75, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x09, 0x79, 0xC1, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	
	0xD0, 0x03, 0x03, 0x00, 0x68, 0x68, 0x00, 0xE0, 0x68, 0x68, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 
	0x14, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00,			//(8) 0x50E0E824 -> Timestamp
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

//	0xFC, 0xE7, 0x58, 0x01, 0x09, 0x79, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00, 0x00,			//(4) 0x00C17909 -> Character Id
//	0x57, 0x72, 0x65, 0x6E, 0x69, 0x78, 0x20, 0x57, 0x72, 0x6F, 0x6E, 0x67, 0x00, 0x00, 0x00, 0x00,			//(0) "Wrenix Wrong" -> Character Name
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x52, 0x61, 0x67, 0x6E, 0x61, 0x72, 0x6F, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,			//(0) "Ragnarok" -> Server Name
	0x77, 0x41, 0x51, 0x41, 0x41, 0x4F, 0x6F, 0x6E, 0x49, 0x79, 0x4D, 0x4E, 0x41, 0x41, 0x41, 0x41,			//(0) base64 stuff -> Character Data
	0x56, 0x33, 0x4A, 0x6C, 0x62, 0x6D, 0x6C, 0x34, 0x49, 0x46, 0x64, 0x79, 0x62, 0x32, 0x35, 0x6E, 
	0x41, 0x42, 0x77, 0x41, 0x41, 0x41, 0x41, 0x45, 0x41, 0x41, 0x41, 0x41, 0x41, 0x77, 0x41, 0x41, 
	0x41, 0x41, 0x4D, 0x41, 0x41, 0x41, 0x41, 0x5F, 0x38, 0x4F, 0x41, 0x44, 0x41, 0x41, 0x48, 0x51, 
	0x46, 0x41, 0x41, 0x45, 0x41, 0x41, 0x41, 0x42, 0x41, 0x41, 0x41, 0x41, 0x41, 0x42, 0x54, 0x51, 
	0x43, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x47, 0x45, 0x67, 0x41, 0x41, 0x41, 0x41, 0x4D, 0x51, 0x41, 0x41, 0x51, 0x43, 0x51, 0x41, 
	0x41, 0x4D, 0x41, 0x73, 0x41, 0x41, 0x43, 0x4B, 0x56, 0x41, 0x41, 0x41, 0x41, 0x50, 0x67, 0x43, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x51, 0x41, 
	0x41, 0x41, 0x41, 0x6B, 0x41, 0x77, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x4E, 0x76, 0x62, 0x31, 0x4D, 0x30, 0x35, 0x41, 0x51, 0x41, 0x41, 0x42, 0x42, 0x6F, 0x41, 
	0x41, 0x41, 0x45, 0x41, 0x42, 0x71, 0x6F, 0x69, 0x49, 0x75, 0x49, 0x4B, 0x41, 0x41, 0x41, 0x41, 
	0x63, 0x48, 0x4A, 0x32, 0x4D, 0x45, 0x6C, 0x75, 0x62, 0x6A, 0x41, 0x78, 0x41, 0x42, 0x45, 0x41, 
	0x41, 0x41, 0x42, 0x6B, 0x5A, 0x57, 0x5A, 0x68, 0x64, 0x57, 0x78, 0x30, 0x56, 0x47, 0x56, 0x79, 
	0x63, 0x6D, 0x6C, 0x30, 0x62, 0x33, 0x4A, 0x35, 0x41, 0x41, 0x77, 0x4A, 0x41, 0x68, 0x63, 0x41, 
	0x42, 0x41, 0x41, 0x41, 0x41, 0x41, 0x51, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x67, 0x41, 0x41, 0x41, 0x41, 0x49, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static const uint8 g_characterData[0xF5] =
{
	0xC0, 0x04, 0x00, 0x00, 0xEA, 0x27, 0x23, 0x23, 0x0D, 0x00, 0x00, 0x00, 0x57, 0x72, 0x65, 0x6E, 
	0x69, 0x78, 0x20, 0x57, 0x72, 0x6F, 0x6E, 0x67, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
	0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0xE0, 0x03, 0x00, 0x01, 0xD0, 
	0x14, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0xD0, 0x08, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x20, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x40, 0x24, 0x00, 
	0x00, 0xC0, 0x2C, 0x00, 0x00, 0x8A, 0x54, 0x00, 0x00, 0x00, 0xF8, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x24, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xDB, 0xDB, 0xD4, 0xCD, 0x39, 0x01, 0x00, 0x00, 0x04, 0x1A, 0x00, 0x00, 0x01, 0x00, 0x06,			//(9) 0x04 -> Class, (A)0x1A -> Level
	0xAA, 0x22, 0x22, 0xE2, 0x0A, 0x00, 0x00, 0x00, 0x70, 0x72, 0x76, 0x30, 0x49, 0x6E, 0x6E, 0x30, 
	0x31, 0x00, 0x11, 0x00, 0x00, 0x00, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6C, 0x74, 0x54, 0x65, 0x72, 
	0x72, 0x69, 0x74, 0x6F, 0x72, 0x79, 0x00, 0x0C, 0x09, 0x02, 0x17, 0x00, 0x04, 0x00, 0x00, 0x00, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00 
};

struct PACKETHEADER
{
	uint32 packetType;
	uint16 packetSize;
	uint16 unknown0;
	uint32 unknown1[6];
};
static_assert(sizeof(PACKETHEADER) == 0x20, "Packet header size must be 32 bytes.");

#define LOGNAME		("LobbyServer")

bool HasPacket(Framework::CMemStream& stream)
{
	if(stream.GetSize() < sizeof(PACKETHEADER))
	{
		return false;
	}

	stream.Seek(0, Framework::STREAM_SEEK_SET);

	PACKETHEADER header;
	stream.Read(&header, sizeof(PACKETHEADER));
	stream.Seek(0, Framework::STREAM_SEEK_END);

	if(stream.GetSize() < header.packetSize)
	{
		return false;
	}

	return true;
}

std::vector<uint8> ReadPacket(Framework::CMemStream& stream)
{
	std::vector<uint8> result;

	if(!HasPacket(stream))
	{
		assert(0);
		return result;
	}

	stream.Seek(0, Framework::STREAM_SEEK_SET);
	PACKETHEADER header;
	stream.Read(&header, sizeof(PACKETHEADER));

	stream.Seek(0, Framework::STREAM_SEEK_SET);
	result.resize(header.packetSize);
	stream.Read(&result[0], header.packetSize);
	stream.Truncate();

	stream.Seek(0, Framework::STREAM_SEEK_END);

	return result;
}

std::string DumpPacket(const std::vector<uint8>& packet)
{
	std::string result;

	static const unsigned int lineWidth = 0x10;

	for(unsigned int i = 0; i < (packet.size() + lineWidth - 1) / lineWidth; i++)
	{
		for(unsigned int j = 0; j < lineWidth; j++)
		{
			unsigned int offset = i * lineWidth + j;
			if(offset >= packet.size())
			{
				result += "   ";
			}
			else
			{
				char byteString[4];
				sprintf(byteString, "%0.2X ", packet[offset]);
				result += byteString;
			}
		}

		result += "     ";

		for(unsigned int j = 0; j < lineWidth; j++)
		{
			unsigned int offset = i * lineWidth + j;
			if(offset >= packet.size()) continue;
			char character = packet[offset];
			if((character >= 0) && (isdigit(character) || isalpha(character)))
			{
				result += character;
			}
			else
			{
				result += ".";
			}
		}

		result += "\r\n";
	}

	return result;
}

void DecryptPacket(std::vector<uint8>& packet)
{
	if(packet.size() <= sizeof(PACKETHEADER))
	{
		assert(0);
		return;
	}
	uint8* data = &packet[0] + sizeof(PACKETHEADER);
	uint32 dataLength = packet.size() - sizeof(PACKETHEADER);
	for(unsigned int i = 0; i < dataLength; i += 8)
	{
		Blowfish_decipher(
			reinterpret_cast<unsigned long*>(data + i), 
			reinterpret_cast<unsigned long*>(data + i + 4));
	}
}

void EncryptPacket(std::vector<uint8>& packet)
{
	if(packet.size() <= sizeof(PACKETHEADER))
	{
		assert(0);
		return;
	}

	uint8* data = &packet[0];
	uint32 size = packet.size();

	data += 0x10;
	size -= 0x10;

	while(1)
	{
		uint32 subPacketSize = *reinterpret_cast<uint16*>(data);
		size -= subPacketSize;
		subPacketSize -= 0x10;
		data += 0x10;
		for(unsigned int i = 0; i < subPacketSize; i += 8)
		{
			Blowfish_encipher(
				reinterpret_cast<unsigned long*>(data + i), 
				reinterpret_cast<unsigned long*>(data + i + 4));
		}
		data += subPacketSize;
		if(size == 0) break;
	}
}

void ClientThreadProc(SOCKET clientSocket)
{
	u_long notBlockingMode = 1;
	ioctlsocket(clientSocket, FIONBIO, &notBlockingMode);
	Framework::CMemStream incomingStream;

	printf("%s: Received connection.\r\n", LOGNAME);

	while(1)
	{
		//Read from socket
		{
			static const unsigned int maxPacketSize = 0x10000;
			uint8 incomingPacket[maxPacketSize];
			int read = recv(clientSocket, reinterpret_cast<char*>(incomingPacket), maxPacketSize, 0);
			if(read == 0)
			{
				//Client disconnected
				printf("%s: Client disconnected.\r\n", LOGNAME);
				break;
			}
			if(read > 0)
			{
				incomingStream.Write(incomingPacket, read);
			}
		}
		if(HasPacket(incomingStream))
		{
			auto incomingPacket = ReadPacket(incomingStream);
			if((incomingPacket.size() == 0x288) && (incomingPacket[0x44] == 'T'))		//Test Ticket Data
			{
				uint32 clientTime = *reinterpret_cast<uint32*>(&incomingPacket[0x84]);

				//We assume clientTime is 0x50E0E812, but we need to generate a proper key later
				uint8 blowfishKey[0x10] = { 0xB4, 0xEE, 0x3F, 0x6C, 0x01, 0x6F, 0x5B, 0xD9, 0x71, 0x50, 0x0D, 0xB1, 0x85, 0xA2, 0xAB, 0x43 };
				InitializeBlowfish(reinterpret_cast<char*>(blowfishKey), 0x10);

				printf("%s: Received encryption key: 0x%0.8X.\r\n", 
					LOGNAME, clientTime);

				//Respond with acknowledgment
				std::vector<uint8> outgoingPacket(std::begin(g_secureConnectionAcknowledgment), std::end(g_secureConnectionAcknowledgment));
				EncryptPacket(outgoingPacket);
				int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
				assert(sent == outgoingPacket.size());
			}
			else if(incomingPacket.size() == 0xC0)
			{
				DecryptPacket(incomingPacket);
				printf("%s: Got acknowledgment for secure session.\r\n", LOGNAME);
				printf("%s: SESSION_ID: %s\r\n", LOGNAME, &incomingPacket[0x40]);
				printf("%s: CLIENT_VERSION: %s\r\n", LOGNAME, &incomingPacket[0x80]);

				std::vector<uint8> outgoingPacket(std::begin(g_loginAcknowledgment), std::end(g_loginAcknowledgment));
				EncryptPacket(outgoingPacket);
				int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
				assert(sent == outgoingPacket.size());
			}
			else if(incomingPacket.size() == 0x40)
			{
				DecryptPacket(incomingPacket);
				printf("%s: GetCharacters.\r\n", LOGNAME);

				std::vector<uint8> outgoingPacket(std::begin(g_characterListPacket), std::end(g_characterListPacket));
				
				auto encodedCharacterData = Framework::ToBase64(g_characterData, sizeof(g_characterData));
				std::replace(std::begin(encodedCharacterData), std::end(encodedCharacterData), '+', '-');
				std::replace(std::begin(encodedCharacterData), std::end(encodedCharacterData), '/', '_');
				
				for(unsigned int i = 0; i < encodedCharacterData.size(); i++)
				{
					outgoingPacket[0x8A0 + i] = encodedCharacterData[i];
				}

				EncryptPacket(outgoingPacket);
				int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
				assert(sent == outgoingPacket.size());
			}
			else
			{
				printf("%s: Received unknown packet of size 0x%0.4X.\r\n%s\r\n",
					LOGNAME, incomingPacket.size(), DumpPacket(incomingPacket).c_str());
			}
		}
		Sleep(16);
	}
}

CLobbyServer::CLobbyServer()
{

}

CLobbyServer::~CLobbyServer()
{

}

void CLobbyServer::Start()
{
	std::thread serverThread(std::bind(&CLobbyServer::ServerThreadProc, this));
	m_serverThread = std::move(serverThread);
}

void CLobbyServer::ServerThreadProc()
{
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in service;
	service.sin_family			= AF_INET;
	service.sin_addr.s_addr		= inet_addr("127.0.0.1");
	service.sin_port			= htons(54994);
	if(bind(listenSocket, reinterpret_cast<sockaddr*>(&service), sizeof(sockaddr_in)))
	{
		printf("Failed to bind socket.\r\n");
		return;
	}

	if(listen(listenSocket, SOMAXCONN))
	{
		printf("Failed to listen on socket.\r\n");
		return;
	}

	printf("Lobby server started.\r\n");

	while(1)
	{
		sockaddr_in incomingAddr;
		int incomingAddrSize = sizeof(sockaddr_in);
		SOCKET incomingSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&incomingAddr), &incomingAddrSize);
		std::thread clientThread(std::bind(&ClientThreadProc, incomingSocket));
		clientThread.detach();
	}

	closesocket(listenSocket);
}
