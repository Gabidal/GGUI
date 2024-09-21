#ifndef _SUPER_STRING_H_
#define _SUPER_STRING_H_

#include <string>
#include <cstring>
#include <vector>

namespace GGUI{
    // And lighter-weight version of the UTF class. [Probably after making the RGBA use unsigned char instead of float, and thus making the overall size into 32 bits, replace this class with UTF.]
    class Compact_String{
    public:
        union{
            const char* Unicode_Data;
            char Ascii_Data;
        } Data = { nullptr };

        unsigned int Size = 0;

        // Only for resize!!!
        Compact_String() = default;

        Compact_String(const char* data){
            Size = std::strlen(data); 

            if (Size > 1)
                Data.Unicode_Data = data;
            else
                Data.Ascii_Data = data[0];
        }

        Compact_String(char data){
            Data.Ascii_Data = data;
            Size = 1;
        }

        Compact_String(const char* data, unsigned int size, bool Force_Unicode = false){
            Size = size;

            if (Size > 1 || Force_Unicode)
                Data.Unicode_Data = data;
            else
                Data.Ascii_Data = data[0];
        }

        char operator[](unsigned int index){
            if (Size > 1)
                return Data.Unicode_Data[index];
            else
                return Data.Ascii_Data;
        }
    };

    // Instead of reconstructing new strings every time, this class stores the components, and then only one time constructs the final string representation.
    class Super_String{
    public:
        std::vector<Compact_String> Data;
        unsigned int Current_Index = 0;

        Super_String(unsigned int Final_Size = 1){
            Data.resize(Final_Size);
            Current_Index = 0;
        }

        void Clear(){
            Current_Index = 0;
        }

        void Add(const char* data, int size){
            Data[Current_Index++] = Compact_String(data, size);
        }

        void Add(char data){
            Data[Current_Index++] = Compact_String(data);
        }

        void Add(const std::string& data){
            Data[Current_Index++] = Compact_String(data.data(), data.size());
        }

        void Add(Super_String* other, bool Expected = false){
            // enlarge the reservation
            if (!Expected)
                Data.resize(Current_Index + other->Current_Index);

            for (unsigned int i = 0; i < other->Current_Index; i++){

                Data[Current_Index++] = other->Data[i];
            }
        }
        
        void Add(Super_String& other, bool Expected = false){
            // enlarge the reservation
            if (!Expected)
                Data.resize(Current_Index + other.Current_Index);

            for (unsigned int i = 0; i < other.Current_Index; i++){

                Data[Current_Index++] = other.Data[i];
            }
        }

        void Add(const Compact_String& other){
            Data[Current_Index++] = other;
        }

        std::string To_String(){
            unsigned int Overall_Size = 0;

            for(unsigned int i = 0; i < Current_Index; i++){
                Overall_Size += Data[i].Size;
            }

            std::string result;
            result.resize(Overall_Size);

            int Current_UTF_Insert_Index = 0;
            for(unsigned int i = 0; i < Current_Index; i++){
                Compact_String data = Data[i];

                // Size of ones are always already loaded from memory into a char.
                if (data.Size > 1){
                    result.replace(Current_UTF_Insert_Index, data.Size, data.Data.Unicode_Data);

                    Current_UTF_Insert_Index += data.Size;
                }
                else{
                    result[Current_UTF_Insert_Index++] = data.Data.Ascii_Data;
                }
            }

            return result;
        }
    };

}

#endif