/*
 * File: fat.c
 * 
 * Copyright (c) 2017-2018 Sydney Erickson, John Davis
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <main.h>
#include <kprint.h>
#include <driver/fs/fat.h>
#include <kernel/memory/kheap.h>

#include <math.h>

#include <driver/storage/floppy.h>
#include <driver/storage/storage.h>

bool fat_read_entry(storage_device_t *storageDevice, fat_t *fat, fat_directory_entry_t *entry) {
    if (entry->FileSize == 0)
        return false;

    // Get the total sectors of file.
    uint32_t totalSectors = DIVIDE_ROUND_UP(entry->FileSize, (fat->Header.BPB.SectorsPerCluster * fat->Header.BPB.BytesPerSector));

    // Create space in memory.
    uint8_t *data = (uint8_t*)kheap_alloc(entry->FileSize);
    memset(data, 0, entry->FileSize);

    // Get sectors.
    uint32_t remainingSize = entry->FileSize;
    for (uint32_t sector = 0; sector < 5; sector += fat->Header.BPB.SectorsPerCluster) {
        uint32_t size = remainingSize;
        if (size > (fat->Header.BPB.SectorsPerCluster * fat->Header.BPB.BytesPerSector))
            size = (fat->Header.BPB.SectorsPerCluster * fat->Header.BPB.BytesPerSector);
        storageDevice->Read(storageDevice, (sector + fat->DataStart + entry->StartClusterLow - 2) * fat->Header.BPB.BytesPerSector, size, data+ (sector * fat->Header.BPB.BytesPerSector));
    }

    for (uint32_t i = 0; i < 400; i++)
        kprintf("%c", data[i]);
}

bool fat_init(storage_device_t *storageDevice) {
    // Get header.
    fat_header_t *fatHeader = (fat_header_t*)kheap_alloc(sizeof(fat_header_t));
    memset(fatHeader, 0, sizeof(fat_header_t));

    storageDevice->Read(storageDevice, 0, sizeof(fat_header_t), fatHeader);

    // Create FAT object.
    fat_t *fatVolume = (fat_t*)kheap_alloc(sizeof(fat_t));
    memset(fatVolume, 0, sizeof(fat_t));

    // Populate.
  /*  fatVolume->MaxRootDirectoryEntries = fatHeader->BiosParameterBlock.MaxRootDirectoryEntries;
    fatVolume->TotalSectors = (fatHeader->BiosParameterBlock.TotalSectors16 == 0) ? fatHeader->BiosParameterBlock.TotalSectors32 : fatHeader->BiosParameterBlock.TotalSectors16;
    fatVolume->TableSizeSectors = fatHeader->BiosParameterBlock.TableSize16;
    fatVolume->RootDirectorySizeSectors = ((fatVolume->MaxRootDirectoryEntries * FAT_DIRECTORY_ENTRY_SIZE) + (fatHeader->BiosParameterBlock.BytesPerSector - 1)) / fatHeader->BiosParameterBlock.BytesPerSector;
    fatVolume->StartTableSector = fatHeader->BiosParameterBlock.ReservedSectors;
    fatVolume->StartDataSector = fatVolume->StartTableSector + (fatHeader->BiosParameterBlock.TableCount * fatVolume->TableSizeSectors) + fatVolume->RootDirectorySizeSectors;
    fatVolume->BytesPerSector = fatHeader->BiosParameterBlock.BytesPerSector;
    fatVolume->StartRootDirectorySector = fatVolume->StartDataSector - fatVolume->RootDirectorySizeSectors;

    fatVolume->TotalDataSectors = fatVolume->TotalSectors - (fatHeader->BiosParameterBlock.ReservedSectors + (fatHeader->BiosParameterBlock.TableCount * fatVolume->TableSizeSectors) + fatVolume->RootDirectorySizeSectors);
    fatVolume->TotalClusters = fatVolume->TotalDataSectors / fatHeader->BiosParameterBlock.SectorsPerCluster;

    strncpy(fatVolume->VolumeName, fatHeader->ExtendedBootRecord.VolumeLabel, 11);
    fatVolume->VolumeName[11] = '\n';*/

    // Populate.
    fatVolume->Header = *fatHeader;
    fatVolume->TableStart = fatVolume->Header.BPB.ReservedSectorsCount;
    fatVolume->TableLength = fatVolume->Header.BPB.TableSize;
    fatVolume->RootDirectoryStart = fatVolume->TableStart + (fatVolume->Header.BPB.TableSize * fatVolume->Header.BPB.TableCount);
    fatVolume->RootDirectoryLength = ((fatVolume->Header.BPB.MaxRootDirectoryEntries * sizeof(fat_directory_entry_t)) + (fatVolume->Header.BPB.BytesPerSector - 1)) / fatVolume->Header.BPB.BytesPerSector;
    fatVolume->DataStart = fatVolume->RootDirectoryStart + fatVolume->RootDirectoryLength;
    fatVolume->DataLength = fatVolume->Header.BPB.TotalSectors - fatVolume->DataStart;

    char tempVolName[12];
    strncpy(tempVolName, fatVolume->Header.VolumeLabel, 11);
    tempVolName[11] = '\0';

    char fatType[9];
    strncpy(fatType, fatVolume->Header.FileSystemType, 8);
    fatType[8] = '\0';

    // Get total sectors.
    kprintf("FAT: Volume \"%s\" | %u bytes | %u sectors per cluster\n", tempVolName, fatVolume->Header.BPB.TotalSectors * fatVolume->Header.BPB.BytesPerSector, fatVolume->Header.BPB.SectorsPerCluster);
    kprintf("FAT:   FAT type: %s | Serial number: 0x%X\n", fatType, fatVolume->Header.SerialNumber);
    kprintf("FAT:   FAT start: sector %u | Length: %u sectors\n", fatVolume->TableStart, fatVolume->TableLength);
    kprintf("FAT:   Root Dir start: sector %u | Length: %u sectors\n", fatVolume->RootDirectoryStart, fatVolume->RootDirectoryLength);
    kprintf("FAT:   Data start: sector %u | Length: %u sectors\n", fatVolume->DataStart, fatVolume->DataLength);
    kprintf("FAT:   Total sectors: %u\n", fatVolume->Header.BPB.TotalSectors);

    // Get the FAT.
    uint32_t fat12ClustersLength = fatVolume->TableLength * fatVolume->Header.BPB.BytesPerSector;
    fat12_cluster_pair_t *fat12Clusters = (fat12_cluster_pair_t*)kheap_alloc(fat12ClustersLength);
    memset(fat12Clusters, 0, fat12ClustersLength);
    storageDevice->Read(storageDevice, fatVolume->TableStart * fatVolume->Header.BPB.BytesPerSector, fat12ClustersLength, fat12Clusters);
    //uint16_t test1 = fat12Clusters[0];
    //uint16_t test2 = fat12Clusters[1];

    // Get root directory.
    fat_directory_entry_t *rootDir = (fat_directory_entry_t*)kheap_alloc(sizeof(fat_directory_entry_t) * fatVolume->Header.BPB.MaxRootDirectoryEntries);
    memset(rootDir, 0, sizeof(fat_directory_entry_t) * fatVolume->Header.BPB.MaxRootDirectoryEntries);
    
    //floppy_read_sector(0, 18, rootDir, 32);
    storageDevice->Read(storageDevice, fatVolume->RootDirectoryStart * fatVolume->Header.BPB.BytesPerSector, sizeof(fat_directory_entry_t) * fatVolume->Header.BPB.MaxRootDirectoryEntries, rootDir);
    for (uint8_t i = 0; i < fatVolume->Header.BPB.MaxRootDirectoryEntries && rootDir[i].FileName[0] != 0; i++) {
        if (rootDir[i].FileName[0] == 0xE5)
            continue;

        char fileName[13];
        strncpy(fileName, rootDir[i].FileName, 8);
        fileName[8] = '.';
        strncpy(fileName+9, rootDir[i].FileName+8, 3);
        fileName[12] = '\0';
        
        kprintf("FAT:   %u: %s: %s | Size: %u bytes | Cluster %u\n", i, rootDir[i].Subdirectory ? "Folder" : "File", fileName, rootDir[i].FileSize, rootDir[i].StartClusterLow);
        fat_read_entry(storageDevice, fatVolume, rootDir+i);
    }
}
