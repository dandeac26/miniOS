#ifndef _ATA_COMMANDS_H_
#define _ATA_COMMANDS_H_


#include "main.h"

// ATA commands

// The extended version is needed for LBA48 addressing
#define ATA_CMD_READ_SECTORS            0x20
#define ATA_CMD_READ_SECTORS_EXT        0x24
#define ATA_CMD_READ_DMA_EXT            0x25
#define ATA_CMD_WRITE_SECTORS           0x30
#define ATA_CMD_WRITE_SECTORS_EXT       0x34
#define ATA_CMD_WRITE_DMA_EXT           0x35
#define ATA_CMD_READ_DMA                0xC8
#define ATA_CMD_WRITE_DMA               0xCA

// used for identifying ATA devices
#define ATA_CMD_IDENTIFY                0xEC

#pragma pack(push,1)

// warning C4214: nonstandard extension used: bit field types other than int
#pragma warning(disable:4214)

#define ATA_SERIAL_NO_CHARS             20
#define ATA_MODEL_NO_CHARS              40


#define MAX_DRIVES 2 // Maximum number of ATA drives (Primary and Secondary)
#define STATUS_DRIVE_PRESENT 0x1 // Custom flag for drive presence

#define STATUS_BSY 0x80
#define STATUS_DRQ 0x08
#define STATUS_ERR 0x01

// Base I/O ports for Primary and Secondary buses
#define ATA_PRIMARY_IO  0x1F0
#define ATA_SECONDARY_IO 0x170

// Offset for each register
#define ATA_REG_DATA      0
#define ATA_REG_ERROR     1
#define ATA_REG_FEATURES  1
#define ATA_REG_SECCOUNT0 2
#define ATA_REG_LBA0      3
#define ATA_REG_LBA1      4
#define ATA_REG_LBA2      5
#define ATA_REG_HDDEVSEL  6
#define ATA_REG_COMMAND   7
#define ATA_REG_STATUS    7

#define SECTOR_SIZE 512



typedef struct _ATA_IDENTIFY_RESPONSE
{
    /*
    0 General configuration information
    1 Number of logical cylinders in the default CHS translation
    3 Number of logical heads in the default CHS translation
    6 Number of logical sectors per track in the default CHS translation
    10-19 Serial number (20 ASCII characters)
    23-26 Firmware revision (8 ASCII characters)
    27-46 Model number (40 ASCII characters)
    54 Number of logical cylinders in the current CHS translation
    55 Number of current logical heads in the current CHS translation
    56 Number of current logical sectors per track in the current CHS translation
    57-58 Capacity in sectors in the current CHS translation
    60-61 Total number of addressable sectors (28-bit LBA addressing)
    100-103 Total number of addressable sectors (48-bit LBA addressing)
    160-255 Reserved
    */
    WORD        GeneralConfig;                              // 0
    WORD        LogicalCylinders;                           // 2
    WORD        Reserved0;                                  // 4
    WORD        LogicalHeads;                               // 6
    WORD        Reserved1[2];                               // 8
    WORD        LogicalSectors;                             // 12
    WORD        Reserved2[3];                               // 14
    char        SerialNumbers[ATA_SERIAL_NO_CHARS];         // 20
    WORD        Reserved3[3];                               // 40
    char        FirmwareRevision[8];                        // 46
    char        ModelNumber[ATA_MODEL_NO_CHARS];            // 54
    WORD        Reserved4[7];                               // 94
    WORD        LogicalCylindersCurrent;                    // 108
    WORD        LogicalHeadsCurrent;                        // 110
    WORD        LogicalSectorsCurrent;                      // 112
    DWORD       SectorCapacity;                             // 114
    WORD        Reserved5;                                  // 118
    DWORD       Address28Bit;                               // 120
    WORD        Reserved6[20];                              // 124
    struct {
        WORD    Reserved0 : 4;
        WORD    PacketFeature : 1;
        WORD    Reserved1 : 11;
        WORD    Reserved2 : 10;
        WORD    SupportLba48 : 1;
        WORD    Reserved3 : 5;
        WORD    Reserved4;
    } Features;
    WORD        Reserved7[15];                              // 170
    QWORD       Address48Bit;                               // 200
    WORD        Reserved8[152];                             // 208
} ATA_IDENTIFY_RESPONSE, * PATA_IDENTIFY_RESPONSE;
#pragma warning(default:4124)
#pragma pack(pop)

typedef struct _ATA_DEVICE_INFO {
    int DriveNumber;            // 0 for primary, 1 for secondary
    int IsPresent;
    ATA_IDENTIFY_RESPONSE Info;
} ATA_DEVICE_INFO;

ATA_DEVICE_INFO DetectedDevices[MAX_DRIVES];



void ata_send_command(int drive, BYTE command, DWORD lba, BYTE* buffer, WORD sector_count);
int ata_read_sectors(int drive, DWORD lba, BYTE* buffer, WORD sector_count);
int ata_identify(int drive, ATA_IDENTIFY_RESPONSE* response);
int ata_supports_lba48(ATA_IDENTIFY_RESPONSE* response);
void DetectATADevices();

#endif _ATA_COMMANDS_H_
