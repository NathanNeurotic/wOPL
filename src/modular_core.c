
#include "include/common.h"
#include "include/ioman.h"
#include "include/modular_core.h"
#include "include/extern_irx.h"
#include <malloc.h>

#include <unzip.h>

#define MAX_CORES 10

extern void *eecore_elf;
extern int size_eecore_elf;

extern unsigned char eesync_irx[];
extern unsigned int size_eesync_irx;

typedef struct
{
    char *filePath;
    char *name;

    int hasExtraModes;
    int hasPademu;
    int hasBDM;
} core_t;

static int guiCoreID = 0;
static int nCores = 0;

static core_t cores[MAX_CORES];
static char **guiCoreNames;

core_file_t coreFile[CORE_FILE_COUNT];

static int oplReadCoreEntry(int index, const char *path, const char *separator, const char *name, unsigned int mode)
{
    if (!S_ISDIR(mode)) {
        if (strstr(name, ".zip") || strstr(name, ".ZIP")) {

            core_t *currCore = &cores[nCores + index];

            int length = strlen(path) + 1 + strlen(name) + 1;
            currCore->filePath = (char *)malloc(length * sizeof(char));
            sprintf(currCore->filePath, "%s%s%s", path, separator, name);

            length = strlen(name) - 4 + 1;
            currCore->name = (char *)malloc(length * sizeof(char));
            memcpy(currCore->name, name, length);
            currCore->name[length - 1] = '\0';

            currCore->hasExtraModes = 0;
            if (strstr(name, "(EM)") || strstr(name, "(em)"))
                currCore->hasExtraModes = 1;

            currCore->hasPademu = 0;
            currCore->hasBDM = 0;

            index++;
        }
    }
    return index;
}

int oplAddCores(char *path, const char *separator)
{
    int result;

    result = sbListDir(path, separator, MAX_CORES - nCores, &oplReadCoreEntry);
    nCores += result;

    if (guiCoreNames)
        free(guiCoreNames);

    guiCoreNames = (char **)malloc((nCores + 2) * sizeof(char **));

    // add default compiled in core
    guiCoreNames[0] = "<OPL>";

    int i = 0;
    for (; i < nCores; i++) {
        guiCoreNames[i + 1] = cores[i].name;
    }

    guiCoreNames[nCores + 1] = NULL;

    return result;
}

int oplFindCoreGuiID(const char *coreName)
{
    if (coreName) {
        int i = 0;
        for (; i < nCores; i++) {
            if (strcasecmp(cores[i].name, coreName) == 0)
                return i + 1; // shift for Gui id
        }
    }
    return 0;
}

void oplSetGuiCoreValue(int coreID)
{
    guiCoreID = coreID;
}

char *oplGetCoreValue(void)
{
    return guiCoreNames[guiCoreID];
}

char **oplGetCoreGuiList(void)
{
    return guiCoreNames;
}

int oplCoreHasExtraModes(int coreID)
{
    if (coreID == 0)
        return 0;

    return cores[coreID - 1].hasExtraModes;
}

int oplCoreHasPademu(void)
{
    if (guiCoreID == 0)
        return 1;

    return cores[guiCoreID - 1].hasPademu;
}

int oplCoreHasBDM(void)
{
    if (guiCoreID == 0)
        return 1;

    return cores[guiCoreID - 1].hasBDM;
}

static void oplFreeCoreList(void)
{
    int i = 0;
    for (; i < nCores; i++) {
        free(cores[i].name);
        free(cores[i].filePath);
    }

    free(guiCoreNames);
}

