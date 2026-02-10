#pragma once

#include "plugin.hpp"
#include "Generator.hpp"

using namespace AcidGenerator;

//-----------------------------------------------------------------------------
// Color Palette - vulpes79 Design Language
// Monochrome grayscale base with yellow (#ffff00) accents
//-----------------------------------------------------------------------------
namespace Colors {
    // Panel backgrounds (light theme)
    inline NVGcolor panelBg()    { return nvgRGB(0xe6, 0xe6, 0xe6); }  // Main panel
    inline NVGcolor sectionBg()  { return nvgRGB(0xdc, 0xdc, 0xdc); }  // Section grouping
    inline NVGcolor cvBg()       { return nvgRGB(0x22, 0x22, 0x22); }  // CV input area
    inline NVGcolor outputBg()   { return nvgRGB(0x1a, 0x1a, 0x1a); }  // Output area (darkest)

    // Grays (rebalanced for light panel)
    inline NVGcolor gray100() { return nvgRGB(0x1a, 0x1a, 0x1a); }  // Darkest text
    inline NVGcolor gray90()  { return nvgRGB(0x2a, 0x2a, 0x2a); }
    inline NVGcolor gray80()  { return nvgRGB(0x44, 0x44, 0x44); }
    inline NVGcolor gray70()  { return nvgRGB(0x55, 0x55, 0x55); }
    inline NVGcolor gray60()  { return nvgRGB(0x6f, 0x6f, 0x6f); }
    inline NVGcolor gray50()  { return nvgRGB(0x99, 0x99, 0x99); }  // Icon strokes
    inline NVGcolor gray30()  { return nvgRGB(0xb3, 0xb3, 0xb3); }  // Light labels
    inline NVGcolor gray20()  { return nvgRGB(0xdc, 0xdc, 0xdc); }
    inline NVGcolor gray10()  { return nvgRGB(0xe6, 0xe6, 0xe6); }

    // Yellow accent (used sparingly - divider lines, panel elements only)
    inline NVGcolor accent()     { return nvgRGB(0xff, 0xff, 0x00); }  // #ffff00

    // Display accent - signature cyan/teal per DESIGN.md
    inline NVGcolor displayAccent()    { return nvgRGB(0x79, 0xd8, 0xb9); }  // #79d8b9
    inline NVGcolor displayAccentDim() { return nvgRGB(0x50, 0x90, 0x80); }  // #509080

    // Note colors (uses display accent)
    inline NVGcolor noteActive()   { return displayAccent(); }     // Active notes - cyan/teal
    inline NVGcolor noteNormal()   { return displayAccentDim(); }  // Normal notes - dim teal
    inline NVGcolor noteBg()       { return nvgRGB(0x1a, 0x2a, 0x25); }  // Dark teal tint

    // Accent markers (orange per DESIGN.md)
    inline NVGcolor accentOn()  { return nvgRGB(0xff, 0x80, 0x40); }  // #ff8040
    inline NVGcolor accentOff() { return nvgRGB(0xaa, 0x55, 0x22); }  // #aa5522

    // Slide markers (blue per DESIGN.md)
    inline NVGcolor slideOn()  { return nvgRGB(0x40, 0x80, 0xff); }  // #4080ff
    inline NVGcolor slideOff() { return nvgRGB(0x22, 0x55, 0xaa); }  // #2255aa

    // Display backgrounds
    inline NVGcolor displayBg()     { return nvgRGB(0x0a, 0x0a, 0x0a); }  // Near-black
    inline NVGcolor displayBorder() { return nvgRGB(0x33, 0x33, 0x33); }  // Dark gray border

    // Blue (octave up indicator)
    inline NVGcolor blue60()  { return nvgRGB(0x1a, 0x71, 0xbf); }
    inline NVGcolor blue40()  { return nvgRGB(0x66, 0xac, 0xfb); }
    inline NVGcolor blue10()  { return nvgRGB(0xeb, 0xf5, 0xfe); }

