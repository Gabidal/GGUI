#ifndef _HTML_H_
#define _HTML_H_

#include "element.h"
#include "fileStreamer.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"

namespace GGUI{

    class HTML : public element{
    private:
        // DONT GIVE TO USER !!!
        HTML(){}
    private:
        fileStream* Handle = nullptr;
    public:
    
        /**
         * @brief Constructor of the HTML class.
         * @param File_Name The name of the file to open.
         * 
         * This constructor will pause the GGUI renderer and create a new file stream
         * that will read the file and parse the HTML when it is changed.
         * The parsed HTML will be set as the child of this HTML object.
         */
        HTML(std::string File_Name);

        /**
         * @brief Destructor of the HTML class.
         * 
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the HTML object.
         * It will also close the file stream associated with the HTML object.
         */
        ~HTML() override{
            if (Handle != nullptr){
                delete Handle;
            }

            // Call the base destructor to ensure that all the resources are properly
            // released.
            element::~element();
        }

        /**
         * @brief Creates a deep copy of the HTML object.
         * @return A pointer to the new HTML object.
         * 
         * This function will create a new HTML object and copy all the data from the current
         * HTML object to the new one. This is useful for creating a new HTML object that is
         * a modified version of the current one.
         */
        element* safeMove() const override {
            return new HTML();
        }

        /**
         * @brief Gets the name of the HTML object.
         * @return The name of the HTML object.
         * 
         * This function will return the name of the HTML object, which is the name
         * of the file that was opened using the HTML constructor.
         */
        std::string getName() const override{
            return "HTML<" + Name + ">";
        }
    };

    enum class HTML_GROUP_TYPES{
        UNKNOWN,
        TEXT,
        NUMBER,
        OPERATOR,   // =, 
        WRAPPER,    // <>, [], {}, (), "", ''
        SPACING,    // newline, ' ', '\t'
        ATTRIBUTE,  // Contains attributes as an wrapper extension. id="123"
    };

    enum class PARSE_BY{
        NONE                    = 0,
        TOKEN_WRAPPER           = 1 << 0,
        DYNAMIC_WRAPPER         = 1 << 1, 
        OPERATOR_PARSER         = 1 << 2,
        NUMBER_POSTFIX_PARSER   = 1 << 3,
    };

    enum class HTML_POSITION_TYPE{
        STATIC,     // Default positioning, like in GGUI.
        RELATIVE,   // Relative to parent.
        ABSOLUTE,   // Relative to screen.
        FIXED,      // Relative to screen, but does not move with scrolling.
        STICKY,     // Relative to screen until crosses given threshold.
    };

    /**
     * @brief Bitwise OR operator for PARSE_BY.
     * @details This function takes two PARSE_BY values and returns a new PARSE_BY value that is the result of the bitwise OR operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     * @return The result of the bitwise OR operation on the two input values.
     */
    extern PARSE_BY operator|(PARSE_BY first, PARSE_BY second);

    /**
     * @brief Bitwise AND operator for PARSE_BY.
     * @details This function takes two PARSE_BY values and returns a new PARSE_BY value that is the result of the bitwise AND operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     * @return The result of the bitwise AND operation on the two input values.
     */
    extern PARSE_BY operator&(PARSE_BY first, PARSE_BY second);

    /**
     * @brief Bitwise OR operator for PARSE_BY.
     * @details This function takes a PARSE_BY value and a PARSE_BY value and sets the first value to the result of the bitwise OR operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     */
    extern void operator|=(PARSE_BY& first, PARSE_BY second);

    class HTMLToken{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        std::string Data = "";
        std::vector<HTMLToken*> Childs;    // also contains attributes!
        filePosition Position;

        PARSE_BY Parsed_By = PARSE_BY::NONE;

        /**
         * @brief Constructor for an HTML_Token.
         * @param Type The type of the token.
         * @param Data The data of the token.
         */
        HTMLToken(HTML_GROUP_TYPES type, std::string data) {
            this->Type = type;
            this->Data = data;
        }

        /**
         * @brief Constructor for an HTML_Token.
         * @param Type The type of the token.
         * @param Data The data of the token.
         * @param position The position of the token in the source file.
         */
        HTMLToken(HTML_GROUP_TYPES type, char data, filePosition position){
            this->Type = type;
            this->Data.push_back(data);
            this->Position = position;
        }

        /**
         * @brief Checks if the Parsed_By contains specific bit mask.
         * @details This function takes a PARSE_BY value and checks if the bit mask of that value is set in the Parsed_By of the current token.
         * @param f The PARSE_BY value to check.
         * @return True if the bit mask of the value is set, otherwise false.
         */
        bool is(PARSE_BY f){
            return (Parsed_By & f) == f;
        }

