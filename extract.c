#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#pragma pack(1)

struct boot_sector {
    unsigned char jmp[3];
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char num_fats;
    unsigned short root_entries;
    unsigned short total_sectors;
    unsigned char media;
    unsigned short sectors_per_fat;
    unsigned short sectors_per_track;
    unsigned short num_heads;
    unsigned int hidden_sectors;
    unsigned int total_sectors_big;
    unsigned short physical_drive_num;
    unsigned char current_head;
    unsigned char boot_sector_signature;
    unsigned int volume_id;
    unsigned char volume_label[11];
    unsigned char fs_type[8];
    unsigned char boot_code[448];
    unsigned short boot_sector_signature2;
    char padding[2];
};

struct RootEntry {
    char DIR_Name[11];
    unsigned char DIR_Attr;
    unsigned char reserve[10];
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClus;
    unsigned int DIR_FileSize;
};

struct Data_cluster {
    unsigned char data[512 * 4];
};

int open_image(const char *filename);
void *map_image(int fd, size_t size);
void unmap_image(void *addr, size_t size);
unsigned int get_fat_entry(unsigned char *fat, unsigned int cluster_num);
void extract_pdf(struct boot_sector *bs, unsigned char *fat_entry, struct RootEntry *root_dir);

int main() {
    int fd = open_image("./fat12.img");
    if (fd < 0) {
        return 1;
    }

    char *addr = map_image(fd, 512 * 4096);
    if (addr == MAP_FAILED) {
        close(fd);
        return 1;
    }

    struct boot_sector *bs = (struct boot_sector *)addr;
    struct RootEntry *root_dir = (struct RootEntry *)((char *)bs + bs->bytes_per_sector * (bs->reserved_sectors + bs->num_fats * bs->sectors_per_fat));

    unsigned char *fat_entry = ((unsigned char *)bs + bs->bytes_per_sector * bs->reserved_sectors);
    extract_pdf(bs, fat_entry, root_dir);

    unmap_image(addr, 512 * 4096);
    close(fd);
    return 0;
}

int open_image(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
    }
    return fd;
}

void *map_image(int fd, size_t size) {
    char *addr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed");
    }
    return addr;
}

void unmap_image(void *addr, size_t size) {
    if (munmap(addr, size) < 0) {
        perror("munmap failed");
    }
}

unsigned int get_fat_entry(unsigned char *fat, unsigned int cluster_num) {
    unsigned int offset = (cluster_num * 3) / 2;
    unsigned int entry;

    if (cluster_num & 0x1) { // 奇数簇号
        entry = ((fat[offset] & 0xF0) >> 4) | (fat[offset + 1] << 4);
    } else { // 偶数簇号
        entry = fat[offset] | ((fat[offset + 1] & 0x0F) << 8);
    }

    return entry & 0xFFF; // 确保只返回 12 位
}

void extract_pdf(struct boot_sector *bs, unsigned char *fat_entry, struct RootEntry *root_dir) {
    struct RootEntry *root_entry;
    FILE *fp = fopen("./FAT_EXTRACT.pdf", "wb");
    if (!fp) {
        perror("Failed to create output file");
        return;
    }

    for (int i = 0; i < bs->root_entries; i++) {
        root_entry = (struct RootEntry *)(root_dir + i);
        if (strncmp(root_entry->DIR_Name, "FAT     PDF", 11) == 0) {
            printf("Serial number: %d\n", i);
            printf("%s\n", root_entry->DIR_Name);
            printf("size: %d\n", root_entry->DIR_FileSize);
            printf("DIR_WrtTime: %d\n", root_entry->DIR_WrtTime);
            printf("DIR_FstClus: %d\n", root_entry->DIR_FstClus);
            break;
        }
    }

    unsigned int cluster_num = root_entry->DIR_FstClus;
    struct Data_cluster *data_cluster_entry = (struct Data_cluster *)((char *)bs + bs->bytes_per_sector * (bs->reserved_sectors + bs->num_fats * bs->sectors_per_fat + bs->root_entries * 32 / bs->bytes_per_sector));

    while (1) {
        fwrite((data_cluster_entry + cluster_num - 2)->data, 512 * 4, 1, fp);

        // 获取下一个簇
        cluster_num = get_fat_entry(fat_entry, cluster_num);
        if (cluster_num == 0xFFF || cluster_num == 0xff8) {
            break;
        }
    }

    fclose(fp);
}