    // Green (octave down indicator)
    inline NVGcolor green60() { return nvgRGB(0x14, 0x81, 0x26); }
    inline NVGcolor green40() { return nvgRGB(0x40, 0xc0, 0x50); }

    // Helper to create alpha version
    inline NVGcolor withAlpha(NVGcolor c, float a) {
        return nvgRGBAf(c.r, c.g, c.b, a);
    }

    // LED display colors (cyan/teal to match display accent)
    inline NVGcolor ledOn()  { return nvgRGB(0x79, 0xd8, 0xb9); }  // #79d8b9
    inline NVGcolor ledOff() { return nvgRGB(0x15, 0x2a, 0x22); }  // Dark teal
    inline NVGcolor ledGlow() { return nvgRGBA(0x79, 0xd8, 0xb9, 0x40); }  // Teal glow
}

//-----------------------------------------------------------------------------
// LED Segment Display - 14-segment style for text display
//-----------------------------------------------------------------------------
// Displays text using LED-style segments. Supports A-Z, 0-9, space, #, -.
// Each character is drawn as illuminated segments for that classic hardware look.
//-----------------------------------------------------------------------------

struct LEDSegmentDisplay : widget::OpaqueWidget {
    std::string text = "";
    float charWidth = 10.f;
    float charHeight = 16.f;
    float charSpacing = 2.f;
    bool centerText = true;

    // 14-segment encoding for each character
    // Segments: 0=top, 1=topRight, 2=botRight, 3=bot, 4=botLeft, 5=topLeft
    //           6=midLeft, 7=midRight, 8=topLeftDiag, 9=topRightDiag
    //           10=botLeftDiag, 11=botRightDiag, 12=topMid, 13=botMid

    void drawCharacter(NVGcontext* vg, float x, float y, char c) {
        // Segment positions relative to character origin
        float w = charWidth;
        float h = charHeight;
        float hw = w / 2;
        float hh = h / 2;
        float t = 1.5f;  // Segment thickness

        // Get segment mask for character
        uint16_t segments = getSegments(c);

        auto drawSegment = [&](int seg) {
            bool on = (segments >> seg) & 1;
            nvgFillColor(vg, on ? Colors::ledOn() : Colors::ledOff());

            // Add glow for lit segments
            if (on) {
                nvgBeginPath(vg);
                switch (seg) {
                    case 0:  // Top horizontal
                        nvgRoundedRect(vg, x + t, y, w - 2*t, t, t/2);
                        break;
                    case 1:  // Top right vertical
                        nvgRoundedRect(vg, x + w - t, y + t, t, hh - t, t/2);
                        break;
                    case 2:  // Bottom right vertical
                        nvgRoundedRect(vg, x + w - t, y + hh, t, hh - t, t/2);
                        break;
                    case 3:  // Bottom horizontal
                        nvgRoundedRect(vg, x + t, y + h - t, w - 2*t, t, t/2);
                        break;
                    case 4:  // Bottom left vertical
                        nvgRoundedRect(vg, x, y + hh, t, hh - t, t/2);
                        break;
                    case 5:  // Top left vertical
                        nvgRoundedRect(vg, x, y + t, t, hh - t, t/2);
                        break;
                    case 6:  // Middle left horizontal
                        nvgRoundedRect(vg, x + t, y + hh - t/2, hw - t, t, t/2);
                        break;
                    case 7:  // Middle right horizontal
                        nvgRoundedRect(vg, x + hw, y + hh - t/2, hw - t, t, t/2);
                        break;
                    case 12: // Top middle vertical
                        nvgRoundedRect(vg, x + hw - t/2, y + t, t, hh - t, t/2);
                        break;
                    case 13: // Bottom middle vertical
                        nvgRoundedRect(vg, x + hw - t/2, y + hh, t, hh - t, t/2);
                        break;
                    // Diagonals (8, 9, 10, 11) - simplified as small lines
                    case 8:  // Top left diagonal
                        nvgMoveTo(vg, x + t, y + t);
                        nvgLineTo(vg, x + hw - t/2, y + hh - t/2);
                        nvgStrokeColor(vg, Colors::ledOn());
                        nvgStrokeWidth(vg, t);
                        nvgStroke(vg);
                        return;
                    case 9:  // Top right diagonal
                        nvgMoveTo(vg, x + w - t, y + t);
                        nvgLineTo(vg, x + hw + t/2, y + hh - t/2);
                        nvgStrokeColor(vg, Colors::ledOn());
                        nvgStrokeWidth(vg, t);
                        nvgStroke(vg);
                        return;
                    case 10: // Bottom left diagonal
                        nvgMoveTo(vg, x + t, y + h - t);
                        nvgLineTo(vg, x + hw - t/2, y + hh + t/2);
                        nvgStrokeColor(vg, Colors::ledOn());
                        nvgStrokeWidth(vg, t);
                        nvgStroke(vg);
                        return;
                    case 11: // Bottom right diagonal
                        nvgMoveTo(vg, x + w - t, y + h - t);
                        nvgLineTo(vg, x + hw + t/2, y + hh + t/2);
                        nvgStrokeColor(vg, Colors::ledOn());
                        nvgStrokeWidth(vg, t);
                        nvgStroke(vg);
                        return;
                }
                nvgFill(vg);
            }
        };

        // Draw all 14 segments
        for (int i = 0; i < 14; i++) {
            drawSegment(i);
        }
    }