        /**
         * @brief Checks if the Parsed_By contains specific bit mask.
         * @details This function takes a PARSE_BY value and checks if the bit mask of that value is set in the Parsed_By of the current token.
         * @param f The PARSE_BY value to check.
         * @return True if the bit mask of the value is set, otherwise false.
         */
        bool has(PARSE_BY f) {
            return (f & Parsed_By) > PARSE_BY::NONE;
        }

        /**
         * @brief Default constructor for an HTML_Token.
         * @details This constructor does not initialize any values and should be used with caution.
         */
        HTMLToken() = default;
    };

    class HTMLGroup{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        char Start = 0;
        char End = 0;
        bool Is_Sticky = true;

        /**
         * @brief Constructor for an HTML_Group.
         * @details This constructor sets the type, start and end of the group.
         * @param Type The type of the group.
         * @param Start The start of the group.
         * @param End The end of the group.
         * @param Is_Sticky Is the group sticky?
         */
        HTMLGroup(HTML_GROUP_TYPES type, char start, char end, bool is_sticky = true){
            this->Type = type;
            this->Start = start;
            this->End = end;
            this->Is_Sticky = is_sticky;
        }
    };

    class HTMLNode{
    public:
        std::string Tag_Name = "";  // DIV, HREF, etc...
        
        std::vector<HTMLNode*> Childs;
        HTMLNode* parent = nullptr;

        filePosition Position;

        HTMLToken* RAW = nullptr;
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;

        // Postfixes are in child[0] for numbers.
        // Decimals are also number typed.
        // Operators left is Child[0] and Right at Child[1].
        // Attributes cannot be computed, before some contextual data on AST level is constructed, since the postfix operands depend on these kind of information from parent.
        std::unordered_map<std::string, GGUI::HTMLToken*> Attributes;    // contains ID, Name, Class, Color, BG_Color, etc...
    };

