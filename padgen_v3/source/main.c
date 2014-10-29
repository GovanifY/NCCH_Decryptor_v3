#include "3dstypes.h"
#include "3ds.h"
#include "draw.h"
#include "libc.h"
#include "crypto.h"
#include "FS.h"

/*
#####
#ncchinfo.bin format
#
#4 bytes = 0xFFFFFFFF Meant to prevent previous versions of padgen from using these new files
#4 bytes   Number of entries
#8 bytes   Reserved
#
#entry (160 bytes in size):
#  16 bytes   Counter
#  16 bytes   KeyY
#   4 bytes   Size in MB(rounded up)
#   8 bytes   Reserved
#   4 bytes   Uses 7x crypto? (0 or 1)
# 112 bytes   Output file name in UTF-16 (format used: "/titleId.partitionName.sectionName.xorpad")
#####
*/

#define MAXENTRIES 512
struct ncch_info_entry {
	uint8_t  CTR[16];
	uint8_t  keyY[16];
	uint32_t size_mb;
	uint8_t  reserved[8];
	uint32_t uses7xCrypto;
	wchar_t  filename[56]; 
} __attribute__((packed));

struct ncch_info {
	uint32_t padding;
	uint32_t n_entries;
	uint8_t  reserved[8];
	struct ncch_info_entry entries[MAXENTRIES];
} __attribute__((packed, aligned(16)));

static const uint8_t zero_buf[16] __attribute__((aligned(16))) = {0};

void ncch_info_createpad(struct ncch_info_entry *info)
{
	#define BUFFER_ADDR ((volatile uint8_t*)0x21000000)
	#define BLOCK_SIZE  (1*1024*1024)
	
	uint32_t handle = FileOpen(info->filename);
	
	uint8_t keyslot = 0x2C;
	if (info->uses7xCrypto == 0x1) {
		keyslot = 0x25;
	}
	setup_aeskey(keyslot, AES_BIG_INPUT|AES_NORMAL_INPUT, info->keyY);
	use_aeskey(keyslot);
	
	uint8_t ctr[16] __attribute__((aligned(32)));
	memcpy(ctr, info->CTR, 16);
	
	uint32_t size_bytes = info->size_mb*1024*1024;
	uint32_t size_100 = size_bytes/100;
	uint32_t i, j;
	for (i = 0; i < size_bytes; i += BLOCK_SIZE) {
		for (j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j+= 16) {
			set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
			aes_decrypt((void*)zero_buf, (void*)BUFFER_ADDR+j, ctr, 1);
			add_ctr(ctr, 1);
		}
		draw_fillrect(SCREEN_TOP_W-33, 1, 32, 8, BLACK);
		font_draw_stringf(SCREEN_TOP_W-33, 1, CYAN, "%i%%", (i+j)/size_100);
		
		//FlushProcessDataCache(0xFFFF8001, (void*)BUFFER_ADDR, j);
		FileWriteOffset(handle, (void*)BUFFER_ADDR, j, i);
	}
}

//Beginning of freeze workaround
static uint32_t blockThreads = 0; //Requires '-fno-zero-initialized-in-bss' due to our build setup.
static uint32_t checkThread = 1;
int main()
{
	while(blockThreads){}; //Just in case a third thread tries to run out code. Shouldn't happen.

	if (checkThread) {
		checkThread = 0;
		uint32_t temphandle = FileOpen(L"/DeleteMe.bin");
		FileWrite(temphandle, &checkThread, 4);
		uint32_t x;
		for(x = 0; x < 1000; x++) {
			if (blockThreads) {
				ExitThread();
				while(1){}; //Just in case ExitThread doesn't work or something.
			}
		}
	}
	blockThreads = 1;
	//Explanation: Sometimes, using the FS functions just works.
	//Sometimes it causes another thread to start executing our code.
	//If that happens, we want to run on that second thread.
	//This only happens the first time they're used, so we just check at the beginning.
//End of freeze workaround

	ClearScreen();
	DEBUG("3DS NCCH DECRYPTOR by VOiD"); 
	
	struct ncch_info *info;
	info = (struct ncch_info *)((void *)0x20316000);

	//Replace 7.x keyX here.
	uint8_t slot0x25KeyX[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //:)
	setup_aeskeyX(0x25, slot0x25KeyX);

	DEBUG("Opening sd:/ncchinfo.bin ...");
	uint32_t handle = FileOpen(L"/ncchinfo.bin");
	DEBUG("Opened! Reading info...");
	FileRead(handle, info, 16); //Read number of entries
	DEBUG("Number of entries: %i", info->n_entries);
	
	if (!info->n_entries || info->n_entries > MAXENTRIES) {
		ERROR("Nothing to do.:/ (%i)", info->n_entries);
		return 0;
	}
	
	FileRead(handle, info, info->n_entries*sizeof(struct ncch_info_entry)+16);
	
	char str[32];
	uint32_t i;
	for(i = 0; i < info->n_entries; i++) {
		wstr_to_str(info->entries[i].filename, str);
		DEBUG("Creating pad number: %i  size (MB): %i", i+1, info->entries[i].size_mb);
		//DEBUG("\tFilename: %s", str); //Glitches out screen when filename is too long
		ncch_info_createpad(&info->entries[i]);
		DEBUG("\tDone!");
	}

	console_y = (SCREEN_TOP_H-10);
	DEBUG("Finished! You can turn off your 3DS now :P");
	return 0;
}

