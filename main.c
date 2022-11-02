#include <stdio.h>

// https://github.com/nothings/stb/
#define STB_IMAGE_IMPLEMENTATION
#include "./headers/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "./headers/stb_image_resize.h"

int main(int Arguments_Count, char** Arguments_Value)
{
    // Data: CONSTANTS
    const int OUT_CHANNELS_NUM    = 3;  // aka, Components, or Number of Components
    const int OUT_WIDTH_MAX       = 90; // <-- Configure it
    const int OUT_WIDTH_DEFAULT   = 50; // <-- Configure it
    const int OUT_WIDTH_MIN       = 10; // <-- Configure it
    // https://en.wikipedia.org/wiki/ANSI_escape_code#24-bit
    const char TERMINAL_PIXEL_STRING_HEAD[] = "\033[48;2;"; // Background Color Mode
    const char TERMINAL_PIXEL_STRING_TAIL[] = "\033[m";

    const char HELP_DOC[] = 
"ShowInputImage (sii)\n\n"
"0. Usage\n"
"sii input_image_path [out_width]";

    // Data: Environemnt Variables

    // Data: User Input
    // Data: User Input: Input_Image_Path
    char Input_Image_Path[255];
    if(Arguments_Count >= 2){
        int Input_Image_Path_Length = strlen(Arguments_Value[1]);
        if(Input_Image_Path_Length >= 255){
            printf("Error: Input_Image_Path longer than 180 bytes");
            exit(1);
        }else{
            strcpy(Input_Image_Path, Arguments_Value[1]);
        }
    }else{
        printf("%s", HELP_DOC);
        exit(1);
    }

    // Data: User Input: Out_Width
    int Out_Width = atoi(Arguments_Value[2]);
    if(Out_Width == 0){
        Out_Width = OUT_WIDTH_DEFAULT;
    }else{
        if(Out_Width > OUT_WIDTH_MAX){
            Out_Width = OUT_WIDTH_MAX;
        }
        if(Out_Width < OUT_WIDTH_MIN){
            Out_Width = OUT_WIDTH_MIN;
        }
    }

    // Data: Generated

    // Text: Get Image
    int Image_Width, Image_Height, Channel_Num;
    unsigned char* Image_Data = stbi_load(
        Input_Image_Path, 
        &Image_Width, &Image_Height, &Channel_Num,
        OUT_CHANNELS_NUM
    );
    if(Image_Data == NULL){
        printf("Error: Can't get Image_Data via stbi_load");
        exit(1);
    }

    // Text: Resize Image
    double Image_Height_f = Image_Height;
    double Image_Width_f  = Image_Width;
    double Out_Width_f    = Out_Width;
    double Out_Height_f = ( Image_Height_f / Image_Width_f ) * Out_Width_f;
    int Out_Height = Out_Height_f;
    unsigned char* Resized_Image_Data = malloc( Out_Width * Out_Height * OUT_CHANNELS_NUM );
    stbir_resize_uint8(
        Image_Data,   Image_Width, Image_Height, 0,
        Resized_Image_Data, Out_Width,   Out_Height,   0, OUT_CHANNELS_NUM
    );

    // Text: Output Image
    int Pixel_Num_Max = Out_Width * Out_Height;

    for(
        int Pixel_Num_Current = 0; 
        Pixel_Num_Current<Pixel_Num_Max;
        Pixel_Num_Current++
        )
    {
        int R_Index = Pixel_Num_Current * 3;
        int G_Index = Pixel_Num_Current * 3 + 1;
        int B_Index = Pixel_Num_Current * 3 + 2;
        int R = Resized_Image_Data[R_Index];
        int G = Resized_Image_Data[G_Index];
        int B = Resized_Image_Data[B_Index];
        printf("%s%d;%d;%dm  %s", 
            TERMINAL_PIXEL_STRING_HEAD,
            R, G, B,
            TERMINAL_PIXEL_STRING_TAIL
        );
        if(Pixel_Num_Current != 0 && (Pixel_Num_Current+1)%Out_Width == 0){
            printf("\n");
        }
    }

    // Text: Memory Management
    stbi_image_free(Image_Data);
    free(Resized_Image_Data);

    return 0;
}