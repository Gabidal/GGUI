#include "File_Streamer.h"
#include "../Renderer.h"

#include <filesystem>

#if _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <cstring>
#endif

namespace GGUI{
    std::unordered_map<std::string, FILE_STREAM*> File_Streamer_Handles;

    std::string FILE_STREAM::Read(){
        if (Buffer_Capture){
            return Buffer_Capture->Read();
        }
        else{
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

        // uuhhh.. how did we end up here?
    }

    void FILE_STREAM::Changed(){
        // read the new content and check if the hash has changed
        std::string New_Buffer = Read();

        if (New_Buffer.size() == 0){
            Report("Failed to check if file: '" + this->Name + "' changes!");

            return;
        }

        unsigned long long New_Hash = 0;

        for (int i = 0; New_Buffer[i]!='\0'; i++){
            New_Hash = New_Buffer[i] + (New_Hash << 6) + (New_Hash << 16) - New_Hash;
        }

        if (New_Hash != Previous_Hash){
            Previous_Hash = New_Hash;
            Previous_Content = New_Buffer;
            for (auto& on_change : On_Change){
                on_change();
            }
        }
    }

    FILE_STREAM::FILE_STREAM(std::string File_Name, std::function<void()> on_change, bool read_from_std_cout){
        Name = File_Name;
        
        int Type = std::ios::in;

        if (read_from_std_cout){
            Type |= std::ios::out | std::ios::app;
        }

        Handle = std::fstream(Name.c_str(), (std::ios_base::openmode)Type);

        // if the read_from_std_cout is invoked, then we need to create a file where we are going to pipe the std::cout into
        if (read_from_std_cout)
            Buffer_Capture = new INTERNAL::BUFFER_CAPTURE(on_change);
        else
            On_Change.push_back(on_change);
        
        if (!Handle.is_open()) {
            // GGUI::Report("Could not open file: '" + File_Name + "' !");
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

    std::string Pull_STDIN(){
        std::string Result = "";
        
        // check if TTY is enabled for assurance
        if (Has_Started_As_TTY()){
            Report("Cannot pull STDIN from a TTY enabled environment!");

            return Result;
        }
        
        // Now that we know the exe has been started as an non TTY object we can assume the STDIN buffer holds the previous executed output
        char buffer[256];

        while (std::cin.getline(buffer, sizeof(buffer))){
            Result += buffer;
        }

        return Result;
    }

    #ifdef _WIN32
        CMD::CMD(){
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(sa);
            sa.lpSecurityDescriptor = NULL;
            sa.bInheritHandle = TRUE;

            if (!CreatePipe(&In, &Out, &sa, 0)) {
                Report("Failed to create pipe for CMD!");
            }
        }

        std::string CMD::Run(std::string Command){
            PROCESS_INFORMATION pi;
            STARTUPINFO si;

            ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
            ZeroMemory(&si, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.hStdOutput = Out;
            si.hStdError = Out;
            si.dwFlags |= STARTF_USESTDHANDLES;

            if (!CreateProcess(NULL, (LPSTR)Command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
                Report("Failed to run command: '" + Command + "' !");
            }

            CloseHandle(Out);

            DWORD bytesRead;
            CHAR buffer[4096];
            std::string output;
            while (ReadFile(In, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                output += buffer;
            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            return output;
        }

        bool Has_Started_As_TTY(){
            return (_isatty(_fileno(stdin)));
        }
    #else

        CMD::CMD(){ 
            // Unix implementation
            pipe(File_Descriptor.FDS);
        }

        std::string CMD::Run(std::string Command){
            if (fork() == 0) {
                // Child process
                dup2(File_Descriptor.Way.Out, STDOUT_FILENO);
                system(Command.c_str());
                exit(0);
            } else {
                // Parent process
                close(File_Descriptor.Way.Out);

                char buffer[256];
                std::string output = "";

                while (read(File_Descriptor.Way.In, buffer, sizeof(buffer)) > 0) {

                    output += buffer;
                    memset(buffer, 0, sizeof(buffer));

                }
                wait(NULL);  // Wait for child process to finish
                return output;
            }
        }

        bool Has_Started_As_TTY(){
            return (isatty(STDIN_FILENO));
        }
    #endif

    namespace INTERNAL{

        BUFFER_CAPTURE::BUFFER_CAPTURE(std::function<void()> on_change, std::string Name, bool Global) : Name(Name), Is_Global(Global){
            Current_Line = "";

            // Store the previous handle
            STD_COUT_RESTORATION_HANDLE = std::cout.rdbuf();

            // Insert this as the new cout output stream.
            std::cout.rdbuf(this);

            On_Change.push_back(on_change);

            if (Global){
                Global_Buffer_Captures.push_back(this);
            }
        }

        int BUFFER_CAPTURE::overflow(int c) {
            if (c == '\n') {
                Console_History.push_back(Current_Line);
                Current_Line.clear();

                // call all the function which need to informed about the change.
                for (auto& on_change : On_Change){
                    on_change();
                }

                // TODO: this functionality is volatile to change.
                // If current BUFFER_CAPTURE is a global it will only then inform other global BUFFER_CAPTURES about the events.
                if (Is_Global){
                    Inform_All_Global_BUFFER_CAPTURES(this);
                }

            } else {
                Current_Line += static_cast<char>(c);
            }

            return STD_COUT_RESTORATION_HANDLE->sputc(c);   
        }

        void BUFFER_CAPTURE::Close(){
            // If the STD_COUT_RESTORATION_HANDLE is nullptr at this point, it means that the predecessor BUFFER_CAPTURE has been already disclosed.
            if (STD_COUT_RESTORATION_HANDLE)
                std::cout.rdbuf(STD_COUT_RESTORATION_HANDLE);
        }

        std::string BUFFER_CAPTURE::Read(){
            std::string Output = "";

            for (auto& Line : Console_History){
                Output += Line + "\n";
            }

            return Output;
        }

        // Expects that the informer, has the latest data.
        // Will also clear the un-composed line which this capture holds.
        bool BUFFER_CAPTURE::Sync(BUFFER_CAPTURE* Informer){
            // first check if the syncable BUFFER_CAPTURE is in the sync history.
            if (Synced.find(Informer) != Synced.end()){
                // this means that these two object have previously been synced, so only the latest row needs to be shared.
                Console_History.push_back(Informer->Console_History.back());

                return true;    // The synchronization was successful.
            }

            // now if these two have not previously synced, we need to check whether these two are compatible.
            int Difference = Console_History.size() - Informer->Console_History.size();

            if (Difference > 0){
                // This means that the informer has less data than this object. in which case in basic GIT systems, the synchronization is not possible.
                Report(
                    "Failed to sync buffer capture: '" + Get_Name() + "' with: '" + Informer->Get_Name() + "' !\n" + 
                    "Try obj->Merge(Informer) before trying to sync them again."
                );

                return false;
            }
            else{
                // This means that the informer has more data than this object. in which case in basic GIT systems, the synchronization is possible.
                for (int i = 0; i < Difference; i++){
                    int Actual_Offset = Console_History.size() - Difference + i - 1;

                    Console_History.push_back(Informer->Console_History[Actual_Offset]);
                }

                Synced[Informer] = true;

                return true;
            }
        }

        std::string BUFFER_CAPTURE::Get_Name(){
            if (Name.size() == 0){
                Name = "BUFFER_CAPTURE<" + std::to_string((unsigned long long)this) + ">";
            }

            return Name;
        }
    }

}
