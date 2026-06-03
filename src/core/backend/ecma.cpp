#include "ecma.h"
#include "terminal.h"

#include "../utils/types.h"

namespace GGUI {
    namespace terminal {
        namespace ecma {
            table::configuration::page C0(table::configuration::layout::functional::getRelativeFunctionalPageLayout(table::configuration::layout::functional::type::C0));
            table::configuration::page C1(table::configuration::layout::functional::getRelativeFunctionalPageLayout(table::configuration::layout::functional::type::C1));
            table::configuration::page G0(table::configuration::layout::graphical::getRelativeGraphicalPageLayout(table::configuration::layout::graphical::type::B));

            static table::configuration::manager pageState;

            std::pair<IVector2, IVector2> components::getPresentationDirectionAsVector() {
                IVector2 linePath, characterPath;

                switch (currentStates.components.currentPresentationDirection) {
                    case sequences::presentationDirections::HORIZONTAL_TOP_LEFT_TO_BOTTOM_RIGHT:
                        linePath = {0, 1};  // top to bottom
                        characterPath = {1, 0}; // left to right
                        break;
                    case sequences::presentationDirections::HORIZONTAL_TOP_RIGHT_TO_BOTTOM_LEFT:
                        linePath = {0, 1};  // top to bottom
                        characterPath = {-1, 0}; // right to left
                        break;
                    case sequences::presentationDirections::HORIZONTAL_BOTTOM_RIGHT_TO_TOP_LEFT:
                        linePath = {0, -1};  // bottom to top
                        characterPath = {-1, 0}; // right to left
                        break;
                    case sequences::presentationDirections::HORIZONTAL_BOTTOM_LEFT_TO_TOP_RIGHT:
                        linePath = {0, -1};  // bottom to top
                        characterPath = {1, 0}; // left to right
                        break;
                    case sequences::presentationDirections::VERTICAL_TOP_LEFT_TO_BOTTOM_RIGHT:
                        linePath = {1, 0};  // left to right
                        characterPath = {0, 1}; // top to bottom
                        break;
                    case sequences::presentationDirections::VERTICAL_BOTTOM_LEFT_TO_TOP_RIGHT:
                        linePath = {1, 0};  // left to right
                        characterPath = {0, -1}; // bottom to top
                        break;
                    case sequences::presentationDirections::VERTICAL_BOTTOM_RIGHT_TO_TOP_LEFT:
                        linePath = {-1, 0};  // right to left
                        characterPath = {0, -1}; // bottom to top
                        break;
                    case sequences::presentationDirections::VERTICAL_TOP_RIGHT_TO_BOTTOM_LEFT:
                        linePath = {-1, 0};  // right to left
                        characterPath = {0, 1}; // top to bottom
                        break;
                }

                return {linePath, characterPath};
            }

            namespace sequence {
                std::string prefix::toString() const {
                    // only primary set
                    if (std::holds_alternative<table::C0>(function)) {
                        return table::toString(std::get<table::C0>(function));
                    } else {
                        return table::toString(table::C0::ESC) + table::toString(std::get<table::C1>(function));
                    }
                }

                std::string toString(std::variant<table::finalWithoutIntermediate, table::finalWithIntermediate> controlStringFinalByte) {
                    std::string result = "";

                    if (std::holds_alternative<table::finalWithoutIntermediate>(controlStringFinalByte)) {
                        result += static_cast<char>(std::get<table::finalWithoutIntermediate>(controlStringFinalByte));
                    } else {
                        result += static_cast<char>(std::get<table::finalWithIntermediate>(controlStringFinalByte));
                    }

                    return result;
                }

                /**
                 * Converts the control string to its string representation.
                 * 
                 * Control strings follow the format:
                 *   - 7-bit: ESC (01/11) + opening delimiter (C1) + characters + ESC + ST (05/12)
                 *   - 8-bit: 8-bit opening delimiter + characters + 8-bit ST (09/12)
                 * 
                 * The opening delimiter can be one of: APC, DCS, OSC, PM, or SOS from the C1 table.
                 */
                std::string string::toString() const {
                    std::string result = prefix::toString();

                    // Output all character bytes
                    for (uint8_t characterByte : characters) {
                        result += static_cast<char>(characterByte);
                    }

                    result += terminator.toString();

                    return result;
                }

