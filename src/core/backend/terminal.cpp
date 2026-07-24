#include "terminal.h"
#include "../../elements/element.h"

/**
 * Cross platform functions and containers are held here:
*/

namespace GGUI {
    namespace terminal {
        base* currentStates = nullptr;

        size_t outputCapture::getActiveIndex() const {
            return (cursor.y * dimensions.x) + cursor.x;
        }

        void outputCapture::link(element* DOM) {
            dom = DOM;
        }

        // Used as an helper tool to easily transform between the two identities.
        constexpr ecma::activeSGRStyle rasterize(ActiveStyle point) {
            ecma::activeSGRStyle result;

            if (point.opacity != UINT8_MAX) {
                // we need to ask the terminal for the active default colors
                TODO("Needs XTERM module, for active default terminal colors.")
            }

            // Since the activeTextAttributes are aliased from ecma::textAttributes this is ok.
            result.textAttributes = point.activeTextAttributes.getData();
            result.textColor = point.activeTextColor;
            result.backgroundColor = point.activeBackgroundColor;

            return result;
        }

        // first we go recursively inside the container and the child that contains this point.
        std::pair<bool, ActiveStyle> outputCapture::trace(IVector2 point, element* currentContainer) {
            // since overflow content inside a container only is allowed is the container is dynamic in size, otherwise overflow content is always hidden, so the lowest identity pool is always the largest. 
            if (!currentContainer->graphicalIdentityPool.back().area.hits(point)) { return {false, {} }; }

            std::pair<bool, ActiveStyle> handle = {false, {} };
            
            // now check if any of the childs inside this area is closer via hit
            for (auto* child : currentContainer->getVisibleChilds()) {  // should return via z-priority
                handle = trace(point, child);   // this should return the closest hit.

                if (handle.first) break;    // since z-priority the first match should be the best one.
            }

            // If none of the childs hit, then this current Container is the closest hit
            if (!handle.first) {      
                for (auto& localGraphicalPool : currentContainer->graphicalIdentityPool) {
                    if (localGraphicalPool.area.hits(point)) {
                        handle.second = localGraphicalPool;    // this is the closest hit.
                        handle.first = true;
                        handle.second.activeTextAttributes.add(textAttributeTypes::FOREGROUND_COLOR);
                        handle.second.activeTextAttributes.add(textAttributeTypes::BACKGROUND_COLOR);
                        break;
                    }
                }

                // regardless of opacity there is no more data here, we need to go back to current containers parent container.
                return handle;
            }

            // only process further if opacity is not 100%
            if (handle.second.opacity == UINT8_MAX) return handle;

            // here we now have to process the opacity.
            // Since we are currently in the >>second<< closest hit element
            // Two probable instances: 
            //   first: the incoming handle still hovers over some other current container child reflection pool
            //   second: the incoming handle needs to be calculated via this current containers own identity pool
            // Since the second is an extension of the first instance, we can do both of them at the same time.
            
            size_t start = 0;

            // First find the index of the handle
            for (; start < currentContainer->graphicalReflectionPool.size(); start++) {
                const ActiveStyle* current = currentContainer->graphicalReflectionPool[start];
                if (current->origin == handle.second.origin && current->area.hits(point)) break;    // we found the handle, now we can start processing from this index onwards.
            }
            
            // skip styles from the same origin and find the layer below
            for (; 
                start < currentContainer->graphicalReflectionPool.size() && 
                currentContainer->graphicalReflectionPool[start]->origin == handle.second.origin
                ; start++
            );

            // now that we have the start index, we can start iterating from that point onwards and every reflected style that hits should contribute to the color via the opacity compute.
            for (; start < currentContainer->graphicalReflectionPool.size(); start++) {
                auto* reflectedStyle = currentContainer->graphicalReflectionPool[start];

                if (reflectedStyle->area.hits(point)) {
                    handle.second = handle.second.computeColor(reflectedStyle);

                    if (handle.second.opacity == UINT8_MAX) break;    // if we reach 100% opacity, we can stop processing further.
                }
            }

            // It is ok if handles opacity is not 100% here, we just let the recursion handle the rest.
            // Note that if an x<100% opacity handle gets into the final SGR style, then it will be mixed by the terminals active default colors.
            return handle;
        }

        size_t outputCapture::getIndexOf(IVector2 val) const {
            return (val.y * dom->getWidth()) + val.x;
        }

