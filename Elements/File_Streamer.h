#ifndef _FILE_STREAMER_H_
#define _FILE_STREAMER_H_

#include "Element.h"

#include <fstream>
#include <functional>
#include <unordered_map>
#include <stdio.h>
#include <deque>

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

    // Custom stream buffer that adds lines to a history
    class OUTBOX_BUFFER : public std::streambuf {
    private:
        std::streambuf* Rendered_Stream_Buffer = nullptr;
        std::string Current_Line = "";
        std::deque<std::string> Console_History;
        
        // When 0, starts at bottom, the higher it goes the more it: 'Y - Scroll_Index'
        int Scroll_Index = 0;
    public:
        OUTBOX_BUFFER(std::streambuf* oldBuffer);
        
        OUTBOX_BUFFER();

        ~OUTBOX_BUFFER(){
            Close();
        }

        // Called from streambuf base class.
        int overflow(int c) override;

        // Safe close of std buffer hijack.
        void Close();

        void Scroll_Up(int Speed = 1);
        void Scroll_Down(int Speed = 1);

        GGUI::Coordinates Get_History_Dimensions();
    };


    #if _WIN32
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