#ifndef _STYLE_H_
#define _STYLE_H_

#include "units.h"
#include "logger.h"

#include <variant>
#include <array>
#include <string>
#include <cassert>
#include <cstdarg>
#include <algorithm>

namespace GGUI{
    // Externies
    class element;
    class textField;
    class styling;
    enum class STAIN_TYPE;
    namespace INTERNAL{
        extern void reportStack(const std::string& problemDescription);
        extern void EXIT(int signum);

        template <typename T>
        std::string Get_Type_Name() {
        #if defined(__clang__) || defined(__GNUC__)
            constexpr const char* func = __PRETTY_FUNCTION__;
        #elif defined(_MSC_VER)
            constexpr const char* func = __FUNCSIG__;
        #else
            #error Unsupported compiler
        #endif

            // Extract the type name from the function signature
            const char* start = strstr(func, "T = ") + 4; // Find "T = " and move the pointer past it
            const char* end = strchr(start, ';');         // Find the closing bracket
            size_t length = end - start;                  // Calculate the length of the type name

            return std::string(start, length);            // Construct and return a std::string
        }
    }

    enum class ANCHOR{
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

    enum class EMBED_ORDER{
        INSTANT,
        DELAYED
    };

    enum class DIRECTION{
        ROW,
        COLUMN
    };

    enum class EVALUATION_TYPE{
        DEFAULT,        // no further evaluation needed, just return the value
        PERCENTAGE     // the value is a percentage of the parent attribute
    };

    // This namespace is an wrapper for the user not to see these !!
    namespace STYLING_INTERNAL{

        template<typename P>
        constexpr bool Is_Non_Discriminant_Scalar(const P value, const float scalar);

        template<typename P>
        constexpr std::string To_String(const P value);

        template<typename T>
        class value {
        protected:
            T data;
            float percentage;   // This will be changed later on into an std::variant holding different scaling types.
            EVALUATION_TYPE evaluation_type = EVALUATION_TYPE::DEFAULT;

        public:
            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             * @param use_constexpr Whether to use constexpr or not
             */
            constexpr value(const T d, const EVALUATION_TYPE type = EVALUATION_TYPE::DEFAULT)
                : data(d), percentage(0.0f), evaluation_type(type) {}

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
            constexpr value(const float f, const EVALUATION_TYPE type = EVALUATION_TYPE::PERCENTAGE)
                : data{}, percentage(f), evaluation_type(type) {}

            /**
             * Copy constructor
             * @param other The object to be copied from
             *
             * This constructor is used to create a copy of the value object.
             * The data and the evaluation type are copied from the other object.
             */
            constexpr value(const value<T>& other)
                : data(other.data), percentage(other.percentage), evaluation_type(other.evaluation_type) {}


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
                percentage = other.percentage;
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
            constexpr value& operator=(const T initialization_data) {
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
            constexpr value& operator=(const float initialization_data) {
                // Set the data to the float value passed in
                percentage = initialization_data;
                // Set the evaluation type to PERCENTAGE
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
                // Return the object itself, for chaining
                return *this;
            }

            constexpr bool operator==(const value<T>& other) const {
                // if (evaluation_type != other.evaluation_type){
                //     INTERNAL::reportStack("Cannot compare two different eval type values!");
                //     INTERNAL::EXIT(1);
                //     return false;   // for warnings.
                // }
                // else{
                //     switch (evaluation_type)
                //     {
                //     case EVALUATION_TYPE::DEFAULT:
                        return data == other.data;
                //     case EVALUATION_TYPE::PERCENTAGE:
                //         return percentage == other.percentage;
                //     default:
                //         INTERNAL::reportStack("Evaluation type: " + std::to_string((int)evaluation_type) + " not supported!");
                //         INTERNAL::EXIT(1);
                //         return false;   // for warnings.
                //     }
                // }
            }

            constexpr value<T> operator+(const value<T>& other){
                if (evaluation_type != other.evaluation_type){
                    INTERNAL::reportStack("Cannot add two different eval type values!");
                    INTERNAL::EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluation_type)
                    {
                    case EVALUATION_TYPE::DEFAULT:
                        return value<T>(data + other.data);
                    case EVALUATION_TYPE::PERCENTAGE:
                        return value<T>(percentage + other.percentage);
                    default:
                        INTERNAL::reportStack("Evaluation type: " + std::to_string((int)evaluation_type) + " not supported!");
                        INTERNAL::EXIT(1);
                        return value<T>(0);
                    }
                }
            }

            constexpr value<T> operator-(const value<T>& other){
                if (evaluation_type != other.evaluation_type){
                    // TODO: add capability to call reportStack in Styles.h
                    INTERNAL::LOGGER::Log("Cannot substract two different eval type values!");
                    INTERNAL::EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluation_type)
                    {
                    case EVALUATION_TYPE::DEFAULT:
                        return value<T>(data - other.data);
                    case EVALUATION_TYPE::PERCENTAGE:
                        return value<T>(percentage - other.percentage);
                    default:
                        INTERNAL::LOGGER::Log("Evaluation type: " + std::to_string((int)evaluation_type) + " not supported!");
                        INTERNAL::EXIT(1);
                        return value<T>(0);
                    }
                }
            }

