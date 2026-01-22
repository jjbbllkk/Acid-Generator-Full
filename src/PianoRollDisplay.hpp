#pragma once

#include "plugin.hpp"
#include "Generator.hpp"

using namespace AcidGenerator;

//-----------------------------------------------------------------------------
// Color Palette (ported from colors.less)
//-----------------------------------------------------------------------------
namespace Colors {
    // Grays
    inline NVGcolor gray100() { return nvgRGB(0x16, 0x16, 0x16); }
    inline NVGcolor gray90()  { return nvgRGB(0x26, 0x26, 0x26); }
    inline NVGcolor gray80()  { return nvgRGB(0x39, 0x39, 0x39); }
    inline NVGcolor gray70()  { return nvgRGB(0x52, 0x52, 0x52); }
    inline NVGcolor gray60()  { return nvgRGB(0x6f, 0x6f, 0x6f); }
    inline NVGcolor gray30()  { return nvgRGB(0xc6, 0xc6, 0xc6); }
    inline NVGcolor gray20()  { return nvgRGB(0xe0, 0xe0, 0xe0); }

    // Emerald (active notes)
    inline NVGcolor emerald30() { return nvgRGB(0x79, 0xd8, 0xb9); }
    inline NVGcolor emerald50() { return nvgRGB(0x1e, 0x9f, 0x7e); }
    inline NVGcolor emerald80() { return nvgRGB(0x1c, 0x3f, 0x34); }

    // Red (accents)
    inline NVGcolor red60()  { return nvgRGB(0xd0, 0x30, 0x3d); }
    inline NVGcolor red80()  { return nvgRGB(0x6c, 0x1c, 0x21); }

    // Yellow (slides)
    inline NVGcolor yellow60() { return nvgRGB(0x8c, 0x6a, 0x14); }
    inline NVGcolor yellow40() { return nvgRGB(0xd0, 0xa1, 0x23); }

    // Blue (playhead, octave up)
    inline NVGcolor blue60()  { return nvgRGB(0x1a, 0x71, 0xbf); }
    inline NVGcolor blue40()  { return nvgRGB(0x66, 0xac, 0xfb); }
    inline NVGcolor blue10()  { return nvgRGB(0xeb, 0xf5, 0xfe); }

    // Green (octave down)
    inline NVGcolor green60() { return nvgRGB(0x14, 0x81, 0x26); }
    inline NVGcolor green40() { return nvgRGB(0x40, 0xc0, 0x50); }

    // Helper to create alpha version
    inline NVGcolor withAlpha(NVGcolor c, float a) {
        return nvgRGBAf(c.r, c.g, c.b, a);
    }
}

//-----------------------------------------------------------------------------
// Scale Helper - Check if a semitone is in the current scale
//-----------------------------------------------------------------------------
inline bool isSemitoneInScale(int semitone, Scale scale) {
    const ScaleData& scaleData = SCALES[static_cast<int>(scale)];
    int normalized = ((semitone % 12) + 12) % 12;
    for (int i = 0; i < scaleData.length; i++) {
        if (scaleData.intervals[i] == normalized) return true;
    }
    return false;
}

// Find scale degree for a semitone, returns -1 if not in scale
inline int semitoneToScaleDegree(int semitone, Scale scale) {
    const ScaleData& scaleData = SCALES[static_cast<int>(scale)];
    int normalized = ((semitone % 12) + 12) % 12;
    for (int i = 0; i < scaleData.length; i++) {
        if (scaleData.intervals[i] == normalized) return i;
    }
    return -1;
}

//-----------------------------------------------------------------------------
// PianoRollDisplay - Custom Widget for Pattern Visualization
//-----------------------------------------------------------------------------
// Displays a grid of steps (columns) x notes (rows)
// - Shows active notes from the pattern
// - Highlights current playhead position
// - Visual indicators for accent and slide
// - Click and drag to edit notes
// - Scrolling for patterns > 16 steps
//-----------------------------------------------------------------------------

struct PianoRollDisplay : widget::OpaqueWidget {
    // Module reference (set by parent widget)
    Module* module = nullptr;

