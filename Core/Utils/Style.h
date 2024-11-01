#ifndef _STYLE_H_
#define _STYLE_H_

#include "Units.h"
#include <variant>

namespace GGUI{
    // Externies
    class Element;
    class Styling;
    enum class STAIN_TYPE;
    extern void Report_Stack(std::string Problem);

    enum class ALIGN{
        UP,
        DOWN,
        LEFT,
        RIGHT,
        CENTER
    };

    enum class VALUE_STATE{
        UNINITIALIZED,
        INITIALIZED,
        VALUE
    };

    enum class DIRECTION{
        ROW,
        COLUMN
    };

    enum class EVALUATION_TYPE{
        DEFAULT,        // no further evaluation needed, just return the value
        PIXELS,         // the value is multiplied by 1:1
        PERCENTAGE,     // the value is a percentage of the parent attribute
        EM,             // the value is a multiple of the font size
        REM,            // the value is a multiple of the root font size
        VW,             // the value is a percentage of the viewport width
        VH,             // the value is a percentage of the viewport height
        CH,             // the value is a relative to the width of the "0" (zero) character in the element’s font.
        VMIN,           // the value is a percentage of the viewport’s smaller dimension
        VMAX,           // the value is a percentage of the viewport’s larger dimension
        EX,             // the value is a multiple of the x-height of the element’s font
        FRACTION,       // the value is a fraction of the parent attribute

        // PHYSICAL MEASUREMENTS
        CM,             // the value is in centimeters defaults to 1:1 in terminal mode
        MM,             // the value is in millimeters defaults to 1:1 in terminal mode
        IN,             // the value is in inches defaults to 1:1 in terminal mode
        PT,             // the value is in points defaults to 1:1 in terminal mode
        PC,             // the value is in picas defaults to 1:1 in terminal mode
    };

    // This namespace is an wrapper for the user not to see these !!
    namespace STYLING_INTERNAL{
        template<typename T>
        class value {
        protected:
            std::variant<T, float> data;  // Can hold either normal value or percentage
            EVALUATION_TYPE evaluation_type = EVALUATION_TYPE::DEFAULT;

        public:
            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             * @param use_constexpr Whether to use constexpr or not
             */
            constexpr value(T value, EVALUATION_TYPE type, [[maybe_unused]] bool use_constexpr)
                : data(value), evaluation_type(type) {}

            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             * @param use_constexpr Whether to use constexpr or not
             *
             * This constructor is used when the value is a float and the type is
             * specified. The constexpr parameter is used to determine whether to
             * use a constexpr constructor or not.
             */
            constexpr value(float value, EVALUATION_TYPE type, [[maybe_unused]] bool use_constexpr)
                : data(value), evaluation_type(type) {}


            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             *
             * This constructor is used to create a value with a type specified.
             * The default type is DEFAULT.
             */
            value(T value, EVALUATION_TYPE type = EVALUATION_TYPE::DEFAULT)
                : data(value), evaluation_type(type) {}

            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             *
             * This constructor is used when the value is a float and the type is
             * specified. If the type is not specified, the default type is
             * PERCENTAGE.
             */
            value(float value, EVALUATION_TYPE type = EVALUATION_TYPE::PERCENTAGE)
                : data(value), evaluation_type(type) {}

            /**
             * Copy constructor
             * @param other The object to be copied from
             *
             * This constructor is used to create a copy of the value object.
             * The data and the evaluation type are copied from the other object.
             */
            constexpr value(const value<T>& other)
                : data(other.data), evaluation_type(other.evaluation_type) {}


            /**
             * Assignment operator
             * @param other The object to be assigned from
             * @return The object itself, for chaining
             *
             * This operator is used to assign the data and the evaluation type
             * from another object of the same type.
             */
            constexpr value& operator=(const value& other) {
                // Copy the data and the evaluation type from the other object
                data = other.data;
                evaluation_type = other.evaluation_type;
                // Return the object itself, for chaining
                return *this;
            }

            /**
             * Assignment operator
             * @param initialization_data The value to be assigned to the data member
             * @return The object itself, for chaining
             *
             * This operator is used to assign a value of type T to the data member.
             * The type of the value is set to DEFAULT.
             */
            constexpr value& operator=(T initialization_data) {
                // Set the data to the value passed in
                data = initialization_data;
                // Set the evaluation type to DEFAULT
                evaluation_type = EVALUATION_TYPE::DEFAULT;
                // Return the object itself, for chaining
                return *this;
            }

            /**
             * Assignment operator for float type
             * @param initialization_data The float value to be assigned to the data member
             * @return The object itself, for chaining
             *
             * This operator is used to assign a float value to the data member.
             * The type of the value is set to PERCENTAGE.
             */
            constexpr value& operator=(float initialization_data) {
                // Set the data to the float value passed in
                data = initialization_data;
                // Set the evaluation type to PERCENTAGE
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
                // Return the object itself, for chaining
                return *this;
            }