static void oplSetCoreDefaults(void)
{
    int i;

    coreFile[CDVDFSV_IRX].data = (void *)&cdvdfsv_irx;
    coreFile[CDVDFSV_IRX].size = size_cdvdfsv_irx;
    coreFile[EE_CORE_ELF].data = (u8 *)&eecore_elf;
    coreFile[EESYNC_IRX].size = size_eesync_irx;
    coreFile[IMGDRV_IRX].data = (void *)&imgdrv_irx;
    coreFile[IMGDRV_IRX].size = size_imgdrv_irx;
    coreFile[UDNL_IRX].data = (void *)&udnl_irx;
    coreFile[UDNL_IRX].size = size_udnl_irx;
    coreFile[HDD_CDVDMAN_IRX].data = (void *)&hdd_cdvdman_irx;
    coreFile[HDD_CDVDMAN_IRX].size = size_hdd_cdvdman_irx;
    coreFile[HDD_HDPRO_CDVDMAN_IRX].data = (void *)&hdd_hdpro_cdvdman_irx;
    coreFile[HDD_HDPRO_CDVDMAN_IRX].size = size_hdd_hdpro_cdvdman_irx;
    coreFile[HDD_MCEMU_IRX].data = (void *)&hdd_mcemu_irx;
    coreFile[HDD_MCEMU_IRX].size = size_hdd_mcemu_irx;
    coreFile[SMAP_INGAME_IRX].data = (void *)&smap_ingame_irx;
    coreFile[SMAP_INGAME_IRX].size = size_smap_ingame_irx;
    coreFile[SMBINIT_IRX].data = (void *)&smbinit_irx;
    coreFile[SMBINIT_IRX].size = size_smbinit_irx;
    coreFile[SMSTCPIP_INGAME_IRX].data = (void *)&ingame_smstcpip_irx;
    coreFile[SMSTCPIP_INGAME_IRX].size = size_ingame_smstcpip_irx;
    coreFile[SMB_CDVDMAN_IRX].data = (void *)&smb_cdvdman_irx;
    coreFile[SMB_CDVDMAN_IRX].size = size_smb_cdvdman_irx;
    coreFile[SMB_MCEMU_IRX].data = (void *)&smb_mcemu_irx;
    coreFile[SMB_MCEMU_IRX].size = size_smb_mcemu_irx;
#ifdef PADEMU
    coreFile[PADEMU_IRX].data = (void *)&pademu_irx;
    coreFile[PADEMU_IRX].size = size_pademu_irx;
#endif
    coreFile[USBD_IRX].data = (void *)&usbd_irx;
    coreFile[USBD_IRX].size = size_usbd_irx;
    coreFile[USBMASS_BD_IRX].data = (void *)&usbmass_bd_irx;
    coreFile[USBMASS_BD_IRX].size = size_usbmass_bd_irx;
    coreFile[BDM_CDVDMAN_IRX].data = (void *)&bdm_cdvdman_irx;
    coreFile[BDM_CDVDMAN_IRX].size = size_bdm_cdvdman_irx;
    coreFile[BDM_ATA_CDVDMAN_IRX].data = (void *)&bdm_ata_cdvdman_irx;
    coreFile[BDM_ATA_CDVDMAN_IRX].size = size_bdm_ata_cdvdman_irx;
    coreFile[BDM_MCEMU_IRX].data = (void *)&bdm_mcemu_irx;
    coreFile[BDM_MCEMU_IRX].size = size_bdm_mcemu_irx;
    coreFile[ILINKMAN_IRX].data = (void *)&iLinkman_irx;
    coreFile[ILINKMAN_IRX].size = size_iLinkman_irx;
    coreFile[ILINK_BD_IRX].data = (void *)&IEEE1394_bd_irx;
    coreFile[ILINK_BD_IRX].size = size_IEEE1394_bd_irx;
    coreFile[MX4SIO_BD_IRX].data = (void *)&mx4sio_bd_irx;
    coreFile[MX4SIO_BD_IRX].size = size_mx4sio_bd_irx;

    for (i = 0; i < CORE_FILE_COUNT; i++)
        coreFile[i].builtin = 1;
}

