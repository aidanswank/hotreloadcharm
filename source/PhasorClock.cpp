#include "PhasorClock.h"

PhasorClock::PhasorClock(double sr)
    : sample_rate(sr)
{
    update_increment();
}

void PhasorClock::set_bpm(double new_bpm, double sr)
{
    bpm         = new_bpm;
    sample_rate = sr;
    update_increment();
}

void PhasorClock::update_increment()
{
    if (bpm <= 0.0 || sample_rate <= 0.0) return;

    // One bar = 4 beats
    double seconds_per_beat = 60.0 / bpm;
    double seconds_per_bar  = seconds_per_beat * 4.0;
    double samples_per_bar  = seconds_per_bar * sample_rate;
    phase_increment         = 1.0 / samples_per_bar;
}

void PhasorClock::resync_from_ppq(double ppq)
{
    // ppq is in beats, so divide by 4 to get bar phase
    phase = std::fmod(ppq / osc_time_bars, 1.0);
}

void PhasorClock::process()
{
    phase += phase_increment;
    if (phase >= 1.0)
        phase -= 1.0;

    outputs[PHASOR_CLOCK_OUT] = static_cast<float>(phase);
}