    // Display configuration
    static constexpr int NUM_ROWS = 12;      // One octave of notes
    static constexpr int VISIBLE_STEPS = 16; // Steps visible at once
    static constexpr int MAX_PATTERN_STEPS = 64;

    // Cached pattern pointer (obtained from module)
    Pattern* patternPtr = nullptr;
    int* currentStepPtr = nullptr;
    int* patternLengthPtr = nullptr;
    Scale* scalePtr = nullptr;

    // Scrolling state
    int viewOffset = 0;  // First visible step
    bool autoFollow = true;  // Auto-scroll to follow playhead

    // Drag state
    bool isDragging = false;
    int dragStartCol = -1;
    int dragStartRow = -1;
    int dragNoteValue = -1;  // Note being painted (-1 for rest)
    int dragOctave = 0;

    // Flash feedback for invalid clicks
    float invalidFlashTimer = 0.f;
    int invalidFlashCol = -1;
    int invalidFlashRow = -1;

    // Which notes are "white keys" (Major scale intervals)
    static constexpr int WHITE_KEYS[] = {0, 2, 4, 5, 7, 9, 11};

    static bool isWhiteKey(int note) {
        for (int i = 0; i < 7; i++) {
            if (WHITE_KEYS[i] == note) return true;
        }
        return false;
    }

    PianoRollDisplay() {
        box.size = Vec(160, 60);
    }

    // Scroll to ensure a step is visible
    void scrollToStep(int step) {
        if (step < viewOffset) {
            viewOffset = step;
        } else if (step >= viewOffset + VISIBLE_STEPS) {
            viewOffset = step - VISIBLE_STEPS + 1;
        }
        clampViewOffset();
    }

    void clampViewOffset() {
        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        int maxOffset = std::max(0, patternLength - VISIBLE_STEPS);
        viewOffset = std::clamp(viewOffset, 0, maxOffset);
    }

