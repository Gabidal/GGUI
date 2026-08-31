#include "terminal.h"
#include "ecma.h"

#include "../utils/utils.h"

#include <algorithm> // std::remove_if

namespace GGUI {
    namespace terminal {
        namespace ecma {
            TODO("maybe make these ptr, so that initialization is on demand.")
            configuration::page C0(configuration::layout::functional::getRelativeFunctionalPageLayout(configuration::layout::functional::type::C0));
            configuration::page C1(configuration::layout::functional::getRelativeFunctionalPageLayout(configuration::layout::functional::type::C1));
            configuration::page G0(configuration::layout::graphical::getRelativeGraphicalPageLayout(configuration::layout::graphical::type::B));

            std::pair<IVector2, IVector2> components::getPresentationDirectionAsVector() {
                IVector2 linePath, characterPath;

                switch (currentStates->ecmaComponents.currentPresentationDirection) {
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
                TODO("maybe for better compatibility try giving a custom default param value, so that it can be used instead of the hardcoded zero.")

                // Slices the input by the sequence::parameter::delimeter character
                std::vector<sequence::parameter::numeric> parseParameterSequence(std::string_view input) {
                    std::vector<sequence::parameter::numeric> result;
                    std::vector<size_t> delimeterIndicies;

                    // First fetch all delimeter indicies
                    for (size_t i = 0; i < input.size(); i++) if ((table::parameters)input[i] == table::parameters::SEPARATOR || i == 0) delimeterIndicies.push_back(i);

                    // now we can loop through the indicies and pair each [i, i+1], and create parameters
                    for (size_t i = 0; i < delimeterIndicies.size(); i++) {
                        size_t start = delimeterIndicies[i];
                        size_t end = i == delimeterIndicies.size() - 1 ? input.size() : delimeterIndicies[i + 1];

                        // +1 to skip the delimeter itself. 
                        if ((table::parameters)input[start] == table::parameters::SEPARATOR) start += 1;

                        size_t tmp = 0;     // Useless in this case, since we already know the sizes.
                        result.push_back({
                            input.substr(start, end - start),
                            tmp
                        });
                    }

                    return result;
                }

                std::vector<table::intermediate::identifiers> parseintermediates(std::string_view input) {
                    std::vector<table::intermediate::identifiers> result;

                    // Find all intermediates:
                    for (size_t i = 0; i < input.size() && GGUI::table::contains<table::intermediate::identifiers>(input[i]); i++) {
                        result.push_back(static_cast<table::intermediate::identifiers>(input[i]));
                    }

                    return result;
                }

                std::pair<size_t, sequence::base*> parsePostfixForC1(std::string_view input) {
                    table::C1 header = static_cast<table::C1>(input.front());

                    if (header == table::C1::CSI) {
                        // Find the ending function of the current control function introducer
                        size_t finalFunctionAt = 0;
                        size_t intermediateAt = 0;

                        for (size_t i = 1; i < input.size(); i++) {
                            if (GGUI::table::contains<table::finalWithoutIntermediate>(input[i]) || GGUI::table::contains<table::finalWithIntermediate>(input[i])) {
                                finalFunctionAt = i;
                                break;
                            } else if (GGUI::table::contains<table::intermediate::identifiers>(input[i]) && intermediateAt == 0) {    // Only set intermediate once at the first occurrence
                                intermediateAt = i;
                            }
                        }

                        // The final function has to be at a different location than the intermediate
                        bool hasIntermediate = finalFunctionAt > intermediateAt && intermediateAt != 0;

                        uint8_t finalFunction = input[finalFunctionAt];

                        // Now we can create a postfix
                        postfix<uint8_t> tail(parseintermediates(input.substr(intermediateAt)), finalFunction);

                        // std::max(intermediateAt, 1) is done, because the actual code CSI is at index 0, and intermediateAt == 0, means no intermediate present, thus minimum offset +1
                        size_t earliestNonParametricIndex = std::min(finalFunctionAt, std::max(intermediateAt, (size_t)1));

                        std::vector<sequence::parameter::numeric> params;

                        // Check if there are any parameters present
                        if (earliestNonParametricIndex < finalFunctionAt) {
                            params = parseParameterSequence(input.substr(earliestNonParametricIndex, finalFunctionAt - earliestNonParametricIndex));
                        }

                        return {
                            finalFunctionAt + 1,
                            new sequence::control<sequence::parameter::numeric, uint8_t>(params, tail)
                        };

                    } else if (header == table::C1::APC || header == table::C1::DCS || header == table::C1::OSC || header == table::C1::PM || header == table::C1::SOS) {

                        TODO("Implement parsing for APC, DCS, OSC, PM, and SOS sequences.");
                        return {0, nullptr};

                    } else {    TODO("Implement parsing for other C1 sequences.");
                        return {0, nullptr};
                    }
                }

                std::pair<size_t, sequence::base*> parseIndependentFunctions(std::string_view input) {
                    if (input.size() <= 1)  return {0, nullptr};
                    else if (static_cast<table::C0>(input.front()) != table::C0::ESC) return {0, nullptr};
                    else if (!GGUI::table::contains<table::independentFunctions>(input[1])) return {0, nullptr};

                    sequence::function<table::independentFunctions>* result = new sequence::function<table::independentFunctions>(static_cast<table::independentFunctions>(input[1]));

                    return {1 + 1, result};
                }

                std::pair<size_t, sequence::base*> parsePostfixForC0(std::string_view input) {
                    // First try independent function sequence matching
                    auto [independentSize, independentSequence] = parseIndependentFunctions(input);

                    if (independentSize != 0) return {independentSize, independentSequence};

                    if (!GGUI::table::contains<table::C0>(input.front())) return {0, nullptr};

                    TODO("This could be wrong!")
                    // Assume C0 to be single byte sequence.
                    return {1, new sequence::prefix<table::C0>(static_cast<table::C0>(input.front()))};
                }

                std::pair<size_t, sequence::base*> defaultSequenceParser(std::string_view input) {
                    std::pair<size_t, sequence::base*> result;
                    size_t i = 0;

                    if (GGUI::table::contains<table::C0>(input[i])) {
                        // NOTE: for ESC + complex patterns GGUI sees them as two separate sequences, it uses ESC to load C1 layout for the following sequence to jump into.
                        result = parsePostfixForC0(input.substr(i));
                    } else if (GGUI::table::contains<table::C1>(input[i])) { 
                        result = parsePostfixForC1(input.substr(i));
                    }
                    else {  // header == nullptr => means this is a graphical character
                        result = {i, new graphicalCharacter(input[i])}; // i == 0, because parser will increment i by one after loop.
                    }

                    return result;
                }

                std::vector<sequence::base*> parse(std::string_view input) {
                    std::vector<sequence::base*> result;

                    for (size_t i = 0; i < input.size();) {
                        // First check while temporary loads are active from previous loop
                        auto [parsedLength, parsedSequence] = currentStates->ecmaComponents.pageManager.interpret(input.substr(i));

                        currentStates->ecmaComponents.pageManager.update(); // refresh temporary pages

                        result.push_back(parsedSequence);

                        currentStates->ecmaComponents.currentParsingSequenceIndex++;    // Only for META operators

                        i += std::max(parsedLength, (size_t)1); // Safe skip graphic characters

                        TODO("there is a possibility that we need to put the Active Data Position to be incremented here as the index does.")
                    }
                    return result;
                }
            }

            // parses sequence coming from SGR
            std::pair<size_t, RGB> parseColorFromSGR(activeSGRStyle::directColorTypes dt, size_t start, std::vector<sequence::parameter::selectable<graphicalTextAttributes>>& params) {
                switch (dt) {
                    case activeSGRStyle::directColorTypes::INDEXED:
                        return {1, currentStates->colorIndexMap[(uint8_t)params[start].getValueAsInteger()]};
                    case activeSGRStyle::directColorTypes::RGB:
                        return {3, RGB(
                            (uint8_t)params[start].getValueAsInteger(),
                            (uint8_t)params[start + 1].getValueAsInteger(),
                            (uint8_t)params[start + 2].getValueAsInteger()
                        )};
                    case activeSGRStyle::directColorTypes::CMY:
                        return {3, RGB(
                            (uint8_t)(UINT8_MAX - (uint8_t)params[start].getValueAsInteger()),
                            (uint8_t)(UINT8_MAX - (uint8_t)params[start + 1].getValueAsInteger()),
                            (uint8_t)(UINT8_MAX - (uint8_t)params[start + 2].getValueAsInteger())
                        )};
                    case activeSGRStyle::directColorTypes::CMYK: {
                        uint8_t c = (uint8_t)params[start].getValueAsInteger();
                        uint8_t m = (uint8_t)params[start + 1].getValueAsInteger();
                        uint8_t y = (uint8_t)params[start + 2].getValueAsInteger();
                        uint8_t k = (uint8_t)params[start + 3].getValueAsInteger();

                        return {4, RGB(
                            (uint8_t)(((UINT8_MAX - c) * (UINT8_MAX - k)) / UINT8_MAX),
                            (uint8_t)(((UINT8_MAX - m) * (UINT8_MAX - k)) / UINT8_MAX),
                            (uint8_t)(((UINT8_MAX - y) * (UINT8_MAX - k)) / UINT8_MAX)
                        )};
                    } case activeSGRStyle::directColorTypes::TRANSPARENT:
                        return {0, RGB(0, 0, 0)};
                    default:
                        throw std::runtime_error("Invalid color type.");
                }
            }

            void activeSGRStyle::parseArguments(std::vector<sequence::parameter::selectable<graphicalTextAttributes>>& params) {
                for (size_t i = 0; i < params.size(); i++) {
                    graphicalTextAttributes currentType = params[i].getValueAsInteger();

                    if (currentType == graphicalTextAttributes::FOREGROUND_COLOR || currentType == graphicalTextAttributes::BACKGROUND_COLOR) {
                        activeSGRStyle::directColorTypes colorType = (activeSGRStyle::directColorTypes)params[++i].getValueAsInteger();

                        std::pair<size_t, RGB> parsedColor = parseColorFromSGR(colorType, ++i, params);
                        
                        if (currentType == graphicalTextAttributes::FOREGROUND_COLOR)   this->textColor = parsedColor.second;
                        else if (currentType == graphicalTextAttributes::BACKGROUND_COLOR) this->backgroundColor = parsedColor.second;

                        i += parsedColor.first - 1; // -1, since we already +1 at the start of the loop
                    } else {
                        add(params[i].getValueAsInteger());
                    }
                }
            }

            // Loads the default pages, C0, C1 and G0
            components::components() : activeModes(mode::types::DEFAULT, mode::values::SET) {
                pageManager.add(C0, configuration::repertoire::C0);
                pageManager.add(C1, configuration::repertoire::C1);
                pageManager.add(G0, configuration::repertoire::G0);

                // 7-bit layout is flashed during runtime with ESC as shift function.
                // pageManager.flash(configuration::bitType::_8BIT);
                pageManager.flash();
            }

            namespace sequences {   TODO("add multi selectable types for parameters.")
                namespace delimiters {}

                namespace introducers {
                    void operateEscapeToLoadC1(sequence::base*) {
                        // Simplify memory loading where colliding cells with G0, by loading C1 page as temporary shifts
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::C1, 
                            configuration::layout::functional::getRelativeFunctionalPageLayout(configuration::layout::functional::type::C1), 
                            configuration::lifetime::types::TEMPORARY
                        );
                    }
                }

                namespace shiftFunctions {
                    auto layoutType = configuration::layout::graphical::type::A;     TODO("Dynamically adjust this.")

                    void operateShift_LS0(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G0, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS1(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G1, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_SS2(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G2, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            configuration::lifetime::types::TEMPORARY
                        );
                    }

                    void operateShift_SS3(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G3, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            configuration::lifetime::types::TEMPORARY
                        );
                    }

                    void operateShift_LS1R(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G1, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                            configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS2(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G2, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS2R(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G2, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                            configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS3(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G3, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                            configuration::lifetime::types::LOCKING
                        );
                    }

                    void operateShift_LS3R(sequence::base*) {
                        currentStates->ecmaComponents.pageManager.load(
                            configuration::repertoire::G3, 
                            configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                            configuration::lifetime::types::LOCKING
                        );
                    }
                    
                }

                namespace formatEffectors {
                    void operate_BACKSPACE(sequence::base*) {
                        // First we get the direction and a base vector for the opposite direction
                        IVector2 oppositeDirection = currentStates->ecmaComponents.activeCharacterMovementDirection * -1;
                    
                        currentStates->ecmaComponents.activeDataPosition += oppositeDirection;
                    }

                    void operate_CARRIAGE_RETURN(sequence::base*) {
                        if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION)) {
                            if (currentStates->ecmaComponents.toCharacterMovementDirection(currentStates->ecmaComponents.activeCharacterMovementDirection) == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates->ecmaComponents.activePresentationPosition.x = currentStates->ecmaComponents.homeLinePosition.x;
                            } else {
                                currentStates->ecmaComponents.activePresentationPosition.x = currentStates->ecmaComponents.lineLimitPosition.x;
                            }
                        } else if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_DATA)) {
                            if (currentStates->ecmaComponents.toCharacterMovementDirection(currentStates->ecmaComponents.activeCharacterMovementDirection) == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates->ecmaComponents.activeDataPosition.x = currentStates->ecmaComponents.homeLinePosition.x;
                            } else {
                                currentStates->ecmaComponents.activeDataPosition.x = currentStates->ecmaComponents.lineLimitPosition.x;
                            }
                        }
                    }

