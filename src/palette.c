#include "palette.h"

// https://en.wikipedia.org/wiki/HSL_and_HSV#To_RGB
// Fonction convertissant une valeur de teinte en RGB (Système HSV avec H la teinte variable, S et V fixés)
void hue2rgb(float hue, bitmap_rgb * rgb){
    
    hue = hue*360;      // Convertir la teinte de [0,1] à [0,360]
    float c = 1;        // Chrominance
    float x = c*(1-fabs(fmod(hue/60,2)-1)); // Valeur intermédiaire

    float r,g,b;    
    if (hue<60)
    {
        r = c;
        g = x;
        b = 0;
    }else if (hue<120)
    {
        r = x;
        g = c;
        b = 0;
    }else if (hue<180)
    {
        r = 0;
        g = c;
        b = x;
    }else if (hue<240)
    {
        r = 0;
        g = x;
        b = c;
    }else if (hue<300)
    {
        r = x;
        g = 0;
        b = c;
    }else
    {
        r = c;
        g = 0;
        b = x;
    }
    

    rgb->red = r*255;
    rgb->green = g*255;
    rgb->blue = b*255;
}