    void drawLayer(const DrawArgs& args, int layer) override {
        if (layer != 1) return;

        NVGcontext* vg = args.vg;
        Rect b = box.zeroPos();

        float cellWidth = b.size.x / VISIBLE_STEPS;
        float cellHeight = b.size.y / NUM_ROWS;

        // Get module data if available
        int currentStep = -1;
        int patternLength = 16;
        Scale scale = Scale::MINOR;

        if (module) {
            if (currentStepPtr) currentStep = *currentStepPtr;
            if (patternLengthPtr) patternLength = *patternLengthPtr;
            if (scalePtr) scale = *scalePtr;

            // Auto-follow playhead
            if (autoFollow && currentStep >= 0) {
                scrollToStep(currentStep);
            }
        }

        clampViewOffset();

        // Update flash timer
        if (invalidFlashTimer > 0.f) {
            invalidFlashTimer -= APP->window->getLastFrameDuration();
        }

        // --- Draw Background Grid ---
        for (int row = 0; row < NUM_ROWS; row++) {
            int noteInOctave = row;  // 0 = C, 1 = C#, etc. (bottom to top)
            bool inScale = isSemitoneInScale(noteInOctave, scale);

            for (int visCol = 0; visCol < VISIBLE_STEPS; visCol++) {
                int col = visCol + viewOffset;
                float x = visCol * cellWidth;
                float y = b.size.y - (row + 1) * cellHeight;  // Flip Y (low notes at bottom)

                // Cell background
                NVGcolor bgColor;
                if (inScale) {
                    // Scale notes: white/black key pattern
                    bgColor = isWhiteKey(noteInOctave)
                        ? Colors::withAlpha(Colors::gray20(), 0.35f)
                        : Colors::withAlpha(Colors::gray70(), 0.5f);
                } else {
                    // Non-scale notes: darker, less prominent
                    bgColor = Colors::withAlpha(Colors::gray90(), 0.25f);
                }

                // Dim steps beyond pattern length
                if (col >= patternLength) {
                    bgColor = Colors::withAlpha(bgColor, 0.3f);
                }

                nvgBeginPath(vg);
                nvgRect(vg, x, y, cellWidth - 0.5f, cellHeight - 0.5f);
                nvgFillColor(vg, bgColor);
                nvgFill(vg);

                // Flash invalid click feedback
                if (invalidFlashTimer > 0.f && col == invalidFlashCol && row == invalidFlashRow) {
                    nvgBeginPath(vg);
                    nvgRect(vg, x, y, cellWidth - 0.5f, cellHeight - 0.5f);
                    nvgFillColor(vg, Colors::withAlpha(Colors::red60(), invalidFlashTimer * 2.f));
                    nvgFill(vg);
                }

                // Grid lines
                nvgBeginPath(vg);
                nvgRect(vg, x, y, cellWidth, cellHeight);
                nvgStrokeColor(vg, Colors::withAlpha(Colors::gray60(), 0.2f));
                nvgStrokeWidth(vg, 0.5f);
                nvgStroke(vg);

                // Beat markers (every 4 steps)
                if (col % 4 == 0 && col < patternLength) {
                    nvgBeginPath(vg);
                    nvgMoveTo(vg, x, 0);
                    nvgLineTo(vg, x, b.size.y);
                    nvgStrokeColor(vg, Colors::withAlpha(Colors::gray30(), 0.3f));
                    nvgStrokeWidth(vg, 1.0f);
                    nvgStroke(vg);
                }
            }
        }

        // --- Draw Pattern Notes ---
        if (module && patternPtr) {
            for (int visCol = 0; visCol < VISIBLE_STEPS; visCol++) {
                int col = visCol + viewOffset;
                if (col >= patternLength) continue;

                const SequenceStep& step = patternPtr->steps[col];
                if (step.isRest()) continue;

                // Get the note within the octave (0-11)
                int semitone = getNoteInScale(step.note, scale, 0, 0);
                int displayNote = ((semitone % 12) + 12) % 12;

                float x = visCol * cellWidth;
                float y = b.size.y - (displayNote + 1) * cellHeight;
                float noteWidth = cellWidth - 1.0f;
                float noteHeight = cellHeight - 1.0f;

                // Slide extends into next cell
                if (step.slide && visCol < VISIBLE_STEPS - 1) {
                    noteWidth = cellWidth * 1.3f;
                }

                // Note color based on accent
                NVGcolor noteColor = step.accent
                    ? Colors::withAlpha(Colors::emerald30(), 0.9f)
                    : Colors::withAlpha(Colors::emerald50(), 0.7f);

                // Draw note rectangle
                nvgBeginPath(vg);
                nvgRoundedRect(vg, x + 0.5f, y + 0.5f, noteWidth, noteHeight, 2.0f);
                nvgFillColor(vg, noteColor);
                nvgFill(vg);

                // Accent indicator (red border)
                if (step.accent) {
                    nvgBeginPath(vg);
                    nvgRoundedRect(vg, x + 0.5f, y + 0.5f, noteWidth, noteHeight, 2.0f);
                    nvgStrokeColor(vg, Colors::red60());
                    nvgStrokeWidth(vg, 1.5f);
                    nvgStroke(vg);
                }

                // Slide indicator (yellow tail)
                if (step.slide) {
                    nvgBeginPath(vg);
                    nvgMoveTo(vg, x + cellWidth - 2, y + noteHeight * 0.3f);
                    nvgLineTo(vg, x + cellWidth + cellWidth * 0.2f, y + noteHeight * 0.5f);
                    nvgLineTo(vg, x + cellWidth - 2, y + noteHeight * 0.7f);
                    nvgFillColor(vg, Colors::yellow40());
                    nvgFill(vg);
                }

                // Octave indicator
                if (step.octave != 0) {
                    float indicatorSize = 4.0f;
                    float ix = x + noteWidth - indicatorSize - 1;
                    float iy = y + 1;

                    nvgBeginPath(vg);
                    nvgCircle(vg, ix + indicatorSize/2, iy + indicatorSize/2, indicatorSize/2);
                    nvgFillColor(vg, step.octave > 0 ? Colors::blue40() : Colors::green40());
                    nvgFill(vg);
                }
            }
        }

        // --- Draw Playhead ---
        int visiblePlayhead = currentStep - viewOffset;
        if (visiblePlayhead >= 0 && visiblePlayhead < VISIBLE_STEPS) {
            float x = visiblePlayhead * cellWidth;

            // Playhead column highlight
            nvgBeginPath(vg);
            nvgRect(vg, x, 0, cellWidth, b.size.y);
            nvgFillColor(vg, Colors::withAlpha(Colors::blue60(), 0.2f));
            nvgFill(vg);

            // Playhead line
            nvgBeginPath(vg);
            nvgMoveTo(vg, x, 0);
            nvgLineTo(vg, x, b.size.y);
            nvgStrokeColor(vg, Colors::blue40());
            nvgStrokeWidth(vg, 2.0f);
            nvgStroke(vg);
        }

        // --- Draw Scroll Indicators ---
        if (viewOffset > 0) {
            // Left arrow indicator
            nvgBeginPath(vg);
            nvgMoveTo(vg, 8, b.size.y / 2);
            nvgLineTo(vg, 2, b.size.y / 2 - 4);
            nvgLineTo(vg, 2, b.size.y / 2 + 4);
            nvgClosePath(vg);
            nvgFillColor(vg, Colors::withAlpha(Colors::gray30(), 0.7f));
            nvgFill(vg);
        }

        if (viewOffset + VISIBLE_STEPS < patternLength) {
            // Right arrow indicator
            nvgBeginPath(vg);
            nvgMoveTo(vg, b.size.x - 8, b.size.y / 2);
            nvgLineTo(vg, b.size.x - 2, b.size.y / 2 - 4);
            nvgLineTo(vg, b.size.x - 2, b.size.y / 2 + 4);
            nvgClosePath(vg);
            nvgFillColor(vg, Colors::withAlpha(Colors::gray30(), 0.7f));
            nvgFill(vg);
        }

        // --- Draw Page Indicator ---
        if (patternLength > VISIBLE_STEPS) {
            int numPages = (patternLength + VISIBLE_STEPS - 1) / VISIBLE_STEPS;
            int currentPage = viewOffset / VISIBLE_STEPS;
            float dotSpacing = 6.f;
            float totalWidth = numPages * dotSpacing;
            float startX = (b.size.x - totalWidth) / 2.f;

            for (int i = 0; i < numPages; i++) {
                nvgBeginPath(vg);
                nvgCircle(vg, startX + i * dotSpacing + 2, b.size.y - 4, 2);
                nvgFillColor(vg, i == currentPage ? Colors::blue40() : Colors::withAlpha(Colors::gray60(), 0.5f));
                nvgFill(vg);
            }
        }

        // --- Draw Border ---
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, b.size.x, b.size.y);
        nvgStrokeColor(vg, Colors::gray60());
        nvgStrokeWidth(vg, 1.0f);
        nvgStroke(vg);

