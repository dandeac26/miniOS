#include "ata_commands.h"


static inline WORD ata_get_base_io(int drive) {
    return (drive == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
}

#pragma optimize("", off)
void ata_send_command(int drive, BYTE command, DWORD lba, BYTE* buffer, WORD sector_count) {
    WORD base = ata_get_base_io(drive);

    // Wait for the drive to be ready
    while (__inbyte(base + ATA_REG_STATUS) & STATUS_BSY);

    // Set drive and LBA mode
    __outbyte(base + ATA_REG_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));

    // Sends sector count and LBA regs
    __outbyte(base + ATA_REG_SECCOUNT0, sector_count);
    __outbyte(base + ATA_REG_LBA0, lba & 0xFF);
    __outbyte(base + ATA_REG_LBA1, (lba >> 8) & 0xFF);
    __outbyte(base + ATA_REG_LBA2, (lba >> 16) & 0xFF);

    
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


#pragma optimize("", off)
void DetectATADevices() {
    for (int drive = 0; drive < MAX_DRIVES; drive++) {
        
        DetectedDevices[drive].DriveNumber = drive;
        DetectedDevices[drive].IsPresent = 0;

        ATA_IDENTIFY_RESPONSE identifyResponse = { 0 };

        
        if (ata_identify(drive, &identifyResponse) == 0) {
            DetectedDevices[drive].IsPresent = STATUS_DRIVE_PRESENT;
            DetectedDevices[drive].Info = identifyResponse;

            
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

#pragma optimize("", off)
int DetectedATADevice(int drive) {
    if (drive < MAX_DRIVES) 
    {
        DetectedDevices[drive].DriveNumber = drive;
        DetectedDevices[drive].IsPresent = 0;

        ATA_IDENTIFY_RESPONSE identifyResponse = { 0 };

        if (ata_identify(drive, &identifyResponse) == 0)
        {
            DetectedDevices[drive].IsPresent = STATUS_DRIVE_PRESENT;
            DetectedDevices[drive].Info = identifyResponse;

            return 1;
        }
    }
    return 0;
}
#pragma optimize("", on)

int ata_read_sectors(int drive, DWORD lba, BYTE* buffer, WORD sector_count)
{
    if (lba > 0xFFFFFFF) { // Check if LBA48 is required
        // LBA48 addressing
        ata_send_command(drive, ATA_CMD_READ_SECTORS_EXT, lba, buffer, sector_count);
    }
    else {
        // LBA28 addressing
        ata_send_command(drive, ATA_CMD_READ_SECTORS, lba, buffer, sector_count);
    }
    return 0;
}


#pragma optimize("", off)
int ata_identify(int drive, ATA_IDENTIFY_RESPONSE* response)
{
    BYTE buffer[SECTOR_SIZE] = { 0 }; // ATA IDENTIFY returns 512 bytes of data

    ata_send_command(drive, ATA_CMD_IDENTIFY, 0, &buffer, 1);

    // Parse the response buffer into the structure
    BYTE* src = buffer;
    BYTE* dest = (BYTE*)response;
    for (int i = 0; i < sizeof(ATA_IDENTIFY_RESPONSE); i++) {
        dest[i] = src[i];
    }

    return 0;
}
#pragma optimize("", on)

// Function to determine support for LBA48
int ata_supports_lba48(ATA_IDENTIFY_RESPONSE* response) {
    return response->Features.SupportLba48;
}


