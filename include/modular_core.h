enum CORE_FILES {
    CDVDFSV_IRX = 0,
    EE_CORE_ELF,
    EESYNC_IRX,
    IMGDRV_IRX,
    UDNL_IRX,
    HDD_CDVDMAN_IRX,
    HDD_HDPRO_CDVDMAN_IRX,
    HDD_MCEMU_IRX,
    SMAP_INGAME_IRX,
    SMBINIT_IRX,
    SMSTCPIP_INGAME_IRX,
    SMB_CDVDMAN_IRX,
    SMB_MCEMU_IRX,
    PADEMU_IRX,
    USBD_IRX,
    USBMASS_BD_IRX,
    USBMASS_BD_SINGLE_IRX,
    BDM_CDVDMAN_IRX,
    BDM_ATA_CDVDMAN_IRX,
    BDM_MCEMU_IRX,
    ILINKMAN_IRX,
    ILINK_BD_IRX,
    MX4SIO_BD_IRX,

    CORE_FILE_COUNT
};

typedef struct
{
    void *data;
    int size;
    int builtin;
} core_file_t;

extern core_file_t coreFile[CORE_FILE_COUNT];

int oplFindCoreGuiID(const char *coreName);
char *oplGetCoreValue(void);
char **oplGetCoreGuiList(void);
void oplSetGuiCoreValue(int coreID);

int oplCoreHasExtraModes(int coreID);
int oplCoreHasPademu(void);
int oplCoreHasBDM(void);

int oplAddCores(char *path, const char *separator);
void oplGetCoreFiles(int coreID, int mode);