    uint16_t getSegments(char c) {
        // 14-segment encoding
        // Bits: 0=top, 1=TR, 2=BR, 3=bot, 4=BL, 5=TL, 6=ML, 7=MR, 8=TLD, 9=TRD, 10=BLD, 11=BRD, 12=TM, 13=BM
        switch (toupper(c)) {
            case '0': return 0b00000000111111;  // No diagonals, outer ring
            case '1': return 0b00000000000110;
            case '2': return 0b00000011011011;
            case '3': return 0b00000011001111;
            case '4': return 0b00000011100110;
            case '5': return 0b00000011101101;
            case '6': return 0b00000011111101;
            case '7': return 0b00000000000111;
            case '8': return 0b00000011111111;
            case '9': return 0b00000011101111;

            case 'A': return 0b00000011110111;
            case 'B': return 0b10010011001111;
            case 'C': return 0b00000000111001;
            case 'D': return 0b10010000001111;
            case 'E': return 0b00000011111001;
            case 'F': return 0b00000011110001;
            case 'G': return 0b00000010111101;
            case 'H': return 0b00000011110110;
            case 'I': return 0b10010000001001;
            case 'J': return 0b00000000011110;
            case 'K': return 0b00110011110000;
            case 'L': return 0b00000000111000;
            case 'M': return 0b00000100110110;  // Using diagonals differently
            case 'N': return 0b00100100110110;
            case 'O': return 0b00000000111111;
            case 'P': return 0b00000011110011;
            case 'Q': return 0b00100000111111;
            case 'R': return 0b00100011110011;
            case 'S': return 0b00000011101101;
            case 'T': return 0b10010000000001;
            case 'U': return 0b00000000111110;
            case 'V': return 0b01001000110000;
            case 'W': return 0b01100000110110;
            case 'X': return 0b01101100000000;
            case 'Y': return 0b10001100000000;
            case 'Z': return 0b01001000001001;

            case '-': return 0b00000011000000;
            case '#': return 0b10010011000110;
            case ' ':
            default:  return 0b00000000000000;
        }
    }

    void draw(const DrawArgs& args) override {
        NVGcontext* vg = args.vg;

        // Background (dark display inset into light panel)
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, box.size.x, box.size.y, 3);
        nvgFillColor(vg, nvgRGB(0x0a, 0x0a, 0x0a));
        nvgFill(vg);
        nvgStrokeColor(vg, Colors::displayBorder());
        nvgStrokeWidth(vg, 1);
        nvgStroke(vg);

