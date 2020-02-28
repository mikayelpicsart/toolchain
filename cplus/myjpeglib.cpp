#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <setjmp.h>
#include "include/myjpeglib.h"

struct my_error_mgr
{
    struct jpeg_error_mgr pub; /* "public" fields */
    jmp_buf setjmp_buffer;     /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

static void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

Image *readJpeg(BYTE *jpegData, ULONG dataSize)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY buffer;
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        return 0;
    }
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (BYTE *)jpegData, dataSize);
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);
    ULONG width = cinfo.output_width;
    ULONG height = cinfo.output_height;
    int pixelSize = cinfo.output_components;
    Image *pImage = (Image *)malloc(sizeof(Image));
    pImage->width = width;
    pImage->height = height;
    pImage->compressedSize = dataSize;
    pImage->data = (BYTE *)malloc(width * height * pixelSize);
    row_stride = cinfo.output_width * cinfo.output_components;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        BYTE *buffer_array[1];
        buffer_array[0] = pImage->data + (cinfo.output_scanline) * row_stride;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }
    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return pImage;
}

BYTE *writeJpeg(BYTE *bmp, ULONG width, ULONG height, ULONG quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    ULONG bufferSize = 0;
    BYTE *buffer = NULL;
    jpeg_mem_dest(&cinfo, &buffer, &bufferSize);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &bmp[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    // Image *pImage = (Image *)malloc(sizeof(Image));
    // pImage->width = width;
    // pImage->height = height;
    // pImage->compressedSize = bufferSize;
    // pImage->data = (BYTE *)malloc(bufferSize);
    // memcpy(pImage->data, buffer, bufferSize);
    // free(buffer);

    BYTE *dst = (BYTE *)calloc(bufferSize + sizeof(int), sizeof(BYTE));
    // ULONG *infos = (ULONG *)dst;
    // infos[0] = width;
    // infos[1] = width;
    // infos[2] = bufferSize;
    memcpy(&dst[sizeof(int)], buffer, bufferSize);
    unsigned char intvalue[sizeof(int)];
    intToBytes((int) bufferSize, intvalue);
    memcpy(dst, intvalue, sizeof(int));
    free(buffer);
    return dst;
}