                // Slices the input by the sequence::parameter::delimeter character
                std::vector<sequence::parameter::numeric> parseParameterSequence(std::string_view input) {
                    std::vector<sequence::parameter::numeric> result;
                    std::vector<size_t> delimeterIndicies;

                    // First fetch all delimeter indicies
                    for (size_t i = 0; i < input.size(); i++) if (input[i] == sequence::parameter::delimeter) delimeterIndicies.push_back(i);

                    // now we can loop through the indicies and pair each [i, i+1], and create parameters
                    for (size_t i = 0; i < delimeterIndicies.size(); i++) {
                        size_t start = delimeterIndicies[i];
                        size_t end = i == delimeterIndicies.size() - 1 ? input.size() : delimeterIndicies[i + 1];

                        size_t tmp = 0;     // Useless in this case, since we already know the sizes.
                        result.push_back({
                            input.substr(start + 1, end),   // +1 to skip the delimeter itself. 
                            tmp
                        });
                    }

                    return result;
                }

                std::vector<table::intermediate::identifiers> parseintermediates(std::string_view input) {
                    std::vector<table::intermediate::identifiers> result;

                    // Find all intermediates:
                    for (size_t i = 0; i < input.size() && table::contains<table::intermediate::identifiers>(input[i]); i++) {
                        result.push_back(static_cast<table::intermediate::identifiers>(input[i]));
                    }

                    return result;
                }

                std::pair<prefix*, size_t> parsePostfixForC1(std::string_view input) {
                    table::C1 header = static_cast<table::C1>(input.front());

                    if (header == table::C1::CSI) {
                        // Find the ending function of the current control function introducer
                        size_t finalFunctionAt = 0;
                        size_t intermediateAt = 0;

                        for (size_t i = 1; i < input.size(); i++) {
                            if (table::contains<table::finalWithoutIntermediate>(input[i]) || table::contains<table::finalWithIntermediate>(input[i])) {
                                finalFunctionAt = i;
                                break;
                            } else if (table::contains<table::intermediate::identifiers>(input[i]) && intermediateAt == 0) {    // Only set intermediate once at the first occurrence
                                intermediateAt = i;
                            }
                        }

                        // The final function has to be at a different location than the intermediate
                        bool hasIntermediate = finalFunctionAt > intermediateAt && intermediateAt != 0;
                        std::variant<
                            table::finalWithoutIntermediate,
                            table::finalWithIntermediate
                        > finalFunction;
                        
                        if (hasIntermediate) {
                            finalFunction = static_cast<table::finalWithIntermediate>(input[finalFunctionAt]);
                        } else {
                            finalFunction = static_cast<table::finalWithoutIntermediate>(input[finalFunctionAt]);
                        }

                        // Now we can create a postfix
                        postfix<std::variant<
                            table::finalWithoutIntermediate,
                            table::finalWithIntermediate
                        >> tail(parseintermediates(input.substr(intermediateAt)), finalFunction);

                        // std::max(intermediateAt, 1) is done, because the actual code CSI is at index 0, and intermediateAt == 0, means no intermediate present, thus minimum offset +1
                        size_t earliestNonParametricIndex = std::min(finalFunctionAt, std::max(intermediateAt, (size_t)1));

                        std::vector<sequence::parameter::numeric> params;

                        // Check if there are any parameters present
                        if (earliestNonParametricIndex != 0) {
                            params = parseParameterSequence(input.substr(earliestNonParametricIndex - 1));
                        }

                        return {
                            new sequence::control<sequence::parameter::numeric>(params, tail),
                            finalFunctionAt + 1
                        };

                    } else if (header == table::C1::APC || header == table::C1::DCS || header == table::C1::OSC || header == table::C1::PM || header == table::C1::SOS) {

                        // TODO: ...
                        return {nullptr, 0};

                    } else {    // TODO: ...
                        return {nullptr, 0};
                    }
                }

