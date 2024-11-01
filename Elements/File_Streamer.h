#ifndef _FILE_STREAMER_H_
#define _FILE_STREAMER_H_

#include "Element.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

#include <fstream>
#include <functional>
#include <unordered_map>
#include <stdio.h>
#include <deque>
#include <fcntl.h>

#if _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

namespace GGUI{

    /*
        Utilities to manage file streams.
    */

    class FILE_STREAM;

    extern std::unordered_map<std::string, FILE_STREAM*> File_Streamer_Handles;

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
    extern void Add_File_Stream_Handle(std::string File_Handle, std::function<void()> Handle);

    /**
     * @brief Returns the file stream handle associated with the given file name.
     * @param File_Name The name of the file to retrieve the handle for.
     * @return The file stream handle associated with the given file name, or nullptr if no handle exists.
     */
    extern FILE_STREAM* Get_File_Stream_Handle(std::string File_Name);

    /**
     * @brief Returns the current working directory of the program.
     * @return The current working directory as a string.
     *
     * This function returns the current working directory of the program
     * as a string. This is useful for finding the location of configuration
     * files, data files, and other resources that need to be accessed from
     * the program.
     */
    extern std::string Get_Current_Location();
 
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
    extern std::string Pull_STDIN();
    
    /**
     * @brief Checks if the application was started from a TTY
     * @return True if the application was started from a TTY, false otherwise
     *
     * This function checks if the application was started from a TTY
     * by checking if STDIN is a TTY.
     */
    extern bool Has_Started_As_TTY();

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
            BUFFER_CAPTURE(std::function<void()> on_change, std::string Name = "", bool Global = false);

            /**
             * @brief Default constructor
             *
             * This constructor is explicitly defined as default, which means that the compiler will generate a default implementation for it.
             * This is needed because otherwise, the compiler would not generate a default constructor for this class, since we have a user-declared constructor.
             */
            BUFFER_CAPTURE() = default;