            /**
             * Evaluate function
             * @param parental_value The value to be multiplied by. Only used if the evaluation type is PERCENTAGE.
             * @return The evaluated value.
             * @throws std::bad_variant_access If the evaluation type doesn't match the type of the data.
             * @throws std::invalid_argument If the evaluation type is not supported.
             */
            T Evaluate(T parental_value) {
                switch (evaluation_type) {
                    case EVALUATION_TYPE::DEFAULT:
                        // If the evaluation type is DEFAULT then just return the data without any modification
                        return std::get<T>(data);
                    case EVALUATION_TYPE::PERCENTAGE:
                        // If the evaluation type is PERCENTAGE then multiply the parental value by the data and return the result
                        return static_cast<T>(static_cast<T>(parental_value) * std::get<float>(data));
                    default:
                        Report_Stack("Evaluation type not supported!");
                        // If the evaluation type is not supported then just return the data without any modification
                        return std::get<T>(data);
                }
            }

            /**
             * Get the value of the variant.
             * @tparam P The type to cast the variant to.
             * @return The value of the variant as the requested type.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            constexpr P Get() const {
                /**
                 * In debug mode, check if the requested type matches the type of the data.
                 * If it doesn't, throw an exception.
                 */
                #ifdef _DEBUG
                if (!std::holds_alternative<P>(data)) {
                    Report_Stack("Value is not of the requested type!");
                    throw std::bad_variant_access();  // Exception if the requested type doesn't match
                }
                #endif
                return std::get<P>(data);  // In release mode, or after the check passes in debug mode
            }

            /**
             * Getter methods
             * @tparam P The type to cast the variant to.
             * @return The value of the variant as the requested type.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            inline P Get() {                
                /**
                 * In debug mode, check if the requested type matches the type of the data.
                 * If it doesn't, throw an exception.
                 */
                #ifdef _DEBUG
                if (!std::holds_alternative<P>(data)) {
                    Report_Stack("Value is not of the requested type!");
                    throw std::bad_variant_access();  // Exception if the requested type doesn't match
                }
                #endif
                return std::get<P>(data);  // In release mode, or after the check passes in debug mode
            }

