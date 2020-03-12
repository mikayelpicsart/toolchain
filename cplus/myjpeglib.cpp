#include <stdio.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdarg.h>
#include "include/myjpeglib.h"
#include <emscripten.h>

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

BYTE *intToBytes(int paramInt)
{
    static BYTE *temp = new BYTE(sizeof(paramInt));
    for (int i = 0; i < sizeof(int); i++)
        temp[sizeof(int) - 1 - i] = (paramInt >> (i * 8));
    return temp;
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
    BYTE *bufferSizeByteArray = intToBytes((int)bufferSize);
    memcpy(dst, bufferSizeByteArray, sizeof(int));
    free(buffer);
    return dst;
}

void abort_(const char *s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

Image *read_png(unsigned char *pngData, ULONG size)
{
    Image *pImage = (Image *)malloc(sizeof(Image));
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
    FILE *fp = fmemopen((char*)pngData, size, "r");
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();
    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();
    if (setjmp(png_jmpbuf(png)))
        abort();
    png_init_io(png, fp);
    png_read_info(png, info);
    pImage->width = png_get_image_width(png, info);
    pImage->height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    if (row_pointers)
        abort();
    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * pImage->height);
    for (int y = 0; y < pImage->height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
    }
    png_read_image(png, row_pointers);
    pImage->data = (BYTE *)malloc(pImage->width * pImage->height * 4);
    for (int y = 0; y < pImage->height; y++)
    {
        memcpy(pImage->data + (y * pImage->width * 4), row_pointers[y], pImage->width * 4);
    }
    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    return pImage;
}