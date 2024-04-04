#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256
#define NUM_PAGES 256
#define NUM_FRAMES 256
#define FRAME_SIZE 256
#define TLB_SIZE 16
#define MEMORY_SIZE NUM_FRAMES *FRAME_SIZE
#define BACKING_STORE "BACKING_STORE.bin"


int TLB[TLB_SIZE][2]; 
int pageTable[NUM_PAGES];
signed char physicalMemory[MEMORY_SIZE];

int tlbIndex = 0; 
int pageFaultCount = 0, tlbHitCount = 0, addressCount = 0;
int nextFrame = 0; 

void initialize();
int getFrameNumber(int pageNumber);
void readFromBackingStore(int pageNumber);
void addToTLB(int pageNumber, int frameNumber);
int extractOffset(int logicalAddress);
int extractPageNumber(int logicalAddress);

int main(int argc, char *argv[])
{
    initialize();

    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // char addressFile[] = "addresses.txt";
    // FILE *addressPointer = fopen(addressFile, "r");
    char *addressFile = argv[1]; 
    FILE *addressPointer = fopen(addressFile, "r");
    int logicalAddress, physicalAddress, pageNumber, frameNumber, offset;
    signed char value;

    if (addressPointer == NULL)
    {
        fprintf(stderr, "Error opening addresses.txt\n");
        return -1;
    }

    while (fscanf(addressPointer, "%d", &logicalAddress) != EOF)
    {
        pageNumber = extractPageNumber(logicalAddress);
        offset = extractOffset(logicalAddress);
        frameNumber = getFrameNumber(pageNumber);
        physicalAddress = frameNumber * FRAME_SIZE + offset;
        value = physicalMemory[physicalAddress];

        printf("Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, physicalAddress, value);
        addressCount++;
    }

    fclose(addressPointer);

    printf("Number of Translated Addresses = %d\n", addressCount);
    printf("Page Faults = %d\n", pageFaultCount);
    printf("Page Fault Rate = %.3f\n", (float)pageFaultCount / addressCount);
    printf("TLB Hits = %d\n", tlbHitCount);
    printf("TLB Hit Rate = %.3f\n", (float)tlbHitCount / addressCount);

    return 0;
}

void initialize()
{
    memset(TLB, -1, sizeof(TLB));
    memset(pageTable, -1, sizeof(pageTable));
    memset(physicalMemory, 0, sizeof(physicalMemory));
}

void readFromBackingStore(int pageNumber)
{
    FILE *backingStorePtr = fopen(BACKING_STORE, "rb");
    if (backingStorePtr == NULL)
    {
        fprintf(stderr, "Error opening BACKING_STORE.bin\n");
        exit(EXIT_FAILURE);
    }

    fseek(backingStorePtr, pageNumber * PAGE_SIZE, SEEK_SET);

    fread(&physicalMemory[nextFrame * FRAME_SIZE], sizeof(signed char), PAGE_SIZE, backingStorePtr);

    pageTable[pageNumber] = nextFrame;

    nextFrame = (nextFrame + 1) % NUM_FRAMES;

    fclose(backingStorePtr);
}

void addToTLB(int pageNumber, int frameNumber)
{
    TLB[tlbIndex][0] = pageNumber;
    TLB[tlbIndex][1] = frameNumber;

    tlbIndex = (tlbIndex + 1) % TLB_SIZE;
}

int getFrameNumber(int pageNumber)
{
    for (int i = 0; i < TLB_SIZE; ++i)
    {
        if (TLB[i][0] == pageNumber)
        {
            tlbHitCount++;
            return TLB[i][1];
        }
    }

    if (pageTable[pageNumber] != -1)
    {
        return pageTable[pageNumber];
    }

    pageFaultCount++;
    readFromBackingStore(pageNumber);
    addToTLB(pageNumber, nextFrame - 1);

    return nextFrame - 1;
}

int extractOffset(int logicalAddress)
{
    return logicalAddress & 0x000000ff;
}

int extractPageNumber(int logicalAddress)
{
    return (logicalAddress & 0x0000ff00) >> 8;
}
