#include "save_bmp.h"


/* typedef struct {
  uint16_t  type;             // Magic identifier: 0x4d42
  uint32_t  size;             // File size in bytes
  uint32_t  reserved;         // Not used
  uint32_t  offset;           // Offset to image data in bytes from beginning of file (54 bytes)
} bitmap_file_header;

typedef struct {
  uint32_t  info_header_size;  // DIB Header size in bytes (40 bytes)
  int32_t  width_px;         // Width of the image
  int32_t  height_px;        // Height of image
  uint16_t  num_planes;       // Number of color planes
  uint16_t  bits_per_pixel;   // Bits per pixel
  uint32_t  compression;      // Compression type
  uint32_t  image_size_bytes; // Image size in bytes
  int32_t   x_resolution_ppm; // Pixels per meter
  int32_t   y_resolution_ppm; // Pixels per meter
  uint32_t  num_colors;       // Number of colors
  uint32_t  important_colors; // Important colors
} bitmap_info_header; */

void write_bmp(char *filename, bitmap_file_header * output_file_header, bitmap_info_header * output_info_header, bitmap_rgb *output_pixels)
{
     FILE *output_file = fopen(filename, "wb+");
    if (!output_file)
    {
        printf("Impossible de créer le fichier '%s'.\n", filename);
        exit(0);
    } 

    fwrite(&output_file_header->type, sizeof(output_file_header->type), 1, output_file);
    fwrite(&output_file_header->size, sizeof(output_file_header->size), 1, output_file);
    fwrite(&output_file_header->reserved, sizeof(output_file_header->reserved), 1, output_file);
    fwrite(&output_file_header->offset, sizeof(output_file_header->offset), 1, output_file);

    fwrite(&output_info_header->size, sizeof(output_info_header->size), 1, output_file);
    fwrite(&output_info_header->width_px, sizeof(output_info_header->width_px), 1, output_file);
    fwrite(&output_info_header->height_px, sizeof(output_info_header->height_px), 1, output_file);
    fwrite(&output_info_header->planes, sizeof(output_info_header->planes), 1, output_file);
    fwrite(&output_info_header->bits_per_pixel, sizeof(output_info_header->bits_per_pixel), 1, output_file);
    fwrite(&output_info_header->compression, sizeof(output_info_header->compression), 1, output_file);
    fwrite(&output_info_header->image_size_bytes, sizeof(output_info_header->image_size_bytes), 1, output_file);
    fwrite(&output_info_header->x_resolution_ppm, sizeof(output_info_header->x_resolution_ppm), 1, output_file);
    fwrite(&output_info_header->y_resolution_ppm, sizeof(output_info_header->y_resolution_ppm), 1, output_file);
    fwrite(&output_info_header->num_colors, sizeof(output_info_header->num_colors), 1, output_file);
    fwrite(&output_info_header->important_colors, sizeof(output_info_header->important_colors), 1, output_file);

    // 100*24/8 = 300
    int unpaddedRowSize = output_info_header->width_px*output_info_header->bits_per_pixel/8;
    int paddedRowSize = (unpaddedRowSize + 4) & (~4);
    paddedRowSize = output_info_header->image_size_bytes/output_info_header->height_px;

    for ( int i = output_info_header->height_px-1  ;  i >= 0 ; i--){
            int pixelOffset = (((output_info_header->height_px  - i) - 1)*unpaddedRowSize)/3;
            fwrite(&output_pixels[pixelOffset], 1, paddedRowSize, output_file);
    } 
    
}