            /**
             * @brief Direct access to the underlying data of the variant.
             *        This is an exclusive Dev function, with serious repercussions for misuse.
             * @tparam P The type to cast the variant to.
             * @return A reference to the underlying data of the variant.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            P& Direct() { 
                /**
                 * In debug mode, check if the requested type matches the type of the data.
                 * If it doesn't, throw an exception.
                 */
                #ifdef _DEBUG
                if (!std::holds_alternative<P>(data)) {
                    Report_Stack("Value is not of the requested type!");
                    throw std::bad_variant_access();  // Exception if the requested type doesn't match
                }
                #endif
                return std::get<P>(data);  // In release mode, or after the check passes in debug mode
            }

            /**
             * @brief Set the value of the variant.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value.
             *          The evaluation type is set to EVALUATION_TYPE::DEFAULT.
             */
            inline void Set(T value) {
                data = value;
                evaluation_type = EVALUATION_TYPE::DEFAULT;
            }

            /**
             * @brief Set the value of the variant to a percentage.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value, and sets the evaluation type to EVALUATION_TYPE::PERCENTAGE.
             */
            inline void Set(float value){
                data = value;
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
            }
        };

        class style_base{
        public:
            // This is used to prevent accidental overwrites.
            VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

            // This is used to store all appended style_bases through the operator|.
            style_base* Other = nullptr;

            /**
             * @brief Construct a new constexpr style_base object.
             * @param status The status to initialize the style_base with.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr style_base(VALUE_STATE status, [[maybe_unused]] bool use_constexpr) : Status(status), Other(nullptr){}
            
            /**
             * @brief Construct a new style_base object.
             * @param status The status to initialize the style_base with.
             */
            style_base(VALUE_STATE status) : Status(status), Other(nullptr){}

            /**
             * @brief Default constructor for style_base.
             * @details This constructor does nothing and is only provided to make the class default constructible.
             */
            style_base() = default;

            /**
             * @brief Destructor of the style_base class.
             *
             * This destructor takes care of deleting the "Other" pointer if it is a heap-allocated object.
             * It does this by calling the Is_Deletable() function to check if the pointer is likely to be
             * a heap-allocated object. If it is, it deletes the object using the delete keyword.
             */
            virtual ~style_base();

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A pointer to the style_base that was appended to.
             * @details This function is used to append a style_base to another style_base. It does this by
             *          setting the Other pointer of the current object to the address of the other object.
             *          Then it returns a pointer to the current object.
             */
            style_base* operator|(style_base* other){
                Other = other;

                return this;
            }

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A reference to the style_base that was appended to.
             * @details This function is used to append a style_base to another style_base. It does this by
             *          setting the Other pointer of the current object to the address of the other object.
             *          Then it returns a reference to the current object.
             */
            style_base& operator|(style_base& other){
                Other = &other;

                return *this;
            }

            /**
             * @brief Overload the | operator to allow for appending of style_bases using move semantics.
             * @param other The style_base to append.
             * @return A reference to the style_base that was appended to.
             * @details This function is used to append a style_base to another style_base using move semantics.
             *          It sets the Other pointer of the moved object to the current object. Then it returns
             *          a reference to the moved object to ensure proper chaining of operations.
             * @note Since operators for "a | b | c" will re-write the previous "a | b" when at c with "ab | c",
             *       it will produce "ac" and not "abc". Thus, we return the secondary object so that: 
             *       "a | b | c" -> "b(a) | c" -> "c(b(a))".
             */
            style_base& operator|(style_base&& other){
                other.Other = this;

                return other;
            }

            /**
             * @brief Evaluates the style value based on a given Styling object.
             * @param host The Styling object to evaluate the style value with.
             * @details This function is used to evaluate the style value based on a given Styling object.
             *          It is called by the Styling class when the style value needs to be evaluated.
             *          The function is responsible for setting the Status variable to the evaluated status.
             *          The function is also responsible for setting the Value variable to the evaluated value.
             *          The function should be implemented by the derived classes to perform the evaluation.
             */
            virtual void Evaluate(Styling* host) = 0;

            /**
             * @brief Imprints the style's identity into the Styling object.
             * @param host The Styling object to imprint the style into.
             * @param owner The Element that owns the style.
             * @return The type of stain this style will leave.
             * @details This function allows single style classes to incorporate their unique characteristics into a Styling object. 
             *          It should be implemented by derived classes to define how the style affects the Styling and Element objects.
             */
            virtual STAIN_TYPE Embed_Value(Styling* host, Element* owner) = 0;
        };

        class RGB_VALUE : public style_base{
        public:
            value<RGB> Value = RGB(0, 0, 0);

            /**
             * @brief Construct a new RGB_VALUE object.
             * @param value The initial value of the RGB_VALUE object.
             * @param Default The default status of the RGB_VALUE object.
             * @details This constructor initializes the RGB_VALUE object with the specified value and default status.
             */
            RGB_VALUE(RGB value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The RGB value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGB_VALUE(const GGUI::RGB value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) 
                : style_base(Default, true), Value(value, EVALUATION_TYPE::DEFAULT, true) {}
            RGB_VALUE() = default;

            /**
             * @brief Destructor for the RGB_VALUE class.
             * @details This destructor is necessary to ensure that the base class destructor is called.
             */
            ~RGB_VALUE() override { style_base::~style_base(); }

            /**
             * @brief Overload the assignment operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to assign from.
             * @return A reference to this RGB_VALUE object.
             * @details This function assigns the value and status of the other RGB_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            RGB_VALUE& operator=(const RGB_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to assign from.
             * @return A reference to this RGB_VALUE object.
             * @details This function assigns the RGB value of the other RGB_VALUE object to this one.
             *          It sets the status of the object to VALUE_STATE::VALUE after the assignment.
             */
            RGB_VALUE& operator=(const GGUI::RGB other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Copy constructor for RGB_VALUE.
             * @param other The RGB_VALUE object to copy from.
             * @details This constructor creates a new RGB_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object, and sets the status to VALUE_STATE::VALUE.
             */
            constexpr RGB_VALUE(const GGUI::STYLING_INTERNAL::RGB_VALUE& other) : style_base(other.Status, true), Value(other.Value){}


            /**
             * @brief Embeds the value of an RGB_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] Styling* host, Element* owner) override;

            /**
             * @brief Evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             */
            void Evaluate(Styling* owner) override = 0;
        };

        class BOOL_VALUE : public style_base{
        public:
            bool Value = false;

            /**
             * @brief Constructor for a BOOL_VALUE object.
             * @param value The value to set the BOOL_VALUE to.
             * @param Default The default value state to use if the value is not set.
             * @details This constructor creates a new BOOL_VALUE object with the given value and default value state.
             */
            BOOL_VALUE(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            /**
             * @brief Construct a new constexpr BOOL_VALUE object.
             * @param value The boolean value to set.
             * @param Default The default value state to use.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr BOOL_VALUE(bool value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) 
                : style_base(Default, true), Value(value) {}

            /**
             * @brief Default constructor for BOOL_VALUE.
             * @details This constructor is provided to ensure that BOOL_VALUE is default constructible.
             */
            BOOL_VALUE() = default;
            
            /**
             * @brief Destructor for the BOOL_VALUE class.
             * @details This destructor is responsible for properly deallocating all the memory
             * allocated by the BOOL_VALUE object, including its parent class resources.
             */
            ~BOOL_VALUE() override { style_base::~style_base(); }

            /**
             * @brief Overload the assignment operator for BOOL_VALUE.
             * @param other The other BOOL_VALUE object to assign from.
             * @return A reference to this BOOL_VALUE object.
             * @details This function assigns the value and status of the other BOOL_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            BOOL_VALUE& operator=(const BOOL_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value; // Copy the value from the other object
                    
                    Status = other.Status; // Update the status to the other object's status
                }
                return *this; // Return a reference to this object
            }

            /**
             * @brief Overload the assignment operator for BOOL_VALUE.
             * @param other The boolean value to assign to this BOOL_VALUE.
             * @return A reference to this BOOL_VALUE object.
             * @details This function assigns the boolean value to the BOOL_VALUE object,
             *          setting the status to VALUE_STATE::VALUE.
             */
            BOOL_VALUE& operator=(const bool other){
                Value = other; // Assign the boolean value to the Value member
                Status = VALUE_STATE::VALUE; // Set the status to indicate a valid value
                return *this; // Return a reference to this object
            }
        
            /**
             * @brief Copy constructor for BOOL_VALUE.
             * @param other The BOOL_VALUE object to copy from.
             * @details This constructor creates a new BOOL_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object.
             */
            constexpr BOOL_VALUE(const GGUI::STYLING_INTERNAL::BOOL_VALUE& other) 
                : style_base(other.Status, true), Value(other.Value) {}
            
            /**
             * @brief Evaluate the BOOL_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This function is a no-op for BOOL_VALUE, as it does not have any dynamically computable values.
             */
            void Evaluate([[maybe_unused]] Styling* owner) override {};
            
            /**
             * @brief Embeds the value of a BOOL_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] Styling* host,  Element* owner) override;
        };
        
        class NUMBER_VALUE : public style_base{
        public:
            value<int> Value = 0;

            /**
             * @brief Construct a new NUMBER_VALUE object.
             * @param value The integer value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @details This constructor initializes the NUMBER_VALUE with the provided integer value and default state.
             */
            NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default) {
                Value = value; // Initialize the Value member with the provided integer value
            }

            /**
             * @brief Construct a new NUMBER_VALUE object from a float.
             * @param value The floating point value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @details This constructor initializes the NUMBER_VALUE with the provided float value and default state.
             *          The value is converted to a percentage (multiplying by 0.01) and stored as a float in the Value member.
             */
            NUMBER_VALUE(float value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value, EVALUATION_TYPE::PERCENTAGE){}

            /**
             * @brief Construct a new NUMBER_VALUE object from an integer using constexpr.
             * @param value The integer value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             * @details This constructor initializes a NUMBER_VALUE object with the provided integer value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr NUMBER_VALUE(int value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value, EVALUATION_TYPE::DEFAULT, true){}

            /**
             * @brief Default constructor for NUMBER_VALUE.
             * @details This constructor is provided to ensure that NUMBER_VALUE is default constructible.
             *          It does not initialize any values and leaves the object in an undefined state.
             */
            NUMBER_VALUE() = default;

            /**
             * @brief Destructor for NUMBER_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the NUMBER_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            ~NUMBER_VALUE() override { style_base::~style_base(); }

            /**
             * @brief Overload the assignment operator for NUMBER_VALUE.
             * @param other The other NUMBER_VALUE object to assign from.
             * @return A reference to this NUMBER_VALUE object.
             * @details This function assigns the value and status of the other NUMBER_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            NUMBER_VALUE& operator=(const NUMBER_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for NUMBER_VALUE.
             * @param other The other number to assign from.
             * @return A reference to this NUMBER_VALUE object.
             * @details This function assigns the value of the other number to this NUMBER_VALUE object.
             *          It sets the status to VALUE_STATE::VALUE and returns the modified object.
             */
            NUMBER_VALUE& operator=(const int other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Construct a new NUMBER_VALUE object from another NUMBER_VALUE object using constexpr.
             * @param other The other NUMBER_VALUE object to construct from.
             * @details This constructor initializes a new NUMBER_VALUE object with the same value and status as the given object.
             */
            constexpr NUMBER_VALUE(const GGUI::STYLING_INTERNAL::NUMBER_VALUE& other) : style_base(other.Status, true), Value(other.Value){}
            

            /**
             * @brief Embeds the value of a NUMBER_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
            
            /**
             * @brief Evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             *          When called, the function should evaluate the RGB value based on the owner object and set the Value property accordingly.
             */
            void Evaluate(Styling* owner) override = 0;

            /**
             * @brief Directly access the value of this NUMBER_VALUE object.
             * @return A reference to the value of this NUMBER_VALUE object.
             * @details This function returns a reference to the value of this NUMBER_VALUE object, allowing it to be directly accessed and modified.
             */
            int& Direct() { return Value.Direct<int>(); }
        };

        template<typename T>
        class ENUM_VALUE : public style_base{
        public:
            T Value;

            /**
             * @brief Construct a new ENUM_VALUE object.
             * @param value The initial value of the ENUM_VALUE object.
             * @param Default The default value state to initialize the ENUM_VALUE object with.
             * @details This constructor initializes the ENUM_VALUE object with the provided value
             *          and default value state, inheriting from the style_base class.
             */
            ENUM_VALUE(T value, VALUE_STATE Default = VALUE_STATE::INITIALIZED) : style_base(Default) {
                Value = value;
            }

            /**
             * @brief Construct a new ENUM_VALUE object using constexpr.
             * @param value The enum value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an ENUM_VALUE object with the given enum value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr ENUM_VALUE(T value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value){}

            /**
             * @brief Default constructor for ENUM_VALUE.
             * @details This constructor is declared as default, allowing the compiler to generate a default
             *          implementation for the constructor.
             */
            ENUM_VALUE() = default;

            /**
             * @brief Destructor for ENUM_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the ENUM_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            ~ENUM_VALUE() override { style_base::~style_base(); }

            /**
             * @brief Overload the assignment operator for ENUM_VALUE.
             * @param other The other ENUM_VALUE object to assign from.
             * @return A reference to this ENUM_VALUE object.
             * @details This function assigns the value and status of the other ENUM_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            ENUM_VALUE& operator=(const ENUM_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for ENUM_VALUE.
             * @param other The other value to assign from.
             * @return A reference to this ENUM_VALUE object.
             * @details This function assigns the value and status of the other ENUM_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            ENUM_VALUE& operator=(const T other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

        
            /**
             * @brief Copy constructor for ENUM_VALUE.
             * @param other The ENUM_VALUE object to copy from.
             * @details This constructor creates a new ENUM_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object.
             */
            constexpr ENUM_VALUE(const GGUI::STYLING_INTERNAL::ENUM_VALUE<T>& other) 
                : style_base(other.Status, true), Value(other.Value) {}
                        
            /**
             * @brief Evaluate the style.
             * @param owner The Styling object that owns this style.
             * @details This function is used to evaluate the style with the given Styling object.
             *          It is used to support dynamic values like percentage depended values.
             *          The function does not do anything as of now.
             */
            void Evaluate([[maybe_unused]] Styling* owner) override {};


            /**
             * @brief Embed the value of this style into the given Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return The type of stain that this style embeds.
             * @details This function embeds the value of this style into the given Styling object.
             *          The value is not evaluated or modified in any way.
             *          It is used to support dynamic values like percentage depended values.
             *          The function does not do anything as of now.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] Styling* host, [[maybe_unused]] Element* owner) override { return (STAIN_TYPE)0; };

        };
        
        class Vector : public style_base{
        public:
            value<IVector3> Value = IVector3();

            /**
             * @brief Construct a new Vector object.
             * @param value The initial value of the Vector object.
             * @param Default The default value state to initialize the Vector object with.
             * @details This constructor initializes the Vector object with the provided value
             *          and default value state, inheriting from the style_base class.
             */
            Vector(IVector3 value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            /**
             * @brief Construct a new Vector object using constexpr.
             * @param value The value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes a Vector object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr Vector(const GGUI::IVector3 value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value, EVALUATION_TYPE::DEFAULT, true){}

            /**
             * @brief Default constructor for Vector.
             * @details This constructor does not perform any initialization of the Vector object.
             */
            Vector() = default;
            
            /**
             * @brief Destructor for Vector.
             * @details This destructor is responsible for cleaning up all resources allocated by the Vector object.
             *          It calls the base class destructor (style_base::~style_base()) to ensure all parent class resources
             *          are properly cleaned up.
             */
            ~Vector() override { style_base::~style_base(); }

            /**
             * @brief Overload the assignment operator for Vector.
             * @param other The other Vector object to assign from.
             * @return A reference to this Vector object.
             * @details This function assigns the value and status of the other Vector object to this one.
             *          It only copies the information if the other object is enabled.
             */
            Vector& operator=(const Vector& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for Vector.
             * @param other The IVector3 object to assign from.
             * @return A reference to this Vector object.
             * @details This function assigns the value and status of the other IVector3 object to this one.
             *          It sets the status to VALUE_STATE::VALUE and the value to the given IVector3 object.
             */
            Vector& operator=(const GGUI::IVector3 other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

            /**
             * @brief Construct a new constexpr Vector object from another Vector.
             * @param other The Vector to copy from.
             * @details This constructor initializes a Vector object using the status and value from another Vector object,
             *          utilizing constexpr for compile-time evaluation.
             */
            constexpr Vector(const GGUI::STYLING_INTERNAL::Vector& other) 
                : style_base(other.Status, true), Value(other.Value) {}
            
            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details This function returns the current value stored in the Vector.
             */
            IVector3 Get() { 
                return Value.Get<IVector3>(); 
            }

            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details This function returns the current value stored in the Vector.
             *          This value can be used to get the current value of the Vector as an IVector3 object.
             */
            constexpr IVector3 Get() const { return Value.Get<IVector3>(); }

            /**
             * @brief Set the current value of the Vector.
             * @param value The new value to set the Vector to.
             * @details This function sets the current value of the Vector to the given IVector3 object.
             *          It also sets the Status of the Vector to VALUE_STATE::VALUE.
             */
            void Set(IVector3 value){
                Value = value;
                Status = VALUE_STATE::VALUE;
            }

            /**
             * @brief Get a direct reference to the value stored in the Vector.
             * @return A reference to the IVector3 object stored in the Vector.
             * @details This function returns a direct reference to the IVector3 object stored in the Vector.
             *          This can be used to directly manipulate the value of the Vector.
             */
            IVector3& Direct() { return Value.Direct<IVector3>(); }
        
            /**
             * @brief Evaluate the Vector value.
             * @param owner The Styling object that the Vector is a part of.
             * @details This function evaluates the Vector value.
             *          For dynamically computable values like percentage depended this function is overridden.
             *          Currently it covers:
             *          - screen space
             */
            void Evaluate([[maybe_unused]] Styling* owner) override {};
            
            /**
             * @brief Embeds the value of a Vector object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] Styling* host,  Element* owner) override;
        };
    
    }

    class position : public STYLING_INTERNAL::Vector{
    public:
        position(IVector3 value, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value, Default){}

        position(int x, int y, int z = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(IVector3(x, y, z), Default){}

        position() = default;

        constexpr position(const GGUI::position& other) : Vector(((const Vector&)other).Get(), other.Status, true){}

        position& operator=(const position& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;

        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class width : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        width(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        width() = default;

        constexpr width(const GGUI::width& other) : NUMBER_VALUE(other.Value.Get<int>(), other.Status, true){}

        width& operator=(const width& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;

        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;

        int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class height : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        height(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        height() = default;

        constexpr height(const GGUI::height& other) : NUMBER_VALUE(other.Value.Get<int>(), other.Status, true){}

        height& operator=(const height& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;

        int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class enable_border : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        enable_border(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        enable_border() = default;

        constexpr enable_border(const GGUI::enable_border& other) : BOOL_VALUE(other.Value, other.Status, true){}

        enable_border& operator=(const enable_border& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        text_color() = default;

        constexpr text_color(const GGUI::text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        text_color& operator=(const text_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        background_color() = default;

        constexpr background_color(const GGUI::background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        background_color& operator=(const background_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        border_color() = default;

        constexpr border_color(const GGUI::border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        border_color& operator=(const border_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        border_background_color() = default;

        constexpr border_background_color(const GGUI::border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        border_background_color& operator=(const border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class hover_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_border_color() = default;

        constexpr hover_border_color(const GGUI::hover_border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_border_color& operator=(const hover_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class hover_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_text_color() = default;

        constexpr hover_text_color(const GGUI::hover_text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_text_color& operator=(const hover_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class hover_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_background_color() = default;

        constexpr hover_background_color(const GGUI::hover_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_background_color& operator=(const hover_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class hover_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_border_background_color() = default;

        constexpr hover_border_background_color(const GGUI::hover_border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_border_background_color& operator=(const hover_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class focus_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_border_color() = default;

        constexpr focus_border_color(const GGUI::focus_border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_border_color& operator=(const focus_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class focus_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_text_color() = default;

        constexpr focus_text_color(const GGUI::focus_text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_text_color& operator=(const focus_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class focus_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_background_color() = default;

        constexpr focus_background_color(const GGUI::focus_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_background_color& operator=(const focus_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class focus_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_border_background_color() = default;

        constexpr focus_border_background_color(const GGUI::focus_border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_border_background_color& operator=(const focus_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class styled_border : public STYLING_INTERNAL::style_base{
    public:
        const char* TOP_LEFT_CORNER             = "┌";//"\e(0\x6c\e(B";
        const char* BOTTOM_LEFT_CORNER          = "└";//"\e(0\x6d\e(B";
        const char* TOP_RIGHT_CORNER            = "┐";//"\e(0\x6b\e(B";
        const char* BOTTOM_RIGHT_CORNER         = "┘";//"\e(0\x6a\e(B";
        const char* VERTICAL_LINE               = "│";//"\e(0\x78\e(B";
        const char* HORIZONTAL_LINE             = "─";//"\e(0\x71\e(B";
        const char* VERTICAL_RIGHT_CONNECTOR    = "├";//"\e(0\x74\e(B";
        const char* VERTICAL_LEFT_CONNECTOR     = "┤";//"\e(0\x75\e(B";
        const char* HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        const char* HORIZONTAL_TOP_CONNECTOR    = "┴";//"\e(0\x77\e(B";
        const char* CROSS_CONNECTOR             = "┼";//"\e(0\x6e\e(B";
    
        /**
         * @brief A structure to hold the border style of a widget.
         *
         * The style is represented as a vector of strings, each string being a character
         * that will be used to draw the border of the widget. The vector must have the
         * following size and order:
         * - 0: Top left corner
         * - 1: Bottom left corner
         * - 2: Top right corner
         * - 3: Bottom right corner
         * - 4: Vertical line
         * - 5: Horizontal line
         * - 6: Vertical right connector
         * - 7: Vertical left connector
         * - 8: Horizontal bottom connector
         * - 9: Horizontal top connector
         * - 10: Cross connector
         */
        styled_border(std::vector<const char*> values, VALUE_STATE Default = VALUE_STATE::VALUE);

        // Re-import defaults:
        styled_border() = default; // This should also call the base class
        ~styled_border() = default;
        styled_border& operator=(const styled_border& other){
            if (other.Status >= Status){
                TOP_LEFT_CORNER = other.TOP_LEFT_CORNER;
                BOTTOM_LEFT_CORNER = other.BOTTOM_LEFT_CORNER;
                TOP_RIGHT_CORNER = other.TOP_RIGHT_CORNER;
                BOTTOM_RIGHT_CORNER = other.BOTTOM_RIGHT_CORNER;
                VERTICAL_LINE = other.VERTICAL_LINE;
                HORIZONTAL_LINE = other.HORIZONTAL_LINE;
                VERTICAL_RIGHT_CONNECTOR = other.VERTICAL_RIGHT_CONNECTOR;
                VERTICAL_LEFT_CONNECTOR = other.VERTICAL_LEFT_CONNECTOR;
                HORIZONTAL_BOTTOM_CONNECTOR = other.HORIZONTAL_BOTTOM_CONNECTOR;
                HORIZONTAL_TOP_CONNECTOR = other.HORIZONTAL_TOP_CONNECTOR;
                CROSS_CONNECTOR = other.CROSS_CONNECTOR;

                Status = other.Status;
            }
            return *this;
        }
    
        constexpr styled_border(const GGUI::styled_border& other) : style_base(other.Status, true){
            TOP_LEFT_CORNER = other.TOP_LEFT_CORNER;
            BOTTOM_LEFT_CORNER = other.BOTTOM_LEFT_CORNER;
            TOP_RIGHT_CORNER = other.TOP_RIGHT_CORNER;
            BOTTOM_RIGHT_CORNER = other.BOTTOM_RIGHT_CORNER;
            VERTICAL_LINE = other.VERTICAL_LINE;
            HORIZONTAL_LINE = other.HORIZONTAL_LINE;
            VERTICAL_RIGHT_CONNECTOR = other.VERTICAL_RIGHT_CONNECTOR;
            VERTICAL_LEFT_CONNECTOR = other.VERTICAL_LEFT_CONNECTOR;
            HORIZONTAL_BOTTOM_CONNECTOR = other.HORIZONTAL_BOTTOM_CONNECTOR;
            HORIZONTAL_TOP_CONNECTOR = other.HORIZONTAL_TOP_CONNECTOR;
            CROSS_CONNECTOR = other.CROSS_CONNECTOR;
        }

        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class flow_priority : public STYLING_INTERNAL::ENUM_VALUE<DIRECTION>{
    public:
        flow_priority(DIRECTION value, VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        flow_priority() = default;

        constexpr flow_priority(const GGUI::flow_priority& other) : ENUM_VALUE(other.Value, other.Status, true){}

        flow_priority& operator=(const flow_priority& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};

        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class wrap : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        wrap(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        wrap() = default;

        constexpr wrap(const GGUI::wrap& other) : BOOL_VALUE(other.Value, other.Status, true){}

        wrap& operator=(const wrap& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class allow_overflow : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        allow_overflow(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        allow_overflow() = default;

        constexpr allow_overflow(const GGUI::allow_overflow& other) : BOOL_VALUE(other.Value, other.Status, true){}

        allow_overflow& operator=(const allow_overflow& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class allow_dynamic_size : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        allow_dynamic_size(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        allow_dynamic_size() = default;

        constexpr allow_dynamic_size(const GGUI::allow_dynamic_size& other) : BOOL_VALUE(other.Value, other.Status, true){}

        allow_dynamic_size& operator=(const allow_dynamic_size& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class margin : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<unsigned int> Top = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Bottom = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Left = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Right = (unsigned)0;

        margin(unsigned int top = 0, unsigned int bottom = 0, unsigned int left = 0, unsigned int right = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Top(top), Bottom(bottom), Left(left), Right(right){}

        // operator overload for copy operator
        margin& operator=(const margin& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Top = other.Top;
                Bottom = other.Bottom;
                Left = other.Left;
                Right = other.Right;

                Status = other.Status;
            }
            return *this;
        }

        constexpr margin(const GGUI::margin& other) : style_base(other.Status, true), Top(other.Top), Bottom(other.Bottom), Left(other.Left), Right(other.Right){}
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class shadow : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<FVector3> Direction = FVector3{0, 0, 0.5};
        STYLING_INTERNAL::value<RGB> Color = RGB{};
        float Opacity = 1;
        bool Enabled = false;

        shadow(FVector3 direction, RGB color, float opacity, bool enabled, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            Direction = direction;
            Color = color;
            Opacity = opacity;
            Enabled = enabled;
        }

        shadow() : style_base(){}

        shadow& operator=(const shadow& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Direction = other.Direction;
                Color = other.Color;
                Opacity = other.Opacity;
                Enabled = other.Enabled;

                Status = other.Status;
            }
            return *this;
        }
    
        constexpr shadow(const GGUI::shadow& other) : style_base(other.Status, true), Direction(other.Direction), Color(other.Color), Opacity(other.Opacity), Enabled(other.Enabled){}
    
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class opacity : public STYLING_INTERNAL::style_base{
    protected:
        float Value;
    public:
        opacity(float value, VALUE_STATE state) : style_base(state), Value(value){}

        opacity() = default;

        opacity& operator=(const opacity& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        constexpr opacity(const GGUI::opacity& other) : style_base(other.Status, true), Value(other.Value){}

        // Since opacity always represents an percentile of its self being displayed on top of its parent.
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;

        inline float Get() { return Value; }

        void Set(float value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class allow_scrolling : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        allow_scrolling(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        allow_scrolling() = default;

        constexpr allow_scrolling(const GGUI::allow_scrolling& other) : BOOL_VALUE(other.Value, other.Status, true){}

        allow_scrolling& operator=(const allow_scrolling& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class align : public STYLING_INTERNAL::ENUM_VALUE<ALIGN>{
    public:
        align(ALIGN value, VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        align() = default;

        constexpr align(const GGUI::align& other) : ENUM_VALUE(other.Value, other.Status, true){}

        align& operator=(const align& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class node : public STYLING_INTERNAL::style_base{
    public:
        Element* Value;

        node(Element* value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        node() = default;
        
        constexpr node(const GGUI::node& other) : style_base(other.Status, true), Value(other.Value){}

        node& operator=(const node& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] Styling* owner) override {};

        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class childs : public STYLING_INTERNAL::style_base{
    public:
        std::initializer_list<Element*> Value;

        childs(std::initializer_list<Element*> value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}
        
        childs() = default;

        constexpr childs(const GGUI::childs& other) : style_base(other.Status, true), Value(other.Value){}

        childs& operator=(const childs& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] Styling* owner) override {};

        STAIN_TYPE Embed_Value(Styling* host, Element* owner) override;
    };

    class Styling{
    public:
        position Position;

        width Width = 1;
        height Height = 1;

        enable_border                   Border_Enabled = enable_border(false, VALUE_STATE::INITIALIZED);
        text_color                      Text_Color = text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        background_color                Background_Color = background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        border_color                    Border_Color = border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        border_background_color         Border_Background_Color = border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        
        hover_border_color              Hover_Border_Color = hover_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_text_color                Hover_Text_Color = hover_text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_background_color          Hover_Background_Color = hover_background_color(COLOR::DARK_GRAY, VALUE_STATE::INITIALIZED);
        hover_border_background_color   Hover_Border_Background_Color = hover_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        focus_border_color              Focus_Border_Color = focus_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_text_color                Focus_Text_Color = focus_text_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        focus_background_color          Focus_Background_Color = focus_background_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_border_background_color   Focus_Border_Background_Color = focus_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        styled_border Border_Style;
        
        flow_priority Flow_Priority = flow_priority(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        wrap Wrap = wrap(false, VALUE_STATE::INITIALIZED);

        allow_overflow Allow_Overflow = allow_overflow(false, VALUE_STATE::INITIALIZED);
        allow_dynamic_size Allow_Dynamic_Size = allow_dynamic_size(false, VALUE_STATE::INITIALIZED);
        margin Margin;

        shadow Shadow;
        opacity Opacity = opacity(1.0f, VALUE_STATE::INITIALIZED);  // 100%

        allow_scrolling Allow_Scrolling = allow_scrolling(false, VALUE_STATE::INITIALIZED);

        align Align = align(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        std::vector<Element*> Childs;

        // The construction time given styles are first put here, before embedding them into this class.
        STYLING_INTERNAL::style_base* un_parsed_styles;

        /// Default constructor for Styling, initializes all values to their defaults.
        /// 
        /// This constructor is the default constructor for the Styling class. It will initialize all the values to their defaults.
        Styling() = default;

        Styling(STYLING_INTERNAL::style_base* attributes){
            un_parsed_styles = attributes;
        }

        Styling(STYLING_INTERNAL::style_base& attributes){
            un_parsed_styles = &attributes;
        }

        Styling(STYLING_INTERNAL::style_base&& attributes){
            un_parsed_styles = &attributes;
        }

        // This function will expect that listed elements are given as recursive style_base inheritants. Nested AST or tree based nodes are thus always captured by their preceding element nodes, which will capture all that is given to it.
        void Embed_Styles(Element* owner);

        /**
         * @brief Copies the values of the given Styling object to the current object.
         *
         * This will copy all the values of the given Styling object to the current object.
         *
         * @param other The Styling object to copy from.
         */
        void Copy(const Styling& other);

        void Copy(const Styling* other){
            // use the reference one
            Copy(*other);
        }
    
        // This acts like the Render() overload for elements, where the function will go through flagged states and checks what needs to be updated.
        // Called within the STAIN::STRETCH handling, since this is about dynamic size attributes, and they need to be checked when the current element has been resized or its parent has.
        void Evaluate_Dynamic_Attribute_Values(Element* owner);
    };

    namespace STYLES{
        namespace BORDER{
            const inline styled_border Double = std::vector<const char*>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            const inline styled_border Round = std::vector<const char*>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline styled_border Single = std::vector<const char*>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline styled_border Bold = std::vector<const char*>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            const inline styled_border Modern = std::vector<const char*>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
            
        }

        namespace CONSTANTS{
            inline Styling Default;
        }
    
        inline enable_border border = enable_border(true, VALUE_STATE::INITIALIZED);
    };

}

#endif