                std::pair<prefix*, size_t> parsePostfixForC0(std::string_view input) {
                    table::C0 header = static_cast<table::C0>(input.front());

                    if (header == table::C0::ESC) { // Proceeding bytes cannot be that of CSI, since it would have been captured at the previous parsing stage.

                        size_t startOfIntermediates = 1;

                        // Parse intermediates
                        std::vector<table::intermediate::identifiers> intermediates = parseintermediates(input.substr(startOfIntermediates));

                        sequence::function<table::independentFunctions>* result = new sequence::function<table::independentFunctions>({
                            intermediates,
                            static_cast<table::independentFunctions>(input.at(startOfIntermediates + intermediates.size()))
                        });

                        return {result, startOfIntermediates + intermediates.size()};

                    } else {
                        return {nullptr, 0};
                    }
                }

                std::pair<size_t, prefix*> defaultSequenceParser(std::string_view input) {
                    prefix* result = nullptr;

                    prefix* header = nullptr;
                    size_t i = 0;

                    // Header prefetch ------------------------------------------------
                    if (table::contains<table::C0>(input[i])) {
                        header = new prefix(static_cast<table::C0>(input[i]));

                        // We can skip ESC and set header to point into C1 if possible
                        if (header->contains(table::C0::ESC)) {     // Now we can check if i+1 contains a C1 bytecode
                            i++;

                            // We can promote the ECS + C1 code into a single 8-bit C1 bytecode
                            if (i < input.size() && table::contains<table::C1>(input[i])) {
                                header = new prefix(static_cast<table::C1>(input[i]));
                            }
                        }
                    } else if (table::contains<table::C1>(input[i]))  header = new prefix(static_cast<table::C1>(input[i]));
                    else {  // header == nullptr => means this is a graphical character
                        result = new graphicalCharacter(input[i]);
                    }
                    // ----------------------------------------------------------------

                    // Header Extension -----------------------------------------------
                    std::pair<prefix*, size_t> extension;

                    // Now we can check for extensions of prefix type class:
                    if (std::holds_alternative<table::C1>(header->getFunction())){
                        extension = parsePostfixForC1(input.substr(i));
                    } else {    // table::C0
                        extension = parsePostfixForC0(input.substr(i));
                    }

                    // Even shifts are reported for status checks, put this after shift check to disable shift reporting.
                    if (extension.first != nullptr) {
                        result = extension.first;
                        i += extension.second;
                    }
                    // ----------------------------------------------------------------

                    return {i, result};
                }

                std::vector<prefix*> parse(std::string_view input) {
                    std::vector<prefix*> result;

                    for (size_t i = 0; i < input.size(); i++) {
                        // First check while temporary loads are active from previous loop
                        auto pageCallReturn = pageState.interpret(input.substr(i));

                        pageState.update();     // refresh temporary pages

                        result.push_back(pageCallReturn.second);

                        currentStates.components.currentParsingSequenceIndex++;     // Only for META operators

                        i += pageCallReturn.first;      // TODO: check for maybe adding -1, since the loop increases 'i' either way.
                    }
                    return result;
                }
            }

            std::pair<size_t, sequence::prefix*> table::configuration::manager::interpret(std::string_view input) {
                auto currentRepertoire = map[static_cast<uint8_t>(input.front())];
                auto currentPage = pages[static_cast<size_t>(currentRepertoire)];

                // Jump through and fetch the page cell
                table::configuration::cell currentCell = currentPage.get(input.front());

                // Call the sequence parser
                auto parsedArea = currentCell.parser(input);

                if (parsedArea.first == 0) return {0, nullptr};    // No progress, means no match, return null.
                
                // Now that we have parsed the full sequence we know the header and the postfix e.g final function + intermediates
                currentCell = currentPage.get(*parsedArea.second, parsedArea.second->getPostfix());

                // Call the functionality given by the parser
                currentCell.handler(parsedArea.second);

                // return the parsed area
                return parsedArea;
            }

