#pragma once
/*
 *  File: effect.h
 *  demo of vocoder.h
 *
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <climits>

#include "unit_genericfx.h"   // Note: Include base definitions for genericfx units
#include "utils/int_math.h"   // for clipminmaxi32()
#include "vocoder.h"

class Effect {
public:

    enum {
        PITCH = 0U,
        DEPTH,
        DECAY,
        GAIN,
        CHROMA,
        NUM_PARAMS
    };

    Effect(void) {}
    ~Effect(void) {}

    inline int8_t Init(const unit_runtime_desc_t * desc) {
        if (!desc)
            return k_unit_err_undef;

        if (desc->target != unit_header.common.target)
            return k_unit_err_target;

        if (!UNIT_API_IS_COMPAT(desc->api))
            return k_unit_err_api_version;

        if (desc->samplerate != 48000)
            return k_unit_err_samplerate;

        if (desc->input_channels != 2 || desc->output_channels != 2)
            return k_unit_err_geometry;

        runtime_desc_ = *desc;

        vocoder_.Init(desc->samplerate);

        return k_unit_err_none;
    }

    inline void Teardown() {
    }

    inline void Reset() {
        phi_ = 0;
    }

    inline void Resume() {
    }

    inline void Suspend() {
    }

    fast_inline void Process(const float * in, float * out, size_t frames) {
        const float * __restrict in_p = in;
        float * __restrict out_p = out;
        const float * out_e = out_p + (frames << 1);
        const int16_t pitch = p_[PITCH] >> 3;
        const int16_t frac = ((p_[PITCH] & 0x07) << 5) * (1 - p_[CHROMA]);
        const float w0 = osc_w0f_for_note(pitch, frac);
        const float dry = 0.0005f * (1000 - p_[DEPTH]);
        const float wet = 0.0005f * (1000 + p_[DEPTH]);
        for (; out_p != out_e; in_p += 2, out_p += 2) {
            float sig = phi_ * 0.2 - 0.1f;
            float mod = *in_p + *(in_p + 1);
            sig *= noteon_;
            sig = vocoder_.Process(sig, mod * input_gain_);
            out_p[0] = dry * in_p[0] + wet * sig;
            out_p[1] = dry * in_p[1] + wet * sig;
            phi_ += w0;
            phi_ -= (uint32_t) phi_;
        }
    }

    inline void setParameter(uint8_t index, int32_t value) {
        switch(index) {
        case DECAY:
            vocoder_.SetDecayFactor(param_10bit_to_f32(value));
            break;
        case GAIN:
            input_gain_ = 1.f * value / 170.5; // 0..6.0
            input_gain_ = 0.5f + input_gain_ * input_gain_; // 0.5..36.5
            break;
        default:
            break;
        }
        p_[index] = value;
    }

    inline int32_t getParameterValue(uint8_t index) const {
        return p_[index];
    }

    inline const char * getParameterStrValue(uint8_t index, int32_t value) const {
        if (index == CHROMA) {
            return value ? "ON" :"OFF";
        } else {
            return nullptr;
        }
    }

    inline void setTempo(uint32_t tempo) {
        (uint32_t) tempo;
    }

    inline void tempo4ppqnTick(uint32_t counter) {
        (uint32_t) counter;
    }

    inline void touchEvent(uint8_t id, uint8_t phase, uint32_t x, uint32_t y) {
        switch (phase) {
        case k_unit_touch_phase_began:
            noteon_ = 1;
            phi_ = 0.f;
            break;
        case k_unit_touch_phase_moved:
            break;
        case k_unit_touch_phase_ended:
            noteon_ = 0;
            break;
        case k_unit_touch_phase_stationary:
            break;
        case k_unit_touch_phase_cancelled:
            noteon_ = 0;
            break;
        default:
            break;
        }
    }

private:

    unit_runtime_desc_t runtime_desc_;

    int32_t p_[NUM_PARAMS];
    float phi_;
    float input_gain_ = 1.f;
    ChannelVocoder vocoder_;
    int32_t noteon_;
};
