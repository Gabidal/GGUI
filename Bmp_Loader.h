#ifndef _BMP_LOADER_
#define _BMP_LOADER_

#include <string>
#include <vector>
#include <map>
#include <fstream>

class RGB;

class BMP_Header{
public:
    unsigned short Signature;
    unsigned int File_Size;
    unsigned short Reserved_1;
    unsigned short Reserved_2;
    unsigned int Offset;
    unsigned int Header_Size;
    unsigned int Width;
    unsigned int Height;
    unsigned short Planes;
    unsigned short Bits_Per_Pixel;
    unsigned int Compression;
    unsigned int Image_Size;
    unsigned int X_Pixels_Per_Meter;
    unsigned int Y_Pixels_Per_Meter;
    unsigned int Colors_Used;
    unsigned int Important_Colors;
    std::vector<RGB> Color_Table;
    std::vector<unsigned char> Image_Data;
};

class Bmp_Loader{
private:
    std::vector<RGB> Data;
    int Letter_Width = 32;
    int Letter_Height = 32;
    int Image_Width = 0;
    int Image_Height = 0;
    int Row_Padding = 0;
public:

    Bmp_Loader(){}

    // Takes a bitmap image.
    Bmp_Loader(std::string Path){

        std::ifstream File(Path, std::ios::binary);

        if(File.is_open()){

            BMP_Header Header;

            // Only need to read the Width, Height and the offset to the data
            File.read((char*)&Header, sizeof(Header));

            Image_Height = Header.Height;
            Image_Width = Header.Width;

            // now we can start to copy the data.
            Data.resize(Header.Width * Header.Height);

            // calculate the padding
            Row_Padding = Image_Width * 3 % 4;

            // We need to read the data from the end of the file.
            File.seekg(Header.Offset, std::ios::beg);

            // Read the data
            for(int i = 0; i < Image_Height; i++){
                for(int j = 0; j < Image_Width; j++){
                    File.read((char*)&Data[(Image_Height - i - 1) * Image_Width + j], 3);
                }
                File.seekg(Row_Padding, std::ios::cur);
            }

            // flip the data
            std::vector<RGB> Temp = Data;

            for(int i = 0; i < Image_Height; i++){
                for(int j = 0; j < Image_Width; j++){
                    Data[i * Image_Width + j] = Temp[(Image_Height - i - 1) * Image_Width + j];
                }
            }
        }

    }

    std::vector<RGB> Get(unsigned char letter){

        std::vector<RGB> Letter_Data(Letter_Width * Letter_Height);

        int x = (letter % 16) * Letter_Width;
        int y = (letter / 16) * Letter_Height;

        for(int i = 0; i < Letter_Height; i++){
            for(int j = 0; j < Letter_Width; j++){
                Letter_Data[i * Letter_Width + j] = Data[(i + y) * Image_Width + j + x];
            }
        }

        return Letter_Data;

    }

};


#endif