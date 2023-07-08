#include <stdio.h>
#include <windows.h>

// https://github.com/nothings/stb/
#define STB_IMAGE_IMPLEMENTATION
#include "./headers/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "./headers/stb_image_resize.h"

int main(int Arguments_Count, char** Arguments_Value)
{
    // Data: CONSTANTS
    const int OUT_CHANNELS_NUM    = 3;   // aka, Components, or Number of Components
    const int OUT_WIDTH_MAX       = 1080; // <-- Configure it
    const int OUT_WIDTH_MIN       = 16;  // <-- Configure it
    // https://en.wikipedia.org/wiki/ANSI_escape_code#24-bit
    const char TERMINAL_PIXEL_STRING_FOREGROUND_COLOR_HEAD[] = "\033[38;2;";
    const char TERMINAL_PIXEL_STRING_BACKGROUND_COLOR_HEAD[] = ";48;2;";
    const char TERMINAL_PIXEL_STRING_TAIL[] = "\033[m";

    CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
    int TerminalHorizontalPixelCount;
    int TerminalVerticalPixelCount;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleScreenBufferInfo);
    TerminalHorizontalPixelCount = ((ConsoleScreenBufferInfo.srWindow.Right - ConsoleScreenBufferInfo.srWindow.Left + 1)/2) * 2;
    TerminalVerticalPixelCount = (ConsoleScreenBufferInfo.srWindow.Bottom - ConsoleScreenBufferInfo.srWindow.Top + 1) * 2;

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
        Out_Width = TerminalHorizontalPixelCount;
    }

    if(Out_Width > OUT_WIDTH_MAX){
        Out_Width = OUT_WIDTH_MAX;
    }
    if(Out_Width < OUT_WIDTH_MIN){
        Out_Width = OUT_WIDTH_MIN;
    }

    if(Out_Width > TerminalHorizontalPixelCount){
        Out_Width = TerminalHorizontalPixelCount;
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

    if(Out_Height > TerminalVerticalPixelCount){
        Out_Height = TerminalVerticalPixelCount;
        Out_Height_f = Out_Height;
        Out_Width_f = ( Out_Height_f / Image_Height_f ) * Image_Width_f;
        Out_Width = Out_Width_f;
    }

    // Using U+2580 (Upper Half Block), and hence upscaling he size
    // https://www.compart.com/en/unicode/block/U+2580    

    unsigned char* Resized_Image_Data = malloc( Out_Width * Out_Height * OUT_CHANNELS_NUM );
    stbir_resize_uint8(
        Image_Data,   Image_Width, Image_Height, 0,
        Resized_Image_Data, Out_Width,   Out_Height,   0, OUT_CHANNELS_NUM
    );

    // Text: Output Image
    int Pixel_Num_Max = Out_Width * Out_Height;

    int x_Tracker = 0;

    for(
        int Pixel_Num_Current = 0; 
        Pixel_Num_Current<Pixel_Num_Max;
        Pixel_Num_Current++
        )
    {
        int Upper_R_Index = Pixel_Num_Current * 3;
        int Upper_G_Index = Pixel_Num_Current * 3 + 1;
        int Upper_B_Index = Pixel_Num_Current * 3 + 2;
        int Upper_R = Resized_Image_Data[Upper_R_Index];
        int Upper_G = Resized_Image_Data[Upper_G_Index];
        int Upper_B = Resized_Image_Data[Upper_B_Index];

        int Lower_R_Index = (Pixel_Num_Current + Out_Width) * 3;
        int Lower_G_Index = (Pixel_Num_Current + Out_Width) * 3 + 1;
        int Lower_B_Index = (Pixel_Num_Current + Out_Width) * 3 + 2;
        int Lower_R = Resized_Image_Data[Lower_R_Index];
        int Lower_G = Resized_Image_Data[Lower_G_Index];
        int Lower_B = Resized_Image_Data[Lower_B_Index];
        
        printf("%s%d;%d;%d%s%d;%d;%dm▀%s", 
            TERMINAL_PIXEL_STRING_FOREGROUND_COLOR_HEAD,
            Upper_R, Upper_G, Upper_B,
            TERMINAL_PIXEL_STRING_BACKGROUND_COLOR_HEAD,
            Lower_R, Lower_G, Lower_B,
            TERMINAL_PIXEL_STRING_TAIL
        );
        if(Pixel_Num_Current != 0 && (Pixel_Num_Current+1)%Out_Width == 0){
            printf("\n");
        }
        x_Tracker += 1;
        if(x_Tracker == Out_Width){
          Pixel_Num_Current += Out_Width;
          x_Tracker = 0;
        }
    }

    // Text: Memory Management
    stbi_image_free(Image_Data);
    free(Resized_Image_Data);

    return 0;
}