        // Calculate starting position
        float totalWidth = text.length() * (charWidth + charSpacing) - charSpacing;
        float startX = centerText ? (box.size.x - totalWidth) / 2 : 4;
        float startY = (box.size.y - charHeight) / 2;

        // Draw each character
        for (size_t i = 0; i < text.length(); i++) {
            float x = startX + i * (charWidth + charSpacing);
            drawCharacter(vg, x, startY, text[i]);
        }

        OpaqueWidget::draw(args);
    }
};

//-----------------------------------------------------------------------------
// ScaleDisplay - Shows current root note and scale name
//-----------------------------------------------------------------------------

struct ScaleDisplay : widget::OpaqueWidget {
    Module* module = nullptr;
    Scale* scalePtr = nullptr;
    int* rootNotePtr = nullptr;

    static constexpr const char* NOTE_NAMES[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    static const char* getScaleAbbrev(Scale scale) {
        switch (scale) {
            case Scale::MAJOR: return "MAJ";
            case Scale::MINOR: return "MIN";
            case Scale::DORIAN: return "DOR";
            case Scale::MIXOLYDIAN: return "MIX";
            case Scale::LYDIAN: return "LYD";
            case Scale::PHRYGIAN: return "PHR";
            case Scale::LOCRIAN: return "LOC";
            case Scale::HARMONIC_MINOR: return "H-m";
            case Scale::HARMONIC_MAJOR: return "H-M";
            case Scale::DORIAN_NR_4: return "D#4";
            case Scale::PHRYGIAN_DOMINANT: return "PhD";
            case Scale::MELODIC_MINOR: return "Mm";
            case Scale::LYDIAN_AUGMENTED: return "L+";
            case Scale::LYDIAN_DOMINANT: return "LD";
            case Scale::HUNGARIAN_MINOR: return "HUN";
            case Scale::SUPER_LOCRIAN: return "SuL";
            case Scale::SPANISH: return "SPA";
            case Scale::BHAIRAV: return "BHV";
            case Scale::PENTATONIC_MINOR: return "Pm";
            case Scale::PENTATONIC_MAJOR: return "PM";
            case Scale::BLUES_MINOR: return "BLU";
            case Scale::WHOLE_TONE: return "WHL";
            case Scale::CHROMATIC: return "CHR";
            case Scale::JAPANESE_IN_SEN: return "INS";
            default: return "---";
        }
    }

    void draw(const DrawArgs& args) override {
        NVGcontext* vg = args.vg;

        Scale scale = scalePtr ? *scalePtr : Scale::MINOR;
        int rootNote = rootNotePtr ? *rootNotePtr : 0;

        const char* noteName = NOTE_NAMES[rootNote % 12];
        const char* scaleName = getScaleName(scale);

        // Build single-line string: "C# Minor"
        char displayText[32];
        snprintf(displayText, sizeof(displayText), "%s %s", noteName, scaleName);

        // Background bar
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, box.size.x, box.size.y, 2);
        nvgFillColor(vg, Colors::displayBg());
        nvgFill(vg);
        nvgStrokeColor(vg, Colors::displayBorder());
        nvgStrokeWidth(vg, 0.5f);
        nvgStroke(vg);

        // Text - single line, centered
        std::shared_ptr<Font> font = APP->window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
        if (font) {
            nvgFontFaceId(vg, font->handle);
            nvgFontSize(vg, 12);
            nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgFillColor(vg, Colors::displayAccent());
            nvgText(vg, box.size.x / 2, box.size.y / 2, displayText, nullptr);
        }

        OpaqueWidget::draw(args);
    }
};

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
    Pattern* patternPtr = nullptr;           // For display (read-only)
    MasterPattern* masterPatternPtr = nullptr;  // For editing (write)
    bool* forceDisplayRefreshPtr = nullptr;  // Set after edits to trigger refresh
    int* currentStepPtr = nullptr;
    int* patternLengthPtr = nullptr;
    Scale* scalePtr = nullptr;

    // Scrolling state
    int viewOffset = 0;  // First visible step
    bool autoFollow = true;  // Auto-scroll to follow playhead

    // Page navigation constants
    static constexpr int PAGE_SIZE = 16;
    static constexpr int NUM_PAGES = 4;  // 64 steps / 16 per page

    // Drag state
    bool isDragging = false;
    int lastPaintedCol = -1;
    int dragNoteValue = -1;  // Note being painted (-1 for rest)
    int dragOctave = 0;

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

    // Trigger display refresh after edits
    void triggerDisplayRefresh() {
        if (forceDisplayRefreshPtr) {
            *forceDisplayRefreshPtr = true;
        }
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

    // Page navigation
    int getCurrentPage() const {
        return viewOffset / PAGE_SIZE;
    }

    void goToPage(int page) {
        autoFollow = false;  // Disable auto-follow when manually navigating
        viewOffset = page * PAGE_SIZE;
        clampViewOffset();
    }

    void nextPage() {
        int page = getCurrentPage();
        if (page < NUM_PAGES - 1) {
            goToPage(page + 1);
        }
    }

    void prevPage() {
        int page = getCurrentPage();
        if (page > 0) {
            goToPage(page - 1);
        }
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

        // --- Draw Background Grid ---
        for (int row = 0; row < NUM_ROWS; row++) {
            int noteInOctave = row;  // 0 = C, 1 = C#, etc. (bottom to top)
            bool inScale = isSemitoneInScale(noteInOctave, scale);

            for (int visCol = 0; visCol < VISIBLE_STEPS; visCol++) {
                int col = visCol + viewOffset;
                float x = visCol * cellWidth;
                float y = b.size.y - (row + 1) * cellHeight;  // Flip Y (low notes at bottom)

                // Cell background - in-scale rows are clearly clickable, out-of-scale are near-black
                NVGcolor bgColor;
                if (inScale) {
                    bgColor = isWhiteKey(noteInOctave)
                        ? nvgRGBA(0x3a, 0x3a, 0x3a, 0x80)
                        : nvgRGBA(0x30, 0x30, 0x30, 0x80);
                } else {
                    // Dead zone - nearly black
                    bgColor = nvgRGBA(0x10, 0x10, 0x10, 0x60);
                }

                // Dim steps beyond pattern length
                if (col >= patternLength) {
                    bgColor = Colors::withAlpha(bgColor, 0.3f);
                }

                nvgBeginPath(vg);
                nvgRect(vg, x, y, cellWidth - 0.5f, cellHeight - 0.5f);
                nvgFillColor(vg, bgColor);
                nvgFill(vg);

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

        // --- Draw Note Name Labels on left edge ---
        {
            static const char* NOTE_NAMES[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
            std::shared_ptr<Font> font = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
            if (font) {
                nvgFontFaceId(vg, font->handle);
                nvgFontSize(vg, 7);
                nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
                nvgFillColor(vg, Colors::withAlpha(Colors::displayAccent(), 0.5f));
                for (int row = 0; row < NUM_ROWS; row++) {
                    if (!isSemitoneInScale(row, scale)) continue;
                    float y = b.size.y - (row + 0.5f) * cellHeight;
                    nvgText(vg, 1.5f, y, NOTE_NAMES[row], nullptr);
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

                // Note color based on accent (yellow accent per design language)
                NVGcolor noteColor = step.accent
                    ? Colors::withAlpha(Colors::noteActive(), 0.9f)
                    : Colors::withAlpha(Colors::noteNormal(), 0.7f);

                // Draw note rectangle
                nvgBeginPath(vg);
                nvgRoundedRect(vg, x + 0.5f, y + 0.5f, noteWidth, noteHeight, 2.0f);
                nvgFillColor(vg, noteColor);
                nvgFill(vg);

                // Accent indicator (orange border)
                if (step.accent) {
                    nvgBeginPath(vg);
                    nvgRoundedRect(vg, x + 0.5f, y + 0.5f, noteWidth, noteHeight, 2.0f);
                    nvgStrokeColor(vg, Colors::accentOn());
                    nvgStrokeWidth(vg, 1.5f);
                    nvgStroke(vg);
                }

                // Slide indicator (blue tail)
                if (step.slide) {
                    nvgBeginPath(vg);
                    nvgMoveTo(vg, x + cellWidth - 2, y + noteHeight * 0.3f);
                    nvgLineTo(vg, x + cellWidth + cellWidth * 0.2f, y + noteHeight * 0.5f);
                    nvgLineTo(vg, x + cellWidth - 2, y + noteHeight * 0.7f);
                    nvgFillColor(vg, Colors::slideOn());
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

            // Playhead column highlight (white per DESIGN.md)
            nvgBeginPath(vg);
            nvgRect(vg, x, 0, cellWidth, b.size.y);
            nvgFillColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0x33));
            nvgFill(vg);

            // Playhead line
            nvgBeginPath(vg);
            nvgMoveTo(vg, x, 0);
            nvgLineTo(vg, x, b.size.y);
            nvgStrokeColor(vg, nvgRGB(0xff, 0xff, 0xff));
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
                nvgFillColor(vg, i == currentPage ? Colors::displayAccent() : Colors::withAlpha(Colors::gray60(), 0.5f));
                nvgFill(vg);
            }
        }

        // --- Draw Border ---
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, b.size.x, b.size.y);
        nvgStrokeColor(vg, Colors::displayBorder());
        nvgStrokeWidth(vg, 1.0f);
        nvgStroke(vg);

        OpaqueWidget::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override {
        // Background (dark display area contrasts against light panel)
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3);
        nvgFillColor(args.vg, Colors::displayBg());
        nvgFill(args.vg);

        // Subtle border
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 3);
        nvgStrokeColor(args.vg, Colors::displayBorder());
        nvgStrokeWidth(args.vg, 1.0f);
        nvgStroke(args.vg);

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

        if (!module || !patternPtr || !masterPatternPtr) {
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
            const SequenceStep& step = patternPtr->steps[col];
            bool hasNote = !step.isRest() && !masterPatternPtr->muted[col];
            MasterStep& masterStep = masterPatternPtr->steps[col];

            // Modifier clicks: apply to existing note regardless of row
            if (hasNote && (e.mods & (GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT))) {
                if (e.mods & GLFW_MOD_SHIFT) {
                    masterStep.accentProb = (masterStep.accentProb < 0.5f) ? 1.0f : 0.0f;
                } else if (e.mods & GLFW_MOD_CONTROL) {
                    masterStep.slideProb = (masterStep.slideProb < 0.5f) ? 1.0f : 0.0f;
                } else if (e.mods & GLFW_MOD_ALT) {
                    masterStep.octave = (masterStep.octave + 2) % 3 - 1;
                }
            } else {
                // Plain click: place or remove note
                int currentSemitone = step.isRest() ? -1 : getNoteInScale(step.note, scale, 0, 0);
                int currentDisplay = currentSemitone >= 0 ? ((currentSemitone % 12) + 12) % 12 : -1;

                if (hasNote && currentDisplay == row) {
                    // Click on existing note's row — remove it
                    masterPatternPtr->muted[col] = true;
                    dragNoteValue = -1;
                } else {
                    // Try to place a note on this row
                    int newScaleDegree = semitoneToScaleDegree(row, scale);
                    if (newScaleDegree >= 0) {
                        int poolIndex = masterPatternPtr->findNotePoolIndex(newScaleDegree);
                        masterStep.notePoolIndex = poolIndex;
                        masterStep.octave = 0;
                        masterStep.accentProb = 0.5f;
                        masterStep.slideProb = 0.5f;
                        masterPatternPtr->muted[col] = false;
                        dragNoteValue = newScaleDegree;
                        dragOctave = 0;
                    }
                    // Out-of-scale row: silently ignore
                }
            }

            isDragging = true;
            lastPaintedCol = col;

            triggerDisplayRefresh();
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
        if (!isDragging || !module || !patternPtr || !masterPatternPtr) {
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
        if (col >= patternLength || col == lastPaintedCol) {
            OpaqueWidget::onDragHover(e);
            return;
        }

        MasterStep& masterStep = masterPatternPtr->steps[col];

        // Paint the same note value as drag start
        if (dragNoteValue < 0) {
            // Painting rests (mute the step)
            masterPatternPtr->muted[col] = true;
        } else {
            // Painting notes
            int poolIndex = masterPatternPtr->findNotePoolIndex(dragNoteValue);
            masterStep.notePoolIndex = poolIndex;
            masterStep.octave = dragOctave;
            masterStep.accentProb = 0.5f;
            masterStep.slideProb = 0.5f;
            masterPatternPtr->muted[col] = false;
        }

        lastPaintedCol = col;

        triggerDisplayRefresh();

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
    Pattern* patternPtr = nullptr;              // For display (read-only)
    MasterPattern* masterPatternPtr = nullptr;  // For editing (write)
    bool* forceDisplayRefreshPtr = nullptr;     // Set after edits to trigger refresh
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

    void triggerDisplayRefresh() {
        if (forceDisplayRefreshPtr) {
            *forceDisplayRefreshPtr = true;
        }
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
            case RowType::ACCENT: return Colors::accentOn();
            case RowType::SLIDE:  return Colors::slideOn();
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
                        activeColor = Colors::accentOn();
                        break;
                    case RowType::SLIDE:
                        isActive = !step.isRest() && step.slide;
                        activeColor = Colors::slideOn();
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

            // Playhead highlight (white per DESIGN.md)
            if (col == currentStep) {
                nvgBeginPath(vg);
                nvgRect(vg, x, 0, cellWidth, box.size.y);
                nvgFillColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0x4d));
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
        // Dark display background
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 1);
        nvgFillColor(args.vg, Colors::displayBg());
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

        if (!module || !patternPtr || !masterPatternPtr) {
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

        // Check if step is muted or a rest
        const SequenceStep& step = patternPtr->steps[col];
        if (step.isRest() || masterPatternPtr->muted[col]) {
            e.consume(this);
            return;
        }

        // Write to master pattern
        MasterStep& masterStep = masterPatternPtr->steps[col];

        switch (rowType) {
            case RowType::ACCENT:
                // Toggle between always-on (0.0) and always-off (1.0)
                masterStep.accentProb = (masterStep.accentProb < 0.5f) ? 1.0f : 0.0f;
                break;
            case RowType::SLIDE:
                // Toggle between always-on (0.0) and always-off (1.0)
                masterStep.slideProb = (masterStep.slideProb < 0.5f) ? 1.0f : 0.0f;
                break;
            case RowType::OCTAVE:
                // Cycle: 0 -> 1 -> -1 -> 0
                if (masterStep.octave == 0) masterStep.octave = 1;
                else if (masterStep.octave == 1) masterStep.octave = -1;
                else masterStep.octave = 0;
                break;
        }

        // Trigger display refresh to show the edit
        triggerDisplayRefresh();

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
                : (col == currentStep ? Colors::displayAccent() : Colors::gray60());

            char num[4];
            snprintf(num, sizeof(num), "%d", col + 1);

            nvgFillColor(vg, textColor);
            nvgText(vg, x, y, num, nullptr);
        }

        OpaqueWidget::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override {
        // Dark display background
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 1);
        nvgFillColor(args.vg, Colors::displayBg());
        nvgFill(args.vg);

        OpaqueWidget::draw(args);
    }
};

//-----------------------------------------------------------------------------
// PageButton - Simple arrow button for page navigation
//-----------------------------------------------------------------------------

struct PageButton : widget::OpaqueWidget {
    enum Direction { LEFT, RIGHT };
    Direction direction = RIGHT;
    std::function<void()> onClick;
    bool pressed = false;

    PageButton() {
        box.size = Vec(12, 12);
    }

    void draw(const DrawArgs& args) override {
        NVGcontext* vg = args.vg;

        // Button background (light theme - subtle gray buttons)
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, box.size.x, box.size.y, 2);
        nvgFillColor(vg, pressed ? Colors::sectionBg() : Colors::panelBg());
        nvgFill(vg);
        nvgStrokeColor(vg, Colors::gray30());
        nvgStrokeWidth(vg, 1);
        nvgStroke(vg);

        // Arrow
        float cx = box.size.x / 2;
        float cy = box.size.y / 2;
        float arrowSize = 3.f;

        nvgBeginPath(vg);
        if (direction == LEFT) {
            nvgMoveTo(vg, cx + arrowSize, cy - arrowSize);
            nvgLineTo(vg, cx - arrowSize, cy);
            nvgLineTo(vg, cx + arrowSize, cy + arrowSize);
        } else {
            nvgMoveTo(vg, cx - arrowSize, cy - arrowSize);
            nvgLineTo(vg, cx + arrowSize, cy);
            nvgLineTo(vg, cx - arrowSize, cy + arrowSize);
        }
        nvgStrokeColor(vg, Colors::gray100());
        nvgStrokeWidth(vg, 1.5f);
        nvgStroke(vg);

        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
            if (e.action == GLFW_PRESS) {
                pressed = true;
                if (onClick) onClick();
            } else if (e.action == GLFW_RELEASE) {
                pressed = false;
            }
            e.consume(this);
        }
    }

    void onLeave(const LeaveEvent& e) override {
        pressed = false;
        OpaqueWidget::onLeave(e);
    }
};