                    void operate_FORM_FEED(sequence::base*) {
                        // FF causes the active presentation position to be moved to the corresponding 
                        // character position of the line at the page home position of the next form or page.
                        // Move to the next page by advancing past the current active area
                        if (currentStates->ecmaComponents.activeArea.getUpper().row != 0) {
                            currentStates->ecmaComponents.activeDataPosition.y = currentStates->ecmaComponents.activeArea.getUpper().row + 1;
                        }
                        
                        // Set the presentation position to the home line position of the new page
                        currentStates->ecmaComponents.activePresentationPosition = currentStates->ecmaComponents.homeLinePosition;
                    }

                    void operate_CHARACTER_POSITION_ABSOLUTE(sequence::base* input) {
                        auto controlSequence = formatEffectors::CHARACTER_POSITION_ABSOLUTE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        currentStates->ecmaComponents.activeDataPosition.x = params.front().getValueAsInteger();
                    }

                    void operate_CHARACTER_POSITION_BACKWARD(sequence::base* input) {
                        auto controlSequence = formatEffectors::CHARACTER_POSITION_BACKWARD.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        // Get the current direction vector and multiply it by the scalar of n via input and -1 to get the opposite vector.
                        auto directionVector = currentStates->ecmaComponents.activeCharacterMovementDirection * -static_cast<signed int>(params.front().getValueAsInteger());

                        currentStates->ecmaComponents.activeDataPosition += directionVector;
                    }

