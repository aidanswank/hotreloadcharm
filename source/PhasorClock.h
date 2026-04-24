#pragma once

#include <cmath>

class PhasorClock
{
public:
    enum Outputs
    {
        PHASOR_CLOCK_OUT,
        NUM_OUTPUTS
    };

    // Host sync
    double bpm         = 120.0;
    double sample_rate = 44100.0;

    // Internal oscillator
    double phase           = 0.0; // 0 → 1
    double phase_increment = 0.0;

    float outputs[NUM_OUTPUTS] = {};

    explicit PhasorClock(double sr = 44100.0);

    // Update increment when BPM or sample_rate changes
    void set_bpm(double new_bpm, double sr);
    void update_increment();

    // Called once per block (from process_block) to align to bar grid
    void resync_from_ppq(double ppq);

    // Called per sample
    void process();
};