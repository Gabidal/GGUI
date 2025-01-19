#include "Logger.h"
#include "../Settings.h"
#include "../Renderer.h"

#include "../../Elements/File_Streamer.h"

#include <functional>
#include <vector>

namespace GGUI{
    namespace SETTINGS{
        extern void Init_Settings();
    }
    
    namespace INTERNAL{
        namespace LOGGER{
            // File handle for logging to files for Atomic access across different threads.
            INTERNAL::Atomic::Guard<FILE_STREAM> Handle;

            void Init(){
                Handle([](GGUI::FILE_STREAM& self){
                    if (self.Get_type() == FILE_STREAM_TYPE::UN_INITIALIZED){    // If the Log is called before GGUI_Init, then we need to skip this in GGUI_Init
                        if (SETTINGS::LOGGER::File_Name.size() == 0){
                            SETTINGS::Init_Settings();
                        }
                        new (&self) FILE_STREAM(SETTINGS::LOGGER::File_Name, [](){}, FILE_STREAM_TYPE::WRITE, true);
                    }
                });
            }

            // Writes the given text into the Log file:
            // [TIME]: [Text]
            void Log(std::string Text){
                Handle([&Text](GGUI::FILE_STREAM& self){
                    // If this log is called before the normal GGUI_Init is called, then we need to manually init this.
                    if (self.Get_type() == FILE_STREAM_TYPE::UN_INITIALIZED)
                        Init();

                    // Get the current time as string
                    std::string now = "[" + GGUI::INTERNAL::Now() + "]: ";

                    // When the Stack_Traced report is given we need to push all the following lines after the first line by the length of the now string.
                    for (unsigned int i = 0; i < Text.size(); i++){
                        if (Text[i] == '\n'){
                            // now we can add before the newline spaces by the amount of the length of the now time.
                            for (unsigned int j = 0; j < now.size(); j++)
                                Text.insert(Text.begin() + i + 1, ' ');
                        }
                    }

                    // Write the time and the text into the log file.
                    self.Append(now + Text);
                });
            }
        }
    }
}