                    void operate_CHARACTER_POSITION_FORWARD(sequence::base* input) {
                        auto controlSequence = formatEffectors::CHARACTER_POSITION_FORWARD.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        // Get the current direction vector and multiply it by the scalar of n via input to get the movement vector.
                        auto directionVector = currentStates->ecmaComponents.activeCharacterMovementDirection * static_cast<signed int>(params.front().getValueAsInteger());

                        currentStates->ecmaComponents.activeDataPosition += directionVector;
                    }
                    
                    void operate_CHARACTER_TABULATION(sequence::base*) {
                        tabulationStop nextTabulation;

                        // Find next tabulation 
                        for (auto currentTabulation : currentStates->ecmaComponents.tabulationStops) {
                            if (
                                currentTabulation.position.y == currentStates->ecmaComponents.activePresentationPosition.y && 
                                currentTabulation.position.x >= currentStates->ecmaComponents.activePresentationPosition.x &&
                                currentTabulation.position.x < nextTabulation.position.x  // This is meant to find the closest next tabulation stop
                            ) {
                                nextTabulation = currentTabulation;
                            }
                        }

                        // Now we move our active presentation position into it
                        currentStates->ecmaComponents.activePresentationPosition = nextTabulation.position;

                        // Now we need to also enable the current tabulation mode so that the following string literals are aligned properly.
                        currentStates->ecmaComponents.activeTabulationAlignment = nextTabulation.mode;

                        TODO("add here the code for detecting multi-line tabulation support and if so, also move the activeLinePosition.")
                    }

                    void operate_CHARACTER_TABULATION_SET(sequence::base*) {
                        // This function sets a tabulation stop at the current active line position and presentation position, with the current tabulation alignment mode.
                        currentStates->ecmaComponents.tabulationStops.push_back(tabulationStop{
                            currentStates->ecmaComponents.activeTabulationAlignment,
                            tabulationStop::types::CHARACTER,
                            currentStates->ecmaComponents.activePresentationPosition
                        });
                    }

                    void operate_CHARACTER_AND_LINE_POSITION(sequence::base* input) {
                        auto controlSequence = formatEffectors::CHARACTER_AND_LINE_POSITION.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 2);

                        auto y = params[0].getValueAsInteger();
                        auto x = params[1].getValueAsInteger();

                        currentStates->ecmaComponents.activeDataPosition = {x, y};
                    }

