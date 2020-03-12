typedef unsigned char BYTE;  // 1 byte
typedef unsigned long ULONG; // 4 bytes

typedef struct Image
{
    ULONG width;
    ULONG height;
    ULONG compressedSize;
    BYTE *data;
    Image()
    {
        this->width = 0;
        this->height = 0;
        this->compressedSize = 0;
        this->data = NULL;
    }
    Image(int width, int height, int compressedSize, BYTE *data)
    {
        this->width = width;
        this->height = height;
        this->compressedSize = compressedSize;
        this->data = data;
    }

} Image;

extern Image *readJpeg(BYTE *jpegData, ULONG dataSize);
extern BYTE *writeJpeg(BYTE *bmp, ULONG width, ULONG height, ULONG quality);
extern Image *read_png(unsigned char *pngData, ULONG size);
BYTE* intToBytes(int paramInt);