static int oplReadFileFromZipToMem(unzFile *zip, core_file_t *file, const char *filename)
{
    unz_file_info info;
    void *buffer = NULL;
    int ret, size = 0;

    ret = unzLocateFile(zip, filename, 1);
    if (ret != UNZ_OK) {
        LOG("Failed to find Core File: %s\n", filename);
        return -1;
    }

    unzGetCurrentFileInfo(zip, &info, NULL, 0, NULL, 0, NULL, 0);

    size = info.uncompressed_size;
    buffer = memalign(64, size);
    if (buffer == NULL) {
        LOG("Failed to allocate memory for Core File: %s\n", filename);
        return -1;
    }

    ret = unzOpenCurrentFile(zip);
    if (ret != UNZ_OK) {
        LOG("Failed to open Core File: %s\n", filename);
        free(buffer);
        return -1;
    }

    ret = unzReadCurrentFile(zip, buffer, size);
    if (ret != size) {
        LOG("Failed to read Core File: %s, %d (expected %d)\n", filename, ret, size);
        free(buffer);
        return -1;
    }

    ret = unzCloseCurrentFile(zip);
    if (ret == UNZ_CRCERROR) {
        LOG("Core File: %s, CRC Error\n", filename);
        free(buffer);
        return -1;
    }

    file->data = buffer;
    file->size = size;
    file->builtin = 0;

    return 0;
}

void oplGetCoreFiles(int coreID, int mode)
{
    char *fileStrings[CORE_FILE_COUNT] = {
        "cdvdfsv.irx",
        "ee_core.elf",
        "eesync.irx",
        "imgdrv.irx",
        "udnl.irx",
        "hdd_cdvdman.irx",
        "hdd_hdpro_cdvdman.irx",
        "hdd_mcemu.irx",
        "smap.irx",
        "smbinit.irx",
        "SMSTCPIP.irx",
        "smb_cdvdman.irx",
        "smb_mcemu.irx",
        "bt_pademu.irx",
        "usb_pademu.irx",
        "usbd.irx",
        "usbmass_bd.irx",
        "bdm_cdvdman.irx",
        "bdm_mcemu.irx",
        "iLinkman.irx",
        "IEEE1394_bd.irx",
        "mx4sio_bd.irx",
    };

    oplSetCoreDefaults();

    if (coreID == 0) {
        LOG("Default Core\n");
        oplFreeCoreList();
        return;
    }

    core_t *currCore = &cores[coreID - 1];
    LOG("Opening core %s\n", currCore->filePath);

    unzFile zip = unzOpen(currCore->filePath);
    if (zip != NULL) {
        // get universal modules
        int i;
        for (i = 0; i <= UDNL_IRX; i++) {
            int ret = oplReadFileFromZipToMem(zip, &coreFile[i], fileStrings[i]);
            if (ret == 0)
                LOG("Core File read: %s (size %d)\n", fileStrings[i], coreFile[i].size);
        }

        int start = 0, end = 0;
        if (mode == BDM_MODE) {
            start = USBD_IRX;
            end = MX4SIO_BD_IRX;
        } else if (mode == HDD_MODE) {
            start = HDD_CDVDMAN_IRX;
            end = HDD_MCEMU_IRX;
        } else if (mode == ETH_MODE) {
            start = SMAP_INGAME_IRX;
            end = SMB_MCEMU_IRX;
        }

        // get mode specific modules
        for (i = start; i <= end; i++) {
            int ret = oplReadFileFromZipToMem(zip, &coreFile[i], fileStrings[i]);
            if (ret == 0) {
                LOG("Core File read: %s (size %d)\n", fileStrings[i], coreFile[i].size);
                if (i == USBMASS_BD_IRX)
                    currCore->hasBDM = 1;
            }
        }

#ifdef PADEMU
        // get pademu modules
        end = mode == BDM_MODE ? PADEMU_IRX : USBD_IRX;

        for (i = PADEMU_IRX; i <= end; i++) {
            int ret = oplReadFileFromZipToMem(zip, &coreFile[i], fileStrings[i]);
            if (ret == 0) {
                LOG("Core File read: %s (size %d)\n", fileStrings[i], coreFile[i].size);
                if ((i == PADEMU_IRX))
                    currCore->hasPademu = 1;
            }
        }
#endif

        unzClose(zip);
    }

    oplFreeCoreList();
}