            /**
             * Evaluate function
             * @param parental_value The value to be multiplied by. Only used if the evaluation type is PERCENTAGE.
             * 
             * This function is used to evaluate the value of the variant based on the evaluation type.
             * If the evaluation type is DEFAULT, the data is returned without any modification.
             * If the evaluation type is PERCENTAGE, the parental value is multiplied by the data and the result is returned.
             * If the evaluation type is not supported, an error message is printed and the data is returned without any modification.
             */
            constexpr void Evaluate(const T parental_value) {
                switch (evaluation_type) {
                    case EVALUATION_TYPE::DEFAULT:
                        // If the evaluation type is DEFAULT then just return the data without any modification
                        return;
                    case EVALUATION_TYPE::PERCENTAGE:
                        // If the evaluation type is PERCENTAGE then multiply the parental value by the data and return the result
                        data = static_cast<T>(static_cast<T>(parental_value) * percentage);

                        #if GGUI_DEBUG
                        if (Is_Non_Discriminant_Scalar<T>(parental_value, percentage)){
                            INTERNAL::LOGGER::Log("Percentage value of: '" + std::to_string(percentage) + "' causes non-discriminant results with: '" + To_String(parental_value) + "'.");
                        }
                        #endif

                        return;
                    default:
                        INTERNAL::LOGGER::Log("Evaluation type not supported!");
                        // If the evaluation type is not supported then just return the data without any modification
                        return;
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
                return data;
            }

            /**
             * Getter methods
             * @tparam P The type to cast the variant to.
             * @return The value of the variant as the requested type.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            constexpr P Get() {                
                return data;
            }

            /**
             * Get the evaluation type of the variant.
             * @return The evaluation type of the variant.
             */
            constexpr EVALUATION_TYPE Get_Type() { return evaluation_type; }

            /**
             * Get the evaluation type of the variant.
             * @return The evaluation type of the variant.
             */
            constexpr EVALUATION_TYPE Get_Type() const { return evaluation_type; }

            /**
             * @brief Direct access to the underlying data of the variant.
             *        This is an exclusive Dev function, with serious repercussions for misuse.
             * @tparam P The type to cast the variant to.
             * @return A reference to the underlying data of the variant.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            constexpr P& Direct() { 
                return data;
            }

            /**
             * @brief Set the value of the variant.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value.
             *          The evaluation type is set to EVALUATION_TYPE::DEFAULT.
             */
            constexpr void Set(const T d) {
                data = d;
                evaluation_type = EVALUATION_TYPE::DEFAULT;
            }

            /**
             * @brief Set the value of the variant to a percentage.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value, and sets the evaluation type to EVALUATION_TYPE::PERCENTAGE.
             */
            constexpr void Set(const float f){
                percentage = f;
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
            }
        };

        class style_base{
        public:
            // This is used to prevent accidental overwrites.
            VALUE_STATE Status;

            // Represents when the value is embedded.
            EMBED_ORDER Order;

            // This is used to store all appended style_bases through the operator|.
            style_base* Other;

            /**
             * @brief Construct a new constexpr style_base object.
             * @param status The status to initialize the style_base with.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr style_base(VALUE_STATE status = VALUE_STATE::UNINITIALIZED, EMBED_ORDER order = EMBED_ORDER::INSTANT) : Status(status), Order(order), Other(nullptr) {}

            /**
             * @brief Destructor of the style_base class.
             *
             * This destructor takes care of deleting the "Other" pointer if it is a heap-allocated object.
             * It does this by calling the Is_Deletable() function to check if the pointer is likely to be
             * a heap-allocated object. If it is, it deletes the object using the delete keyword.
             */
            virtual ~style_base();

            virtual style_base* Copy() const = 0;

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A pointer to the style_base that was appended to.
             * @note This bind the two objects indefinefly!
             */
            constexpr style_base* operator|(style_base* other){
                other->Other = this;

                return other;
            }

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A reference to the style_base that was appended to.
             * @note This bind the two objects indefinefly!
             */
            constexpr style_base& operator|(style_base& other){
                other.Other = this;

                return other;
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
            constexpr style_base& operator|(style_base&& other){
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
            virtual void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* host) {};

            /**
             * @brief Imprints the style's identity into the Styling object.
             * @param host The Styling object to imprint the style into.
             * @param owner The Element that owns the style.
             * @return The type of stain this style will leave.
             * @details This function allows single style classes to incorporate their unique characteristics into a Styling object. 
             *          It should be implemented by derived classes to define how the style affects the Styling and Element objects.
             */
            virtual STAIN_TYPE Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return STAIN_TYPE::CLEAN; };
        };

        class RGB_VALUE : public style_base{
        public:
            value<RGB> Value = RGB(0, 0, 0);

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The RGB value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGB_VALUE(const GGUI::RGB value, const VALUE_STATE Default = VALUE_STATE::VALUE) 
                : style_base(Default), Value(value, EVALUATION_TYPE::DEFAULT) {}

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The relative percentage of inherited from parent.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGB_VALUE(const float value, const VALUE_STATE Default = VALUE_STATE::VALUE) 
                : style_base(Default), Value(value, EVALUATION_TYPE::PERCENTAGE) {}
            
            constexpr RGB_VALUE() = default;

            /**
             * @brief Destructor for the RGB_VALUE class.
             * @details This destructor is necessary to ensure that the base class destructor is called.
             */
            inline ~RGB_VALUE() override { style_base::~style_base(); }