        OpaqueWidget::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override {
        // Background
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, Colors::gray100());
        nvgFill(args.vg);

        OpaqueWidget::draw(args);
    }

    //-------------------------------------------------------------------------
    // Mouse Wheel - Scroll the view
    //-------------------------------------------------------------------------
    void onHover(const HoverEvent& e) override {
        OpaqueWidget::onHover(e);
        e.consume(this);
    }

    void onHoverScroll(const HoverScrollEvent& e) override {
        // Horizontal scroll
        int scrollAmount = (e.scrollDelta.x != 0) ? -static_cast<int>(e.scrollDelta.x / 10.f) : static_cast<int>(e.scrollDelta.y / 10.f);
        if (scrollAmount != 0) {
            autoFollow = false;  // Disable auto-follow when manually scrolling
            viewOffset += scrollAmount;
            clampViewOffset();
            e.consume(this);
        }
    }

    //-------------------------------------------------------------------------
    // Click/Drag Interaction - Edit notes in pattern
    //-------------------------------------------------------------------------
    void onButton(const ButtonEvent& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
            OpaqueWidget::onButton(e);
            return;
        }

        if (!module || !patternPtr) {
            OpaqueWidget::onButton(e);
            return;
        }

        float cellWidth = box.size.x / VISIBLE_STEPS;
        float cellHeight = box.size.y / NUM_ROWS;

        int visCol = static_cast<int>(e.pos.x / cellWidth);
        int row = NUM_ROWS - 1 - static_cast<int>(e.pos.y / cellHeight);
        int col = visCol + viewOffset;

        if (visCol < 0 || visCol >= VISIBLE_STEPS || row < 0 || row >= NUM_ROWS) {
            OpaqueWidget::onButton(e);
            return;
        }

        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        if (col >= patternLength) {
            OpaqueWidget::onButton(e);
            return;
        }

        Scale scale = scalePtr ? *scalePtr : Scale::MINOR;

        if (e.action == GLFW_PRESS) {
            SequenceStep& step = patternPtr->steps[col];
            int currentSemitone = step.isRest() ? -1 : getNoteInScale(step.note, scale, 0, 0);
            int currentDisplay = currentSemitone >= 0 ? ((currentSemitone % 12) + 12) % 12 : -1;

            if (currentDisplay == row) {
                // Clicking same note - modify or toggle
                if (e.mods & GLFW_MOD_SHIFT) {
                    step.accent = !step.accent;
                } else if (e.mods & GLFW_MOD_CONTROL) {
                    step.slide = !step.slide;
                } else if (e.mods & GLFW_MOD_ALT) {
                    step.octave = (step.octave + 2) % 3 - 1;
                } else {
                    // Regular click - toggle to rest
                    step.note = -1;
                    step.accent = false;
                    step.slide = false;
                    dragNoteValue = -1;
                }
            } else {
                // Clicking different row - try to set new note
                int newScaleDegree = semitoneToScaleDegree(row, scale);

                if (newScaleDegree >= 0) {
                    step.note = newScaleDegree;
                    step.octave = 0;
                    step.accent = false;
                    step.slide = false;
                    dragNoteValue = newScaleDegree;
                    dragOctave = 0;
                } else {
                    // Invalid note - flash feedback
                    invalidFlashTimer = 0.3f;
                    invalidFlashCol = col;
                    invalidFlashRow = row;
                }
            }

            // Start drag
            isDragging = true;
            dragStartCol = col;
            dragStartRow = row;

            e.consume(this);
        } else if (e.action == GLFW_RELEASE) {
            isDragging = false;
            e.consume(this);
        }
    }

    void onDragMove(const DragMoveEvent& e) override {
        // Drag painting is handled in onDragHover which has position info
        OpaqueWidget::onDragMove(e);
    }

    void onDragHover(const DragHoverEvent& e) override {
        if (!isDragging || !module || !patternPtr) {
            OpaqueWidget::onDragHover(e);
            return;
        }

        float cellWidth = box.size.x / VISIBLE_STEPS;
        float cellHeight = box.size.y / NUM_ROWS;

        int visCol = static_cast<int>(e.pos.x / cellWidth);
        int row = NUM_ROWS - 1 - static_cast<int>(e.pos.y / cellHeight);
        int col = visCol + viewOffset;

        if (visCol < 0 || visCol >= VISIBLE_STEPS || row < 0 || row >= NUM_ROWS) {
            OpaqueWidget::onDragHover(e);
            return;
        }

        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        if (col >= patternLength || col == dragStartCol) {
            OpaqueWidget::onDragHover(e);
            return;
        }

        SequenceStep& step = patternPtr->steps[col];

        // Paint the same note value as drag start
        if (dragNoteValue < 0) {
            // Painting rests
            step.note = -1;
            step.accent = false;
            step.slide = false;
        } else {
            // Painting notes
            step.note = dragNoteValue;
            step.octave = dragOctave;
            step.accent = false;
            step.slide = false;
        }

        dragStartCol = col;  // Update to prevent re-painting same cell

        e.consume(this);
    }

    void onDragEnd(const DragEndEvent& e) override {
        isDragging = false;
        OpaqueWidget::onDragEnd(e);
    }

    void onLeave(const LeaveEvent& e) override {
        isDragging = false;
        OpaqueWidget::onLeave(e);
    }

    //-------------------------------------------------------------------------
    // Double-click to toggle auto-follow
    //-------------------------------------------------------------------------
    void onDoubleClick(const DoubleClickEvent& e) override {
        autoFollow = !autoFollow;
        e.consume(this);
    }
};

