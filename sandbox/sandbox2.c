#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

//  struct for rgb
typedef struct rgb_t{
    float red;
    float green;
    float blue;
} rgb_t;

int main(int argc, char const *argv[]);
void hue2rgb(float hue, rgb_t * rgb);
void print_rgb(rgb_t * rgb);

int main(int argc, char const *argv[])
{
    float hue = atof(argv[1]);
    printf("hue = %f\n",hue);
    rgb_t * rgb = malloc(sizeof(rgb_t));
    hue2rgb(hue,rgb);
    print_rgb(rgb);
    
    return 0;
}

// function to convert hue to rgb, with hue in [0,1]
void hue2rgb(float hue, rgb_t * rgb){
    // convert hue from [0,1] to [0,360]
    hue = hue*360;
    float c = 1;
    float h = hue/60;
    float x = c*(1-fabs(fmod(h,2)-1));
    printf("fmod(h,2) = %f\n",fmod(h,2));
    float m = 0.5 - c/2;
    printf("c = %f, h = %f, x = %f, m = %f\n",c,h,x,m);
    if (hue<60)
    {
        rgb->red = c;
        rgb->green = x;
        rgb->blue = 0;
    }else if (hue<120)
    {
        rgb->red = x;
        rgb->green = c;
        rgb->blue = 0;
    }else if (hue<180)
    {
        rgb->red = 0;
        rgb->green = c;
        rgb->blue = x;
    }else if (hue<240)
    {
        rgb->red = 0;
        rgb->green = x;
        rgb->blue = c;
    }else if (hue<300)
    {
        rgb->red = x;
        rgb->green = 0;
        rgb->blue = c;
    }else if (hue<360)
    {
        rgb->red = c;
        rgb->green = 0;
        rgb->blue = x;
    }else{
        rgb->red = 0;
        rgb->green = 0;
        rgb->blue = 0;
    }
    
    rgb->red = rgb->red;
    rgb->green = rgb->green;
    rgb->blue = rgb->blue;

}

void print_rgb(rgb_t * rgb){
    printf("rgb(%f,%f,%f)\n",rgb->red,rgb->green,rgb->blue);
}

// command to compile this file