        void outputCapture::computeSGRAreas() {
            // Clear residue from previous render
            activeGraphicAttributes.clear();    TODO("Change this to a dif to only render changed areas.")

            for (const auto& pos : dom->getVerticalFaces()) {
                auto currentCellStyle = trace(pos, dom).second;
                auto rasterizedCellStyle = rasterize(currentCellStyle);

                if (!activeGraphicAttributes.empty() && rasterizedCellStyle == activeGraphicAttributes.back()) continue;
                
                rasterizedCellStyle.start = pos; // SGR needs to know where this style begins
                activeGraphicAttributes.push_back(rasterizedCellStyle);
            }
        }

        void outputCapture::preparePresentationBuffer() {
            static std::string result; // internal cache between renders

            size_t liquefiedSize = 0;

            TODO("Remove this when moving from compactString into std::u32string")
            for (auto& cs : buffer) {
                liquefiedSize += cs.size;
            }

            static std::vector<std::string> preBakedSGRSequences;
            
            if (preBakedSGRSequences.size() != activeGraphicAttributes.size()) {
                preBakedSGRSequences.clear();
                preBakedSGRSequences.resize(activeGraphicAttributes.size(), {});
            }

            // Since we know how many SGR style attributes were gonna get
            for (size_t i = 0; i < activeGraphicAttributes.size(); i++) {
                preBakedSGRSequences[i] = ecma::sequence::toString(
                    ecma::sequences::presentationControlFunctions::SELECT_GRAPHIC_RENDITION.compile(activeGraphicAttributes[i].compile())
                );

                liquefiedSize += preBakedSGRSequences[i].size();
            }

            if (result.size() != liquefiedSize){
                // Resize a std::string to the total size.
                result.resize(liquefiedSize, '\0');
            }

            // Fast-path pointer access to avoid bounds checks and replace overhead
            unsigned int outputIndex = 0;

            size_t currentSGRIndex = 0;
            size_t nextSGRStartPositionAsIndex = UINT32_MAX;

            if (!activeGraphicAttributes.empty()) {
                nextSGRStartPositionAsIndex = getIndexOf(activeGraphicAttributes[currentSGRIndex].start);
            }

            IVector2 start = {0, 0};
            // IVector2 end = cursor + dimensions;
            IVector2 end = cursor + IVector2{dom->getWidth(), dom->getHeight()};

            for (int y = start.y; y < end.y; y++) {
                for (int x = start.x; x < end.x; x++) {
                    const compactString& data = buffer[(y * dom->getWidth()) + x];

                    if (getIndexOf({x, y}) == nextSGRStartPositionAsIndex) {
                        // Insert the SGR sequence into the output buffer
                        auto sgrSequence = preBakedSGRSequences[currentSGRIndex];

                        std::memcpy(result.data() + outputIndex, sgrSequence.data(), sgrSequence.size());
                        outputIndex += sgrSequence.size();

                        currentSGRIndex++;

                        if (currentSGRIndex < activeGraphicAttributes.size()) {
                            nextSGRStartPositionAsIndex = getIndexOf(activeGraphicAttributes[currentSGRIndex].start);
                        } else {
                            nextSGRStartPositionAsIndex = UINT32_MAX; // No more SGR sequences to insert
                        }
                    }

                    // Copy multi-byte unicode sequence directly
                    std::memcpy(result.data() + outputIndex, data.text, data.size);
                    outputIndex += data.size;
                }
            }

            liquefiedBuffer = &result;
        }

        void outputCapture::renderBuffer() {
            // Write cursor-home, then the frame buffer. Avoid stdio printf/fflush.
            static const std::string cursorReset = ecma::sequence::toString(ecma::sequences::cursorControlFunctions::CURSOR_POSITION.compile({0, 0}));

            queryOutput({
                cursorReset,
                *currentStates->screen.liquefiedBuffer
            });
        }

        void outputCapture::update() {
            buffer = dom->render();
        }

        void init() {
            currentStates = new base();

            platformInit();
        }

        extern void platformDeinit();
        void deinit() {
            // Calls based on feature flags correct public and/or private SGR or other extension CSI's.

            // Now cal platform specific de-initializers
            platformDeinit();
        }

        void parseInput() {
            // Parses input based on modular features, each brought by their own respective flag.

            // If special loaders needed to be present they better have been initialized properly at initialization phase when the handshake/probing happens.
            for (auto sequence : ecma::sequence::parse(std::string_view(currentStates->transmission.inputBuffer.data(), currentStates->transmission.inputSize))) {

                // This is likely redundant, since all operations have their own handler to process the functionality of the specific operation
                switch (sequence->getType()) {
                    default:
                        break;
                }

            }
        }
    }
}