            namespace sequences {
                namespace delimiters {}

                namespace introducers {}

                namespace shiftFunctions {
                    auto layoutType = table::configuration::layout::graphical::type::A;     // TODO: Dynamically adjust this.

                    void operateShift_LS0(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G0, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS1(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G1, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_SS2(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G2, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            table::configuration::lifetime::types::TEMPORARY
                        );
                    }

                    void operateShift_SS3(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G3, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            table::configuration::lifetime::types::TEMPORARY
                        );
                    }

                    void operateShift_LS1R(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G1, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS2(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G2, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS2R(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G2, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS3(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G3, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS3R(sequence::prefix*) {
                        pageState.load(
                            table::configuration::repertoire::G3, 
                            table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                            table::configuration::lifetime::types::LOCKING
                        );
                    }
                    
                }

                namespace formatEffectors {
                    void operate_BACKSPACE(sequence::prefix* /*ignored*/) {
                        // First we get the direction and a base vector for the opposite direction
                        IVector2 oppositeDirection = currentStates.components.activeCharacterMovementDirection * -1;
                    
                        currentStates.components.activeDataPosition += oppositeDirection;
                    }

                    void operate_CARRIAGE_RETURN(sequence::prefix* /*ignored*/) {
                        if (currentStates.components.activeModes.has(table::mode::presets::DCSM_PRESENTATION)) {
                            if (currentStates.components.toCharacterMovementDirection(currentStates.components.activeCharacterMovementDirection) == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates.components.activePresentationPosition.x = currentStates.components.homeLinePosition.x;
                            } else {
                                currentStates.components.activePresentationPosition.x = currentStates.components.lineLimitPosition.x;
                            }
                        } else if (currentStates.components.activeModes.has(table::mode::presets::DCSM_DATA)) {
                            if (currentStates.components.toCharacterMovementDirection(currentStates.components.activeCharacterMovementDirection) == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates.components.activeDataPosition.x = currentStates.components.homeLinePosition.x;
                            } else {
                                currentStates.components.activeDataPosition.x = currentStates.components.lineLimitPosition.x;
                            }
                        }
                    }

                    void operate_FORM_FEED(sequence::prefix* /*ignored*/) {
                        // FF causes the active presentation position to be moved to the corresponding 
                        // character position of the line at the page home position of the next form or page.
                        // Move to the next page by advancing past the current active area
                        if (currentStates.components.activeArea.getUpper().row != 0) {
                            currentStates.components.activeDataPosition.y = currentStates.components.activeArea.getUpper().row + 1;
                        }
                        
                        // Set the presentation position to the home line position of the new page
                        currentStates.components.activePresentationPosition = currentStates.components.homeLinePosition;
                    }

                    void operate_CHARACTER_POSITION_ABSOLUTE(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        currentStates.components.activeDataPosition.x = params.front().getValueAsInteger();
                    }

                    void operate_CHARACTER_POSITION_BACKWARD(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        // Get the current direction vector and multiply it by the scalar of n via input and -1 to get the opposite vector.
                        auto directionVector = currentStates.components.activeCharacterMovementDirection * -static_cast<signed int>(params.front().getValueAsInteger());

                        currentStates.components.activeDataPosition += directionVector;
                    }

                    void operate_CHARACTER_POSITION_FORWARD(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        // Get the current direction vector and multiply it by the scalar of n via input to get the movement vector.
                        auto directionVector = currentStates.components.activeCharacterMovementDirection * static_cast<signed int>(params.front().getValueAsInteger());

                        currentStates.components.activeDataPosition += directionVector;
                    }
                    
                    void operate_CHARACTER_TABULATION(sequence::prefix* /*ignored*/) {
                        tabulationStop nextTabulation;

                        // Find next tabulation 
                        for (auto currentTabulation : currentStates.components.tabulationStops) {
                            if (
                                currentTabulation.position.y == currentStates.components.activePresentationPosition.y && 
                                currentTabulation.position.x >= currentStates.components.activePresentationPosition.x &&
                                currentTabulation.position.x < nextTabulation.position.x  // This is meant to find the closest next tabulation stop
                            ) {
                                nextTabulation = currentTabulation;
                            }
                        }

                        // Now we move our active presentation position into it
                        currentStates.components.activePresentationPosition = nextTabulation.position;

                        // Now we need to also enable the current tabulation mode so that the following string literals are aligned properly.
                        currentStates.components.activeTabulationAlignment = nextTabulation.mode;

                        // TODO: add here the code for detecting multi-line tabulation support and if so, also move the activeLinePosition.
                    }

                    void operate_CHARACTER_TABULATION_SET(sequence::prefix* /*ignored*/) {
                        // This function sets a tabulation stop at the current active line position and presentation position, with the current tabulation alignment mode.
                        currentStates.components.tabulationStops.push_back(tabulationStop{
                            currentStates.components.activeTabulationAlignment,
                            tabulationStop::types::CHARACTER,
                            currentStates.components.activePresentationPosition
                        });
                    }

                    void operate_CHARACTER_AND_LINE_POSITION(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 2);

                        auto y = params[0].getValueAsInteger();
                        auto x = params[1].getValueAsInteger();

                        currentStates.components.activeDataPosition = {x, y};
                    }

                    void operate_LINE_FEED(sequence::prefix* /*ignored*/) {
                        if (currentStates.components.activeModes.has(table::mode::presets::DCSM_PRESENTATION)) {
                            currentStates.components.activePresentationPosition = currentStates.components.activeDataPosition;
                        } else if (currentStates.components.activeModes.has(table::mode::presets::DCSM_DATA)) {
                            currentStates.components.activeDataPosition = currentStates.components.activePresentationPosition;
                        }
                    }

                    void operate_NEXT_LINE(sequence::prefix* /*ignored*/) {
                        bool has_presentation = currentStates.components.activeModes.has(table::mode::presets::DCSM_PRESENTATION);
                        bool has_data = currentStates.components.activeModes.has(table::mode::presets::DCSM_DATA);
                        auto movement_direction = currentStates.components.toCharacterMovementDirection(currentStates.components.activeCharacterMovementDirection);
                        
                        if (has_presentation) {
                            if (movement_direction == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates.components.activePresentationPosition.y = currentStates.components.homeLinePosition.y;
                            } else {
                                currentStates.components.activePresentationPosition.y = currentStates.components.lineLimitPosition.y;
                            }
                        } else if (has_data) {
                            if (movement_direction == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates.components.activeDataPosition.y = currentStates.components.homeLinePosition.y;
                            } else {
                                currentStates.components.activeDataPosition.y = currentStates.components.lineLimitPosition.y;
                            }
                        }
                    }

                    void operate_PARTIAL_LINE_FORWARD(sequence::prefix* /*ignored*/) {
                        auto direction = imaginaryLine::types::SUBSCRIPT;

                        // This part is going to be ugly, TODO: clean this up:
                        switch (currentStates.components.currentPresentationDirection) {
                            case presentationDirections::HORIZONTAL_TOP_LEFT_TO_BOTTOM_RIGHT:
                                direction = imaginaryLine::types::SUBSCRIPT;
                                break;
                            case presentationDirections::VERTICAL_TOP_RIGHT_TO_BOTTOM_LEFT:
                                direction = imaginaryLine::types::SUBSCRIPT;
                                break;
                            case presentationDirections::VERTICAL_TOP_LEFT_TO_BOTTOM_RIGHT:
                                direction = imaginaryLine::types::SUBSCRIPT;
                                break;
                            case presentationDirections::HORIZONTAL_TOP_RIGHT_TO_BOTTOM_LEFT:
                                direction = imaginaryLine::types::SUBSCRIPT;
                                break;
                            case presentationDirections::VERTICAL_BOTTOM_LEFT_TO_TOP_RIGHT:
                                direction = imaginaryLine::types::SUPERSCRIPT;
                                break;
                            case presentationDirections::HORIZONTAL_BOTTOM_RIGHT_TO_TOP_LEFT:
                                direction = imaginaryLine::types::SUPERSCRIPT;
                                break;
                            case presentationDirections::VERTICAL_BOTTOM_RIGHT_TO_TOP_LEFT:
                                direction = imaginaryLine::types::SUPERSCRIPT;
                                break;
                            case presentationDirections::HORIZONTAL_BOTTOM_LEFT_TO_TOP_RIGHT:
                                direction = imaginaryLine::types::SUPERSCRIPT;
                                break;
                            
                            default:
                                assert(false && "Invalid presentation direction");
                        }

                        currentStates.components.imaginaryLines.push_back(imaginaryLine{
                            direction,
                            currentStates.components.activePresentationPosition,
                            {}  // This is set by PLU
                        });
                    }

                    void operate_PARTIAL_LINE_BACKWARD(sequence::prefix* /*ignored*/) {
                        currentStates.components.imaginaryLines.back().end = currentStates.components.activePresentationPosition;
                    }

                    void operate_PAGE_POSITION_ABSOLUTE(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto PageIndex = params.front().getValueAsInteger();

                        currentStates.components.activePageIndex = PageIndex;
                    }

                    void operate_PAGE_POSITION_BACKWARD(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto PageIndex = params.front().getValueAsInteger();

                        currentStates.components.activeDataPosition.y = currentStates.components.dataPages[currentStates.components.activePageIndex - PageIndex].start.y;
                    }

                    void operate_PAGE_POSITION_FORWARD(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto PageIndex = params.front().getValueAsInteger();

                        currentStates.components.activeDataPosition.y = currentStates.components.dataPages[currentStates.components.activePageIndex + PageIndex].start.y;
                    }

                    void operate_REVERSE_LINE_FEED(sequence::prefix* /*ignored*/) {
                        if (currentStates.components.activeModes.has(table::mode::presets::DCSM_PRESENTATION)) {
                            currentStates.components.activePresentationPosition.y--;
                        } else if (currentStates.components.activeModes.has(table::mode::presets::DCSM_DATA)) {
                            currentStates.components.activeDataPosition.y--;
                        }
                    }

                    void operate_TABULATION_CLEAR(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::selectable<sequences::formatEffectors::TABULATION_CLEAR::types>>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        // If, false, then multiline tabulation is enabled.
                        // bool singleTabulationMode = currentStates.components.activeModes.has(table::mode::presets::TSM_SINGLE);
                    
                        switch (params.front().getValueAsInteger()) {
                            using namespace sequences::formatEffectors::TABULATION_CLEAR;

                            case types::ALL_LINE_AND_CHARACTER_TABULATORS: {
                                currentStates.components.tabulationStops.clear();
                                break;
                            }
                            case types::ALL_LINE_TABULATORS: {
                                currentStates.components.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates.components.tabulationStops.begin(), 
                                        currentStates.components.tabulationStops.end(), 
                                        [](tabulationStop stop) { return stop.type == tabulationStop::types::LINE; }
                                    ),
                                    currentStates.components.tabulationStops.end()
                                );
                                break;
                            }
                            case types::ALL_CHARACTER_TABULATORS: {
                                currentStates.components.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates.components.tabulationStops.begin(), 
                                        currentStates.components.tabulationStops.end(), 
                                        [](tabulationStop stop) { return stop.type == tabulationStop::types::CHARACTER; }
                                    ),
                                    currentStates.components.tabulationStops.end()
                                );
                                break;
                            }
                            case types::ALL_CHARACTER_TABULATORS_IN_ACTIVE_LINE: {
                                currentStates.components.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates.components.tabulationStops.begin(), 
                                        currentStates.components.tabulationStops.end(), 
                                        [](tabulationStop stop) { 
                                            return stop.type == tabulationStop::types::CHARACTER && stop.position.y == currentStates.components.activePresentationPosition.y; 
                                        }
                                    ),
                                    currentStates.components.tabulationStops.end()
                                );
                                break;
                            }
                            case types::LINE_TABULATOR_IN_ACTIVE_LINE: {
                                currentStates.components.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates.components.tabulationStops.begin(), 
                                        currentStates.components.tabulationStops.end(), 
                                        [](tabulationStop stop) { 
                                            return stop.type == tabulationStop::types::LINE && stop.position.y == currentStates.components.activePresentationPosition.y; 
                                        }
                                    ),
                                    currentStates.components.tabulationStops.end()
                                );
                                break;
                            }
                            case types::CHARACTER_TABULATOR_IN_ACTIVE_POSITION: {
                                currentStates.components.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates.components.tabulationStops.begin(), 
                                        currentStates.components.tabulationStops.end(), 
                                        [](tabulationStop stop) { 
                                            return stop.type == tabulationStop::types::CHARACTER && stop.position == currentStates.components.activePresentationPosition; 
                                        }
                                    ),
                                    currentStates.components.tabulationStops.end()
                                );
                                break;
                            }
                        }
                    }

                    void operate_TABULATION_STOP_REMOVE(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto index = params.front().getValueAsInteger();

                        assert(index != UINT32_MAX);    // -1 means default, but this operation does not accept default values!

                        // TODO: This one wont break after hit, so maybe change into a normal loop.
                        currentStates.components.tabulationStops.erase(
                            std::remove_if(
                                currentStates.components.tabulationStops.begin(), 
                                currentStates.components.tabulationStops.end(), 
                                [](tabulationStop stop) { 
                                    return stop.type == tabulationStop::types::CHARACTER && stop.position == currentStates.components.activePresentationPosition;
                                }
                            ),
                            currentStates.components.tabulationStops.end()
                        );
                    }

                    void operate_LINE_POSITION_ABSOLUTE(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto line = static_cast<signed int>(params.front().getValueAsInteger());

                        auto lineProgression = currentStates.components.getPresentationDirectionAsVector().first;

                        currentStates.components.activeDataPosition.y = lineProgression.y * line;
                    }

                    void operate_LINE_POSITION_BACKWARD(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto line = static_cast<signed int>(params.front().getValueAsInteger());

                        auto lineProgression = currentStates.components.getPresentationDirectionAsVector().first;

                        currentStates.components.activeDataPosition.y += -lineProgression.y * line;
                    }

                    void operate_LINE_POSITION_FORWARD(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto line = static_cast<signed int>(params.front().getValueAsInteger());

                        auto lineProgression = currentStates.components.getPresentationDirectionAsVector().first;

                        currentStates.components.activeDataPosition.y += lineProgression.y * line;
                    }

                    void operate_LINE_TABULATION(sequence::prefix* /*ignore*/) {
                        // First find the tabulation top at the current presentation position line
                        for (auto currentTabStop : currentStates.components.tabulationStops) {
                            if (
                                currentTabStop.position.y == currentStates.components.activePresentationPosition.y &&
                                currentTabStop.position.x > currentStates.components.activePresentationPosition.x &&
                                currentTabStop.type == tabulationStop::types::LINE
                            ) {
                                currentStates.components.activePresentationPosition = currentTabStop.position;
                                break;
                            }
                        }
                    }

                    void operate_LINE_TABULATION_SET(sequence::prefix* /*ignore*/) {
                        currentStates.components.tabulationStops.push_back(tabulationStop{
                            currentStates.components.activeTabulationAlignment,
                            tabulationStop::types::LINE,
                            currentStates.components.activePresentationPosition
                        });
                    }
                }

                namespace presentationControlFunctions {
                    void operate_BREAK_PERMITTED_HERE(sequence::prefix* /*ignore*/) {
                        currentStates.components.lineBreaks.push_back(currentStates.components.activePresentationPosition);
                        currentStates.components.activePresentationPosition.y++;
                        currentStates.components.activePresentationPosition.x = 0;
                    }

                    void operate_DIMENSION_TEXT_AREA(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 2);

                        auto start = params.front().getValueAsInteger();
                        auto end = params.back().getValueAsInteger();

                        currentStates.components.establishedCurrentDefaultPage = {start, end};
                    }

                    void operate_FONT_SELECTION(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::selectable<fontSlots>>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 2);

                        auto fontSlot = params.front().getValueAsInteger();
                        auto fontID = static_cast<uint8_t>(params.back().getValueAsInteger());

                        currentStates.components.activeFonts[(size_t)fontSlot] = fontID;
                    }

                    void operate_GRAPHIC_CHARACTER_COMBINATION(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::selectable<GRAPHIC_CHARACTER_COMBINATION::types>>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto combinationType = params.front().getValueAsInteger();

                        auto currentParsingIndex = currentStates.components.currentParsingSequenceIndex;
                        auto& callBacks = currentStates.components.callBacks;

                        auto callBackHandler = [](callBack /*self*/){
                            return; // TODO: ...
                        };

                        switch (combinationType) {
                            case GRAPHIC_CHARACTER_COMBINATION::types::DOUBLE_WIDE: {
                                callBacks.push_back({
                                    currentParsingIndex+1,  // Ignore current sequence
                                    currentParsingIndex+2,
                                    callBackHandler
                                });
                                break;
                            }
                            case GRAPHIC_CHARACTER_COMBINATION::types::START: {
                                callBacks.push_back({
                                    currentParsingIndex+1,  // Ignore current sequence
                                    0,
                                    callBackHandler
                                });
                                break;
                            }
                            case GRAPHIC_CHARACTER_COMBINATION::types::END: {
                                if (callBacks.back().end != 0) {
                                    assert(false && "Invalid sequence combination: END found without a corresponding START");
                                }

                                callBacks.back().end = currentParsingIndex;  // Base trust. previous GCC had to be a START one.
                                break;
                            }
                        }
                    }

                    void operate_GRAPHIC_SIZE_MODIFICATION(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 2);

                        IVector2 scalar = { 
                            (int)params.front().getValueAsInteger(), 
                            (int)params.back().getValueAsInteger()
                        };

                        size_t previousFontSize = 0;

                        // We need to check if our scalar is different from the latest scalar
                        if (!currentStates.components.registeredFontAttributes.empty()) {
                            auto &latestFontAttribute = currentStates.components.registeredFontAttributes.back();

                            previousFontSize = latestFontAttribute.fontSize;

                            if (latestFontAttribute.fontScalar == scalar) {
                                return;     
                            } else {
                                // End the last font attribute at the current presentation position
                                latestFontAttribute.end = currentStates.components.activePresentationPosition;
                            }
                        }

                        // Now we can safely add the new font attribute with the new scalar from this point onward
                        currentStates.components.registeredFontAttributes.emplace_back(
                            currentStates.components.activePresentationPosition,    // start
                            previousFontSize,    // font size is inherited from the previous font attribute, as per ECMA-48 specification
                            scalar
                        );

                    }

                    void operate_GRAPHIC_SIZE_SELECTION(sequence::prefix* input) {
                        auto controlSequence = static_cast<sequence::control<sequence::parameter::numeric>*>(input);

                        auto params = controlSequence->getParameters();

                        assert(params.size() == 1);

                        auto scalar = params.front().getValueAsInteger();

                        // if this triggers, it means this is the closing GSS
                        if (!currentStates.components.registeredFontAttributes.empty()) {
                            fontAttributes& previous = currentStates.components.registeredFontAttributes.back();

                            previous.end = currentStates.components.activePresentationPosition;
                        } 

                        // Regardless of previous GSS encounters, we will always create a new succeeding GSS from thi point onward
                        currentStates.components.registeredFontAttributes.emplace_back(
                            currentStates.components.activePresentationPosition,    // start
                            scalar
                        );

                    }
                }

            }
        }
    }
}