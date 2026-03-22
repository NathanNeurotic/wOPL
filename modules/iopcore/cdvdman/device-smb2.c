/*
  Copyright 2009-2010, jimmikaelkael
  Licenced under Academic Free License version 3.0
  Review Open PS2 Loader README & LICENSE files for further details.
  Modified for SMB2 support
*/
// AFAZER: Converter para a usmb2
#include "internal.h"
#include "device.h"
#include "../../network/libsmb2/lib/ps2/ps2smb2.h"

extern struct cdvdman_settings_smb2 cdvdman_settings;

extern struct irx_export_table _exp_oplsmb2;

//extern int smb2man_io_sema;

void DeviceInit(void)
{
    RegisterLibraryEntries(&_exp_oplsmb2);
}

void DeviceDeinit(void)
{ // Close all files and disconnect before IOP reboots. Note that this seems to help prevent VMC corruption in some games.
    DeviceUnmount();
}

int DeviceReady(void)
{
    return SCECdComplete;
}

void DeviceFSInit(void)
{
    int i = 0;
    char tmp_str[256];
    smb2Connect_in_t connect_in;
    smb2Connect_out_t connect_out;

    // Prepare connection parameters
    strcpy(connect_in.name, "games");
    strcpy(connect_in.username, cdvdman_settings.smb_user);
    strcpy(connect_in.password, cdvdman_settings.smb_password);
    //sprintf(connect_in.url, "smb://%s:%d/%s", cdvdman_settings.smb_ip, cdvdman_settings.smb_port, cdvdman_settings.smb_share);

#if 0
    // Connect to SMB2 share
    if (fileXioDevctl("smb:", SMB2_DEVCTL_CONNECT, (void *)&connect_in, sizeof(connect_in), (void *)&connect_out, sizeof(connect_out)) >= 0) {
        // Connection successful
        if (!(cdvdman_settings.common.flags & IOPCORE_SMB_FORMAT_USBLD)) {
            if (cdvdman_settings.smb_prefix[0]) {
                sprintf(tmp_str, "%s/%s/%s", cdvdman_settings.smb_prefix, cdvdman_settings.common.media == 0x12 ? "CD" : "DVD", cdvdman_settings.filename);
            } else {
                sprintf(tmp_str, "%s/%s", cdvdman_settings.common.media == 0x12 ? "CD" : "DVD", cdvdman_settings.filename);
            }

            // Open file using SMB2
            cdvdman_settings.FIDs[i++] = fileXioOpen(tmp_str, O_RDONLY, 0);
        } else {
            // Open all parts files
            for (i = 0; i < cdvdman_settings.common.NumParts; i++) {
                if (cdvdman_settings.smb_prefix[0])
                    sprintf(tmp_str, "%s/%s.%02x", cdvdman_settings.smb_prefix, cdvdman_settings.filename, i);
                else
                    sprintf(tmp_str, "%s.%02x", cdvdman_settings.filename, i);

                cdvdman_settings.FIDs[i] = fileXioOpen(tmp_str, O_RDONLY, 0);
            }
        }
    }
#endif
}

void DeviceLock(void)
{
    //WaitSema(smb2man_io_sema);
}

void DeviceUnmount(void)
{
#if 0
    int i;
    for (i = 0; i < cdvdman_settings.common.NumParts; i++) {
        if (cdvdman_settings.FIDs[i] >= 0) {
            fileXioClose(cdvdman_settings.FIDs[i]);
            cdvdman_settings.FIDs[i] = -1;
        }
    }
    // Disconnect all shares
    fileXioDevctl("smb:", SMB2_DEVCTL_DISCONNECT_ALL, NULL, 0, NULL, 0);
#endif
}

void DeviceStop(void)
{
}

int DeviceReadSectors(u64 lsn, void *buffer, unsigned int sectors)
{
    register u32 r, sectors_to_read, lbound, ubound, nlsn, offslsn;
    register int i, esc_flag = 0;
    u8 *p = (u8 *)buffer;
    int rv = SCECdErNO;

    lbound = 0;
    ubound = (cdvdman_settings.common.NumParts > 1) ? 0x80000 : 0xFFFFFFFF;
    offslsn = (u32)lsn;
    r = nlsn = 0;
    sectors_to_read = sectors;

    for (i = 0; i < cdvdman_settings.common.NumParts; i++, lbound = ubound, ubound += 0x80000, offslsn -= 0x80000) {

        if ((u32)lsn >= lbound && (u32)lsn < ubound) {
            if (((u32)lsn + sectors) > (ubound - 1)) {
                sectors_to_read = ubound - (u32)lsn;
                sectors -= sectors_to_read;
                nlsn = ubound;
            } else
                esc_flag = 1;

#if 0
            // Seek to the correct position
            fileXioLseek(cdvdman_settings.FIDs[i], offslsn * 2048, SEEK_SET);
            
            // Read the data
            if (fileXioRead(cdvdman_settings.FIDs[i], &p[r], sectors_to_read * 2048) != (int)(sectors_to_read * 2048)) {
                rv = SCECdErREAD;
                break;
            }
#endif

            r += sectors_to_read * 2048;
            offslsn += sectors_to_read;
            sectors_to_read = sectors;
            lsn = nlsn;
        }

        if (esc_flag)
            break;
    }

    return rv;
}