#include "File_Streamer.h"
#include "../Renderer.h"

#include <filesystem>

namespace GGUI{
    std::unordered_map<std::string, FILE_STREAM*> File_Streamer_Handles;

    std::string FILE_STREAM::Read(){
        // Get the length of the file.
        Handle.seekg(0, std::ios::end);
        int Length = Handle.tellg();

        if (Length == 0){
            Report("Empty file: '" + this->Name + "'");
            return "";
        }
        else if (Length < 0){
            Report("Failed to read file: '" + this->Name + "' !");
            return "";
        }

        // Allocate the buffer.
        char* Buffer = new char[Length];

        // Read from top.
        Handle.seekg(0, std::ios::beg);
        Handle.read(Buffer, Length);

        // Return the data.
        std::string Data = std::string(Buffer, Length);
        delete[] Buffer;
        return Data;
    }

    void FILE_STREAM::Changed(){
        // read the new content and check if the hash has changed
        std::string New_Buffer = Read();

        if (New_Buffer.size() == 0){
            Report("Failed to check if file: '" + this->Name + "' changes!");

            return;
        }

        unsigned long long New_Hash = 0;

        for (auto& c : New_Buffer){
            New_Hash ^= c;
        }

        New_Hash /= New_Buffer.size();

        if (New_Hash != Previous_Hash){
            Previous_Hash = New_Hash;
            Previous_Content = New_Buffer;
            for (auto& on_change : On_Change){
                on_change();
            }
        }
    }

    FILE_STREAM::FILE_STREAM(std::string File_Name, std::function<void()> on_change){
        Name = File_Name;
        Handle = std::ifstream(Name.c_str(), std::ios::in);
        On_Change.push_back(on_change);
        
        if (!Handle.is_open()) {
            GGUI::Report("Could not open file: '" + File_Name + "' !");
        }

        // Check if there is already a file handle for this file name
        auto it = File_Streamer_Handles.find(File_Name);

        // If there is not, create one.
        if (it == File_Streamer_Handles.end()){
            File_Streamer_Handles[File_Name] = this;
        }
        // If there is, add the handle to the list.
        else{
            it->second->Add_On_Change_Handler(on_change);
        }
    }

    // Add an event handler that is called when the file is changed.
    void Add_File_Stream_Handle(std::string File_Name, std::function<void()> Handle){
        // Check if there is already a file handle for this file name
        auto it = File_Streamer_Handles.find(File_Name);

        // If there is not, create one.
        if (it == File_Streamer_Handles.end()){
            File_Streamer_Handles[File_Name] = new FILE_STREAM(File_Name, Handle);
        }
        // If there is, add the handle to the list.
        else{
            it->second->Add_On_Change_Handler(Handle);
        }
    }

    FILE_STREAM* Get_File_Stream_Handle(std::string File_Name){
        auto it = File_Streamer_Handles.find(File_Name);
        if (it != File_Streamer_Handles.end()){
            return it->second;
        }

        return nullptr;
    }

    std::string Get_Current_Location(){
        return std::filesystem::current_path().string();
    }
}