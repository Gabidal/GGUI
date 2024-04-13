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

    namespace INTERNAL{
        // When ever creating a new Buffer Capture, the previous Buffer Capture will not get notified about new lines of text, after the new Buffer Capture had been constructed.
        // These black boxes work like Stack Frames, where the data collected will be deleted when the current "Frame" capturer is destructed.
        class BUFFER_CAPTURE : public std::streambuf{
        private:
            std::streambuf* STD_COUT_RESTORATION_HANDLE = nullptr;
            std::string Current_Line = "";
            std::deque<std::string> Console_History;

            // Multiple handlers.
            std::vector<std::function<void()>> On_Change = {};

            // For speeding up.
            std::unordered_map<BUFFER_CAPTURE*, bool> Synced;

            std::string Name = "";
        public:
            // We could just search it from the global listing, but that would be slow.
            // Stuck into the constructed position.
            const bool Is_Global = false;

            BUFFER_CAPTURE(std::function<void()> on_change, bool Global = false, std::string Name = "");

            BUFFER_CAPTURE() = default;

            ~BUFFER_CAPTURE(){
                Close();
            }

            // Called from streambuf base class.
            int overflow(int c) override;

            // Safe close of std buffer hijack.
            void Close();

            std::string Read();

            void Add_On_Change_Handler(std::function<void()> on_change){                
                On_Change.push_back(on_change);
            }

            bool Sync(BUFFER_CAPTURE* Informer);

            std::string Get_Name();

            void Set_Name(std::string Name){
                this->Name = Name;
            }
        };

    }

    class FILE_STREAM{
    private:
        INTERNAL::BUFFER_CAPTURE* Buffer_Capture = nullptr;
        std::fstream Handle;
        std::vector<std::function<void()>> On_Change = {};
        std::string Previous_Content = "";
        unsigned long long Previous_Hash = 0;
    public:
        std::string Name = "";

        FILE_STREAM(std::string File_Name, std::function<void()> on_change, bool read_from_std_cout = false);

        ~FILE_STREAM(){
            if (Buffer_Capture)
                Buffer_Capture->Close();

            Handle.close();
        }

        std::string Read();
    
        std::string Fast_Read() { return Previous_Content; }

        void Changed();

        void Add_On_Change_Handler(std::function<void()> on_change){
            if (Buffer_Capture)
                Buffer_Capture->Add_On_Change_Handler(on_change);
            else
                On_Change.push_back(on_change);
        }

        bool Is_Cout_Stream(){
            return Buffer_Capture != nullptr;
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