//-----------------------------------------------------------------------------
// Accent/Slide/Octave Row Display (with labels and click editing)
//-----------------------------------------------------------------------------

struct StepIndicatorDisplay : widget::OpaqueWidget {
    Module* module = nullptr;
    Pattern* patternPtr = nullptr;
    int* currentStepPtr = nullptr;
    int* patternLengthPtr = nullptr;
    int* viewOffsetPtr = nullptr;  // Shared with piano roll for synchronized scrolling

    enum class RowType { OCTAVE, SLIDE, ACCENT };
    RowType rowType = RowType::ACCENT;

    static constexpr int VISIBLE_STEPS = 16;
    static constexpr float LABEL_WIDTH = 12.f;  // Width reserved for label

    StepIndicatorDisplay() {
        box.size = Vec(160, 8);
    }

    int getViewOffset() const {
        return viewOffsetPtr ? *viewOffsetPtr : 0;
    }

    const char* getLabel() const {
        switch (rowType) {
            case RowType::ACCENT: return "A";
            case RowType::SLIDE:  return "S";
            case RowType::OCTAVE: return "O";
        }
        return "";
    }

    NVGcolor getLabelColor() const {
        switch (rowType) {
            case RowType::ACCENT: return Colors::red60();
            case RowType::SLIDE:  return Colors::yellow40();
            case RowType::OCTAVE: return Colors::blue40();
        }
        return Colors::gray60();
    }

