#ifndef _STYLE_H_
#define _STYLE_H_

#include "utf.h"
#include "types.h"
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
    extern void EXIT(int signum);
    namespace INTERNAL{
        enum class STAIN_TYPE;
        extern void reportStack(const std::string& problemDescription);

        // autoGen: Ignore start
        template <typename T>
        std::string getTypeName() {
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
        // autoGen: Ignore end
    
        enum class EMBED_ORDER{
            INSTANT,
            DELAYED
        };

        enum class EVALUATION_TYPE{
            DEFAULT,        // no further evaluation needed, just return the value
            PERCENTAGE     // the value is a percentage of the parent attribute
        };
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

    enum class DIRECTION{
        ROW,
        COLUMN
    };

    // This namespace is an wrapper for the user not to see these !!
    namespace STYLING_INTERNAL{

        template<typename P>
        constexpr bool isNonDiscriminantScalar(const P value, const float scalar);

        template<typename P>
        std::string toString(const P value);

        template<typename T>
        class value {
        protected:
            T data;
            float percentage;   // This will be changed later on into an std::variant holding different scaling types.
            INTERNAL::EVALUATION_TYPE evaluationType = INTERNAL::EVALUATION_TYPE::DEFAULT;

        public:
            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             * @param use_constexpr Whether to use constexpr or not
             */
            constexpr value(const T d, const INTERNAL::EVALUATION_TYPE type = INTERNAL::EVALUATION_TYPE::DEFAULT)
                : data(d), percentage(0.0f), evaluationType(type) {}

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
            constexpr value(const float f, const INTERNAL::EVALUATION_TYPE type = INTERNAL::EVALUATION_TYPE::PERCENTAGE)
                : data{}, percentage(f), evaluationType(type) {}

            /**
             * Copy constructor
             * @param other The object to be copied from
             *
             * This constructor is used to create a copy of the value object.
             * The data and the evaluation type are copied from the other object.
             */
            constexpr value(const value<T>& other)
                : data(other.data), percentage(other.percentage), evaluationType(other.evaluationType) {}


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
                evaluationType = other.evaluationType;
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
                evaluationType = INTERNAL::EVALUATION_TYPE::DEFAULT;
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
                evaluationType = INTERNAL::EVALUATION_TYPE::PERCENTAGE;
                // Return the object itself, for chaining
                return *this;
            }

            constexpr bool operator==(const value<T>& other) const {
                // if (evaluationType != other.evaluationType){
                //     INTERNAL::reportStack("Cannot compare two different eval type values!");
                //     EXIT(1);
                //     return false;   // for warnings.
                // }
                // else{
                //     switch (evaluationType)
                //     {
                //     case INTERNAL::EVALUATION_TYPE::DEFAULT:
                        return data == other.data;
                //     case INTERNAL::EVALUATION_TYPE::PERCENTAGE:
                //         return percentage == other.percentage;
                //     default:
                //         INTERNAL::reportStack("Evaluation type: " + std::to_string((int)evaluationType) + " not supported!");
                //         EXIT(1);
                //         return false;   // for warnings.
                //     }
                // }
            }

            constexpr value<T> operator+(const value<T>& other){
                if (evaluationType != other.evaluationType){
                    INTERNAL::reportStack("Cannot add two different eval type values!");
                    EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluationType)
                    {
                    case INTERNAL::EVALUATION_TYPE::DEFAULT:
                        return value<T>(data + other.data);
                    case INTERNAL::EVALUATION_TYPE::PERCENTAGE:
                        return value<T>(percentage + other.percentage);
                    default:
                        INTERNAL::reportStack("Evaluation type: " + std::to_string((int)evaluationType) + " not supported!");
                        EXIT(1);
                        return value<T>(0);
                    }
                }
            }

            constexpr value<T> operator-(const value<T>& other){
                if (evaluationType != other.evaluationType){
                    // TODO: add capability to call reportStack in Styles.h
                    INTERNAL::LOGGER::log("Cannot substract two different eval type values!");
                    EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluationType)
                    {
                    case INTERNAL::EVALUATION_TYPE::DEFAULT:
                        return value<T>(data - other.data);
                    case INTERNAL::EVALUATION_TYPE::PERCENTAGE:
                        return value<T>(percentage - other.percentage);
                    default:
                        INTERNAL::LOGGER::log("Evaluation type: " + std::to_string((int)evaluationType) + " not supported!");
                        EXIT(1);
                        return value<T>(0);
                    }
                }
            }

            /**
             * evaluate function
             * @param parental_value The value to be multiplied by. Only used if the evaluation type is PERCENTAGE.
             * 
             * This function is used to evaluate the value of the variant based on the evaluation type.
             * If the evaluation type is DEFAULT, the data is returned without any modification.
             * If the evaluation type is PERCENTAGE, the parental value is multiplied by the data and the result is returned.
             * If the evaluation type is not supported, an error message is printed and the data is returned without any modification.
             */
            constexpr void evaluate(const T parental_value) {
                switch (evaluationType) {
                    case INTERNAL::EVALUATION_TYPE::DEFAULT:
                        // If the evaluation type is DEFAULT then just return the data without any modification
                        return;
                    case INTERNAL::EVALUATION_TYPE::PERCENTAGE:
                        // If the evaluation type is PERCENTAGE then multiply the parental value by the data and return the result
                        data = static_cast<T>(static_cast<T>(parental_value) * percentage);

                        #if GGUI_DEBUG
                        if (isNonDiscriminantScalar<T>(parental_value, percentage)){
                            INTERNAL::LOGGER::log("Percentage value of: '" + std::to_string(percentage) + "' causes non-discriminant results with: '" + toString(parental_value) + "'.");
                        }
                        #endif

                        return;
                    default:
                        INTERNAL::LOGGER::log("Evaluation type not supported!");
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
            constexpr P get() const {
                return data;
            }

            /**
             * Getter methods
             * @tparam P The type to cast the variant to.
             * @return The value of the variant as the requested type.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            constexpr P get() {                
                return data;
            }

            /**
             * Get the evaluation type of the variant.
             * @return The evaluation type of the variant.
             */
            constexpr INTERNAL::EVALUATION_TYPE getType() { return evaluationType; }

            /**
             * Get the evaluation type of the variant.
             * @return The evaluation type of the variant.
             */
            constexpr INTERNAL::EVALUATION_TYPE getType() const { return evaluationType; }

            /**
             * @brief Direct access to the underlying data of the variant.
             *        This is an exclusive Dev function, with serious repercussions for misuse.
             * @tparam P The type to cast the variant to.
             * @return A reference to the underlying data of the variant.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            constexpr P& direct() { 
                return data;
            }

            /**
             * @brief Set the value of the variant.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value.
             *          The evaluation type is set to INTERNAL::EVALUATION_TYPE::DEFAULT.
             */
            constexpr void set(const T d) {
                data = d;
                evaluationType = INTERNAL::EVALUATION_TYPE::DEFAULT;
            }

            /**
             * @brief Set the value of the variant to a percentage.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value, and sets the evaluation type to INTERNAL::EVALUATION_TYPE::PERCENTAGE.
             */
            constexpr void set(const float f){
                percentage = f;
                evaluationType = INTERNAL::EVALUATION_TYPE::PERCENTAGE;
            }
        };

        class styleBase{
        public:
            // This is used to prevent accidental overwrites.
            VALUE_STATE status;

            // Represents when the value is embedded.
            INTERNAL::EMBED_ORDER order;

            // This is used to store all appended style_bases through the operator|.
            styleBase* next;

            /**
             * @brief Construct a new constexpr style_base object.
             * @param status The status to initialize the style_base with.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr styleBase(VALUE_STATE Status = VALUE_STATE::UNINITIALIZED, INTERNAL::EMBED_ORDER Order = INTERNAL::EMBED_ORDER::INSTANT) : status(Status), order(Order), next(nullptr) {}

            /**
             * @brief Destructor of the style_base class.
             *
             * This destructor takes care of deleting the "Other" pointer if it is a heap-allocated object.
             * It does this by calling the Is_Deletable() function to check if the pointer is likely to be
             * a heap-allocated object. If it is, it deletes the object using the delete keyword.
             */
            virtual ~styleBase();

            virtual styleBase* copy() const = 0;

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A pointer to the style_base that was appended to.
             * @note This bind the two objects indefinefly!
             */
            constexpr styleBase* operator|(styleBase* other){
                other->next = this;

                return other;
            }

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A reference to the style_base that was appended to.
             * @note This bind the two objects indefinefly!
             */
            constexpr styleBase& operator|(styleBase& other){
                other.next = this;

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
            constexpr styleBase&& operator|(styleBase&& other){
                other.next = this;

                return std::move(other);
            }

            /**
             * @brief evaluates the style value based on a given Styling object.
             * @param host The Styling object to evaluate the style value with.
             * @details This function is used to evaluate the style value based on a given Styling object.
             *          It is called by the Styling class when the style value needs to be evaluated.
             *          The function is responsible for setting the Status variable to the evaluated status.
             *          The function is also responsible for setting the value variable to the evaluated value.
             *          The function should be implemented by the derived classes to perform the evaluation.
             */
            virtual void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* host) {};

            /**
             * @brief Imprints the style's identity into the Styling object.
             * @param host The Styling object to imprint the style into.
             * @param owner The Element that owns the style.
             * @return The type of stain this style will leave.
             * @details This function allows single style classes to incorporate their unique characteristics into a Styling object. 
             *          It should be implemented by derived classes to define how the style affects the Styling and Element objects.
             */
            virtual INTERNAL::STAIN_TYPE embedValue([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return INTERNAL::STAIN_TYPE::CLEAN; };
        };

        class RGBValue : public styleBase{
        public:
            value<RGB> color = RGB(0, 0, 0);

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The RGB value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGBValue(const GGUI::RGB Value, const VALUE_STATE Default = VALUE_STATE::VALUE) 
                : styleBase(Default), color(Value, INTERNAL::EVALUATION_TYPE::DEFAULT) {}

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The relative percentage of inherited from parent.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGBValue(const float Value, const VALUE_STATE Default = VALUE_STATE::VALUE) 
                : styleBase(Default), color(Value, INTERNAL::EVALUATION_TYPE::PERCENTAGE) {}
            
            constexpr RGBValue() : styleBase(), color(RGB(0,0,0)) {}

            /**
             * @brief Destructor for the RGB_VALUE class.
             * @details This destructor is necessary to ensure that the base class destructor is called.
             */
            inline ~RGBValue() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new RGBValue(*this);
            }

            /**
             * @brief Overload the assignment operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to assign from.
             * @return A reference to this RGB_VALUE object.
             * @details This function assigns the value and status of the other RGB_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr RGBValue& operator=(const RGBValue& other){
                // Only copy the information if the other is enabled.
                if (other.status >= status){
                    color = other.color;

                    status = other.status;
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
            constexpr RGBValue& operator=(const GGUI::RGB other){
                color = other;
                status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Overload the compare operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to compare against.
             * @return A boolean indicating whether the two RGB_VALUE objects are equal.
             * @details This function compares the value and status of the two RGB_VALUE objects.
             */
            constexpr bool operator==(const RGBValue& other) const{
                return color.get<RGB>() == other.color.get<RGB>();
            }

            /**
             * @brief Overload the compare operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to compare against.
             * @return A boolean indicating whether the two RGB_VALUE objects are not equal.
             * @details This function compares the value and status of the two RGB_VALUE objects.
             */
            constexpr bool operator!=(const RGBValue& other) const{
                return !(color.get<RGB>() == other.color.get<RGB>());
            }

            /**
             * @brief Copy constructor for RGB_VALUE.
             * @param other The RGB_VALUE object to copy from.
             * @details This constructor creates a new RGB_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object, and sets the status to VALUE_STATE::VALUE.
             */
            constexpr RGBValue(const GGUI::STYLING_INTERNAL::RGBValue& other) : styleBase(other.status), color(other.color){}

            /**
             * @brief Embeds the value of an RGB_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
             */
            INTERNAL::STAIN_TYPE embedValue([[maybe_unused]] styling* host, element* owner) override;

            /**
             * @brief evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             */
            inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        };

        class boolValue : public styleBase{
        public:
            bool value = false;

            /**
             * @brief Construct a new constexpr BOOL_VALUE object.
             * @param value The boolean value to set.
             * @param Default The default value state to use.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr boolValue(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) 
                : styleBase(Default), value(Value) {}
            
            constexpr boolValue() : styleBase(), value(false) {}

            /**
             * @brief Destructor for the BOOL_VALUE class.
             * @details This destructor is responsible for properly deallocating all the memory
             * allocated by the BOOL_VALUE object, including its parent class resources.
             */
            inline ~boolValue() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new boolValue(*this);
            }

            /**
             * @brief Overload the assignment operator for BOOL_VALUE.
             * @param other The other BOOL_VALUE object to assign from.
             * @return A reference to this BOOL_VALUE object.
             * @details This function assigns the value and status of the other BOOL_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr boolValue& operator=(const boolValue& other){
                // Only copy the information if the other is enabled.
                if (other.status >= status){
                    value = other.value; // Copy the value from the other object
                    
                    status = other.status; // Update the status to the other object's status
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
            constexpr boolValue& operator=(const bool other){
                value = other; // Assign the boolean value to the value member
                status = VALUE_STATE::VALUE; // Set the status to indicate a valid value
                return *this; // Return a reference to this object
            }
        
            /**
             * @brief Copy constructor for BOOL_VALUE.
             * @param other The BOOL_VALUE object to copy from.
             * @details This constructor creates a new BOOL_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object.
             */
            constexpr boolValue(const GGUI::STYLING_INTERNAL::boolValue& other) 
                : styleBase(other.status), value(other.value) {}
            
            /**
             * @brief evaluate the BOOL_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This function is a no-op for BOOL_VALUE, as it does not have any dynamically computable values.
             */
            inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
            
            /**
             * @brief Embeds the value of a BOOL_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
             */
            INTERNAL::STAIN_TYPE embedValue([[maybe_unused]] styling* host,  element* owner) override;
        };
        
        class numberValue : public styleBase{
        public:
            value<int> number = 0;

            /**
             * @brief Construct a new NUMBER_VALUE object from a float.
             * @param value The floating point value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @details This constructor initializes the NUMBER_VALUE with the provided float value and default state.
             *          The value is converted to a percentage (multiplying by 0.01) and stored as a float in the value member.
             */
            constexpr numberValue(float Value, VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), number(Value, INTERNAL::EVALUATION_TYPE::PERCENTAGE){}

            /**
             * @brief Construct a new NUMBER_VALUE object from an integer using constexpr.
             * @param value The integer value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             * @details This constructor initializes a NUMBER_VALUE object with the provided integer value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr numberValue(int Value, VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), number(Value, INTERNAL::EVALUATION_TYPE::DEFAULT){}
            
            constexpr numberValue(unsigned int Value, VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), number((signed int)Value, INTERNAL::EVALUATION_TYPE::DEFAULT){}

            constexpr numberValue() : styleBase(), number(0) {}

            /**
             * @brief Destructor for NUMBER_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the NUMBER_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            inline ~numberValue() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new numberValue(*this);
            }

            /**
             * @brief Overload the assignment operator for NUMBER_VALUE.
             * @param other The other NUMBER_VALUE object to assign from.
             * @return A reference to this NUMBER_VALUE object.
             * @details This function assigns the value and status of the other NUMBER_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr numberValue& operator=(const numberValue& other){
                // Only copy the information if the other is enabled.
                if (other.status >= status){
                    number = other.number;

                    status = other.status;
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
            constexpr numberValue& operator=(const int other){
                number = other;
                status = VALUE_STATE::VALUE;
                return *this;
            }

            /**
             * @brief Overload the compare operator for the NUMBER_VALUE class.
             * @param other The other NUMBER_VALUE object to compare against.
             * @return true if the two objects are equal; false otherwise.
             * @details This function compares the value and status of the two NUMBER_VALUE objects.
             */
            constexpr bool operator==(const numberValue& other) const{
                return number.get<int>() == other.number.get<int>();
            }

            /**
             * @brief Overload the compare operator for the NUMBER_VALUE class.
             * @param other The other NUMBER_VALUE object to compare against.
             * @return true if the two objects are not equal; false otherwise.
             * @details This function compares the value and status of the two NUMBER_VALUE objects.
             */
            constexpr bool operator!=(const numberValue& other) const{
                return number.get<int>() != other.number.get<int>();
            }
        
            /**
             * @brief Construct a new NUMBER_VALUE object from another NUMBER_VALUE object using constexpr.
             * @param other The other NUMBER_VALUE object to construct from.
             * @details This constructor initializes a new NUMBER_VALUE object with the same value and status as the given object.
             */
            constexpr numberValue(const GGUI::STYLING_INTERNAL::numberValue& other) : styleBase(other.status), number(other.number){}
            
            /**
             * @brief Embeds the value of a NUMBER_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
             */
            INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
            
            /**
             * @brief evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             *          When called, the function should evaluate the RGB value based on the owner object and set the value property accordingly.
             */
            inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

            /**
             * @brief Directly access the value of this NUMBER_VALUE object.
             * @return A reference to the value of this NUMBER_VALUE object.
             * @details This function returns a reference to the value of this NUMBER_VALUE object, allowing it to be directly accessed and modified.
             */
            constexpr int& direct() { return number.direct<int>(); }
        };

        template<typename T>
        class enumValue : public styleBase{
        public:
            T value;

            /**
             * @brief Construct a new ENUM_VALUE object using constexpr.
             * @param value The enum value to set.
             * @param Default The default value state.
             * @details This constructor initializes an ENUM_VALUE object with the given enum value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr enumValue(const T Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

            constexpr enumValue() : styleBase(), value(static_cast<T>(0)) {}

            /**
             * @brief Destructor for ENUM_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the ENUM_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            inline ~enumValue() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new enumValue(*this);
            }

            /**
             * @brief Overload the assignment operator for ENUM_VALUE.
             * @param other The other ENUM_VALUE object to assign from.
             * @return A reference to this ENUM_VALUE object.
             * @details This function assigns the value and status of the other ENUM_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr enumValue& operator=(const enumValue& other){
                // Only copy the information if the other is enabled.
                if (other.status >= status){
                    value = other.value;

                    status = other.status;
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
            constexpr enumValue& operator=(const T other){
                value = other;
                status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Copy constructor for ENUM_VALUE.
             * @param other The ENUM_VALUE object to copy from.
             * @details This constructor creates a new ENUM_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object.
             */
            constexpr enumValue(const GGUI::STYLING_INTERNAL::enumValue<T>& other) 
                : styleBase(other.status), value(other.value) {}
                        
            /**
             * @brief evaluate the style.
             * @param owner The Styling object that owns this style.
             * @details This function is used to evaluate the style with the given Styling object.
             *          It is used to support dynamic values like percentage depended values.
             *          The function does not do anything as of now.
             */
            inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

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
            inline INTERNAL::STAIN_TYPE embedValue([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) override { return INTERNAL::STAIN_TYPE::CLEAN; };
        };
        
        class vectorValue : public styleBase{
        public:
            value<int> x = 0;
            value<int> y = 0;
            value<int> z = 0;

            /**
             * @brief Construct a new Vector object using constexpr.
             * @param value The value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes a Vector object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr vectorValue(const GGUI::IVector3 Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), 
                x(Value.x, INTERNAL::EVALUATION_TYPE::DEFAULT), y(Value.y, INTERNAL::EVALUATION_TYPE::DEFAULT), z(Value.z, INTERNAL::EVALUATION_TYPE::DEFAULT){}
            
            constexpr vectorValue(const value<int> X, const value<int> Y, const value<int> Z = 0, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), x(X), y(Y), z(Z){}

            constexpr vectorValue() : styleBase(), x(0), y(0), z(0) {}

            /**
             * @brief Destructor for Vector.
             * @details This destructor is responsible for cleaning up all resources allocated by the Vector object.
             *          It calls the base class destructor (style_base::~style_base()) to ensure all parent class resources
             *          are properly cleaned up.
             */
            inline ~vectorValue() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new vectorValue(*this);
            }

            /**
             * @brief Overload the assignment operator for Vector.
             * @param other The other Vector object to assign from.
             * @return A reference to this Vector object.
             * @details This function assigns the value and status of the other Vector object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr vectorValue& operator=(const vectorValue& other){
                // Only copy the information if the other is enabled.
                if (other.status >= status){
                    x = other.x;
                    y = other.y;
                    z = other.z;

                    status = other.status;
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
            constexpr vectorValue& operator=(const GGUI::IVector3 other){
                x = other.x;
                y = other.y;
                z = other.z;
                status = VALUE_STATE::VALUE;
                return *this;
            }

            /** 
             * @brief Overload the equals to operator for Vector.
             * @param other The other Vector object to compare with.
             * @return A boolean indicating whether the two Vector objects are equal.
             */
            constexpr bool operator==(const vectorValue& other) const {
                return x == other.x && y == other.y && z == other.z;
            }

            /** 
             * @brief Overload the not equals to operator for Vector.
             * @param other The other Vector object to compare with.
             * @return A boolean indicating whether the two Vector objects are not equal.
             */
            constexpr bool operator!=(const vectorValue& other) const {
                return !(*this == other);
            }

            /**
             * @brief Construct a new constexpr Vector object from another Vector.
             * @param other The Vector to copy from.
             * @details This constructor initializes a Vector object using the status and value from another Vector object,
             *          utilizing constexpr for compile-time evaluation.
             */
            constexpr vectorValue(const GGUI::STYLING_INTERNAL::vectorValue& other) 
                : styleBase(other.status), x(other.x), y(other.y), z(other.z) {}
            
            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details Packs the values into an IVector3; Returns it.
             */
            constexpr IVector3 get() { 
                return IVector3(x.get<int>(), y.get<int>(), z.get<int>());
            }

            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details This function returns the current value stored in the Vector.
             *          This value can be used to get the current value of the Vector as an IVector3 object.
             */
            constexpr IVector3 get() const { return IVector3(x.get<int>(), y.get<int>(), z.get<int>()); }

            /**
             * @brief Set the current value of the Vector.
             * @param value The new value to set the Vector to.
             * @details This function sets the current value of the Vector to the given IVector3 object.
             *          It also sets the Status of the Vector to VALUE_STATE::VALUE.
             */
            constexpr void set(const IVector3 value){
                x = value.x;
                y = value.y;
                z = value.z;
                status = VALUE_STATE::VALUE;
            }

            inline vectorValue operator+(const vectorValue& other) {
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(x.getType() == other.x.getType() && "two different evaluation types for X");
                assert(y.getType() == other.y.getType() && "two different evaluation types for Y");
                assert(z.getType() == other.z.getType() && "two different evaluation types for Z");
                #endif

                return vectorValue(x + other.x, y + other.y, z + other.z);
            }

            inline vectorValue operator-(const vectorValue& other){
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(x.getType() != other.x.getType() && "two different evaluation types for X");
                assert(y.getType() != other.y.getType() && "two different evaluation types for Y");
                assert(z.getType() != other.z.getType() && "two different evaluation types for Z");
                #endif

                return vectorValue(x - other.x, y - other.y, z - other.z);
            }
        
            inline void operator+=(const IVector3 v){
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(x.getType() == INTERNAL::EVALUATION_TYPE::DEFAULT && "X is not a default evaluation type");
                assert(y.getType() == INTERNAL::EVALUATION_TYPE::DEFAULT && "Y is not a default evaluation type");
                assert(z.getType() == INTERNAL::EVALUATION_TYPE::DEFAULT && "Z is not a default evaluation type");
                #endif

                x.direct<int>() += v.x;
                y.direct<int>() += v.y;
                z.direct<int>() += v.z;
            }

            /**
             * @brief evaluate the Vector value.
             * @param owner The Styling object that the Vector is a part of.
             * @details This function evaluates the Vector value.
             *          For dynamically computable values like percentage depended this function is overridden.
             *          Currently it covers:
             *          - screen space
             */
            inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
            
            /**
             * @brief Embeds the value of a Vector object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
             */
            INTERNAL::STAIN_TYPE embedValue([[maybe_unused]] styling* host,  element* owner) override;
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
        constexpr bool hasLeftover(const P A, const float B){
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
        constexpr bool isNonDiscriminantScalar(const P value, const float scalar){
            // Skip checking for redundant scalars.
            if (scalar == 1.0f || scalar == 0.0f){
                return false;
            }

            // For types of int and float, we can use basic std::fmod
            if constexpr (std::is_integral<P>() || std::is_floating_point<P>()){
                return hasLeftover<P>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, RGB>){
                return isNonDiscriminantScalar<unsigned char>(value.red, scalar) && isNonDiscriminantScalar<unsigned char>(value.green, scalar) && isNonDiscriminantScalar<unsigned char>(value.blue, scalar);
            }
            else if constexpr (std::is_same_v<P, FVector2>){
                return isNonDiscriminantScalar<float>(value.X, scalar) && isNonDiscriminantScalar<float>(value.Y, scalar);
            }
            else if constexpr (std::is_same_v<P, FVector3>){
                return isNonDiscriminantScalar<float>(value.Z, scalar) && isNonDiscriminantScalar<FVector2>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, IVector3>){
                return isNonDiscriminantScalar<int>(value.X, scalar) && isNonDiscriminantScalar<int>(value.Y, scalar) && isNonDiscriminantScalar<int>(value.Z, scalar);
            }
            else if constexpr (std::is_same_v<P, GGUI::STYLING_INTERNAL::vectorValue>){
                return isNonDiscriminantScalar<IVector3>(value.Get(), scalar);
            }
            else if constexpr (std::is_same_v<P, RGBValue>){
                return isNonDiscriminantScalar<RGB>(static_cast<RGBValue>(value).color.get<RGB>(), scalar);
            }
            else if constexpr (std::is_same_v<P, numberValue>){
                return isNonDiscriminantScalar<int>(static_cast<numberValue>(value).number.get<int>(), scalar);
            }
            else {
                static_assert(!std::is_same_v<P, P>, "Unsupported type!");
            }
        }

        template<typename P>
        std::string toString(const P value){
            if constexpr (std::is_same_v<P, std::string> || std::is_same_v<P, const char*> || std::is_same_v<P, char*>){
                // These are already strings
                return value;
            }
            else if constexpr (std::is_integral<P>() || std::is_floating_point<P>()){
                return std::to_string(value);
            }
            else if constexpr (std::is_same_v<P, RGB>){
                return toString(value.red) + ", " + toString(value.green) + ", " + toString(value.blue);
            }
            else if constexpr (std::is_same_v<P, FVector2>){
                return toString(value.X) + ", " + toString(value.Y);
            }
            else if constexpr (std::is_same_v<P, FVector3>){
                return toString(static_cast<FVector2>(value)) + ", " + toString(value.Z);
            }
            else if constexpr (std::is_same_v<P, IVector3>){
                return toString(value.X) + ", " + toString(value.Y) + ", " + toString(value.Z);
            }
            else if constexpr (std::is_same_v<P, GGUI::STYLING_INTERNAL::vectorValue>){
                return toString(value.Get());
            }
            else if constexpr (std::is_same_v<P, RGBValue>){
                return toString(static_cast<RGBValue>(value).color.get<RGB>());
            }
            else if constexpr (std::is_same_v<P, numberValue>){
                return toString(static_cast<numberValue>(value).number.get<int>());
            }
            else {
                static_assert(!std::is_same_v<P, P>, "Unsupported type!");
            }
        }
    
        class empty : public styleBase{
        public:
            constexpr empty() : styleBase() {}

            inline ~empty() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new empty(*this);
            }
        };
    }

    class position : public STYLING_INTERNAL::vectorValue{
    public:
        constexpr position(const IVector3 Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : vectorValue(Value, Default){}

        constexpr position(const FVector3 Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : vectorValue(Value.x, Value.y, Value.z, Default){
            transformCenterToTopLeftOrigin();
        }

        constexpr position(const vectorValue&& Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : vectorValue(Value.x, Value.y, Value.z, Default){
            transformCenterToTopLeftOrigin();
        }

        constexpr position(const vectorValue& Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : vectorValue(Value.x, Value.y, Value.z, Default){
            transformCenterToTopLeftOrigin();
        }

        constexpr position(const STYLING_INTERNAL::value<int> X, const STYLING_INTERNAL::value<int> Y, const STYLING_INTERNAL::value<int> Z = 0, const VALUE_STATE Default = VALUE_STATE::VALUE) : vectorValue(X, Y, Z, Default){
            transformCenterToTopLeftOrigin();
        }

        inline ~position() override { vectorValue::~vectorValue(); }

        inline styleBase* copy() const override {
            return new position(*this);
        }

        constexpr position(const GGUI::position& other) : vectorValue(other){}

        position& operator=(const position& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
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
        constexpr void transformCenterToTopLeftOrigin(){
            // Add 0.5f offset to the vectors
            if (x.getType() == INTERNAL::EVALUATION_TYPE::PERCENTAGE)
                x = x + 0.5f;
            if (y.getType() == INTERNAL::EVALUATION_TYPE::PERCENTAGE)
                y = y + 0.5f;
            // no need to affect Z.
        }
    };

    class width : public STYLING_INTERNAL::numberValue{
    public:
        constexpr width(const int Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : numberValue(Value, Default){}
        constexpr width(const unsigned int Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : numberValue(Value, Default){}

        // When this is used we should automatically disable AllowDynamicSize
        constexpr width(const float Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : numberValue(Value, Default){}

        inline ~width() override { numberValue::~numberValue(); }

        inline styleBase* copy() const override {
            return new width(*this);
        }

        constexpr width(const GGUI::width& other) : numberValue(other){}

        width& operator=(const width& other) = default;

        void evaluate(const styling* self, const styling* owner) override;

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;

        constexpr int get() const { return number.get<int>(); }

        constexpr void Set(const int value){
            number = value;
            status = VALUE_STATE::VALUE;
        }
    };

    class height : public STYLING_INTERNAL::numberValue{
    public:
        constexpr height(const int Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : numberValue(Value, Default){}
        constexpr height(const unsigned int Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : numberValue(Value, Default){}

        // When this is used we should automatically disable AllowDynamicSize
        constexpr height(const float Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : numberValue(Value, Default){}

        inline ~height() override { numberValue::~numberValue(); }

        inline styleBase* copy() const override {
            return new height(*this);
        }

        constexpr height(const GGUI::height& other) : numberValue(other){}

        height& operator=(const height& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;

        constexpr int get() const { return number.get<int>(); }

        constexpr void Set(const int value){
            number = value;
            status = VALUE_STATE::VALUE;
        }
    };

    class enableBorder : public STYLING_INTERNAL::boolValue{
    public:
        constexpr enableBorder(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : boolValue(Value, Default){}

        constexpr enableBorder(const GGUI::enableBorder& other) : boolValue(other.value, other.status){}

        inline ~enableBorder() override { boolValue::~boolValue(); }

        inline styleBase* copy() const override {
            return new enableBorder(*this);
        }

        enableBorder& operator=(const enableBorder& other) = default;

        constexpr bool operator==(const enableBorder& other) const{
            return value == other.value;
        }

        constexpr bool operator!=(const enableBorder& other) const{
            return value != other.value;
        }

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class textColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr textColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr textColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~textColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new textColor(*this);
        }

        constexpr textColor(const GGUI::textColor& other) : RGBValue(other){}

        textColor& operator=(const textColor& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class backgroundColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr backgroundColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr backgroundColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~backgroundColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new backgroundColor(*this);
        }

        constexpr backgroundColor(const GGUI::backgroundColor& other) : RGBValue(other){}

        backgroundColor& operator=(const backgroundColor& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class borderColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr borderColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr borderColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~borderColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new borderColor(*this);
        }

        constexpr borderColor(const GGUI::borderColor& other) : RGBValue(other){}

        borderColor& operator=(const borderColor& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class borderBackgroundColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr borderBackgroundColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr borderBackgroundColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~borderBackgroundColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new borderBackgroundColor(*this);
        }

        constexpr borderBackgroundColor(const GGUI::borderBackgroundColor& other) : RGBValue(other){}

        borderBackgroundColor& operator=(const borderBackgroundColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class hoverBorderColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr hoverBorderColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr hoverBorderColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~hoverBorderColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new hoverBorderColor(*this);
        }

        constexpr hoverBorderColor(const GGUI::hoverBorderColor& other) : RGBValue(other){}

        hoverBorderColor& operator=(const hoverBorderColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class hoverTextColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr hoverTextColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr hoverTextColor(const float  relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~hoverTextColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new hoverTextColor(*this);
        }

        constexpr hoverTextColor(const GGUI::hoverTextColor& other) : RGBValue(other){}

        hoverTextColor& operator=(const hoverTextColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class hoverBackgroundColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr hoverBackgroundColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr hoverBackgroundColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~hoverBackgroundColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new hoverBackgroundColor(*this);
        }

        constexpr hoverBackgroundColor(const GGUI::hoverBackgroundColor& other) : RGBValue(other){}

        hoverBackgroundColor& operator=(const hoverBackgroundColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class hoverBorderBackgroundColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr hoverBorderBackgroundColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr hoverBorderBackgroundColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~hoverBorderBackgroundColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new hoverBorderBackgroundColor(*this);
        }

        constexpr hoverBorderBackgroundColor(const GGUI::hoverBorderBackgroundColor& other) : RGBValue(other){}

        hoverBorderBackgroundColor& operator=(const hoverBorderBackgroundColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class focusBorderColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr focusBorderColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr focusBorderColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~focusBorderColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new focusBorderColor(*this);
        }

        constexpr focusBorderColor(const GGUI::focusBorderColor& other) : RGBValue(other){}

        focusBorderColor& operator=(const focusBorderColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class focusTextColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr focusTextColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr focusTextColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~focusTextColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new focusTextColor(*this);
        }

        constexpr focusTextColor(const GGUI::focusTextColor& other) : RGBValue(other){}

        focusTextColor& operator=(const focusTextColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class focusBackgroundColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr focusBackgroundColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr focusBackgroundColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~focusBackgroundColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new focusBackgroundColor(*this);
        }

        constexpr focusBackgroundColor(const GGUI::focusBackgroundColor& other) : RGBValue(other){}

        focusBackgroundColor& operator=(const focusBackgroundColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class focusBorderBackgroundColor : public STYLING_INTERNAL::RGBValue{
    public:
        constexpr focusBorderBackgroundColor(const RGB Color, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(Color, Default){}

        constexpr focusBorderBackgroundColor(const float relative_percentage, const VALUE_STATE Default = VALUE_STATE::VALUE) : RGBValue(relative_percentage, Default){}

        inline ~focusBorderBackgroundColor() override { RGBValue::~RGBValue(); }

        inline styleBase* copy() const override {
            return new focusBorderBackgroundColor(*this);
        }

        constexpr focusBorderBackgroundColor(const GGUI::focusBorderBackgroundColor& other) : RGBValue(other){}

        focusBorderBackgroundColor& operator=(const focusBorderBackgroundColor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class styledBorder : public STYLING_INTERNAL::styleBase{
    public:
        const char* topLeftCorner             = "┌";//"\e(0\x6c\e(B";
        const char* bottomLeftCorner          = "└";//"\e(0\x6d\e(B";
        const char* topRightCorner            = "┐";//"\e(0\x6b\e(B";
        const char* bottomRightCorner         = "┘";//"\e(0\x6a\e(B";
        const char* verticalLine               = "│";//"\e(0\x78\e(B";
        const char* horizontalLine             = "─";//"\e(0\x71\e(B";
        const char* verticalRightConnector    = "├";//"\e(0\x74\e(B";
        const char* verticalLeftConnector     = "┤";//"\e(0\x75\e(B";
        const char* horizontalBottomConnector = "┬";//"\e(0\x76\e(B";
        const char* horizontalTopConnector    = "┴";//"\e(0\x77\e(B";
        const char* crossConnector             = "┼";//"\e(0\x6e\e(B";
    
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
        constexpr styledBorder(const std::array<const char*, 11> values, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default){
            topLeftCorner = values[0];
            bottomLeftCorner = values[1];
            topRightCorner = values[2];
            bottomRightCorner = values[3];
            verticalLine = values[4];
            horizontalLine = values[5];
            verticalRightConnector = values[6];
            verticalLeftConnector = values[7];
            horizontalBottomConnector = values[8];
            horizontalTopConnector = values[9];
            crossConnector = values[10];
        }

        constexpr styledBorder() = default;

        inline ~styledBorder() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new styledBorder(*this);
        }

        constexpr styledBorder& operator=(const styledBorder& other){
            if (other.status >= status){
                topLeftCorner = other.topLeftCorner;
                bottomLeftCorner = other.bottomLeftCorner;
                topRightCorner = other.topRightCorner;
                bottomRightCorner = other.bottomRightCorner;
                verticalLine = other.verticalLine;
                horizontalLine = other.horizontalLine;
                verticalRightConnector = other.verticalRightConnector;
                verticalLeftConnector = other.verticalLeftConnector;
                horizontalBottomConnector = other.horizontalBottomConnector;
                horizontalTopConnector = other.horizontalTopConnector;
                crossConnector = other.crossConnector;

                status = other.status;
            }
            return *this;
        }
    
        constexpr styledBorder(const GGUI::styledBorder& other) : styleBase(other.status){
            topLeftCorner = other.topLeftCorner;
            bottomLeftCorner = other.bottomLeftCorner;
            topRightCorner = other.topRightCorner;
            bottomRightCorner = other.bottomRightCorner;
            verticalLine = other.verticalLine;
            horizontalLine = other.horizontalLine;
            verticalRightConnector = other.verticalRightConnector;
            verticalLeftConnector = other.verticalLeftConnector;
            horizontalBottomConnector = other.horizontalBottomConnector;
            horizontalTopConnector = other.horizontalTopConnector;
            crossConnector = other.crossConnector;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;

        const char* getBorder(const INTERNAL::borderConnection flags);

        INTERNAL::borderConnection getBorderType(const char* border);
    };

    class flowPriority : public STYLING_INTERNAL::enumValue<DIRECTION>{
    public:
        constexpr flowPriority(const DIRECTION Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : enumValue(Value, Default){}

        inline ~flowPriority() override { enumValue<DIRECTION>::~enumValue<DIRECTION>(); }

        inline styleBase* copy() const override {
            return new flowPriority(*this);
        }

        constexpr flowPriority(const GGUI::flowPriority& other) : enumValue(other.value, other.status){}

        flowPriority& operator=(const flowPriority& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class wrap : public STYLING_INTERNAL::boolValue{
    public:
        constexpr wrap(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : boolValue(Value, Default){}

        inline ~wrap() override { boolValue::~boolValue(); }

        inline styleBase* copy() const override {
            return new wrap(*this);
        }

        constexpr wrap(const GGUI::wrap& other) : boolValue(other.value, other.status){}

        wrap& operator=(const wrap& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class allowOverflow : public STYLING_INTERNAL::boolValue{
    public:
        constexpr allowOverflow(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : boolValue(Value, Default){}

        inline ~allowOverflow() override { boolValue::~boolValue(); }

        inline styleBase* copy() const override {
            return new allowOverflow(*this);
        }

        constexpr allowOverflow(const GGUI::allowOverflow& other) : boolValue(other.value, other.status){}

        allowOverflow& operator=(const allowOverflow& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class allowDynamicSize : public STYLING_INTERNAL::boolValue{
    public:
        constexpr allowDynamicSize(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : boolValue(Value, Default){}

        inline ~allowDynamicSize() override { boolValue::~boolValue(); }

        inline styleBase* copy() const override {
            return new allowDynamicSize(*this);
        }

        constexpr allowDynamicSize(const GGUI::allowDynamicSize& other) : boolValue(other.value, other.status){}

        allowDynamicSize& operator=(const allowDynamicSize& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class margin : public STYLING_INTERNAL::styleBase{
    public:
        STYLING_INTERNAL::value<unsigned int> top = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> bottom = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> left = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> right = (unsigned)0;

        constexpr margin(const unsigned int Top, const unsigned int Bottom, const unsigned int Left, const unsigned int Right, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), top(Top), bottom(Bottom), left(Left), right(Right){}

        constexpr margin() = default;

        inline ~margin() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new margin(*this);
        }

        // operator overload for copy operator
        constexpr margin& operator=(const margin& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                top = other.top;
                bottom = other.bottom;
                left = other.left;
                right = other.right;

                status = other.status;
            }
            return *this;
        }

        constexpr margin(const GGUI::margin& other) : styleBase(other.status), top(other.top), bottom(other.bottom), left(other.left), right(other.right){}
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void evaluate(const styling* self, const styling* owner) override;
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class opacity : public STYLING_INTERNAL::styleBase{
    protected:
        unsigned char value = UINT8_MAX;        // 0..255 where 255 is fully opaque

        static constexpr unsigned char To_Opacity_Byte(const float v){
            constexpr float FULL_TRANSPARENCY = 0.0f;
            constexpr float FULL_OPACITY = 1.0f;
            constexpr float HALF_OPACITY = FULL_OPACITY / 2.0f;

            if (v <= FULL_TRANSPARENCY) return 0;
            if (v >= FULL_OPACITY) return UINT8_MAX;
            // Round to nearest so 1.0f reliably maps to 255 and mid values are stable.
            return (unsigned char)(v * (float)UINT8_MAX + HALF_OPACITY);
        }
    public:
        constexpr opacity(const float Value, const VALUE_STATE state = VALUE_STATE::VALUE) : styleBase(state), value(To_Opacity_Byte(Value)){}

        inline ~opacity() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new opacity(*this);
        }

        constexpr opacity& operator=(const opacity& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        constexpr opacity(const GGUI::opacity& other) : styleBase(other.status), value(other.value){}

        // Since opacity always represents an percentile of its self being displayed on top of its parent.
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;

        constexpr unsigned char Get() const { return value; }

        constexpr void Set(const float Value){
            value = To_Opacity_Byte(Value);
            status = VALUE_STATE::VALUE;
        }

        constexpr void Set(const unsigned char Value){
            value = Value;
            status = VALUE_STATE::VALUE;
        }
    };

    class allowScrolling : public STYLING_INTERNAL::boolValue{
    public:
        constexpr allowScrolling(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : boolValue(Value, Default){}

        inline ~allowScrolling() override { boolValue::~boolValue(); }

        inline styleBase* copy() const override {
            return new allowScrolling(*this);
        }

        constexpr allowScrolling(const GGUI::allowScrolling& other) : boolValue(other.value, other.status){}

        allowScrolling& operator=(const allowScrolling& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class anchor : public STYLING_INTERNAL::enumValue<ANCHOR>{
    public:
        constexpr anchor(const ANCHOR Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : enumValue(Value, Default){}

        inline ~anchor() override { enumValue<ANCHOR>::~enumValue<ANCHOR>(); }

        inline styleBase* copy() const override {
            return new anchor(*this);
        }

        constexpr anchor(const GGUI::anchor& other) : enumValue(other.value, other.status){}

        anchor& operator=(const anchor& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        
        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class node : public STYLING_INTERNAL::styleBase{
    public:
        element* value;

        constexpr node(element* Value = nullptr, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default, INTERNAL::EMBED_ORDER::DELAYED), value(Value){}
        
        constexpr node(const GGUI::node& other) : styleBase(other.status, INTERNAL::EMBED_ORDER::DELAYED), value(other.value){}

        inline ~node() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override;

        constexpr node& operator=(const node& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;

                order = other.order;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class childs : public STYLING_INTERNAL::styleBase{
    protected:
        std::array<element*, INT8_MAX> value;
    public:
        constexpr childs(const std::initializer_list<element*> Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default, INTERNAL::EMBED_ORDER::DELAYED), value{}{
            assert(Value.size() <= INT8_MAX);
            for (size_t i = 0; i < Value.size(); i++){
                value[i] = *(Value.begin() + i);
            }
        }

        constexpr childs(const GGUI::childs& other) : styleBase(other.status, INTERNAL::EMBED_ORDER::DELAYED), value(other.value){}

        inline ~childs() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override;

        constexpr childs& operator=(const childs& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;

                order = other.order;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;

        // -----< UTILS >-----

        // iterator fetcher to skip the nullptr tail of the value
        inline std::array<element*, INT8_MAX>::const_iterator begin() const {
            return value.cbegin();
        }

        // iterator fetcher to skip the nullptr tail of the value
        inline std::array<element*, INT8_MAX>::const_iterator end() const {
            return std::find(value.cbegin(), value.cend(), nullptr);
        }

        inline int length() const {
            return std::distance(begin(), end());
        }
    };

    class onInit : public STYLING_INTERNAL::styleBase{
    public:
        void (*value)(element* self);

        constexpr onInit(void (*Value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr onInit(const GGUI::onInit& other) : styleBase(other.status), value(other.value){}

        inline ~onInit() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onInit(*this);
        }

        constexpr onInit& operator=(const onInit& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class onDestroy : public STYLING_INTERNAL::styleBase{
    public:
        void (*value)(element* self);

        constexpr onDestroy(void (*Value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr onDestroy(const GGUI::onDestroy& other) : styleBase(other.status), value(other.value){}

        inline ~onDestroy() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onDestroy(*this);
        }

        constexpr onDestroy& operator=(const onDestroy& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class onHide : public STYLING_INTERNAL::styleBase{
    public:
        void (*value)(element* self);

        constexpr onHide(void (*Value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr onHide(const GGUI::onHide& other) : styleBase(other.status), value(other.value){}

        inline ~onHide() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onHide(*this);
        }

        constexpr onHide& operator=(const onHide& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class onShow : public STYLING_INTERNAL::styleBase{
    public:
        void (*value)(element* self);

        constexpr onShow(void (*Value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr onShow(const GGUI::onShow& other) : styleBase(other.status), value(other.value){}

        inline ~onShow() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onShow(*this);
        }

        constexpr onShow& operator=(const onShow& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class onRender : public STYLING_INTERNAL::styleBase{
    public:
        void (*value)(element* self);

        constexpr onRender(void (*Value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr onRender(const GGUI::onRender& other) : styleBase(other.status), value(other.value){}

        inline ~onRender() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onRender(*this);
        }

        constexpr onRender& operator=(const onRender& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class name : public STYLING_INTERNAL::styleBase{
    public:
        INTERNAL::compactString value;

        constexpr name(INTERNAL::compactString Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr name(const GGUI::name& other) : styleBase(other.status), value(other.value){}

        inline ~name() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new name(*this);
        }

        constexpr name& operator=(const name& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class title : public name{
    public:
        constexpr title(const INTERNAL::compactString&& Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : name(Value, Default){}

        inline ~title() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new title(*this);
        }

        constexpr title(const GGUI::title& other) : name(other.value, other.status){}

        constexpr title& operator=(const title& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;

        constexpr bool empty(){
            return value.empty();
        }
    };

    class display : public STYLING_INTERNAL::boolValue{
    public:
        constexpr display(const bool Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : boolValue(Value, Default){}

        inline ~display() override { boolValue::~boolValue(); }

        inline styleBase* copy() const override {
            return new display(*this);
        }

        constexpr display(const GGUI::display& other) : boolValue(other.value, other.status){}

        constexpr display& operator=(const display& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class sprite;
    class onDraw : public STYLING_INTERNAL::styleBase{
    public:
        GGUI::sprite (*value)(unsigned int x, unsigned int y);

        constexpr onDraw(GGUI::sprite (*Value)(unsigned int x, unsigned int y), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}
        
        constexpr onDraw(const GGUI::onDraw& other) : styleBase(other.status), value(other.value){}

        inline ~onDraw() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onDraw(*this);
        }

        constexpr onDraw& operator=(const onDraw& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class text : public STYLING_INTERNAL::styleBase{
    public:
        const char* value;

        constexpr text(const char* Value, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr text(const GGUI::text& other) : styleBase(other.status), value(other.value){}

        inline ~text() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new text(*this);
        }

        constexpr text& operator=(const text& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class onClick : public STYLING_INTERNAL::styleBase{
    public:
        bool (*value)(element* self);

        constexpr onClick(bool (*Value)(element* self), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default, INTERNAL::EMBED_ORDER::DELAYED), value(Value){}

        constexpr onClick(const GGUI::onClick& other) : styleBase(other.status, INTERNAL::EMBED_ORDER::DELAYED), value(other.value){}

        inline ~onClick() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onClick(*this);
        }

        constexpr onClick& operator=(const onClick& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
                
                order = other.order;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class onInput : public STYLING_INTERNAL::styleBase{
    public:
        void (*value)(textField* self, char c);

        constexpr onInput(void (*Value)(textField* self, char c), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), value(Value){}

        constexpr onInput(const GGUI::onInput& other) : styleBase(other.status), value(other.value){}

        inline ~onInput() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new onInput(*this);
        }

        constexpr onInput& operator=(const onInput& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                value = other.value;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class styling{
    public:
        position                      Position                        = position(IVector3(0, 0, 0), VALUE_STATE::INITIALIZED);

        width                         Width                           = width(1, VALUE_STATE::INITIALIZED);
        height                        Height                          = height(1, VALUE_STATE::INITIALIZED);

        title                         Title                           = title(INTERNAL::compactString(nullptr, 0, true), VALUE_STATE::INITIALIZED);

        enableBorder                  Border_Enabled                  = enableBorder(false, VALUE_STATE::INITIALIZED);
        textColor                     Text_Color                      = textColor(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        backgroundColor               Background_Color                = backgroundColor(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        borderColor                   Border_Color                    = borderColor(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        borderBackgroundColor         Border_Background_Color         = borderBackgroundColor(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        
        hoverBorderColor              Hover_Border_Color              = hoverBorderColor(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hoverTextColor                Hover_Text_Color                = hoverTextColor(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hoverBackgroundColor          Hover_Background_Color          = hoverBackgroundColor(COLOR::DARK_GRAY, VALUE_STATE::INITIALIZED);
        hoverBorderBackgroundColor    Hover_Border_Background_Color   = hoverBorderBackgroundColor(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        focusBorderColor              Focus_Border_Color              = focusBorderColor(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focusTextColor                Focus_Text_Color                = focusTextColor(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        focusBackgroundColor          Focus_Background_Color          = focusBackgroundColor(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focusBorderBackgroundColor    Focus_Border_Background_Color   = focusBorderBackgroundColor(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        styledBorder                  Border_Style;
        
        flowPriority                  Flow_Priority                   = flowPriority(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        wrap                          Wrap                            = wrap(false, VALUE_STATE::INITIALIZED);

        allowOverflow                 Allow_Overflow                  = allowOverflow(false, VALUE_STATE::INITIALIZED);
        allowDynamicSize              Allow_Dynamic_Size              = allowDynamicSize(false, VALUE_STATE::INITIALIZED);
        margin                        Margin;

        opacity                       Opacity                         = opacity(1.0f, VALUE_STATE::INITIALIZED);  // 100%

        allowScrolling                Allow_Scrolling                 = allowScrolling(false, VALUE_STATE::INITIALIZED);

        anchor                        Align                           = anchor(ANCHOR::LEFT, VALUE_STATE::INITIALIZED);

        std::vector<element*>         Childs;

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
        styling(STYLING_INTERNAL::styleBase* attributes){
            unParsedStyles = attributes;
        }

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A reference to a style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::styleBase& attributes){
            unParsedStyles = &attributes;
        }

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::styleBase&& attributes){
            unParsedStyles = &attributes;
        }

        /**
         * @brief Associates the given style attributes with the current object.
         *
         * This function sets the internal pointer to the provided style_base object,
         * allowing the object to use or reference the specified styling attributes.
         *
         * @param attributes Reference to a style_base object containing the style attributes to be added.
         */
        void add(STYLING_INTERNAL::styleBase& attributes){
            unParsedStyles = &attributes;
        }

        /**
         * @brief Adds a new style by moving the given style attributes.
         *
         * Stores a pointer to the provided style attributes, which are passed as an rvalue reference.
         * Note: The lifetime of the passed attributes must outlive the usage of un_parsed_styles.
         *
         * @param attributes Rvalue reference to a style_base object containing style attributes.
         */
        void add(STYLING_INTERNAL::styleBase&& attributes){
            unParsedStyles = &attributes;
        }

        /**
         * Embeds the styles of the current styling object into the element.
         * 
         * This function is used to embed the styles of the current styling object into the element.
         * It takes the element as a parameter and embeds the styles into it.
         * The styles are embedded by looping through the un_parsed_styles list and calling the Embed_value function on each attribute.
         * The Embed_value function is responsible for embedding the attribute into the element.
         * The changes to the element are recorded in the changes variable, which is of type STAIN.
         * The type of the changes is then added to the element's stains list.
         * The function returns nothing.
         * @param owner The element to which the styles will be embedded.
         */
        void embedStyles(element* owner);

        /**
         * @brief Copies the values of the given Styling object to the current object.
         *
         * This will copy all the values of the given Styling object to the current object.
         *
         * @param other The Styling object to copy from.
         */
        void copy(const styling& other);

        /**
         * @brief Copies the styling information from another Styling object.
         * 
         * This function copies the styling information from the provided 
         * Styling object pointer. It internally calls the overloaded Copy 
         * function that takes a reference to a Styling object.
         * 
         * @param other A pointer to the Styling object from which to copy the styling information.
         */
        void copy(const styling* other){
            // use the reference one
            copy(*other);
        }

        void copyUnParsedStyles();
        
        void copyUnParsedStyles(const styling* other);

        // Returns the pointer of whom this evaluation is to reference.
        styling* getReference(element* owner);

        /**
         * @brief evaluates all dynamic attribute values for the owner element.
         *
         * This function evaluates the dynamic attribute values of the styling associated
         * with the specified element. It determines the point of interest, which is
         * either the element's parent or the element itself if no parent exists,
         * and uses its style as a reference for evaluation.
         *
         * @param owner The element whose dynamic attributes are to be evaluated.
         * @return True if there wae changes in the attributes evaluated, false otherwise.
         */
        bool evaluateDynamicAttributevalues(element* owner);

        bool evaluateDynamicPosition(element* owner, styling* reference = nullptr);

        bool evaluateDynamicDimensions(element* owner, styling* reference = nullptr);

        bool evaluateDynamicBorder(element* owner, styling* reference = nullptr);
    
        bool evaluateDynamicColors(element* owner, styling* reference = nullptr);
    protected:
    
        // The construction time given styles are first put here, before embedding them into this class.
        STYLING_INTERNAL::styleBase* unParsedStyles = nullptr;
    };

    namespace STYLES{
        namespace BORDER{
            inline styledBorder Double = std::array<const char*, 11>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            inline styledBorder Round = std::array<const char*, 11>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            inline styledBorder Single = std::array<const char*, 11>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            inline styledBorder Bold = std::array<const char*, 11>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            inline styledBorder Modern = std::array<const char*, 11>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
        }

        namespace CONSTANTS{
            // inline styling Default;
            inline STYLING_INTERNAL::empty Default;
            inline styling defaultStyling;
        }

        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::vectorValue left = GGUI::STYLING_INTERNAL::vectorValue(-0.5f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::vectorValue top = GGUI::STYLING_INTERNAL::vectorValue(0.0f, -0.5f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::vectorValue right = GGUI::STYLING_INTERNAL::vectorValue(0.5f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::vectorValue bottom = GGUI::STYLING_INTERNAL::vectorValue(0.0f, 0.5f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::vectorValue center = GGUI::STYLING_INTERNAL::vectorValue(0.0f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::vectorValue prioritize = GGUI::STYLING_INTERNAL::vectorValue(0.0f, 0.0f, std::numeric_limits<short>::max());
    }

}

#endif