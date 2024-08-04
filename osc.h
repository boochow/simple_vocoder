#pragma once
/*
 *  File: osc.h
 *  demo of vocoder.h
 *
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <climits>

#include "unit_osc.h"   // Note: Include base definitions for osc units
#include "utils/int_math.h"   // for clipminmaxi32()
#include "vocoder.h"

class Osc {
public:

    Osc(void) {}
    ~Osc(void) {}

    inline int8_t Init(const unit_runtime_desc_t * desc) {
        if (!desc)
            return k_unit_err_undef;
    
        if (desc->target != unit_header.target)
            return k_unit_err_target;
    
        if (!UNIT_API_IS_COMPAT(desc->api))
            return k_unit_err_api_version;

        if (desc->samplerate != 48000)
            return k_unit_err_samplerate;

        if (desc->input_channels != 2 || desc->output_channels != 1)
            return k_unit_err_geometry;

        runtime_desc_ = *desc;
    
        vocoder_.Init(desc->samplerate);

        return k_unit_err_none;
    }

    inline void Teardown() {
    }

    inline void Reset() {
        phi_ = 0;
        note_ = 48;
    }

    inline void Resume() {
    }

    inline void Suspend() {
    }

    fast_inline void Process(const float * in, float * out, size_t frames) {
        const float * __restrict in_p = in;
        float * __restrict out_p = out;
        const float * out_e = out_p + frames;

        const float w0 = osc_w0f_for_note(note_, 0);
        for (; out_p != out_e; in_p += 2, out_p += 1) {
            float sig = phi_ * 2 - 1.f;
            float mod = *in_p + *(in_p + 1);
            *out_p = vocoder_.Process(sig, mod * input_gain_);
            phi_ += w0;
            phi_ -= (uint32_t) phi_;
        }
    }

    inline void setParameter(uint8_t index, int32_t value) {
        switch(index) {
        case k_unit_osc_fixed_param_shape:
            vocoder_.SetDecayFactor(param_10bit_to_f32(value));
            break;
        case k_unit_osc_fixed_param_altshape:
            input_gain_ = 1.f * value / 170.5; // 0..6.0
            input_gain_ = 0.5f + input_gain_ * input_gain_; // 0.5..36.5
            break;
        default:
            break;
        }
    }

    inline int32_t getParameterValue(uint8_t index) const {
        (void) index;
        return 0;
    }

    inline const char * getParameterStrValue(uint8_t index, int32_t value) const {
        (void) index;
        (void) value;
        
        return nullptr;
    }

    inline void setTempo(uint32_t tempo) {
        (uint32_t) tempo;
    }

    inline void tempo4ppqnTick(uint32_t counter) {
        (uint32_t) counter;
    }

    inline void NoteOn(uint8_t note, uint8_t velo) {
        phi_ = 0.f;
        note_ = note;
    }

    inline void NoteOff(uint8_t note) {
        (uint8_t) note;
    }

    inline void AllNoteOff() {
    }

    inline void PitchBend(uint16_t bend) {
        (uint16_t) bend;
    }

    inline void ChannelPressure(uint8_t press) {
        (uint8_t) press;
    }

    inline void AfterTouch(uint8_t note, uint8_t press) {
        (uint8_t) note;
        (uint8_t) press;
    }

private:

    unit_runtime_desc_t runtime_desc_;

    float phi_ = 0.f;
    uint8_t note_ = 48;
    float input_gain_ = 1.f;
    ChannelVocoder vocoder_;
};
