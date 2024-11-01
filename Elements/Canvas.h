#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "Element.h"

#include <vector>

namespace GGUI{

    class Canvas : public Element{
    private:
        // DONT GIVE THIS TO USER!
        Canvas(){}
    protected:
        std::vector<RGB> Buffer;
    public:
        Canvas(Styling s) : Element(s){}
        
        // This is to set a color in the canvas, you can set it to not flush, if youre gonna set more than one pixel.
        void Set(unsigned int x, unsigned int y, RGB color, bool Flush = true);
        
        void Flush();

        std::vector<GGUI::UTF>&  Render() override;

        /**
         * @brief Creates a deep copy of the Canvas and returns it as a movable Element.
         * @return A deep copy of the Canvas as a movable Element.
         */
        Element* Safe_Move() override {
            // Create a new Canvas and copy the current Canvas' contents into it.
            Canvas* new_Canvas = new Canvas();
            *new_Canvas = *(Canvas*)this;

            // Return the new Canvas as a movable Element.
            return new_Canvas;
        }

        /**
         * @brief Returns the name of the Canvas as a string.
         * @details The returned string is a combination of the class name and the Name property.
         * @return A string representing the name of the Canvas.
         */
        std::string Get_Name() const override {
            return "Canvas<" + Name + ">";
        }
    };

    class Sprite{
    public:
        std::vector<GGUI::UTF> Frames;

        int Offset = 0;     // This is for more beautiful mass animation systems
        int Speed = 1;      // Using decimals too slow hmmm...

        int Frame_Distance = 0;

        bool Is_Power_Of_Two = false;

        Sprite(std::vector<GGUI::UTF> frames, int offset = 0, int speed = 1);

        /**
         * @brief Constructs a Sprite object with a single frame.
         * @details This constructor initializes the Sprite with a single UTF frame, setting the offset and speed for animation.
         * @param frame A UTF object representing the single frame of the sprite.
         * @param offset The number of frames to skip before playing the animation. Default is 0.
         * @param speed The speed of the animation playback. Default is 1.
         */
        Sprite(GGUI::UTF frame, int offset = 0, int speed = 1) 
            : Offset(offset), Speed(speed), Frame_Distance(UINT8_MAX) {
            // Add the provided frame to the Frames vector.
            Frames.push_back(frame);
        }

        /**
         * @brief Constructs a Sprite object with default values.
         * @details This constructor sets the Sprite to have a single UTF frame, which is a space character, and sets the offset and speed for animation.
         */
        Sprite() : Frame_Distance(UINT8_MAX){
            // Set the default frame to a space character.
            Frames.push_back(GGUI::UTF(' '));
            
            // Set the default offset and speed values.
            Offset = 0;
            Speed = 1;
            
            // Set the Is_Power_Of_Two flag to false, indicating the sprite does not have a power of two size.
            Is_Power_Of_Two = false;
        }


        UTF Render(unsigned char Current_Time);
    };

    namespace GROUP_TYPE{
        // Defines the group sizes for Sprite group optimizing.
        inline unsigned char QUAD = 1 << 2;
        inline unsigned char HEX = 1 << 3;
        inline unsigned char OCTAL = 1 << 4;
    }

    class Terminal_Canvas : public Element{
    private:
        // DONT GIVE THIS TO USER!!!
        Terminal_Canvas(){}
    protected:
        std::vector<Sprite> Buffer;

        unsigned char Current_Animation_Frame = 0;

        // For speeding up sprite sets, to avoid redundant checks in unordered_maps.
        bool Multi_Frame = false;
    public:
        Terminal_Canvas(Styling s) : Element(s){}
        
        ~Terminal_Canvas() override;

        void Set_Next_Animation_Frame() { Current_Animation_Frame++; }

        void Set(unsigned int x, unsigned int y, Sprite& sprite, bool Flush = true);

        void Set(unsigned int x, unsigned int y, UTF& sprite, bool Flush = true);
        
        void Flush(bool Force_Flush = false);
        
        std::vector<GGUI::UTF>&  Render() override;
        
        void Group_Heuristics();

        void Group(unsigned int Start_Index, int length);

        bool Is_Multi_Frame(){ return Multi_Frame; }

        /**
         * @brief Creates a deep copy of the Terminal_Canvas and returns it as a movable Element.
         * @return A deep copy of the Terminal_Canvas as a movable Element.
         */
        Element* Safe_Move() override {
            // Create a new Terminal_Canvas and copy the current Terminal_Canvas' contents into it.
            Terminal_Canvas* new_Terminal_Canvas = new Terminal_Canvas();
            *new_Terminal_Canvas = *(Terminal_Canvas*)this;

            // Return the new Terminal_Canvas as a movable Element.
            return new_Terminal_Canvas;
        }

        /**
         * @brief Returns the name of the Terminal_Canvas as a string.
         * @details The returned string is a combination of the class name and the Name property.
         * @return A string representing the name of the Terminal_Canvas.
         */
        std::string Get_Name() const override {
            // Concatenate class name and Name property to form the full name.
            return "Terminal_Canvas<" + Name + ">";
        }
    
        void Embed_Points(std::vector<bool> pixels, styled_border border_style = GGUI::STYLES::BORDER::Single, bool Flush = true);
    };

    namespace DRAW{

        /**
         * @brief Draws a line on the canvas.
         * @param x1 The x-coordinate of the first point.
         * @param y1 The y-coordinate of the first point.
         * @param x2 The x-coordinate of the second point.
         * @param y2 The y-coordinate of the second point.
         * @param pixels The vector of pixels of the canvas.
         * @param width The width of the canvas.
         * @details
         * This function draws a line on the canvas by setting the pixels to true.
         * It uses the Bresenham line drawing algorithm to determine which pixels to set.
         */
        void Line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width);

        std::vector<bool> Line(FVector2 Start, FVector2 End, int Buffer_Width);

        /**
         * @brief Symmetrical circle draw helper.
         * @param x_center The x position of the center of the circle.
         * @param y_center The y position of the center of the circle.
         * @param x The current x position of the circle.
         * @param y The current y position of the circle.
         * @param pixels The vector of pixels of the canvas.
         * @param width The width of the canvas.
         * @details
         * This function is a helper function for drawing a circle on the canvas.
         * It fills in the circle symmetrically by setting the pixels to true.
         */
        void Symmetry_Filler_For_Circle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width);

        void Circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width);

        std::vector<bool> Circle(FVector2 Center, int Radius, int Buffer_Width);

        void Cubic_Bezier_Curve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, std::vector<bool>& pixels, int width);
        
        std::vector<bool> Cubic_Bezier_Curve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, int Buffer_Width);

    }

    namespace FONT{
        // Based on: https://learn.microsoft.com/en-us/typography/opentype/spec/otff
        class Font_Header{
        public:
        };

        Font_Header Parse_Font_File(std::string File_Name);
    }

}

#endif