//-----------------------------------------------------------------------------
// PageIndicator - Shows current page (1-4) with clickable page numbers
//-----------------------------------------------------------------------------

struct PageIndicator : widget::OpaqueWidget {
    PianoRollDisplay* pianoRoll = nullptr;
    int* patternLengthPtr = nullptr;

    PageIndicator() {
        box.size = Vec(50, 12);
    }

    void draw(const DrawArgs& args) override {
        NVGcontext* vg = args.vg;

        int currentPage = pianoRoll ? pianoRoll->getCurrentPage() : 0;
        int patternLength = patternLengthPtr ? *patternLengthPtr : 16;
        int maxPage = (patternLength - 1) / 16;  // Which pages have content

        float pageWidth = box.size.x / 4;

        nvgFontSize(vg, 9);
        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

        for (int i = 0; i < 4; i++) {
            float x = i * pageWidth + pageWidth / 2;
            float y = box.size.y / 2;

            // Background for current page (cyan display accent)
            if (i == currentPage) {
                nvgBeginPath(vg);
                nvgRoundedRect(vg, i * pageWidth + 1, 1, pageWidth - 2, box.size.y - 2, 2);
                nvgFillColor(vg, Colors::displayAccent());
                nvgFill(vg);
            }

            // Page number
            NVGcolor textColor;
            if (i == currentPage) {
                textColor = Colors::gray100();
            } else if (i <= maxPage) {
                textColor = Colors::gray70();
            } else {
                textColor = Colors::withAlpha(Colors::gray60(), 0.4f);
            }

            char num[2];
            snprintf(num, sizeof(num), "%d", i + 1);
            nvgFillColor(vg, textColor);
            nvgText(vg, x, y, num, nullptr);
        }

        OpaqueWidget::draw(args);
    }

    void onButton(const ButtonEvent& e) override {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            if (pianoRoll) {
                float pageWidth = box.size.x / 4;
                int clickedPage = static_cast<int>(e.pos.x / pageWidth);
                clickedPage = std::clamp(clickedPage, 0, 3);
                pianoRoll->goToPage(clickedPage);
            }
            e.consume(this);
        }
    }
};
