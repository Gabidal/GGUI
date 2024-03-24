#ifndef _FILE_STREAMER_H_
#define _FILE_STREAMER_H_

#include "Element.h"

#include <string>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <stdio.h>

namespace GGUI{

    /*
        Utilities to manage file streams.
    */

    class FILE_STREAM;

    extern std::unordered_map<std::string, FILE_STREAM*> File_Streamer_Handles;

    extern void Add_File_Stream_Handle(std::string File_Handle, std::function<void()> Handle);

    extern FILE_STREAM* Get_File_Stream_Handle(std::string File_Name);

    extern std::string Get_Current_Location();

    class FILE_STREAM{
    private:
        std::ifstream Handle;
        std::vector<std::function<void()>> On_Change = {};
        std::string Previous_Content = "";
        unsigned long long Previous_Hash = 0;
    public:
        std::string Name = "";

        FILE_STREAM(std::string File_Name, std::function<void()> on_change);

        ~FILE_STREAM(){
            Handle.close();
        }

        std::string Read();
    
        std::string Fast_Read() { return Previous_Content; }

        void Changed();

        void Add_On_Change_Handler(std::function<void()> on_change){
            On_Change.push_back(on_change);
        }
    };

    class FILE_POSITION{
    public:
        std::string File_Name = "";     // Originated.
        unsigned int Line_Number = 0;   // Y
        unsigned int Character = 0;     // X

        FILE_POSITION(std::string File_Name, unsigned int Line_Number, unsigned int Character){
            this->File_Name = File_Name;
            this->Line_Number = Line_Number;
            this->Character = Character;
        }

        FILE_POSITION() = default;

        std::string To_String(){
            return File_Name + ":" + std::to_string(Line_Number) + ":" + std::to_string(Character);
        }
    };

    #ifdef _WIN32
        class CMD{
        private:
            void* In;
            void* Out;
        public:
            CMD();
            ~CMD() = default;

            std::string Run(std::string command);
        };
    #else
        class CMD{  // Unix implementation:
        private:
            union FileDescriptor {
                struct {
                    int In;
                    int Out;
                };
                int FDS[2];
            } File_Descriptor;
        public:

            CMD();
            ~CMD() = default;

            std::string Run(std::string command);
        };
    #endif
}

#endif