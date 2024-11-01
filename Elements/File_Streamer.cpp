#include "File_Streamer.h"
#include "../Core/Renderer.h"

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

    /**
     * @brief Read the content of the file.
     * @return The content of the file as a string.
     * 
     * This function will read the content of the file and return it as a string.
     * If the file is empty, this function will return an empty string.
     * If the file does not exist, or any other error occurs, this function will return an empty string and print an error message to the console.
     * If read_from_std_cout is true, this function will read the content from the buffer capture instead of the file.
     */
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

    /**
     * @brief Checks if the file has changed and notifies the event handlers if so.
     *
     * This function is called by the FILE_STREAM class when it wants to check if the file has changed.
     * It reads the new content from the file, calculates the hash of the new content, and compares it with the previous hash.
     * If the hash has changed, it notifies all the event handlers by calling them.
     */
    void FILE_STREAM::Changed(){
        // read the new content and check if the hash has changed
        std::string New_Buffer = Read();

        if (New_Buffer.size() == 0){
            Report("Failed to check if file: '" + this->Name + "' changes!");

            return;
        }

        unsigned long long New_Hash = 0;

        // calculate the hash of the new buffer
        for (int i = 0; New_Buffer[i]!='\0'; i++){
            New_Hash = New_Buffer[i] + (New_Hash << 6) + (New_Hash << 16) - New_Hash;
        }

        if (New_Hash != Previous_Hash){
            // If the hash has changed, update the previous hash and content, and notify all the event handlers.
            Previous_Hash = New_Hash;
            Previous_Content = New_Buffer;
            for (auto& on_change : On_Change){
                on_change();
            }
        }
    }


    /**
     * @brief Constructor of the FILE_STREAM class.
     * @param File_Name The name of the file to open.
     * @param on_change The event handler to be called when the file is changed.
     * @param read_from_std_cout If true, the file stream will be used to pipe the std::cout into the file.
     *
     * If read_from_std_cout is true, a new file is created where the std::cout is piped into.
     * If there is already a file handle for this file name, the event handler is added to the list of event
     * handlers for that file. If not, a new file handle is created and the event handler is added to the list
     * of event handlers for the new file.
     */
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

    /**
     * @brief Adds an event handler that is called when the file is changed.
     * @param File_Name The name of the file to add the event handler for.
     * @param Handle The event handler to be called when the file is changed.
     *
     * If there is already a file handle for this file name, the event handler is
     * added to the list of event handlers for that file. If not, a new file
     * handle is created and the event handler is added to the list of event
     * handlers for that file.
     */
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

    /**
     * @brief Returns the file stream handle associated with the given file name.
     * @param File_Name The name of the file to retrieve the handle for.
     * @return The file stream handle associated with the given file name, or nullptr if no handle exists.
     */
    FILE_STREAM* Get_File_Stream_Handle(std::string File_Name){
        // Check if there is a file handle for this file name
        auto it = File_Streamer_Handles.find(File_Name);
        if (it != File_Streamer_Handles.end()){
            // Return the handle
            return it->second;
        }

        // If no handle exists, return nullptr
        return nullptr;
    }

    /**
     * @brief Returns the current working directory of the program.
     * @return The current working directory as a string.
     *
     * This function returns the current working directory of the program
     * as a string. This is useful for finding the location of configuration
     * files, data files, and other resources that need to be accessed from
     * the program.
     */
    std::string Get_Current_Location(){
        return std::filesystem::current_path().string();
    }

    /**
     * @brief Pulls the current contents of STDIN as a string.
     * @return The contents of STDIN as a string.
     *
     * This function pulls the current contents of STDIN as a string. This is
     * useful for applications that need to access the output of a previous
     * process. Note that this function will only work if the application was
     * started as a non TTY enabled application. If the application was started
     * as a TTY enabled application, this function will fail.
     */
    std::string Pull_STDIN(){
        std::string Result = "";
        
        // Check if TTY is enabled for assurance
        if (Has_Started_As_TTY()){
            Report("Cannot pull STDIN from a TTY enabled environment!");

            return Result;
        }
        
        // Now that we know the exe has been started as an non TTY object we
        // can assume the STDIN buffer holds the previous executed output
        char buffer[256];

        while (std::cin.getline(buffer, sizeof(buffer))){
            Result += buffer;
        }

        return Result;
    }

    #ifdef _WIN32
        /**
         * @brief Constructor for the CMD class
         * @details Initializes a pipe for inter-process communication.
         *          The pipe is created with security attributes that allow
         *          the handles to be inherited by child processes.
         */
        CMD::CMD() {
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(sa);
            sa.lpSecurityDescriptor = NULL; // No security descriptor
            sa.bInheritHandle = TRUE; // Handles are inheritable

            // Create a pipe with the specified security attributes
            if (!CreatePipe(&In, &Out, &sa, 0)) {
                Report("Failed to create pipe for CMD!");
            }
        }

        /**
         * @brief Runs a command in a shell and returns its output as a string
         * @param Command The command to execute
         * @return The output of the command as a string
         *
         * This function runs a command in a shell and captures its output. It
         * then returns the output as a string.
         */
        std::string CMD::Run(std::string Command){
            PROCESS_INFORMATION pi;
            STARTUPINFO si;

            // Clear out the process and startup info
            ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
            ZeroMemory(&si, sizeof(STARTUPINFO));

            // Set up the startup info for the process
            si.cb = sizeof(STARTUPINFO);
            si.hStdOutput = Out;
            si.hStdError = Out;
            si.dwFlags |= STARTF_USESTDHANDLES;

            // Create the process (this will execute the command)
            if (!CreateProcess(NULL, (LPSTR)Command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
                Report("Failed to run command: '" + Command + "' !");
            }

            // Close the output handle now that we're done with it
            CloseHandle(Out);

            // Create a buffer to store the output
            DWORD bytesRead;
            CHAR buffer[4096];
            std::string output;

            // Read the output from the process and store it in the buffer
            while (ReadFile(In, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                output += buffer;
            }

            // Close the handles now that we're done with them
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            // Return the output of the command as a string
            return output;
        }

        /**
         * @brief Checks if the application was started from a TTY
         * @return True if the application was started from a TTY, false otherwise
         *
         * This function checks if the application was started from a TTY
         * by checking if STDIN is a TTY.
         */
        bool Has_Started_As_TTY(){
            return (_isatty(_fileno(stdin)));
        }
    #else

        /**
         * @brief Constructor for the CMD class
         *
         * This function creates a pipe and stores the file descriptors in the
         * File_Descriptor structure. The constructor is used to create a new
         * object of the CMD class.
         */
        CMD::CMD(){ 
            // Unix implementation

            // Create a pipe for inter-process communication
            int Error = pipe(File_Descriptor.FDS);

            // Check if there was an error creating the pipe
            if (Error == -1)
                Report("Failed to create pipe for CMD!");
        }

        /**
         * @brief Run a command using the system shell and capture its output.
         * @param Command The command to be executed.
         * @return The output of the command as a string.
         * 
         * This function forks a new process to execute the specified command
         * using the system shell. The output of the command is redirected to
         * a pipe and captured by the parent process. The function returns the
         * captured output as a string.
         */
        std::string CMD::Run(std::string Command){
            if (fork() == 0) {
                // Child process: redirect standard output to the write end of the pipe
                dup2(File_Descriptor.Way.Out, STDOUT_FILENO);
                int Error = system(Command.c_str());

                if (Error == -1)
                    Report("Failed to run command: '" + Command + "' !");

                exit(0); // Terminate the child process
            } else {
                // Parent process: close the write end of the pipe
                close(File_Descriptor.Way.Out);

                char buffer[256];
                std::string output = "";

                // Read the output of the command from the read end of the pipe
                while (read(File_Descriptor.Way.In, buffer, sizeof(buffer)) > 0) {
                    output += buffer;
                    memset(buffer, 0, sizeof(buffer)); // Clear the buffer
                }
                
                wait(NULL); // Wait for the child process to finish
                return output; // Return the captured output
            }
        }

        bool Has_Started_As_TTY(){
            return (isatty(STDIN_FILENO));
        }
    #endif

    namespace INTERNAL{

        /**
         * @brief Construct a BUFFER_CAPTURE
         * @param on_change Function which will be called when the BUFFER_CAPTURE had changed.
         * @param Name Name of the BUFFER_CAPTURE
         * @param Global If true, this BUFFER_CAPTURE will inform all other global BUFFER_CAPTURES about the change.
         *
         * Construct a BUFFER_CAPTURE
         * This will store the previous handle of std::cout.
         * This will also insert this as the new cout output stream.
         * If Global is true, this BUFFER_CAPTURE will inform all other global BUFFER_CAPTURES about the change.
         */
        BUFFER_CAPTURE::BUFFER_CAPTURE(std::function<void()> on_change, std::string Name, bool Global) : Name(Name), Is_Global(Global){
            Current_Line = "";

            // Store the previous handle.
            STD_COUT_RESTORATION_HANDLE = std::cout.rdbuf();

            // Insert this as the new cout output stream.
            std::cout.rdbuf(this);

            On_Change.push_back(on_change);

            if (Global){
                // Add this to the global list of BUFFER_CAPTURES, so that other global BUFFER_CAPTURES can inform this one when they change.
                Global_Buffer_Captures.push_back(this);
            }
        }

        /**
         * @brief Handles character overflow for BUFFER_CAPTURE.
         * @param c Character to be added to the current line.
         * @return The result of writing the character to the original std::cout buffer.
         *
         * This function is called whenever the buffer overflows, typically when a new character is inserted. 
         * It handles new lines by storing the current line in the console history and notifying all registered 
         * change handlers. If this BUFFER_CAPTURE is global, it informs all other global BUFFER_CAPTURES about 
         * the change.
         */
        int BUFFER_CAPTURE::overflow(int c) {
            if (c == '\n') {
                // Save the current line to the console history and clear it for the next line
                Console_History.push_back(Current_Line);
                Current_Line.clear();

                // Notify all registered change handlers
                for (auto& on_change : On_Change) {
                    on_change();
                }

                // Inform all global BUFFER_CAPTURES if this one is global
                if (Is_Global) {
                    Inform_All_Global_BUFFER_CAPTURES(this);
                }
            } else {
                // Append the character to the current line
                Current_Line += static_cast<char>(c);
            }

            // Write the character to the original std::cout buffer
            return STD_COUT_RESTORATION_HANDLE->sputc(c);
        }

        /**
         * @brief Close the BUFFER_CAPTURE and restore the original std::cout stream.
         *
         * This function is called when the BUFFER_CAPTURE is going out of scope, typically when it is destructed.
         * It checks if the STD_COUT_RESTORATION_HANDLE is nullptr to avoid a double-close of the BUFFER_CAPTURE.
         * If not nullptr, it sets the original std::cout stream back to the previous handle.
         */
        void BUFFER_CAPTURE::Close(){
            // If the STD_COUT_RESTORATION_HANDLE is nullptr at this point, it means that the predecessor BUFFER_CAPTURE has been already disclosed.
            // So there is no need to restore the original stream.
            if (STD_COUT_RESTORATION_HANDLE)
                std::cout.rdbuf(STD_COUT_RESTORATION_HANDLE);
        }

        /**
         * @brief Reads the console history and returns it as a single string.
         *
         * This function concatenates all lines stored in the console history
         * and returns them as a single string, with each line separated by a newline character.
         * 
         * @return A string containing the entire console history.
         */
        std::string BUFFER_CAPTURE::Read() {
            std::string Output = "";

            // Iterate through each line in the console history
            for (auto& Line : Console_History) {
                // Append the line followed by a newline character to the output
                Output += Line + "\n";
            }

            // Return the complete console history as a single string
            return Output;
        }

        /**
         * @brief Synchronizes this BUFFER_CAPTURE with the provided informer.
         * 
         * This function attempts to synchronize the current BUFFER_CAPTURE with another
         * by sharing the latest console history lines. If the BUFFER_CAPTUREs have previously
         * been synchronized, only the latest line is shared. Otherwise, it checks compatibility
         * and synchronizes the entire history as needed.
         * 
         * @param Informer The BUFFER_CAPTURE containing the latest data to synchronize with.
         * @return True if synchronization was successful, false otherwise.
         */
        bool BUFFER_CAPTURE::Sync(BUFFER_CAPTURE* Informer) {
            // Check if the BUFFER_CAPTUREs have previously been synchronized
            if (Synced.find(Informer) != Synced.end()) {
                // Share only the latest line if previously synced
                Console_History.push_back(Informer->Console_History.back());
                return true;  // Synchronization was successful
            }

            // Calculate the difference in console history sizes
            int Difference = Console_History.size() - Informer->Console_History.size();

            // If the informer has less data, synchronization is not possible
            if (Difference > 0) {
                Report(
                    "Failed to sync buffer capture: '" + Get_Name() + "' with: '" + Informer->Get_Name() + "' !\n" +
                    "Try obj->Merge(Informer) before trying to sync them again."
                );
                return false;
            } else {
                // Synchronize the entire history if the informer has more data
                for (int i = 0; i < Difference; i++) {
                    int Actual_Offset = Console_History.size() - Difference + i - 1;
                    Console_History.push_back(Informer->Console_History[Actual_Offset]);
                }

                // Mark the BUFFER_CAPTUREs as synced
                Synced[Informer] = true;
                return true;
            }
        }

        /**
         * @brief Gets the name of this BUFFER_CAPTURE.
         * 
         * If a name has not been set, it defaults to "BUFFER_CAPTURE<address>".
         * @return The name of this BUFFER_CAPTURE.
         */
        std::string BUFFER_CAPTURE::Get_Name() {
            if (Name.size() == 0) {
                // Name not set, default to "BUFFER_CAPTURE<address>"
                Name = "BUFFER_CAPTURE<" + std::to_string((unsigned long long)this) + ">";
            }

            return Name;
        }
    }

}