                    void operate_LINE_FEED(sequence::base*) {
                        if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION)) {
                            currentStates->ecmaComponents.activePresentationPosition.y++;
                        } else if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_DATA)) {
                            currentStates->ecmaComponents.activeDataPosition.y++;
                        }
                    }

                    void operate_NEXT_LINE(sequence::base*) {
                        bool has_presentation = currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION);
                        bool has_data = currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_DATA);
                        auto movement_direction = currentStates->ecmaComponents.toCharacterMovementDirection(currentStates->ecmaComponents.activeCharacterMovementDirection);
                        
                        if (has_presentation) {
                            if (movement_direction == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates->ecmaComponents.activePresentationPosition.y = currentStates->ecmaComponents.homeLinePosition.y;
                            } else {
                                currentStates->ecmaComponents.activePresentationPosition.y = currentStates->ecmaComponents.lineLimitPosition.y;
                            }
                        } else if (has_data) {
                            if (movement_direction == ecma::components::characterMovementDirection::DIRECTION_OF_CHARACTER_PROGRESSION) {
                                currentStates->ecmaComponents.activeDataPosition.y = currentStates->ecmaComponents.homeLinePosition.y;
                            } else {
                                currentStates->ecmaComponents.activeDataPosition.y = currentStates->ecmaComponents.lineLimitPosition.y;
                            }
                        }
                    }

                    void operate_PARTIAL_LINE_FORWARD(sequence::base*) {
                        auto direction = imaginaryLine::types::SUBSCRIPT;

                        TODO("This part is going to be ugly, clean this up!")
                        switch (currentStates->ecmaComponents.currentPresentationDirection) {
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

                        currentStates->ecmaComponents.imaginaryLines.push_back(imaginaryLine{
                            direction,
                            currentStates->ecmaComponents.activePresentationPosition,
                            {}  // This is set by PLU
                        });
                    }

                    void operate_PARTIAL_LINE_BACKWARD(sequence::base*) {
                        currentStates->ecmaComponents.imaginaryLines.back().end = currentStates->ecmaComponents.activePresentationPosition;
                    }

                    void operate_PAGE_POSITION_ABSOLUTE(sequence::base* input) {
                        auto controlSequence = formatEffectors::PAGE_POSITION_ABSOLUTE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto PageIndex = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePageIndex = PageIndex;
                    }

                    void operate_PAGE_POSITION_BACKWARD(sequence::base* input) {
                        auto controlSequence = formatEffectors::PAGE_POSITION_BACKWARD.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto PageIndex = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activeDataPosition.y = currentStates->ecmaComponents.dataPages[currentStates->ecmaComponents.activePageIndex - PageIndex].start.y;
                    }

                    void operate_PAGE_POSITION_FORWARD(sequence::base* input) {
                        auto controlSequence = formatEffectors::PAGE_POSITION_FORWARD.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto PageIndex = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activeDataPosition.y = currentStates->ecmaComponents.dataPages[currentStates->ecmaComponents.activePageIndex + PageIndex].start.y;
                    }

                    void operate_REVERSE_LINE_FEED(sequence::base*) {
                        if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION)) {
                            currentStates->ecmaComponents.activePresentationPosition.y--;
                        } else if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_DATA)) {
                            currentStates->ecmaComponents.activeDataPosition.y--;
                        }
                    }

                    void operate_TABULATION_CLEAR(sequence::base* input) {
                        auto controlSequence = formatEffectors::TABULATION_CLEAR::code.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        // If, false, then multiline tabulation is enabled.
                        // bool singleTabulationMode = currentStates->components.activeModes.has(mode::presets::TSM_SINGLE);
                    
                        switch (params.front().getValueAsInteger()) {
                            using namespace sequences::formatEffectors::TABULATION_CLEAR;

                            case types::ALL_LINE_AND_CHARACTER_TABULATORS: {
                                currentStates->ecmaComponents.tabulationStops.clear();
                                break;
                            }
                            case types::ALL_LINE_TABULATORS: {
                                currentStates->ecmaComponents.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates->ecmaComponents.tabulationStops.begin(), 
                                        currentStates->ecmaComponents.tabulationStops.end(), 
                                        [](tabulationStop stop) { return stop.type == tabulationStop::types::LINE; }
                                    ),
                                    currentStates->ecmaComponents.tabulationStops.end()
                                );
                                break;
                            }
                            case types::ALL_CHARACTER_TABULATORS: {
                                currentStates->ecmaComponents.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates->ecmaComponents.tabulationStops.begin(), 
                                        currentStates->ecmaComponents.tabulationStops.end(), 
                                        [](tabulationStop stop) { return stop.type == tabulationStop::types::CHARACTER; }
                                    ),
                                    currentStates->ecmaComponents.tabulationStops.end()
                                );
                                break;
                            }
                            case types::ALL_CHARACTER_TABULATORS_IN_ACTIVE_LINE: {
                                currentStates->ecmaComponents.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates->ecmaComponents.tabulationStops.begin(), 
                                        currentStates->ecmaComponents.tabulationStops.end(), 
                                        [](tabulationStop stop) { 
                                            return stop.type == tabulationStop::types::CHARACTER && stop.position.y == currentStates->ecmaComponents.activePresentationPosition.y; 
                                        }
                                    ),
                                    currentStates->ecmaComponents.tabulationStops.end()
                                );
                                break;
                            }
                            case types::LINE_TABULATOR_IN_ACTIVE_LINE: {
                                currentStates->ecmaComponents.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates->ecmaComponents.tabulationStops.begin(), 
                                        currentStates->ecmaComponents.tabulationStops.end(), 
                                        [](tabulationStop stop) { 
                                            return stop.type == tabulationStop::types::LINE && stop.position.y == currentStates->ecmaComponents.activePresentationPosition.y; 
                                        }
                                    ),
                                    currentStates->ecmaComponents.tabulationStops.end()
                                );
                                break;
                            }
                            case types::CHARACTER_TABULATOR_IN_ACTIVE_POSITION: {
                                currentStates->ecmaComponents.tabulationStops.erase(
                                    std::remove_if(
                                        currentStates->ecmaComponents.tabulationStops.begin(), 
                                        currentStates->ecmaComponents.tabulationStops.end(), 
                                        [](tabulationStop stop) { 
                                            return stop.type == tabulationStop::types::CHARACTER && stop.position == currentStates->ecmaComponents.activePresentationPosition; 
                                        }
                                    ),
                                    currentStates->ecmaComponents.tabulationStops.end()
                                );
                                break;
                            }
                        }
                    }

                    void operate_TABULATION_STOP_REMOVE(sequence::base* input) {
                        auto controlSequence = formatEffectors::TABULATION_STOP_REMOVE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto index = params.front().getValueAsInteger();

                        assert(index != UINT32_MAX);    // -1 means default, but this operation does not accept default values!

                        TODO("This one wont break after hit, so maybe change into a normal loop.")
                        currentStates->ecmaComponents.tabulationStops.erase(
                            std::remove_if(
                                currentStates->ecmaComponents.tabulationStops.begin(), 
                                currentStates->ecmaComponents.tabulationStops.end(), 
                                [&index](tabulationStop stop) { 
                                    return stop.type == tabulationStop::types::CHARACTER && stop.position == IVector2{ index, currentStates->ecmaComponents.activePresentationPosition.y };
                                }
                            ),
                            currentStates->ecmaComponents.tabulationStops.end()
                        );
                    }

                    void operate_LINE_POSITION_ABSOLUTE(sequence::base* input) {
                        auto controlSequence = formatEffectors::LINE_POSITION_ABSOLUTE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = static_cast<signed int>(params.front().getValueAsInteger());

                        auto lineProgression = currentStates->ecmaComponents.getPresentationDirectionAsVector().first;

                        currentStates->ecmaComponents.activeDataPosition.y = lineProgression.y * line;
                    }

                    void operate_LINE_POSITION_BACKWARD(sequence::base* input) {
                        auto controlSequence = formatEffectors::LINE_POSITION_BACKWARD.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = static_cast<signed int>(params.front().getValueAsInteger());

                        auto lineProgression = currentStates->ecmaComponents.getPresentationDirectionAsVector().first;

                        currentStates->ecmaComponents.activeDataPosition.y += -lineProgression.y * line;
                    }

                    void operate_LINE_POSITION_FORWARD(sequence::base* input) {
                        auto controlSequence = formatEffectors::LINE_POSITION_FORWARD.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = static_cast<signed int>(params.front().getValueAsInteger());

                        auto lineProgression = currentStates->ecmaComponents.getPresentationDirectionAsVector().first;

                        currentStates->ecmaComponents.activeDataPosition.y += lineProgression.y * line;
                    }

                    void operate_LINE_TABULATION(sequence::base* /*ignore*/) {
                        // First find the tabulation top at the current presentation position line
                        for (auto currentTabStop : currentStates->ecmaComponents.tabulationStops) {
                            if (
                                currentTabStop.position.y == currentStates->ecmaComponents.activePresentationPosition.y &&
                                currentTabStop.position.x > currentStates->ecmaComponents.activePresentationPosition.x &&
                                currentTabStop.type == tabulationStop::types::LINE
                            ) {
                                currentStates->ecmaComponents.activePresentationPosition = currentTabStop.position;
                                break;
                            }
                        }
                    }

                    void operate_LINE_TABULATION_SET(sequence::base* /*ignore*/) {
                        currentStates->ecmaComponents.tabulationStops.push_back(tabulationStop{
                            currentStates->ecmaComponents.activeTabulationAlignment,
                            tabulationStop::types::LINE,
                            currentStates->ecmaComponents.activePresentationPosition
                        });
                    }
                }

                namespace presentationControlFunctions {
                    void operate_BREAK_PERMITTED_HERE(sequence::base* /*ignore*/) {
                        currentStates->ecmaComponents.lineBreaks.push_back(currentStates->ecmaComponents.activePresentationPosition);
                        currentStates->ecmaComponents.activePresentationPosition.y++;
                        currentStates->ecmaComponents.activePresentationPosition.x = 0;
                    }

                    void operate_DIMENSION_TEXT_AREA(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::DIMENSION_TEXT_AREA.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 2);

                        auto start = params.front().getValueAsInteger();
                        auto end = params.back().getValueAsInteger();

                        currentStates->ecmaComponents.establishedCurrentDefaultPage = {start, end};
                    }

                    void operate_FONT_SELECTION(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::FONT_SELECTION.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 2);

                        auto fontSlot = params.front().getValueAsInteger();
                        auto fontID = static_cast<uint8_t>(params.back().getValueAsInteger());

                        currentStates->ecmaComponents.activeFonts[(size_t)fontSlot] = fontID;
                    }

                    void operate_GRAPHIC_CHARACTER_COMBINATION(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::GRAPHIC_CHARACTER_COMBINATION::code.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto combinationType = params.front().getValueAsInteger();

                        auto currentParsingIndex = currentStates->ecmaComponents.currentParsingSequenceIndex;
                        auto& callBacks = currentStates->ecmaComponents.callBacks;

                        auto callBackHandler = [](callBack /*self*/, size_t& /*callBackIndex*/, size_t& /*parsingIndex*/, std::vector<sequence::base*>& /*parsed*/){
                            return; TODO("Implement the callback handler for GRAPHIC_CHARACTER_COMBINATION.");
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

                    void operate_GRAPHIC_SIZE_MODIFICATION(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::GRAPHIC_SIZE_MODIFICATION.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 2);

                        IVector2 scalar = { 
                            (int)params.front().getValueAsInteger(), 
                            (int)params.back().getValueAsInteger()
                        };

                        size_t previousFontSize = 0;

                        // We need to check if our scalar is different from the latest scalar
                        if (!currentStates->ecmaComponents.registeredFontAttributes.empty()) {
                            auto &latestFontAttribute = currentStates->ecmaComponents.registeredFontAttributes.back();

                            previousFontSize = latestFontAttribute.fontSize;

                            if (latestFontAttribute.fontScalar == scalar) {
                                return;     
                            } else {
                                // End the last font attribute at the current presentation position
                                latestFontAttribute.end = currentStates->ecmaComponents.activePresentationPosition;
                            }
                        }

                        // Now we can safely add the new font attribute with the new scalar from this point onward
                        currentStates->ecmaComponents.registeredFontAttributes.emplace_back(
                            currentStates->ecmaComponents.activePresentationPosition,    // start
                            previousFontSize,    // font size is inherited from the previous font attribute, as per ECMA-48 specification
                            scalar
                        );

                    }

                    void operate_GRAPHIC_SIZE_SELECTION(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::GRAPHIC_SIZE_SELECTION.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto scalar = params.front().getValueAsInteger();

                        // if this triggers, it means this is the closing GSS
                        if (!currentStates->ecmaComponents.registeredFontAttributes.empty()) {
                            fontAttributes& previous = currentStates->ecmaComponents.registeredFontAttributes.back();

                            previous.end = currentStates->ecmaComponents.activePresentationPosition;
                        } 

                        // Regardless of previous GSS encounters, we will always create a new succeeding GSS from thi point onward
                        currentStates->ecmaComponents.registeredFontAttributes.emplace_back(
                            currentStates->ecmaComponents.activePresentationPosition,    // start
                            scalar
                        );
                    }

                    void operate_JUSTIFY(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::JUSTIFY.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() > 0);

                        // Check if previous justify exists and is trailing.
                        if (!currentStates->ecmaComponents.registeredJustifications.empty()) {
                            auto& trailingJustification = currentStates->ecmaComponents.registeredJustifications.back();
                            
                            if (trailingJustification.end != 0) {   // End trailing justification
                                trailingJustification.end = currentStates->ecmaComponents.activePresentationPosition;
                            }
                        }

                        justify newJustification(currentStates->ecmaComponents.activePresentationPosition);

                        for (auto& p : params) {
                            auto typed = p.getValueAsInteger();

                            newJustification.add(typed);
                        }

                        currentStates->ecmaComponents.registeredJustifications.push_back(newJustification);
                    }

                    void operate_NO_BREAK_HERE(sequence::base*) {
                        currentStates->ecmaComponents.lineContinuations.push_back(currentStates->ecmaComponents.activePresentationPosition);
                    }

                    void operate_PRESENTATION_EXPAND_OR_CONTRACT(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::PRESENTATION_EXPAND_OR_CONTRACT.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto spacingFactorType = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activeSpacingFactor.type = spacingFactorType;
                    }

                    void operate_SELECT_GRAPHIC_RENDITION(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::SELECT_GRAPHIC_RENDITION.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() > 0);

                        activeSGRStyle newAttributes(currentStates->ecmaComponents.activePresentationPosition);

                        newAttributes.parseArguments(params);

                        bool cumulateFromPrevious = currentStates->ecmaComponents.activeModes.has(mode::presets::GRCM_CUMULATIVE);

                        // Check if the current GRCM is replacing or cumulative
                        if (!currentStates->ecmaComponents.registeredGraphicAttributes.empty() && cumulateFromPrevious) {
                            auto& previousAttributes = currentStates->ecmaComponents.registeredGraphicAttributes.back();

                            newAttributes.add(previousAttributes);
                        }

                        currentStates->ecmaComponents.registeredGraphicAttributes.push_back(newAttributes);
                    }

                    void operate_SET_LINE_HOME(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::SET_LINE_HOME.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto characterPosition = params.front().getValueAsInteger();

                        if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION)) {
                            currentStates->ecmaComponents.homeLinePosition = {
                                characterPosition,
                                currentStates->ecmaComponents.activePresentationPosition.y
                            };
                        } else {    // data mode
                            currentStates->ecmaComponents.homeLinePosition = {
                                characterPosition,
                                currentStates->ecmaComponents.activeDataPosition.y
                            };
                        }
                    }

                    void operate_SET_LINE_LIMIT(sequence::base* input) {
                        auto controlSequence = presentationControlFunctions::SET_LINE_LIMIT.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto characterPosition = params.front().getValueAsInteger();

                        if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION)) {
                            currentStates->ecmaComponents.lineLimitPosition = {
                                characterPosition,
                                currentStates->ecmaComponents.activePresentationPosition.y
                            };
                        } else {    // data mode
                            currentStates->ecmaComponents.lineLimitPosition = {
                                characterPosition,
                                currentStates->ecmaComponents.activeDataPosition.y
                            };
                        }
                    }
                }

                namespace editorFunctions {
                    void operate_DELETE_CHARACTER(sequence::base* input) {
                        auto controlSequence = editorFunctions::DELETE_CHARACTER.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto amountToRemove = params.front().getValueAsInteger();

                        const auto& activeModes = currentStates->ecmaComponents.activeModes;

                        if (activeModes.has(mode::presets::DCSM_DATA)) {
                            GGUI::logger::log("GGUI Does not support input data stream manipulation!");
                            return;
                        }

                        auto cursorPositionAtBuffer = currentStates->ecmaComponents.getPresentationPositionAsBufferAddress();
                        if (activeModes.has(mode::group::characterReplacement::IRM_INSERT_HEM_FOLLOWING)) {
                            std::fill(  TODO("These maybe need to be transformed into just deletes instead of filling with spaces!")
                                cursorPositionAtBuffer,
                                cursorPositionAtBuffer + amountToRemove,
                                ' '     // Represents empty cell
                            );
                        } else if (activeModes.has(mode::group::characterReplacement::IRM_INSERT_HEM_PRECEDING)) {
                            std::fill(
                                cursorPositionAtBuffer - amountToRemove,
                                cursorPositionAtBuffer,
                                ' '     // Represents empty cell
                            );
                        } else {
                            GGUI::logger::log("Unknown delete mode at: " + currentStates->ecmaComponents.activePresentationPosition.toString());
                        }
                    }

                    void operate_DELETE_LINE(sequence::base* input) {
                        auto controlSequence = editorFunctions::DELETE_LINE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto amountToRemove = params.front().getValueAsInteger();

                        const auto& activeModes = currentStates->ecmaComponents.activeModes;

                        auto cursorPositionAtBuffer = currentStates->ecmaComponents.getPresentationPositionAsBufferAddress();
                        auto screenWidth = currentStates->ecmaComponents.activeScreenDimensions.x;

                        if (activeModes.has(mode::presets::VEM_FOLLOWING)) {
                            std::fill(
                                cursorPositionAtBuffer,
                                cursorPositionAtBuffer + (amountToRemove * screenWidth),
                                ' '     // Represents empty cell
                            );
                        } else {
                            std::fill(
                                cursorPositionAtBuffer - (amountToRemove * screenWidth),
                                cursorPositionAtBuffer,
                                ' '     // Represents empty cell
                            );
                        }
                    }

                    void operate_INSERT_CHARACTER(sequence::base* input) {
                        auto controlSequence = editorFunctions::INSERT_CHARACTER.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto amountToInsert = params.front().getValueAsInteger();

                        const auto& activeModes = currentStates->ecmaComponents.activeModes;

                        auto cursorPositionAtBuffer = currentStates->ecmaComponents.getPresentationPositionAsBufferAddress();
                        auto lineLimitAtBuffer = currentStates->ecmaComponents.activePresentationBuffer.begin() + (
                            currentStates->screen.getActiveIndex() - currentStates->ecmaComponents.activePresentationPosition.x   // Is is to ge the actual buffer cell position, and then remove the character so that we can insert our own line limit instead
                        ) + currentStates->ecmaComponents.lineLimitPosition.x;
                        auto lineHomeAtBuffer = currentStates->ecmaComponents.activePresentationBuffer.begin() + (
                            currentStates->screen.getActiveIndex() - currentStates->ecmaComponents.activePresentationPosition.x   // Is is to ge the actual buffer cell position, and then remove the character so that we can insert our own line home instead
                        ) + currentStates->ecmaComponents.homeLinePosition.x;

                        // Since we actually cannot insert anything, because this is a screen buffer.
                        // Instead we are going to move the data by the amount
                        if (activeModes.has(mode::presets::HEM_FOLLOWING)) {
                            std::move_backward(
                                cursorPositionAtBuffer,
                                lineLimitAtBuffer,
                                lineLimitAtBuffer + amountToInsert
                            );

                            std::fill(
                                cursorPositionAtBuffer,
                                cursorPositionAtBuffer + amountToInsert,
                                ' '       // Represents empty cell
                            );
                        } else {
                            auto startOfAffectedArea = cursorPositionAtBuffer - amountToInsert + 1;

                            std::move_backward(
                                lineHomeAtBuffer,
                                startOfAffectedArea,
                                cursorPositionAtBuffer + 1
                            );

                            std::fill(
                                startOfAffectedArea,
                                cursorPositionAtBuffer + 1,
                                ' '       // Represents empty cell
                            );
                        }
                    }

                    void operate_INSERT_LINE(sequence::base* input) {
                        auto controlSequence = editorFunctions::INSERT_LINE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto amountToInsert = params.front().getValueAsInteger();

                        const auto& activeModes = currentStates->ecmaComponents.activeModes;

                        auto screenWidth = currentStates->ecmaComponents.activeScreenDimensions.x;

                        auto activeLineBegin = currentStates->ecmaComponents.activePresentationBuffer.begin() + (
                            currentStates->screen.getActiveIndex() - currentStates->ecmaComponents.activePresentationPosition.x
                        );

                        auto lineLimitAtBuffer = currentStates->ecmaComponents.activePresentationBuffer.begin() + (
                            currentStates->ecmaComponents.lineLimitPosition.y + 1
                        ) * screenWidth;

                        auto insertedCellCount = amountToInsert * screenWidth;

                        if (activeModes.has(mode::presets::VEM_FOLLOWING)) {
                            std::move_backward(
                                activeLineBegin,
                                lineLimitAtBuffer,
                                lineLimitAtBuffer + insertedCellCount
                            );

                            std::fill(
                                activeLineBegin,
                                activeLineBegin + insertedCellCount,
                                ' '       // Represents empty cell
                            );
                        } else {
                            auto startOfAffectedArea = activeLineBegin - insertedCellCount + screenWidth;

                            std::move_backward(
                                currentStates->ecmaComponents.activePresentationBuffer.begin(),
                                startOfAffectedArea,
                                activeLineBegin + screenWidth
                            );

                            std::fill(
                                startOfAffectedArea,
                                activeLineBegin + screenWidth,
                                ' '       // Represents empty cell
                            );
                        }
                    }
                }

                namespace cursorControlFunctions {
                    void operate_CURSOR_NEXT_LINE(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_NEXT_LINE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y += line;
                        currentStates->ecmaComponents.activePresentationPosition.x = 0;  TODO("line home position?")
                    }

                    void operate_CURSOR_PRECEDING_LINE(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_PRECEDING_LINE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y -= line;
                        currentStates->ecmaComponents.activePresentationPosition.x = 0;  TODO("line home position?")
                    }

                    void operate_CURSOR_LEFT(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_LEFT.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto character = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.x -= character;
                    }

                    void operate_CURSOR_DOWN(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_DOWN.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y += line;
                    }

                    void operate_CURSOR_RIGHT(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_RIGHT.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto character = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.x += character;
                    }

                    void operate_CURSOR_POSITION(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_POSITION.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 2);

                        auto line = params.front().getValueAsInteger();
                        auto character = params.back().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y = line;
                        currentStates->ecmaComponents.activePresentationPosition.x = character;
                    }

                    void operate_CURSOR_UP(sequence::base* input) {
                        auto controlSequence = cursorControlFunctions::CURSOR_UP.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y -= line;
                    }
                }

                namespace displayControlFunctions {
                    void operate_NEXT_PAGE(sequence::base* input) {
                        auto controlSequence = displayControlFunctions::NEXT_PAGE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto page = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePageIndex += page;
                    }

                    void operate_PRECEDING_PAGE(sequence::base* input) {
                        auto controlSequence = displayControlFunctions::PRECEDING_PAGE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto page = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePageIndex -= page;
                    }

                    void operate_SCROLL_DOWN(sequence::base* input) {
                        auto controlSequence = displayControlFunctions::SCROLL_DOWN.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y += line;   TODO("Missing horizontal scroll")
                    }

                    void operate_SCROLL_UP(sequence::base* input) {
                        auto controlSequence = displayControlFunctions::SCROLL_UP.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() == 1);

                        auto line = params.front().getValueAsInteger();

                        currentStates->ecmaComponents.activePresentationPosition.y -= line;   TODO("Missing horizontal scroll")
                    }
                }

                namespace deviceControlFunctions {
                    void operate_DEVICE_CONTROL_ONE(sequence::base*) {
                        currentStates->ecmaComponents.powerStatus = ancillaryStates::X_ON;
                    }

                    void operate_DEVICE_CONTROL_TWO(sequence::base*) {
                        currentStates->ecmaComponents.powerStatus = ancillaryStates::BASIC_MODE;
                    }

                    void operate_DEVICE_CONTROL_THREE(sequence::base*) {
                        currentStates->ecmaComponents.powerStatus = ancillaryStates::X_OFF;
                    }

                    void operate_DEVICE_CONTROL_FOUR(sequence::base*) {
                        currentStates->ecmaComponents.powerStatus = ancillaryStates::INTERRUPT;
                    }
                }

                namespace modeSettingFunctions {
                    void operate_RESET_MODE(sequence::base* input) {
                        auto controlSequence = modeSettingFunctions::RESET_MODE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() > 0);

                        for (auto& p : params) {
                            auto typed = p.getValueAsInteger();

                            currentStates->ecmaComponents.activeModes.set(typed, static_cast<bool>(mode::values::RESET));
                        }
                    }

                    void operate_SET_MODE(sequence::base* input) {
                        auto controlSequence = modeSettingFunctions::SET_MODE.transform(input);

                        auto params = controlSequence.getParameters();

                        assert(params.size() > 0);

                        for (auto& p : params) {
                            auto typed = p.getValueAsInteger();

                            currentStates->ecmaComponents.activeModes.set(typed, static_cast<bool>(mode::values::SET));
                        }
                    }
                }

                // These have been made according to: ISO/R 1745:1971
                namespace transmissionControlFunctions {
                    void operate_ACKNOWLEDGE(sequence::base*) {
                        // Not used.
                        TODO("Implement ACK operation.")
                    }

                    void operate_DATA_LINK_ESCAPE(sequence::base*) {
                        TODO("Implement DLE operation.");
                    }

                    void operate_ENQUIRY(sequence::base*) {
                        std::string answer;

                        // triggers on first use.
                        static bool hiddenState_isFirstEnquiry = false;

                        if (!hiddenState_isFirstEnquiry) {
                            static const sequence::transmission response("GGUI"); 

                            response.toString(answer);
                        } else {
                            TODO("Implement response for subsequent ENQUIRY signals. This could include system status, version info, etc.");
                        }

                        currentStates->transmission.addToQueue(answer);
                    }

                    void operate_START_OF_TRANSMISSION(sequence::base*) {
                        auto currentParsingIndex = currentStates->ecmaComponents.currentParsingSequenceIndex;
                        auto& callBacks = currentStates->ecmaComponents.callBacks;

                        auto callBackHandler = [](callBack self, size_t& callBackIndex, size_t& parsingIndex, std::vector<sequence::base*>& parsed){
                            auto currentTransmission = static_cast<sequence::prefix<table::C0>*>(parsed[parsingIndex]);

                            sequence::transmission result(currentTransmission->getValue());

                            size_t primaryTransmissionEnd = self.end;
                            
                            bool dualCallBackCombine = (
                                primaryTransmissionEnd == 0 && 
                                currentStates->ecmaComponents.callBacks.back().start > self.start &&
                                result.type == sequence::transmission::types::HEADER
                            );
                            
                            if (dualCallBackCombine) {  // the second callBack is an start of text transmission block, which will indicate our primary set end.
                                primaryTransmissionEnd = currentStates->ecmaComponents.callBacks[callBackIndex + 1].start - 1;   // -1 to ignore the STX/ETX/ETB of the second transmission block
                            }

                            size_t actualTransmissionEnd = primaryTransmissionEnd;
                            
                            std::vector<char> primary;
                            primary.resize(primaryTransmissionEnd - self.start + 1);

                            // Now we can safely copy all graphical character parsed sequences into primary set
                            for (size_t i = self.start + 1; i <= primaryTransmissionEnd; i++) {
                                auto& s = parsed[i];

                                if (s->getType() != sequence::types::GRAPHICAL_CHARACTER) {
                                    GGUI::logger::log("Non-graphical transmission block found!");
                                    continue;
                                }

                                char gc = static_cast<sequence::graphicalCharacter*>(s)->getValue();

                                // i - , because primary starts from zero where as our indexing does not
                                primary[i - self.start] = gc;
                            }

                            result.primary = std::move(primary);

                            if (dualCallBackCombine) {
                                auto& body = currentStates->ecmaComponents.callBacks[callBackIndex + 1];
                                std::vector<char> secondary;
                                secondary.resize(body.end - body.start + 1);

                                for (size_t i = body.start + 1; i <= body.end; i++) {
                                    auto& s = parsed[i];

                                    if (s->getType() != sequence::types::GRAPHICAL_CHARACTER) {
                                        GGUI::logger::log("Non-graphical transmission block found!");
                                        continue;
                                    }

                                    char gc = static_cast<sequence::graphicalCharacter*>(s)->getValue();

                                    // i - , because secondary starts from zero where as our indexing does not
                                    secondary[i - body.start] = gc;
                                }

                                result.secondary = std::move(secondary);

                                actualTransmissionEnd = body.end;
                            }

                            // Now we can remove all prefixes from the parsed
                            parsed.erase(
                                parsed.begin() + self.start,
                                parsed.begin() + actualTransmissionEnd + 1
                            );

                            // Adjust parsing index
                            parsingIndex -= (actualTransmissionEnd - self.start + 1);
                        };

                        // We can use non-terminated callback mean that of end of header and start of body
                        callBacks.push_back({
                            currentParsingIndex+1,  // Ignore current sequence
                            0,
                            callBackHandler
                        });
                    }

                    void operate_END_OF_TRANSMISSION(sequence::base*) {
                        // Check that a open-ended transmission exists.
                        if (currentStates->ecmaComponents.callBacks.empty() || currentStates->ecmaComponents.callBacks.back().end != 0) {
                            GGUI::logger::log("Unexpected EOT/ETX/ETB!");
                            return;
                        }

                        currentStates->ecmaComponents.callBacks.back().end = currentStates->ecmaComponents.currentParsingSequenceIndex;
                    }
                }

                namespace miscellaneousControlFunctions {
                    void operate_ACTIVE_POSITION_REPORT(sequence::base* input) {
                        auto controlSequence = miscellaneousControlFunctions::ACTIVE_POSITION_REPORT.transform(input);
                        
                        auto params = controlSequence.getParameters();
                        
                        assert(params.size() == 2);
                        
                        IVector2 reporting = {
                            params.front().getValueAsInteger(),
                            params.back().getValueAsInteger()
                        };

                        if (currentStates->ecmaComponents.activeModes.has(mode::presets::DCSM_PRESENTATION)) {
                            currentStates->ecmaComponents.activePresentationPosition = reporting;
                        } else {    // DCSM_DATA
                            currentStates->ecmaComponents.activeDataPosition = reporting;
                        }
                    }

                    void operate_DEVICE_ATTRIBUTES(sequence::base* input) {
                        auto controlSequence = miscellaneousControlFunctions::DEVICE_ATTRIBUTES.transform(input);

                        auto params = controlSequence.getParameters();

                        if (params.size() > 1) return;

                        auto deviceType = params.back().getValueAsInteger();

                        if (deviceType == 0) {  // This is an request, and we will need to answer.
                            // We can either respond with our own code declaring GGUI, or using ecma-48 as identification.

                            constexpr uint32_t GGUI_SINGLE_VALUE_IDENTIFIER = 733;

                            std::string response;
                            miscellaneousControlFunctions::DEVICE_ATTRIBUTES.compile({GGUI_SINGLE_VALUE_IDENTIFIER}).toString(response);

                            currentStates->transmission.addToQueue(response);
                        } else {
                            // This needs to be already overridden via the DEC page re-route, something went wrong here...
                            GGUI::logger::log("ERROR: Unjustified device identification: " + std::to_string(deviceType));
                        }
                    }

                    void operate_RESET_TO_INITIAL_STATE(sequence::base*) {
                        currentStates->ecmaComponents.reset();
                    }
                }
            }
        }
    }
}