            /**
             * @brief Destructor
             *
             * Called when the BUFFER_CAPTURE is going out of scope.
             * This will call the Close() method to restore the original std::cout stream.
             */
            ~BUFFER_CAPTURE() {
                Close();
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
            int overflow(int c) override;
            
            /**
             * @brief Close the BUFFER_CAPTURE and restore the original std::cout stream.
             *
             * This function is called when the BUFFER_CAPTURE is going out of scope, typically when it is destructed.
             * It checks if the STD_COUT_RESTORATION_HANDLE is nullptr to avoid a double-close of the BUFFER_CAPTURE.
             * If not nullptr, it sets the original std::cout stream back to the previous handle.
             */
            void Close();

            /**
             * @brief Reads the console history and returns it as a single string.
             *
             * This function concatenates all lines stored in the console history
             * and returns them as a single string, with each line separated by a newline character.
             * 
             * @return A string containing the entire console history.
             */
            std::string Read();

            /**
             * @brief Adds a change handler function to the list.
             * 
             * This function will be called whenever a change occurs. 
             * It appends the provided change handler function to the 
             * internal list of change handlers.
             *
             * @param on_change The function to be called on change.
             */
            void Add_On_Change_Handler(std::function<void()> on_change){                
                // Append the change handler function to the list
                On_Change.push_back(on_change);
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
            bool Sync(BUFFER_CAPTURE* Informer);

            /**
             * @brief Gets the name of this BUFFER_CAPTURE.
             * 
             * If a name has not been set, it defaults to "BUFFER_CAPTURE<address>".
             * @return The name of this BUFFER_CAPTURE.
             */
            std::string Get_Name();

            /**
             * @brief Sets the name of this BUFFER_CAPTURE.
             * @param Name The new name of this BUFFER_CAPTURE.
             *
             * This function sets the name of this BUFFER_CAPTURE.
             * If a name has not been set, it defaults to "BUFFER_CAPTURE<address>".
             * @see Get_Name()
             */
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
        FILE_STREAM(std::string File_Name, std::function<void()> on_change, bool read_from_std_cout = false);

        /**
         * @brief Destructor for the FILE_STREAM class.
         *
         * This destructor ensures that resources are properly released
         * when a FILE_STREAM object is destroyed. It closes the associated
         * BUFFER_CAPTURE if it exists and also closes the file handle.
         */
        ~FILE_STREAM() {
            // Close the BUFFER_CAPTURE if it's active
            if (Buffer_Capture)
                Buffer_Capture->Close();

            // Close the file handle
            Handle.close();
        }

        /**
         * @brief Read the content of the file.
         * @return The content of the file as a string.
         * 
         * This function will read the content of the file and return it as a string.
         * If the file is empty, this function will return an empty string.
         * If the file does not exist, or any other error occurs, this function will return an empty string and print an error message to the console.
         * If read_from_std_cout is true, this function will read the content from the buffer capture instead of the file.
         */
        std::string Read();
    
        /**
         * @brief Read the content of the file quickly without checking if the file has changed.
         * @return The content of the file as a string.
         *
         * This function is faster than the normal Read() function, but it does not check if the file has changed.
         * If the file has changed since the last call to Read(), this function will return the old content.
         * If the file does not exist, or any other error occurs, this function will return an empty string and print an error message to the console.
         * If read_from_std_cout is true, this function will read the content from the buffer capture instead of the file.
         */
        std::string Fast_Read() { return Previous_Content; }

        /**
         * @brief Checks if the file has changed and notifies the event handlers if so.
         *
         * This function is called by the FILE_STREAM class when it wants to check if the file has changed.
         * It reads the new content from the file, calculates the hash of the new content, and compares it with the previous hash.
         * If the hash has changed, it notifies all the event handlers by calling them.
         */
        void Changed();

        /**
         * @brief Adds a new event handler to the list of event handlers for this file.
         * @param on_change The function to be called when the file changes.
         * 
         * This function adds a new event handler to the list of event handlers for this file.
         * If read_from_std_cout is true, the event handler is added to the list of event handlers of the Buffer_Capture object.
         * Otherwise, the event handler is added to the list of event handlers of this FILE_STREAM object.
         */
        void Add_On_Change_Handler(std::function<void()> on_change){
            if (Buffer_Capture)
                Buffer_Capture->Add_On_Change_Handler(on_change);
            else
                On_Change.push_back(on_change);
        }

        /**
         * @brief Checks if the file stream is a std::cout stream.
         * @return True if the file stream is a std::cout stream, false otherwise.
         *
         * This function checks if the file stream is a std::cout stream, i.e. if it is a stream that is
         * associated with the BUFFER_CAPTURE class. If it is, it returns true, otherwise it returns false.
         */
        bool Is_Cout_Stream(){
            return Buffer_Capture != nullptr;
        }
    };

    class FILE_POSITION{
    public:
        std::string File_Name = "";     // Originated.
        unsigned int Line_Number = 0;   // Y
        unsigned int Character = 0;     // X

        /**
         * @brief Constructor for the FILE_POSITION class.
         * @param File_Name The name of the file that the position is in.
         * @param Line_Number The line number of the position.
         * @param Character The character number of the position.
         * 
         * This constructor creates a new FILE_POSITION object with the given file name, line number and character number.
         */
        FILE_POSITION(std::string File_Name, unsigned int Line_Number, unsigned int Character){
            this->File_Name = File_Name;
            this->Line_Number = Line_Number;
            this->Character = Character;
        }

        /**
         * @brief Default constructor for the FILE_POSITION class.
         * @details This constructor creates a new FILE_POSITION object with default values for the file name, line number and character number.
         */
        FILE_POSITION() = default;

        /**
         * @brief Converts the FILE_POSITION object to a string.
         * @return A string that represents the FILE_POSITION object in the format "File_Name:Line_Number:Character".
         *
         * This function converts the FILE_POSITION object to a string by concatenating the file name, line number and character number with a colon in between.
         * The resulting string can be used to log or display the position of the file stream.
         */
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
            /**
             * @brief Constructor for the CMD class
             * @details Initializes a pipe for inter-process communication.
             *          The pipe is created with security attributes that allow
             *          the handles to be inherited by child processes.
             */
            CMD();

            /**
             * @brief Destructor for the CMD class
             * @details Closes the handles created by the constructor and releases
             *          any resources associated with the pipe.
             */
            ~CMD() = default;

            /**
             * @brief Runs a command in a shell and returns its output as a string
             * @param Command The command to execute
             * @return The output of the command as a string
             *
             * This function runs a command in a shell and captures its output. It
             * then returns the output as a string.
             */
            std::string Run(std::string command);
        };
    #else
        class CMD{  // Unix implementation:
        private:
            union __INTERNAL_CMD_FILE_DESCRIPTOR__ {
                struct __INTERNAL_CMD_WAY__ {
                    int In;
                    int Out;
                } Way;
                int FDS[2];
            } File_Descriptor;
        public:

            /**
             * @brief Constructor for the CMD class
             *
             * This function creates a pipe and stores the file descriptors in the
             * File_Descriptor structure. The constructor is used to create a new
             * object of the CMD class.
             */
            CMD();
            
            /**
             * @brief Destructor for the CMD class
             * @details Closes the handles created by the constructor and releases
             *          any resources associated with the pipe.
             */
            ~CMD() = default;

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
            std::string Run(std::string command);
        };
    #endif
}

#endif