    void drawLayer(const DrawArgs& args, int layer) override {
        if (layer != 1) return;

        NVGcontext* vg = args.vg;
        int viewOffset = getViewOffset();
        float gridWidth = box.size.x - LABEL_WIDTH;
        float cellWidth = gridWidth / VISIBLE_STEPS;
        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        int currentStep = currentStepPtr ? *currentStepPtr : -1;

        // Draw label
        nvgFontSize(vg, 7);
        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(vg, getLabelColor());
        nvgText(vg, LABEL_WIDTH / 2, box.size.y / 2, getLabel(), nullptr);

        // Draw cells
        for (int visCol = 0; visCol < VISIBLE_STEPS; visCol++) {
            int col = visCol + viewOffset;
            float x = LABEL_WIDTH + visCol * cellWidth;
            bool isActive = false;
            NVGcolor activeColor = Colors::gray60();

            if (module && patternPtr && col < patternLength) {
                const SequenceStep& step = patternPtr->steps[col];

                switch (rowType) {
                    case RowType::ACCENT:
                        isActive = !step.isRest() && step.accent;
                        activeColor = Colors::red60();
                        break;
                    case RowType::SLIDE:
                        isActive = !step.isRest() && step.slide;
                        activeColor = Colors::yellow40();
                        break;
                    case RowType::OCTAVE:
                        isActive = !step.isRest() && step.octave != 0;
                        activeColor = step.octave > 0 ? Colors::blue40() : Colors::green40();
                        break;
                }
            }

            // Background
            NVGcolor bgColor = col >= patternLength
                ? Colors::withAlpha(Colors::gray90(), 0.3f)
                : Colors::withAlpha(Colors::gray80(), 0.5f);

            nvgBeginPath(vg);
            nvgRect(vg, x, 0, cellWidth - 0.5f, box.size.y);
            nvgFillColor(vg, bgColor);
            nvgFill(vg);

            // Active indicator
            if (isActive) {
                nvgBeginPath(vg);
                nvgRoundedRect(vg, x + 1, 1, cellWidth - 2, box.size.y - 2, 2);
                nvgFillColor(vg, Colors::withAlpha(activeColor, 0.8f));
                nvgFill(vg);
            }

            // Playhead highlight
            if (col == currentStep) {
                nvgBeginPath(vg);
                nvgRect(vg, x, 0, cellWidth, box.size.y);
                nvgFillColor(vg, Colors::withAlpha(Colors::blue60(), 0.3f));
                nvgFill(vg);
            }
        }

        // Border
        nvgBeginPath(vg);
        nvgRect(vg, LABEL_WIDTH, 0, gridWidth, box.size.y);
        nvgStrokeColor(vg, Colors::withAlpha(Colors::gray60(), 0.5f));
        nvgStrokeWidth(vg, 0.5f);
        nvgStroke(vg);

        OpaqueWidget::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, Colors::gray100());
        nvgFill(args.vg);