    /**
     * @brief Parses the HTML tokens.
     * @param Input The vector of tokens to parse.
     * 
     * This function parses the HTML tokens by combining wrappers like: <, >, (, ), etc...
     * It also captures decimals, parses operators in Reverse PEMDAS order, and combines dynamic wrappers like: <html>, </html>
     */
    extern void parse(std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses raw HTML buffer into elements.
     * @param Raw_Buffer The raw HTML buffer to parse.
     * @param parent The parent element to set for top-level nodes.
     * @return A vector of parsed HTML elements.
     */
    extern std::vector<element*> parseHTML(std::string Raw_Buffer, element* parent);
    
    /**
     * @brief Parses the HTML tokens.
     * @param Input The vector of tokens to parse.
     * @return The parsed vector of HTML tokens.
     * 
     * This function parses the HTML tokens by combining wrappers like: <, >, (, ), etc...
     * It also captures decimals, parses operators in Reverse PEMDAS order, and combines dynamic wrappers like: <html>, </html>
     */
    extern std::vector<HTMLToken*>& parseHTML(std::vector<HTMLToken*>& Input);

    extern void parseEmbeddedBytes(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses all wrappers in the given vector of tokens.
     * @param i The index to start from.
     * @param Input The vector of tokens to parse.
     * 
     * This function parses all wrappers in the input, such as:
     * - <>
     * - []
     * - {}
     * - ()
     * - ""
     * - '
     */
    extern void parseAllWrappers(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses all wrappers in the given vector of tokens.
     * @param i The index to start from.
     * @param Input The vector of tokens to parse.
     * @param word The word which is used to identify the dynamic wrapper.
     * 
     * This function parses all wrappers in the input, such as:
     * - <>
     * - []
     * - {}
     * - ()
     * - ""
     * - '
     */
    extern void parseDynamicWrappers(int& i, std::vector<HTMLToken*>& Input, std::string word);

    /**
     * @brief Parses a wrapper token and all of its child tokens.
     * @param start_pattern The pattern that starts the wrapper.
     * @param end_pattern The pattern that ends the wrapper.
     * @param i The index of the start pattern in the input vector.
     * @param Input The input vector of tokens.
     * @details This function starts from the given index and every time it finds a start pattern it starts a new loop from the start pattern index until the end pattern count hits 0 and puts all of the tokens between the start and end pattern into the childs.
     *          The start pattern is not included in the child tokens.
     *          The end pattern is not included in the child tokens.
     *          The parsed by flag is set to the start pattern token.
     *          The function returns nothing, but modifies the input vector by deleting the tokens between the start and end pattern.
     *          If the start pattern is not found in the input vector, the function does nothing.
     *          If the nested count is still above 0 even after looping through all the tokens, just ignore lolw.
     */
    extern void parseWrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTMLToken*>& Input);

    extern const std::vector<HTMLGroup> Groups;

    /**
     * @brief Lexes the raw HTML string into a vector of HTML tokens.
     * @param Raw_Buffer The input HTML string to be tokenized.
     * @return A vector of pointers to HTML_Token objects.
     *
     * This function slices the given HTML text into tokens based on character groups.
     * It identifies different types of tokens such as text, numbers, operators, etc.
     * and returns a vector containing these tokens.
     */
    extern std::vector<HTMLToken*> lexHTML(std::string Raw_Buffer);

    /**
     * @brief Parses a vector of HTML tokens into HTML nodes.
     * @param Input A vector of pointers to HTML_Token objects to be parsed.
     * @return A vector of pointers to HTML_Node objects.
     *
     * This function processes each HTML token in the input vector, 
     * converts them into HTML nodes using the Factory function, 
     * and returns a vector containing these nodes. 
     * Only non-null nodes are added to the result vector.
     */
    extern std::vector<HTMLNode*> parseLexedTokens(std::vector<HTMLToken*> Input);

    extern std::unordered_map<std::string, std::function<GGUI::element* (HTMLNode*)>>* HTMLTranslators;

    extern std::unordered_map<std::string, double> POSTFIX_COEFFICIENT;

    extern std::unordered_map<std::string, void*> RELATIVE_COEFFICIENT;

    /**
     * @brief A simple hash function for strings.
     * @param str The string to be hashed.
     * @param h The current hash value.
     * @return The hashed string.
     * @details This function uses the djb2 hash algorithm to hash the given string.
     *          The hash value is computed in a way that is independent of the order of the characters in the string.
     *          The hash value is a 32-bit unsigned integer.
     */
    constexpr unsigned int hash(const char* str, int h = 0)
    {
        return !str[h] ? 5381 : (hash(str, h+1) * 33) ^ str[h];
    }


    // Helper macro to concatenate two tokens
    #define CONCAT_IMPL(x, y) x##y
    // Macro to concatenate two tokens, using the implementation macro
    #define CONCAT(x, y) CONCAT_IMPL(x, y)

    /**
     * @brief Macro to add a translator for custom HTML tag parsers.
     * @param id The identifier for the HTML tag.
     * @param handler The function handler for processing the HTML tag.
     * 
     * This macro creates a unique variable using the line number and counter.
     * It initializes the HTML_Translators map if it's not already initialized,
     * and inserts the provided id and handler into the map.
     */
    #define GGUIAddTranslator(id, handler) \
        auto CONCAT(CONCAT(_, __LINE__), __COUNTER__) = [](){ \
            if (GGUI::HTMLTranslators == nullptr){ \
                GGUI::HTMLTranslators = new std::unordered_map<std::string, std::function<GGUI::element* (GGUI::HTMLNode*)>>(); \
            } \
            return GGUI::HTMLTranslators->insert({id, handler}); \
        }();

    /**
     * @brief Parses a vector of HTML nodes into elements.
     * @param Input A vector of pointers to HTML_Node objects to be parsed.
     * @return A vector of pointers to Element objects.
     *
     * This function processes each HTML node in the input vector, 
     * tries to find a translator fitting for the node's tag name, 
     * and if found, runs it. If the translator returns a non-null element, 
     * the element is added to the result vector. The translator is run in a 
     * paused GGUI state, meaning all events and updates are paused. After the 
     * translator has been run, the processed node is removed from the input vector.
     */
    extern std::vector<element*> parseTranslators(std::vector<HTMLNode*>& Input);

    /**
     * @brief Converts an HTML token into an HTML node.
     * @param Input The input HTML token to be converted.
     * @return A pointer to an HTML node.
     *
     * This function processes the given HTML token and converts it into an HTML node.
     * It checks if the child is just a text and if so sets the type of the node to be text.
     * Then it iterates over each child of the token and tries to find attributes.
     * If an attribute is found, it's added to the node's attribute list.
     * If the child is not an attribute, it's converted into an HTML node using this function recursively.
     * Finally, the function returns the HTML node.
     */
    extern HTMLNode* factory(HTMLToken* Input);

    /**
     * @brief Parses a postfix for a numeric token.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * 
     * This function processes the token at the given index in the input vector.
     * It checks if the token is a number, and if so, it checks if the next token is a postfix.
     * If the next token is a postfix, it adds it to the current token as a child and marks the current token as having a postfix.
     * Then it removes the postfix token from the input vector.
     */
    extern void parseNumericPostfix(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses a decimal number token.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * 
     * This function processes the token at the given index in the input vector.
     * If the token is a decimal number, it checks if the left and right side of the decimal token are numbers.
     * If they are, it checks if the decimal number is valid by trying to convert it to a double.
     * If the decimal number is valid, it creates a new token with the decimal value and replaces the current token with it.
     * If the decimal number is invalid, it reports an error.
     */
    extern void parseDecimal(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses an operator token and its surrounding tokens.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * @param operator_type The character representing the operator.
     *
     * This function processes the token at the given index in the input vector.
     * It checks if the token is an operator and not already parsed by the operator parser.
     * If the current token matches the operator type and is not already parsed, it adds the
     * left and right tokens as children of the operator token and marks it as parsed by the operator parser.
     * The function also sets the type of the token to ATTRIBUTE and removes the left and right tokens
     * from the input vector. The index is updated accordingly.
     */
    extern void parseOperator(int& i, std::vector<HTMLToken*>& Input, char operator_type);

    /**
     * @brief Reports an error to the user.
     * @param problem The error message to display.
     * @param location The location of the error in the file.
     *
     * This function appends the location of the error to the error message and
     * calls the GGUI::Report function to display the error to the user.
     */
    extern void report(std::string problem, filePosition location);

    /**
     * @brief Converts an Element to an HTML node.
     * @param e The element to convert.
     * @return A pointer to an HTML node.
     *
     * This function creates an HTML node from the given element.
     * It sets the tag name of the node to "div" and the type to wrapper.
     * Then it adds the attributes of the element to the node.
     * The width and height of the element are added as number type attributes.
     */
    extern HTMLNode* elementToNode(element* e);

    /**
     * @brief Computes the value of a given token.
     * @param val The token to compute the value of.
     * @param parent The parent node of the token.
     * @param attr_name The name of the attribute to compute the value of.
     * @return The computed value of the token.
     *
     * This function is called by the translator to compute the value of a token.
     * It checks if the token is an operator and if so, it calls the Compute_Operator
     * function to compute the value of the operator. If the token is not an operator, it
     * checks if the token has a postfix and if so, it calls the Compute_Post_Fix_As_Coefficient
     * function to compute the coefficient of the postfix. The computed value is then returned.
     */
    extern double computeVal(HTMLToken* val, HTMLNode* parent, std::string attr_name);

    /**
     * @brief Computes the result of an operator token.
     * 
     * This function evaluates an operator token by computing the values
     * of its left and right child nodes and applying the operator to
     * those values. Supported operators are +, -, *, /, and =.
     * 
     * @param op The operator token to evaluate.
     * @param parent The parent HTML node of the token.
     * @param attr_name The name of the attribute associated with the token.
     * @return The computed result of the operator.
     */
    extern double computeOperator(HTMLToken* op, HTMLNode* parent, std::string attr_name);

    /**
     * @brief Computes the result of a postfix token as a coefficient.
     * @details This function evaluates a postfix token as a coefficient by
     *          multiplying the coefficient value with the specified attribute
     *          value of the parent node or the parent's parent node.
     *          Supported postfixes are vw, vh, %, vmin, and vmax.
     * @param postfix The postfix token to evaluate.
     * @param parent The parent HTML node of the token.
     * @param attr_name The name of the attribute associated with the token.
     * @return The computed coefficient.
     */
    extern double computePostFixAsCoefficient(std::string postfix, HTMLNode* parent, std::string attr_name);

    /**
     * @brief Translates an HTML node's attributes to an Element.
     * @param e The Element to translate the attributes to.
     * @param input The HTML node to translate the attributes from.
     *
     * This function translates the attributes of the given HTML node to the given Element.
     * It sets the Element's width and height attributes using the Compute_Val function.
     * It also sets the Element's flexbox properties if the flex-direction attribute is set.
     */
    extern void translateAttributesToElement(element* e, HTMLNode* input);

    /**
     * @brief Translates the child nodes of an HTML node to an Element.
     * @param e The Element to which the child nodes are to be translated.
     * @param input The HTML node containing the child nodes.
     * @param Set_Text_To A pointer to a string where the resulting text will be set.
     *
     * This function processes each child node of the given HTML node.
     * If a child node is of type TEXT, its tag name is added to the Raw_Text vector.
     * If a child node has a tag name "br", a newline character is added to the Raw_Text vector.
     * If a translator is available for a child node's tag name, the child node is translated 
     * to an Element, and the Element is added as a child to the parent Element.
     * Finally, the concatenated text from the Raw_Text vector is set to the provided string pointer.
     */
    extern void translateChildsToElement(element* e, HTMLNode* input, std::string* Set_Text_To);

}

#endif