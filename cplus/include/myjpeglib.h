typedef unsigned char BYTE;  // 1 byte
typedef unsigned long ULONG; // 4 bytes

typedef struct Image
{
    ULONG width;
    ULONG height;
    ULONG compressedSize;
    BYTE *data;
} Image;

extern Image *readJpeg(BYTE *jpegData, ULONG dataSize);