        OpaqueWidget::draw(args);
    }

    //-------------------------------------------------------------------------
    // Click to toggle property
    //-------------------------------------------------------------------------
    void onButton(const ButtonEvent& e) override {
        if (e.action != GLFW_PRESS || e.button != GLFW_MOUSE_BUTTON_LEFT) {
            OpaqueWidget::onButton(e);
            return;
        }

        if (!module || !patternPtr) {
            OpaqueWidget::onButton(e);
            return;
        }

        // Check if click is in the grid area (not the label)
        if (e.pos.x < LABEL_WIDTH) {
            OpaqueWidget::onButton(e);
            return;
        }

        int viewOffset = getViewOffset();
        float gridWidth = box.size.x - LABEL_WIDTH;
        float cellWidth = gridWidth / VISIBLE_STEPS;

        int visCol = static_cast<int>((e.pos.x - LABEL_WIDTH) / cellWidth);
        int col = visCol + viewOffset;

        if (visCol < 0 || visCol >= VISIBLE_STEPS) {
            OpaqueWidget::onButton(e);
            return;
        }

        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        if (col >= patternLength) {
            OpaqueWidget::onButton(e);
            return;
        }

        SequenceStep& step = patternPtr->steps[col];

        // Don't modify rests
        if (step.isRest()) {
            e.consume(this);
            return;
        }

        switch (rowType) {
            case RowType::ACCENT:
                step.accent = !step.accent;
                break;
            case RowType::SLIDE:
                step.slide = !step.slide;
                break;
            case RowType::OCTAVE:
                // Cycle: 0 -> 1 -> -1 -> 0
                if (step.octave == 0) step.octave = 1;
                else if (step.octave == 1) step.octave = -1;
                else step.octave = 0;
                break;
        }

        e.consume(this);
    }
};

//-----------------------------------------------------------------------------
// Step Number Display (with scroll synchronization)
//-----------------------------------------------------------------------------

struct StepNumberDisplay : widget::OpaqueWidget {
    int* currentStepPtr = nullptr;
    int* patternLengthPtr = nullptr;
    int* viewOffsetPtr = nullptr;
    Module* module = nullptr;

    static constexpr int VISIBLE_STEPS = 16;
    static constexpr float LABEL_WIDTH = 12.f;

    StepNumberDisplay() {
        box.size = Vec(160, 10);
    }

    int getViewOffset() const {
        return viewOffsetPtr ? *viewOffsetPtr : 0;
    }

    void drawLayer(const DrawArgs& args, int layer) override {
        if (layer != 1) return;

        NVGcontext* vg = args.vg;
        int viewOffset = getViewOffset();
        float gridWidth = box.size.x - LABEL_WIDTH;
        float cellWidth = gridWidth / VISIBLE_STEPS;
        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        int currentStep = currentStepPtr ? *currentStepPtr : -1;

        // Label area (empty for alignment)
        nvgFontSize(vg, 7);
        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(vg, Colors::gray60());
        nvgText(vg, LABEL_WIDTH / 2, box.size.y / 2, "#", nullptr);

        nvgFontSize(vg, 8);

        for (int visCol = 0; visCol < VISIBLE_STEPS; visCol++) {
            int col = visCol + viewOffset;
            float x = LABEL_WIDTH + visCol * cellWidth + cellWidth / 2;
            float y = box.size.y / 2;

            NVGcolor textColor = col >= patternLength
                ? Colors::withAlpha(Colors::gray60(), 0.3f)
                : (col == currentStep ? Colors::blue40() : Colors::gray60());

            char num[4];
            snprintf(num, sizeof(num), "%d", col + 1);

            nvgFillColor(vg, textColor);
            nvgText(vg, x, y, num, nullptr);
        }

        OpaqueWidget::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, Colors::gray100());
        nvgFill(args.vg);

        OpaqueWidget::draw(args);
    }
};
