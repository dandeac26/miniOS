#include "ata_commands.h"

#define MAX_DRIVES 2 // Maximum number of ATA drives (Primary and Secondary)
#define STATUS_DRIVE_PRESENT 0x1 // Custom flag for drive presence

//#include "ata_commands.h"
//#include "IO.h"

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

// Helper to get the base I/O port for a drive
static inline WORD ata_get_base_io(int drive) {
    return (drive == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
}

// Sends an ATA command to the drive
#pragma optimize("", off)
void ata_send_command(int drive, BYTE command, DWORD lba, BYTE* buffer, WORD sector_count) {
    WORD base = ata_get_base_io(drive);

    // Wait for the drive to be ready
    while (__inbyte(base + ATA_REG_STATUS) & STATUS_BSY);

    // Set the drive and LBA mode
    __outbyte(base + ATA_REG_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));

    // Send sector count and LBA registers
    __outbyte(base + ATA_REG_SECCOUNT0, sector_count);
    __outbyte(base + ATA_REG_LBA0, lba & 0xFF);
    __outbyte(base + ATA_REG_LBA1, (lba >> 8) & 0xFF);
    __outbyte(base + ATA_REG_LBA2, (lba >> 16) & 0xFF);

    // Send the command
    __outbyte(base + ATA_REG_COMMAND, command);

    // Wait for the drive to be ready and data to be available
    while (!(__inbyte(base + ATA_REG_STATUS) & STATUS_DRQ));

    // Read data if applicable
    if (command == ATA_CMD_READ_SECTORS || command == ATA_CMD_READ_SECTORS_EXT) {
        for (int i = 0; i < 256 * sector_count; ++i) {
            ((WORD*)buffer)[i] = __inword(base + ATA_REG_DATA);
        }
    }
}
#pragma optimize("", on)


typedef struct _ATA_DEVICE_INFO {
    int DriveNumber;            // 0 for primary, 1 for secondary
    int IsPresent;              // 1 if device is present, 0 otherwise
    ATA_IDENTIFY_RESPONSE Info; // Store identify information for the device
} ATA_DEVICE_INFO;

ATA_DEVICE_INFO DetectedDevices[MAX_DRIVES];

// Detects ATA devices and populates DetectedDevices array


// Function for sending ATA commands
//void ata_send_command(int drive, BYTE command, DWORD lba, BYTE* buffer, WORD sector_count) {
//    // Implementation of sending ATA commands
//    // Perform necessary steps to communicate with the drive
//    // This could involve using I/O ports or DMA, depending on the environment
//    // ...
//}

#pragma optimize("", off)
void DetectATADevices() {
    for (int drive = 0; drive < MAX_DRIVES; drive++) {
        // Clear the structure for this drive
        DetectedDevices[drive].DriveNumber = drive;
        DetectedDevices[drive].IsPresent = 0;

        ATA_IDENTIFY_RESPONSE identifyResponse = { 0 };

        // Attempt to identify the drive
        if (ata_identify(drive, &identifyResponse) == 0) {
            DetectedDevices[drive].IsPresent = STATUS_DRIVE_PRESENT;
            DetectedDevices[drive].Info = identifyResponse;

            // Log device information for debugging
            LogSerialAndScreen(
                "Drive %d detected\n",
                drive
            );
        }
        else {
            LogSerialAndScreen("Drive %d not present.\n", drive);
        }
    }
}
#pragma optimize("", on)

// Example ATA read function
int ata_read_sectors(int drive, DWORD lba, BYTE* buffer, WORD sector_count) {
    // Assume drive refers to the ATA device (0 for master, 1 for slave)
    // and a function ata_send_command exists for sending commands

    if (lba > 0xFFFFFFF) { // Check if LBA48 is required
        // LBA48 addressing
        ata_send_command(drive, ATA_CMD_READ_SECTORS_EXT, lba, buffer, sector_count);
    }
    else {
        // LBA28 addressing
        ata_send_command(drive, ATA_CMD_READ_SECTORS, lba, buffer, sector_count);
    }
    return 0; // Return appropriate status codes
}

// Example ATA identify function
#pragma optimize("", off)
int ata_identify(int drive, ATA_IDENTIFY_RESPONSE* response) {
    BYTE buffer[512] = { 0 }; // ATA IDENTIFY returns 512 bytes of data

    // Send the ATA IDENTIFY command
    ata_send_command(drive, ATA_CMD_IDENTIFY, 0, &buffer, 1);

    // Parse the response buffer into the structure
    BYTE* src = buffer;
    BYTE* dest = (BYTE*)response;
    for (int i = 0; i < sizeof(ATA_IDENTIFY_RESPONSE); i++) {
        dest[i] = src[i];
    }

    // Return success or error status
    return 0;
}
#pragma optimize("", on)

// Example function to determine support for LBA48
int ata_supports_lba48(ATA_IDENTIFY_RESPONSE* response) {
    return response->Features.SupportLba48;
}


