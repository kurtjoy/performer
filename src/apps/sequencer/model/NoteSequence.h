#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "Scale.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class NoteSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<3> GateProbability;
    typedef UnsignedValue<3> Retrigger;
    typedef UnsignedValue<3> RetriggerProbability;
    typedef UnsignedValue<3> Length;
    typedef SignedValue<4> LengthVariationRange;
    typedef UnsignedValue<3> LengthVariationProbability;
    typedef SignedValue<7> Note;
    typedef SignedValue<7> NoteVariationRange;
    typedef UnsignedValue<3> NoteVariationProbability;

    enum class Layer {
        Gate,
        GateProbability,
        Retrigger,
        RetriggerProbability,
        Length,
        LengthVariationRange,
        LengthVariationProbability,
        Note,
        NoteVariationRange,
        NoteVariationProbability,
        Slide,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Gate:                       return "Gate";
        case Layer::GateProbability:            return "Gate Prob.";
        case Layer::Retrigger:                  return "Retrigger";
        case Layer::RetriggerProbability:       return "Retrigger Prob.";
        case Layer::Length:                     return "Length";
        case Layer::LengthVariationRange:       return "Length Var. Range";
        case Layer::LengthVariationProbability: return "Length Var. Prob.";
        case Layer::Note:                       return "Note";
        case Layer::NoteVariationRange:         return "Note Var. Range";
        case Layer::NoteVariationProbability:   return "Note Var. Prob.";
        case Layer::Slide:                      return "Slide";
        case Layer::Last:                       break;
        }
        return nullptr;
    }

    static Types::LayerRange layerRange(Layer layer);

    class Step {
    public:
        // gate

        bool gate() const { return _data0.gate ? true : false; }
        void setGate(bool gate) { _data0.gate = gate; }
        void toggleGate() { setGate(!gate()); }

        // gateProbability

        int gateProbability() const { return _data0.gateProbability; }
        void setGateProbability(int gateProbability) { _data0.gateProbability = gateProbability; }

        // slide

        bool slide() const { return _data0.slide ? true : false; }
        void setSlide(bool slide) { _data0.slide = slide; }
        void toggleSlide() { setSlide(!slide()); }

        // retrigger

        int retrigger() const { return _data1.retrigger; }
        void setRetrigger(int retrigger) { _data1.retrigger = retrigger; }

        // retriggerProbability

        int retriggerProbability() const { return _data1.retriggerProbability; }
        void setRetriggerProbability(int retriggerProbability) { _data1.retriggerProbability = retriggerProbability; }

        // length

        int length() const { return _data0.length; }
        void setLength(int length) { _data0.length = length; }

        // lengthVariationRange

        int lengthVariationRange() const { return LengthVariationRange::Min + _data0.lengthVariationRange; }
        void setLengthVariationRange(int lengthVariationRange) { _data0.lengthVariationRange = lengthVariationRange - LengthVariationRange::Min; }

        // lengthVariationProbability

        int lengthVariationProbability() const { return _data0.lengthVariationProbability; }
        void setLengthVariationProbability(int lengthVariationProbability) { _data0.lengthVariationProbability = lengthVariationProbability; }

        // note

        int note() const { return Note::Min + _data0.note; }
        void setNote(int note) { _data0.note = note - Note::Min; }

        // noteVariationRange

        int noteVariationRange() const { return NoteVariationRange::Min + _data0.noteVariationRange; }
        void setNoteVariationRange(int noteVariationRange) { _data0.noteVariationRange = noteVariationRange - NoteVariationRange::Min; }

        // noteVariationProbability

        int noteVariationProbability() const { return _data0.noteVariationProbability; }
        void setNoteVariationProbability(int noteVariationProbability) { _data0.noteVariationProbability = noteVariationProbability; }

        int layerValue(Layer layer) const;
        void setLayerValue(Layer layer, int value);

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        union {
            uint32_t raw;
            BitField<0, 1> gate;
            BitField<1, 1> slide;
            BitField<2, GateProbability::Bits> gateProbability;
            BitField<5, Length::Bits> length;
            BitField<8, LengthVariationRange::Bits> lengthVariationRange;
            BitField<12, LengthVariationProbability::Bits> lengthVariationProbability;
            BitField<15, Note::Bits> note;
            BitField<22, NoteVariationRange::Bits> noteVariationRange;
            BitField<29, NoteVariationProbability::Bits> noteVariationProbability;
        } _data0;
        union {
            uint16_t raw;
            BitField<0, Retrigger::Bits> retrigger;
            BitField<3, RetriggerProbability::Bits> retriggerProbability;
            // 10 bit left
        } _data1;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // scale

    int scale() const { return _scale; }
    void setScale(int scale) {
        _scale = clamp(scale, -1, Scale::Count - 1);
    }

    void editScale(int value, bool shift) {
        setScale(scale() + value);
    }

    void printScale(StringBuilder &str) const {
        str(scale() < 0 ? "Default" : Scale::get(scale()).name());
    }

    const Scale &selectedScale() const {
        return Scale::get(scale() < 0 ? _defaultScale : scale());
    }

    // rootNote

    int rootNote() const { return _rootNote; }
    void setRootNote(int rootNote) {
        _rootNote = clamp(rootNote, -1, 11);
    }

    void editRootNote(int value, bool shift) {
        setRootNote(rootNote() + value);
    }

    void printRootNote(StringBuilder &str) const {
        if (rootNote() < 0) {
            str("Default");
        } else {
            Types::printNote(str, rootNote());
        }
    }

    int selectedRootNote() const {
        return rootNote() < 0 ? _defaultRootNote : rootNote();
    }

    // divisor

    int divisor() const { return _divisor; }
    void setDivisor(int divisor) {
        _divisor = clamp(divisor, 1, 192);
    }

    int indexedDivisor() const { return ModelUtils::divisorToIndex(divisor()); }
    void setIndexedDivisor(int index) {
        int divisor = ModelUtils::indexToDivisor(index);
        if (divisor > 0) {
            setDivisor(divisor);
        }
    }

    void editDivisor(int value, bool shift) {
        setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
    }

    void printDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, divisor());
        // str("%d", divisor());
    }

    // resetMeasure

    int resetMeasure() const { return _resetMeasure; }
    void setResetMeasure(int resetMeasure) {
        _resetMeasure = clamp(resetMeasure, 0, 128);
    }

    void editResetMeasure(int value, bool shift) {
        setResetMeasure(ModelUtils::adjustedByPowerOfTwo(resetMeasure(), value, shift));
    }

    void printResetMeasure(StringBuilder &str) const {
        if (resetMeasure() == 0) {
            str("off");
        } else {
            str("%d", resetMeasure());
        }
    }

    // runMode

    Types::RunMode runMode() const { return _runMode; }
    void setRunMode(Types::RunMode runMode) {
        _runMode = runMode;
    }

    void editRunMode(int value, bool shift) {
        setRunMode(ModelUtils::adjustedEnum(runMode(), value));
    }

    void printRunMode(StringBuilder &str) const {
        str(Types::runModeName(runMode()));
    }

    // firstStep

    int firstStep() const { return _firstStep; }
    void setFirstStep(int firstStep) {
        _firstStep = clamp(firstStep, 0, lastStep());
    }

    void editFirstStep(int value, bool shift) {
        setFirstStep(firstStep() + value);
    }

    void printFirstStep(StringBuilder &str) const {
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const { return _lastStep; }
    void setLastStep(int lastStep) {
        _lastStep = clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1);
    }

    void editLastStep(int value, bool shift) {
        setLastStep(lastStep() + value);
    }

    void printLastStep(StringBuilder &str) const {
        str("%d", lastStep() + 1);
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();
    void clearSteps();

    void setGates(std::initializer_list<int> gates);
    void setNotes(std::initializer_list<int> notes);

    void shiftSteps(int direction);

    void duplicateSteps();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    int8_t _scale;
    int8_t _rootNote;
    uint8_t _divisor;
    uint8_t _resetMeasure;
    Types::RunMode _runMode;
    uint8_t _firstStep;
    uint8_t _lastStep;
    StepArray _steps;

    static uint8_t _defaultScale;
    static uint8_t _defaultRootNote;

    friend class Project;
};