            inline style_base* Copy() const override {
                return new RGB_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to assign from.
             * @return A reference to this RGB_VALUE object.
             * @details This function assigns the value and status of the other RGB_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr RGB_VALUE& operator=(const RGB_VALUE& other){
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
            constexpr RGB_VALUE& operator=(const GGUI::RGB other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Overload the compare operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to compare against.
             * @return A boolean indicating whether the two RGB_VALUE objects are equal.
             * @details This function compares the value and status of the two RGB_VALUE objects.
             */
            constexpr bool operator==(const RGB_VALUE& other) const{
                return Value.Get<RGB>() == other.Value.Get<RGB>();
            }

            /**
             * @brief Overload the compare operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to compare against.
             * @return A boolean indicating whether the two RGB_VALUE objects are not equal.
             * @details This function compares the value and status of the two RGB_VALUE objects.
             */
            constexpr bool operator!=(const RGB_VALUE& other) const{
                return !(Value.Get<RGB>() == other.Value.Get<RGB>());
            }

            /**
             * @brief Copy constructor for RGB_VALUE.
             * @param other The RGB_VALUE object to copy from.
             * @details This constructor creates a new RGB_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object, and sets the status to VALUE_STATE::VALUE.
             */
            constexpr RGB_VALUE(const GGUI::STYLING_INTERNAL::RGB_VALUE& other) : style_base(other.Status), Value(other.Value){}

            /**
             * @brief Embeds the value of an RGB_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host, element* owner) override;

            /**
             * @brief Evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             */
            inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        };

        class BOOL_VALUE : public style_base{
        public:
            bool Value = false;

            /**
             * @brief Construct a new constexpr BOOL_VALUE object.
             * @param value The boolean value to set.
             * @param Default The default value state to use.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr BOOL_VALUE(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) 
                : style_base(Default), Value(value) {}
            
            constexpr BOOL_VALUE() = default;

            /**
             * @brief Destructor for the BOOL_VALUE class.
             * @details This destructor is responsible for properly deallocating all the memory
             * allocated by the BOOL_VALUE object, including its parent class resources.
             */
            inline ~BOOL_VALUE() override { style_base::~style_base(); }

            inline style_base* Copy() const override {
                return new BOOL_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for BOOL_VALUE.
             * @param other The other BOOL_VALUE object to assign from.
             * @return A reference to this BOOL_VALUE object.
             * @details This function assigns the value and status of the other BOOL_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr BOOL_VALUE& operator=(const BOOL_VALUE& other){
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
            constexpr BOOL_VALUE& operator=(const bool other){
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
                : style_base(other.Status), Value(other.Value) {}
            
            /**
             * @brief Evaluate the BOOL_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This function is a no-op for BOOL_VALUE, as it does not have any dynamically computable values.
             */
            inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
            
            /**
             * @brief Embeds the value of a BOOL_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host,  element* owner) override;
        };
        
        class NUMBER_VALUE : public style_base{
        public:
            value<int> Value = 0;

            /**
             * @brief Construct a new NUMBER_VALUE object from a float.
             * @param value The floating point value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @details This constructor initializes the NUMBER_VALUE with the provided float value and default state.
             *          The value is converted to a percentage (multiplying by 0.01) and stored as a float in the Value member.
             */
            constexpr NUMBER_VALUE(float value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value, EVALUATION_TYPE::PERCENTAGE){}

            /**
             * @brief Construct a new NUMBER_VALUE object from an integer using constexpr.
             * @param value The integer value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             * @details This constructor initializes a NUMBER_VALUE object with the provided integer value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value, EVALUATION_TYPE::DEFAULT){}
            
            constexpr NUMBER_VALUE(unsigned int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value((signed int)value, EVALUATION_TYPE::DEFAULT){}

            constexpr NUMBER_VALUE() = default;

            /**
             * @brief Destructor for NUMBER_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the NUMBER_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            inline ~NUMBER_VALUE() override { style_base::~style_base(); }

            inline style_base* Copy() const override {
                return new NUMBER_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for NUMBER_VALUE.
             * @param other The other NUMBER_VALUE object to assign from.
             * @return A reference to this NUMBER_VALUE object.
             * @details This function assigns the value and status of the other NUMBER_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr NUMBER_VALUE& operator=(const NUMBER_VALUE& other){
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
            constexpr NUMBER_VALUE& operator=(const int other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

            /**
             * @brief Overload the compare operator for the NUMBER_VALUE class.
             * @param other The other NUMBER_VALUE object to compare against.
             * @return true if the two objects are equal; false otherwise.
             * @details This function compares the value and status of the two NUMBER_VALUE objects.
             */
            constexpr bool operator==(const NUMBER_VALUE& other) const{
                return Value.Get<int>() == other.Value.Get<int>();
            }

            /**
             * @brief Overload the compare operator for the NUMBER_VALUE class.
             * @param other The other NUMBER_VALUE object to compare against.
             * @return true if the two objects are not equal; false otherwise.
             * @details This function compares the value and status of the two NUMBER_VALUE objects.
             */
            constexpr bool operator!=(const NUMBER_VALUE& other) const{
                return Value.Get<int>() != other.Value.Get<int>();
            }
        
            /**
             * @brief Construct a new NUMBER_VALUE object from another NUMBER_VALUE object using constexpr.
             * @param other The other NUMBER_VALUE object to construct from.
             * @details This constructor initializes a new NUMBER_VALUE object with the same value and status as the given object.
             */
            constexpr NUMBER_VALUE(const GGUI::STYLING_INTERNAL::NUMBER_VALUE& other) : style_base(other.Status), Value(other.Value){}
            
            /**
             * @brief Embeds the value of a NUMBER_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value(styling* host, element* owner) override;
            
            /**
             * @brief Evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             *          When called, the function should evaluate the RGB value based on the owner object and set the Value property accordingly.
             */
            inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

            /**
             * @brief Directly access the value of this NUMBER_VALUE object.
             * @return A reference to the value of this NUMBER_VALUE object.
             * @details This function returns a reference to the value of this NUMBER_VALUE object, allowing it to be directly accessed and modified.
             */
            constexpr int& Direct() { return Value.Direct<int>(); }
        };

        template<typename T>
        class ENUM_VALUE : public style_base{
        public:
            T Value;

            /**
             * @brief Construct a new ENUM_VALUE object using constexpr.
             * @param value The enum value to set.
             * @param Default The default value state.
             * @details This constructor initializes an ENUM_VALUE object with the given enum value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr ENUM_VALUE(const T value, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

            constexpr ENUM_VALUE() = default;

            /**
             * @brief Destructor for ENUM_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the ENUM_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            inline ~ENUM_VALUE() override { style_base::~style_base(); }

            inline style_base* Copy() const override {
                return new ENUM_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for ENUM_VALUE.
             * @param other The other ENUM_VALUE object to assign from.
             * @return A reference to this ENUM_VALUE object.
             * @details This function assigns the value and status of the other ENUM_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr ENUM_VALUE& operator=(const ENUM_VALUE& other){
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
            constexpr ENUM_VALUE& operator=(const T other){
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
                : style_base(other.Status), Value(other.Value) {}
                        
            /**
             * @brief Evaluate the style.
             * @param owner The Styling object that owns this style.
             * @details This function is used to evaluate the style with the given Styling object.
             *          It is used to support dynamic values like percentage depended values.
             *          The function does not do anything as of now.
             */
            inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

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
            inline STAIN_TYPE Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) override { return STAIN_TYPE::CLEAN; };
        };
        
        class Vector : public style_base{
        public:
            value<int> X = 0;
            value<int> Y = 0;
            value<int> Z = 0;

            /**
             * @brief Construct a new Vector object using constexpr.
             * @param value The value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes a Vector object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr Vector(const GGUI::IVector3 value, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), 
                X(value.X, EVALUATION_TYPE::DEFAULT), Y(value.Y, EVALUATION_TYPE::DEFAULT), Z(value.Z, EVALUATION_TYPE::DEFAULT){}
            
            constexpr Vector(const value<int> x, const value<int> y, const value<int> z = 0, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), X(x), Y(y), Z(z){}

            constexpr Vector() = default;

            /**
             * @brief Destructor for Vector.
             * @details This destructor is responsible for cleaning up all resources allocated by the Vector object.
             *          It calls the base class destructor (style_base::~style_base()) to ensure all parent class resources
             *          are properly cleaned up.
             */
            inline ~Vector() override { style_base::~style_base(); }

            inline style_base* Copy() const override {
                return new Vector(*this);
            }

            /**
             * @brief Overload the assignment operator for Vector.
             * @param other The other Vector object to assign from.
             * @return A reference to this Vector object.
             * @details This function assigns the value and status of the other Vector object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr Vector& operator=(const Vector& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    X = other.X;
                    Y = other.Y;
                    Z = other.Z;

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
            constexpr Vector& operator=(const GGUI::IVector3 other){
                X = other.X;
                Y = other.Y;
                Z = other.Z;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

            /** 
             * @brief Overload the equals to operator for Vector.
             * @param other The other Vector object to compare with.
             * @return A boolean indicating whether the two Vector objects are equal.
             */
            constexpr bool operator==(const Vector& other) const {
                return X == other.X && Y == other.Y && Z == other.Z;
            }

            /** 
             * @brief Overload the not equals to operator for Vector.
             * @param other The other Vector object to compare with.
             * @return A boolean indicating whether the two Vector objects are not equal.
             */
            constexpr bool operator!=(const Vector& other) const {
                return !(*this == other);
            }

            /**
             * @brief Construct a new constexpr Vector object from another Vector.
             * @param other The Vector to copy from.
             * @details This constructor initializes a Vector object using the status and value from another Vector object,
             *          utilizing constexpr for compile-time evaluation.
             */
            constexpr Vector(const GGUI::STYLING_INTERNAL::Vector& other) 
                : style_base(other.Status), X(other.X), Y(other.Y), Z(other.Z) {}
            
            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details Packs the values into an IVector3; Returns it.
             */
            constexpr IVector3 Get() { 
                return IVector3(X.Get<int>(), Y.Get<int>(), Z.Get<int>());
            }

            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details This function returns the current value stored in the Vector.
             *          This value can be used to get the current value of the Vector as an IVector3 object.
             */
            constexpr IVector3 Get() const { return IVector3(X.Get<int>(), Y.Get<int>(), Z.Get<int>()); }

            /**
             * @brief Set the current value of the Vector.
             * @param value The new value to set the Vector to.
             * @details This function sets the current value of the Vector to the given IVector3 object.
             *          It also sets the Status of the Vector to VALUE_STATE::VALUE.
             */
            constexpr void Set(const IVector3 value){
                X = value.X;
                Y = value.Y;
                Z = value.Z;
                Status = VALUE_STATE::VALUE;
            }

            inline Vector operator+(const Vector& other) {
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(X.Get_Type() == other.X.Get_Type() && "two different evaluation types for X");
                assert(Y.Get_Type() == other.Y.Get_Type() && "two different evaluation types for Y");
                assert(Z.Get_Type() == other.Z.Get_Type() && "two different evaluation types for Z");
                #endif

                return Vector(X + other.X, Y + other.Y, Z + other.Z);
            }

            inline Vector operator-(const Vector& other){
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(X.Get_Type() != other.X.Get_Type() && "two different evaluation types for X");
                assert(Y.Get_Type() != other.Y.Get_Type() && "two different evaluation types for Y");
                assert(Z.Get_Type() != other.Z.Get_Type() && "two different evaluation types for Z");
                #endif

                return Vector(X - other.X, Y - other.Y, Z - other.Z);
            }
        
            inline void operator+=(const IVector3 v){
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(X.Get_Type() == EVALUATION_TYPE::DEFAULT && "X is not a default evaluation type");
                assert(Y.Get_Type() == EVALUATION_TYPE::DEFAULT && "Y is not a default evaluation type");
                assert(Z.Get_Type() == EVALUATION_TYPE::DEFAULT && "Z is not a default evaluation type");
                #endif

                X.Direct<int>() += v.X;
                Y.Direct<int>() += v.Y;
                Z.Direct<int>() += v.Z;
            }

            /**
             * @brief Evaluate the Vector value.
             * @param owner The Styling object that the Vector is a part of.
             * @details This function evaluates the Vector value.
             *          For dynamically computable values like percentage depended this function is overridden.
             *          Currently it covers:
             *          - screen space
             */
            inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
            
            /**
             * @brief Embeds the value of a Vector object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host,  element* owner) override;
        };
    
        /**
         * @brief Checks if there is a leftover when multiplying two values.
         *
         * This function takes two parameters of type P and float, respectively, and checks if there is any leftover
         * when multiplying them. It does this by comparing the product of the integer parts of the parameters with
         * the product of the original parameters.
         *
         * @tparam P The type of the first parameter.
         * @param A The first parameter.
         * @param B The second parameter.
         * @return true if there is a leftover, false otherwise.
         */
        template<typename P>
        constexpr bool Has_Left_Over(const P A, const float B){
            return (static_cast<int>(A) * static_cast<int>(B)) - (static_cast<float>(A) * B) != 0;
        }

        /**
         * @brief Checks if a given value is a non-discriminant scalar.
         * @tparam P The type of the value to be checked.
         * @param value The value to be checked.
         * @param scalar The scalar value to compare against.
         * @return true if the value is a non-discriminant scalar, false otherwise.
         * 
         * @note Supported types: float | int | unsigned char | unsigned int | RGB | RGBA | FVector2 | FVector3 | IVector3 | GGUI::STYLING_INTERNAL::Vector | RGB_VALUE | NUMBER_VALUE
         */
        template<typename P>
        constexpr bool Is_Non_Discriminant_Scalar(const P value, const float scalar){
            // Skip checking for redundant scalars.
            if (scalar == 1.0f || scalar == 0.0f){
                return false;
            }

            // For types of int and float, we can use basic std::fmod
            if constexpr (std::is_same_v<P, float> || std::is_same_v<P, int> || std::is_same_v<P, unsigned char> || std::is_same_v<P, unsigned int>){
                return Has_Left_Over<P>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, RGB>){
                return Is_Non_Discriminant_Scalar<unsigned char>(value.Red, scalar) && Is_Non_Discriminant_Scalar<unsigned char>(value.Green, scalar) && Is_Non_Discriminant_Scalar<unsigned char>(value.Blue, scalar);
            }
            else if constexpr (std::is_same_v<P, RGBA>){
                return Is_Non_Discriminant_Scalar<unsigned char>(value.Alpha, scalar) && Is_Non_Discriminant_Scalar<RGB>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, FVector2>){
                return Is_Non_Discriminant_Scalar<float>(value.X, scalar) && Is_Non_Discriminant_Scalar<float>(value.Y, scalar);
            }
            else if constexpr (std::is_same_v<P, FVector3>){
                return Is_Non_Discriminant_Scalar<float>(value.Z, scalar) && Is_Non_Discriminant_Scalar<FVector2>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, IVector3>){
                return Is_Non_Discriminant_Scalar<int>(value.X, scalar) && Is_Non_Discriminant_Scalar<int>(value.Y, scalar) && Is_Non_Discriminant_Scalar<int>(value.Z, scalar);
            }
            else if constexpr (std::is_same_v<P, GGUI::STYLING_INTERNAL::Vector>){
                return Is_Non_Discriminant_Scalar<IVector3>(value.Get(), scalar);
            }
            else if constexpr (std::is_same_v<P, RGB_VALUE>){
                return Is_Non_Discriminant_Scalar<RGB>(static_cast<RGB_VALUE>(value).Value.Get<RGB>(), scalar);
            }
            else if constexpr (std::is_same_v<P, NUMBER_VALUE>){
                return Is_Non_Discriminant_Scalar<int>(static_cast<NUMBER_VALUE>(value).Value.Get<int>(), scalar);
            }
            else {
                static_assert(!std::is_same_v<P, P>, "Unsupported type!");
            }
        }

        template<typename P>
        constexpr std::string To_String(const P value){
            if constexpr (std::is_same_v<P, std::string> || std::is_same_v<P, const char*> || std::is_same_v<P, char*>){
                // These are already strings
                return value;
            }
            else if constexpr (std::is_same_v<P, float> || std::is_same_v<P, int> || std::is_same_v<P, unsigned char> || std::is_same_v<P, unsigned int>){
                return std::to_string(value);
            }
            else if constexpr (std::is_same_v<P, RGB>){
                return To_String(value.Red) + ", " + To_String(value.Green) + ", " + To_String(value.Blue);
            }
            else if constexpr (std::is_same_v<P, RGBA>){
                return To_String(static_cast<RGB>(value)) + ", " + To_String(value.Alpha);
            }
            else if constexpr (std::is_same_v<P, FVector2>){
                return To_String(value.X) + ", " + To_String(value.Y);
            }
            else if constexpr (std::is_same_v<P, FVector3>){
                return To_String(static_cast<FVector2>(value)) + ", " + To_String(value.Z);
            }
            else if constexpr (std::is_same_v<P, IVector3>){
                return To_String(value.X) + ", " + To_String(value.Y) + ", " + To_String(value.Z);
            }
            else if constexpr (std::is_same_v<P, GGUI::STYLING_INTERNAL::Vector>){
                return To_String(value.Get());
            }
            else if constexpr (std::is_same_v<P, RGB_VALUE>){
                return To_String(static_cast<RGB_VALUE>(value).Value.Get<RGB>());
            }
            else if constexpr (std::is_same_v<P, NUMBER_VALUE>){
                return To_String(static_cast<NUMBER_VALUE>(value).Value.Get<int>());
            }
            else {
                static_assert(!std::is_same_v<P, P>, "Unsupported type!");
            }
        }
    
        class empty : public style_base{
        public:
            constexpr empty() = default;

            inline ~empty() override { style_base::~style_base(); }

            inline style_base* Copy() const override {
                return new empty(*this);
            }
        };
    }

    class position : public STYLING_INTERNAL::Vector{
    public:
        constexpr position(const IVector3 value, const VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value, Default){}

        constexpr position(const Vector&& value, const VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value.X, value.Y, value.Z, Default){
            Transform_Center_To_Top_Left_Origin();
        }

        constexpr position(const Vector& value, const VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value.X, value.Y, value.Z, Default){
            Transform_Center_To_Top_Left_Origin();
        }

        // WARNING: Uses zero origin based coordinates!!!
        constexpr position(const STYLING_INTERNAL::value<int> x, const STYLING_INTERNAL::value<int> y, const STYLING_INTERNAL::value<int> z = 0, const VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(x, y, z, Default){
            Transform_Center_To_Top_Left_Origin();
        }

        inline ~position() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new position(*this);
        }

        constexpr position(const GGUI::position& other) : Vector(other){}

        constexpr position& operator=(const position& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    protected:
        /**
         * @brief Transforms the position from center origin to top-left origin.
         *
         * This function adjusts the X and Y coordinates of the position by adding a 0.5f offset
         * if their evaluation type is PERCENTAGE. This transformation is used to convert a 
         * position that is originally centered to one that is based on the top-left corner.
         *
         * @note The Z coordinate is not affected by this transformation.
         */
        constexpr void Transform_Center_To_Top_Left_Origin(){
            // Add 0.5f offset to the vectors
            if (X.Get_Type() == EVALUATION_TYPE::PERCENTAGE)
                X = X + 0.5f;
            if (Y.Get_Type() == EVALUATION_TYPE::PERCENTAGE)
                Y = Y + 0.5f;
            // no need to affect Z.
        }
    };

    class width : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        constexpr width(const int value, const VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}
        constexpr width(const unsigned int value, const VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        // When this is used we should automatically disable AllowDynamicSize
        constexpr width(const float value, const VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        inline ~width() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new width(*this);
        }

        constexpr width(const GGUI::width& other) : NUMBER_VALUE(other){}

        constexpr width& operator=(const width& other) = default;

        void Evaluate(const styling* self, const styling* owner) override;

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        constexpr int Get() const { return Value.Get<int>(); }

        constexpr void Set(const int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class height : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        constexpr height(const int value, const VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}
        constexpr height(const unsigned int value, const VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        // When this is used we should automatically disable AllowDynamicSize
        constexpr height(const float value, const VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        inline ~height() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new height(*this);
        }

        constexpr height(const GGUI::height& other) : NUMBER_VALUE(other){}

        constexpr height& operator=(const height& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        constexpr int Get() const { return Value.Get<int>(); }

        constexpr void Set(const int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class enable_border : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr enable_border(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        constexpr enable_border(const GGUI::enable_border& other) : BOOL_VALUE(other.Value, other.Status){}

        inline ~enable_border() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new enable_border(*this);
        }

        constexpr enable_border& operator=(const enable_border& other) = default;

        constexpr bool operator==(const enable_border& other) const{
            return Value == other.Value;
        }

        constexpr bool operator!=(const enable_border& other) const{
            return Value != other.Value;
        }

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr text_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr text_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~text_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new text_color(*this);
        }

        constexpr text_color(const GGUI::text_color& other) : RGB_VALUE(other){}

        constexpr text_color& operator=(const text_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr background_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr background_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~background_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new background_color(*this);
        }

        constexpr background_color(const GGUI::background_color& other) : RGB_VALUE(other){}

        constexpr background_color& operator=(const background_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr border_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr border_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~border_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new border_color(*this);
        }

        constexpr border_color(const GGUI::border_color& other) : RGB_VALUE(other){}

        constexpr border_color& operator=(const border_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr border_background_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr border_background_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~border_background_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new border_background_color(*this);
        }

        constexpr border_background_color(const GGUI::border_background_color& other) : RGB_VALUE(other){}

        constexpr border_background_color& operator=(const border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_border_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_border_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~hover_border_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new hover_border_color(*this);
        }

        constexpr hover_border_color(const GGUI::hover_border_color& other) : RGB_VALUE(other){}

        constexpr hover_border_color& operator=(const hover_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_text_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_text_color(const float  relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~hover_text_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new hover_text_color(*this);
        }

        constexpr hover_text_color(const GGUI::hover_text_color& other) : RGB_VALUE(other){}

        constexpr hover_text_color& operator=(const hover_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_background_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_background_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~hover_background_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new hover_background_color(*this);
        }

        constexpr hover_background_color(const GGUI::hover_background_color& other) : RGB_VALUE(other){}

        constexpr hover_background_color& operator=(const hover_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_border_background_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_border_background_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~hover_border_background_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new hover_border_background_color(*this);
        }

        constexpr hover_border_background_color(const GGUI::hover_border_background_color& other) : RGB_VALUE(other){}

        constexpr hover_border_background_color& operator=(const hover_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_border_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_border_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~focus_border_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new focus_border_color(*this);
        }

        constexpr focus_border_color(const GGUI::focus_border_color& other) : RGB_VALUE(other){}

        constexpr focus_border_color& operator=(const focus_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_text_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_text_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~focus_text_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new focus_text_color(*this);
        }

        constexpr focus_text_color(const GGUI::focus_text_color& other) : RGB_VALUE(other){}

        constexpr focus_text_color& operator=(const focus_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_background_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_background_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~focus_background_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new focus_background_color(*this);
        }

        constexpr focus_background_color(const GGUI::focus_background_color& other) : RGB_VALUE(other){}

        constexpr focus_background_color& operator=(const focus_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_border_background_color(const RGB color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_border_background_color(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        inline ~focus_border_background_color() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new focus_border_background_color(*this);
        }

        constexpr focus_border_background_color(const GGUI::focus_border_background_color& other) : RGB_VALUE(other){}

        constexpr focus_border_background_color& operator=(const focus_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
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
        constexpr styled_border(const std::array<const char*, 11> values, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            TOP_LEFT_CORNER = values[0];
            BOTTOM_LEFT_CORNER = values[1];
            TOP_RIGHT_CORNER = values[2];
            BOTTOM_RIGHT_CORNER = values[3];
            VERTICAL_LINE = values[4];
            HORIZONTAL_LINE = values[5];
            VERTICAL_RIGHT_CONNECTOR = values[6];
            VERTICAL_LEFT_CONNECTOR = values[7];
            HORIZONTAL_BOTTOM_CONNECTOR = values[8];
            HORIZONTAL_TOP_CONNECTOR = values[9];
            CROSS_CONNECTOR = values[10];
        }

        constexpr styled_border() = default;

        inline ~styled_border() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new styled_border(*this);
        }

        constexpr styled_border& operator=(const styled_border& other){
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
    
        constexpr styled_border(const GGUI::styled_border& other) : style_base(other.Status){
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

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        const char* get_border(const Border_Connection flags);

        Border_Connection get_border_type(const char* border);
    };

    class flow_priority : public STYLING_INTERNAL::ENUM_VALUE<DIRECTION>{
    public:
        constexpr flow_priority(const DIRECTION value, const VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        inline ~flow_priority() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new flow_priority(*this);
        }

        constexpr flow_priority(const GGUI::flow_priority& other) : ENUM_VALUE(other.Value, other.Status){}

        constexpr flow_priority& operator=(const flow_priority& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class wrap : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr wrap(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        inline ~wrap() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new wrap(*this);
        }

        constexpr wrap(const GGUI::wrap& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr wrap& operator=(const wrap& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class allow_overflow : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr allow_overflow(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        inline ~allow_overflow() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new allow_overflow(*this);
        }

        constexpr allow_overflow(const GGUI::allow_overflow& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr allow_overflow& operator=(const allow_overflow& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class allow_dynamic_size : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr allow_dynamic_size(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        inline ~allow_dynamic_size() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new allow_dynamic_size(*this);
        }

        constexpr allow_dynamic_size(const GGUI::allow_dynamic_size& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr allow_dynamic_size& operator=(const allow_dynamic_size& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class margin : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<unsigned int> Top = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Bottom = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Left = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Right = (unsigned)0;

        constexpr margin(const unsigned int top, const unsigned int bottom, const unsigned int left, const unsigned int right, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Top(top), Bottom(bottom), Left(left), Right(right){}

        constexpr margin() = default;

        inline ~margin() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new margin(*this);
        }

        // operator overload for copy operator
        constexpr margin& operator=(const margin& other){
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

        constexpr margin(const GGUI::margin& other) : style_base(other.Status), Top(other.Top), Bottom(other.Bottom), Left(other.Left), Right(other.Right){}
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class shadow : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<FVector3> Direction = FVector3{0, 0, 0.5};
        STYLING_INTERNAL::value<RGB> Color = RGB{};
        float Opacity = 1;
        bool Enabled = false;

        constexpr shadow(const FVector3 direction, const RGB color, const float opacity, const bool enabled, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            Direction = direction;
            Color = color;
            Opacity = opacity;
            Enabled = enabled;
        }

        constexpr shadow() = default;

        inline ~shadow() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new shadow(*this);
        }

        constexpr shadow& operator=(const shadow& other){
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
    
        constexpr shadow(const GGUI::shadow& other) : style_base(other.Status), Direction(other.Direction), Color(other.Color), Opacity(other.Opacity), Enabled(other.Enabled){}
    
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(const styling* self, const styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class opacity : public STYLING_INTERNAL::style_base{
    protected:
        float Value = 1.0f;
    public:
        constexpr opacity(const float value, const VALUE_STATE state = VALUE_STATE::VALUE) : style_base(state), Value(value){}

        inline ~opacity() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new opacity(*this);
        }

        constexpr opacity& operator=(const opacity& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        constexpr opacity(const GGUI::opacity& other) : style_base(other.Status), Value(other.Value){}

        // Since opacity always represents an percentile of its self being displayed on top of its parent.
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        constexpr float Get() const { return Value; }

        constexpr void Set(const float value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class allow_scrolling : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr allow_scrolling(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        inline ~allow_scrolling() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new allow_scrolling(*this);
        }

        constexpr allow_scrolling(const GGUI::allow_scrolling& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr allow_scrolling& operator=(const allow_scrolling& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class anchor : public STYLING_INTERNAL::ENUM_VALUE<ANCHOR>{
    public:
        constexpr anchor(const ANCHOR value, const VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        inline ~anchor() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new anchor(*this);
        }

        constexpr anchor(const GGUI::anchor& other) : ENUM_VALUE(other.Value, other.Status){}

        constexpr anchor& operator=(const anchor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class node : public STYLING_INTERNAL::style_base{
    public:
        element* Value;

        constexpr node(element* value = nullptr, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default, EMBED_ORDER::DELAYED), Value(value){}
        
        constexpr node(const GGUI::node& other) : style_base(other.Status, EMBED_ORDER::DELAYED), Value(other.Value){}

        inline ~node() override { style_base::~style_base(); }

        inline style_base* Copy() const override;

        constexpr node& operator=(const node& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;

                Order = other.Order;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class childs : public STYLING_INTERNAL::style_base{
    protected:
        std::array<element*, INT8_MAX> Value;
    public:

        constexpr childs(const std::initializer_list<element*> value, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default, EMBED_ORDER::DELAYED), Value{}{
            assert(value.size() <= INT8_MAX);
            for (size_t i = 0; i < value.size(); i++){
                Value[i] = *(value.begin() + i);
            }
        }

        // TODO: maybe an std::initializer_list<element> version too?

        constexpr childs(const GGUI::childs& other) : style_base(other.Status, EMBED_ORDER::DELAYED), Value(other.Value){}

        inline ~childs() override { style_base::~style_base(); }

        inline style_base* Copy() const override;

        constexpr childs& operator=(const childs& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;

                Order = other.Order;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        // -----< UTILS >-----

        // iterator fetcher to skip the nullptr tail of the Value
        inline std::array<element*, INT8_MAX>::const_iterator begin() const {
            return Value.cbegin();
        }

        // iterator fetcher to skip the nullptr tail of the Value
        inline std::array<element*, INT8_MAX>::const_iterator end() const {
            return std::find(Value.cbegin(), Value.cend(), nullptr);
        }

        inline int length() const {
            return std::distance(begin(), end());
        }
    };

    class on_init : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_init(void (*value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_init(const GGUI::on_init& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_init() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_init(*this);
        }

        constexpr on_init& operator=(const on_init& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_destroy : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_destroy(void (*value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_destroy(const GGUI::on_destroy& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_destroy() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_destroy(*this);
        }

        constexpr on_destroy& operator=(const on_destroy& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_hide : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_hide(void (*value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_hide(const GGUI::on_hide& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_hide() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_hide(*this);
        }

        constexpr on_hide& operator=(const on_hide& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_show : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_show(void (*value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_show(const GGUI::on_show& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_show() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_show(*this);
        }

        constexpr on_show& operator=(const on_show& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class name : public STYLING_INTERNAL::style_base{
    public:
        Compact_String Value;

        constexpr name(Compact_String value, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr name(const GGUI::name& other) : style_base(other.Status), Value(other.Value){}

        inline ~name() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new name(*this);
        }

        constexpr name& operator=(const name& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class title : public name{
    public:
        constexpr title(const Compact_String&& value, const VALUE_STATE Default = VALUE_STATE::VALUE) : name(value, Default){}

        inline ~title() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new title(*this);
        }

        constexpr title(const GGUI::title& other) : name(other.Value, other.Status){}

        constexpr title& operator=(const title& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        constexpr bool empty(){
            return Value.Empty();
        }
    };

    class display : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr display(const bool value, const VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        inline ~display() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new display(*this);
        }

        constexpr display(const GGUI::display& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr display& operator=(const display& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class sprite;
    class on_draw : public STYLING_INTERNAL::style_base{
    public:
        GGUI::sprite (*Value)(unsigned int x, unsigned int y);

        constexpr on_draw(GGUI::sprite (*value)(unsigned int x, unsigned int y), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}
        
        constexpr on_draw(const GGUI::on_draw& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_draw() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_draw(*this);
        }

        constexpr on_draw& operator=(const on_draw& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class text : public STYLING_INTERNAL::style_base{
    public:
        const char* Value;

        constexpr text(const char* value, const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr text(const GGUI::text& other) : style_base(other.Status), Value(other.Value){}

        inline ~text() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new text(*this);
        }

        constexpr text& operator=(const text& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_click : public STYLING_INTERNAL::style_base{
    public:
        bool (*Value)(element* self);

        constexpr on_click(bool (*value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_click(const GGUI::on_click& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_click() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_click(*this);
        }

        constexpr on_click& operator=(const on_click& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_input : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(textField* self, char c);

        constexpr on_input(void (*value)(textField* self, char c), const VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_input(const GGUI::on_input& other) : style_base(other.Status), Value(other.Value){}

        inline ~on_input() override { style_base::~style_base(); }

        inline style_base* Copy() const override {
            return new on_input(*this);
        }

        constexpr on_input& operator=(const on_input& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        inline void Evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class styling{
    public:
        position                        Position                        = position(IVector3(0, 0, 0), VALUE_STATE::INITIALIZED);

        width                           Width                           = width(1, VALUE_STATE::INITIALIZED);
        height                          Height                          = height(1, VALUE_STATE::INITIALIZED);

        title                           Title                           = title(Compact_String(nullptr, 0, true), VALUE_STATE::INITIALIZED);

        enable_border                   Border_Enabled                  = enable_border(false, VALUE_STATE::INITIALIZED);
        text_color                      Text_Color                      = text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        background_color                Background_Color                = background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        border_color                    Border_Color                    = border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        border_background_color         Border_Background_Color         = border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        
        hover_border_color              Hover_Border_Color              = hover_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_text_color                Hover_Text_Color                = hover_text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_background_color          Hover_Background_Color          = hover_background_color(COLOR::DARK_GRAY, VALUE_STATE::INITIALIZED);
        hover_border_background_color   Hover_Border_Background_Color   = hover_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        focus_border_color              Focus_Border_Color              = focus_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_text_color                Focus_Text_Color                = focus_text_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        focus_background_color          Focus_Background_Color          = focus_background_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_border_background_color   Focus_Border_Background_Color   = focus_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        styled_border                   Border_Style;
        
        flow_priority                   Flow_Priority                   = flow_priority(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        wrap                            Wrap                            = wrap(false, VALUE_STATE::INITIALIZED);

        allow_overflow                  Allow_Overflow                  = allow_overflow(false, VALUE_STATE::INITIALIZED);
        allow_dynamic_size              Allow_Dynamic_Size              = allow_dynamic_size(false, VALUE_STATE::INITIALIZED);
        margin                          Margin;

        shadow                          Shadow;
        opacity                         Opacity                         = opacity(1.0f, VALUE_STATE::INITIALIZED);  // 100%

        allow_scrolling                 Allow_Scrolling                 = allow_scrolling(false, VALUE_STATE::INITIALIZED);

        anchor                           Align                           = anchor(ANCHOR::LEFT, VALUE_STATE::INITIALIZED);

        std::vector<element*>           Childs;

        /**
         * @brief Default constructor for the Styling class.
         * 
         * This constructor initializes a new instance of the Styling class with default values.
         */
        inline styling() = default;

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A pointer to a style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::style_base* attributes){
            un_parsed_styles = attributes;
        }

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A reference to a style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::style_base& attributes){
            un_parsed_styles = &attributes;
        }

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::style_base&& attributes){
            un_parsed_styles = &attributes;
        }

        /**
         * @brief Associates the given style attributes with the current object.
         *
         * This function sets the internal pointer to the provided style_base object,
         * allowing the object to use or reference the specified styling attributes.
         *
         * @param attributes Reference to a style_base object containing the style attributes to be added.
         */
        void Add(STYLING_INTERNAL::style_base& attributes){
            un_parsed_styles = &attributes;
        }

        /**
         * @brief Adds a new style by moving the given style attributes.
         *
         * Stores a pointer to the provided style attributes, which are passed as an rvalue reference.
         * Note: The lifetime of the passed attributes must outlive the usage of un_parsed_styles.
         *
         * @param attributes Rvalue reference to a style_base object containing style attributes.
         */
        void Add(STYLING_INTERNAL::style_base&& attributes){
            un_parsed_styles = &attributes;
        }

        /**
         * Embeds the styles of the current styling object into the element.
         * 
         * This function is used to embed the styles of the current styling object into the element.
         * It takes the element as a parameter and embeds the styles into it.
         * The styles are embedded by looping through the un_parsed_styles list and calling the Embed_Value function on each attribute.
         * The Embed_Value function is responsible for embedding the attribute into the element.
         * The changes to the element are recorded in the changes variable, which is of type STAIN.
         * The type of the changes is then added to the element's stains list.
         * The function returns nothing.
         * @param owner The element to which the styles will be embedded.
         */
        void Embed_Styles(element* owner);

        /**
         * @brief Copies the values of the given Styling object to the current object.
         *
         * This will copy all the values of the given Styling object to the current object.
         *
         * @param other The Styling object to copy from.
         */
        void Copy(const styling& other);

        /**
         * @brief Copies the styling information from another Styling object.
         * 
         * This function copies the styling information from the provided 
         * Styling object pointer. It internally calls the overloaded Copy 
         * function that takes a reference to a Styling object.
         * 
         * @param other A pointer to the Styling object from which to copy the styling information.
         */
        void Copy(const styling* other){
            // use the reference one
            Copy(*other);
        }

        void Copy_Un_Parsed_Styles();
        
        // Returns the pointer of whom this evaluation is to reference.
        styling* Get_Reference(element* owner);

        /**
         * @brief Evaluates all dynamic attribute values for the owner element.
         *
         * This function evaluates the dynamic attribute values of the styling associated
         * with the specified element. It determines the point of interest, which is
         * either the element's parent or the element itself if no parent exists,
         * and uses its style as a reference for evaluation.
         *
         * @param owner The element whose dynamic attributes are to be evaluated.
         * @return True if there wae changes in the attributes evaluated, false otherwise.
         */
        bool Evaluate_Dynamic_Attribute_Values(element* owner);

        bool Evaluate_Dynamic_Position(element* owner, styling* reference = nullptr);

        bool Evaluate_Dynamic_Dimensions(element* owner, styling* reference = nullptr);

        bool Evaluate_Dynamic_Border(element* owner, styling* reference = nullptr);
    
        bool Evaluate_Dynamic_Colors(element* owner, styling* reference = nullptr);
    protected:
    
        // The construction time given styles are first put here, before embedding them into this class.
        STYLING_INTERNAL::style_base* un_parsed_styles = nullptr;
    };

    namespace STYLES{
        namespace BORDER{
            inline styled_border Double = std::array<const char*, 11>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            inline styled_border Round = std::array<const char*, 11>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            inline styled_border Single = std::array<const char*, 11>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            inline styled_border Bold = std::array<const char*, 11>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            inline styled_border Modern = std::array<const char*, 11>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
        }

        namespace CONSTANTS{
            // inline styling Default;
            inline STYLING_INTERNAL::empty Default;
            inline styling defaultStyling;
        }

        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector left = GGUI::STYLING_INTERNAL::Vector(-0.5f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector top = GGUI::STYLING_INTERNAL::Vector(0.0f, -0.5f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector right = GGUI::STYLING_INTERNAL::Vector(0.5f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector bottom = GGUI::STYLING_INTERNAL::Vector(0.0f, 0.5f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector center = GGUI::STYLING_INTERNAL::Vector(0.0f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector prioritize = GGUI::STYLING_INTERNAL::Vector(0.0f, 0.0f, POSITION::Max_Z);
    };

}

#endif