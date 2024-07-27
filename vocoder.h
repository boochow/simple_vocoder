#ifndef BOOCHOW_VOCODER_H_
#define BOOCHOW_VOCODER_H_

#include "dsp/biquad.hpp"
#include "fx_api.h"

#define NUM_BANDS 8
#define FILTER_Q 6.f

using namespace dsp;

const float BANDS[NUM_BANDS] = {
    245, 367, 551, 827, 1240, 1860, 2790, 4185
};

class ChannelVocoder {
public:
    ChannelVocoder(void) {}
    ~ChannelVocoder(void) {}

    inline int8_t Init(uint32_t samplerate) {
        fs_recipf_ = 1.f / samplerate;
        for(int i = 0; i < NUM_BANDS; i++) {
            float wc = analysis_filters1_[i].mCoeffs.wc(BANDS[i], fs_recipf_);
            float k = fx_tanpif(wc);
            analysis_filters1_[i].mCoeffs.setSOBP(k, FILTER_Q);
            synthesis_filters1_[i].mCoeffs.setSOBP(k, FILTER_Q);
            wc = analysis_filters1_[i].mCoeffs.wc(0.9 * BANDS[i], fs_recipf_);
            k = fx_tanpif(wc);
            analysis_filters2_[i].mCoeffs.setSOBP(k, FILTER_Q);
            synthesis_filters2_[i].mCoeffs.setSOBP(k, FILTER_Q);
            gain_[i] = FILTER_Q * sqrt(BANDS[i] * 1.f / BANDS[0]);
        }
        return k_unit_err_none;
    }

    fast_inline float Process(const float carrier, const float modulator) {
        float result = 0;
        for(int i = 0; i < NUM_BANDS; i++) {
            float analyzed = analysis_filters1_[i].process_so(modulator);
            analyzed = analysis_filters2_[i].process_so(analyzed);
            float envelope = EnvelopeFollower(envelopes_[i], analyzed);
            envelopes_[i] = envelope;
            float synthesized = synthesis_filters1_[i].process_so(carrier);
            synthesized *= envelope;
            synthesized = synthesis_filters2_[i].process_so(synthesized);
            result += synthesized * gain_[i];
        }
        return result;
    }

    fast_inline void SetDecayFactor(const float r) {
        decay_factor_ = 0.995 + clip01f(r) * 0.00499f;
    }
private:
    BiQuad analysis_filters1_[NUM_BANDS];
    BiQuad analysis_filters2_[NUM_BANDS];
    BiQuad synthesis_filters1_[NUM_BANDS];
    BiQuad synthesis_filters2_[NUM_BANDS];
    float envelopes_[NUM_BANDS];
    float gain_[NUM_BANDS];
    float fs_recipf_;
    float decay_factor_ = 0.999;

    fast inline float EnvelopeFollower(float env, float sig) {
        float abs_sig = fabsf(sig);
        return (abs_sig > env) ? abs_sig : decay_factor_ * env;
    }
};

#endif // BOOCHOW_VOCODER_H_
