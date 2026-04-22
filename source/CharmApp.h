
#pragma once

#include "ui.h"

#include <nanovg.h>

#include "ui_impl.h"

// #include "AudioGraph.h"

#define MAX_BUFFER 44800 // 1 second at 44.1kHz

// Function pointer types for clipboard operations
typedef void (*ClipboardSetFunc)(void* ctx, const char* text);
typedef const char* (*ClipboardGetFunc)(void* ctx);

#include <cmath>
#include <vector>
#include <complex>
#include <string>
#include <unordered_map>
#include <fftw3.h>
#include <math.h>

namespace juce {
class AudioProcessorValueTreeState;
}

class CharmApp {
public:
    // static constexpr const char* mainGainParamID     = "mainGain";
    // static constexpr const char* phaseInvertParamID  = "phaseInvert";
    // static constexpr const char* filterCutoffParamID = "filterCutoff";
    // static constexpr const char* filterResParamID    = "filterRes";
    // static constexpr const char* filterFoldParamID   = "filterFold";
    // static constexpr const char* filterDriveParamID  = "filterDrive";
    static constexpr const char* knob_1_param_id     = "knob1";
    static constexpr const char* knob_2_param_id     = "knob2";
    static constexpr const char* knob_3_param_id     = "knob3";
    static constexpr const char* knob_4_param_id     = "knob4";

    CharmApp();
    ~CharmApp();

    void init_ui(UIRenderer ui_renderer);
    
    void on_render(charm::Rect window_rect, float scale);

    void draw_visualizers(charm::Rect &track_area);

    void prepare_to_play(double sampleRate, int blockSize);
    void process_audio(float** outputs, int num_channels, int num_samples);

    // Moog VCF – one state struct per channel
    struct MoogFilterState {
        float y1 = 0, y2 = 0, y3 = 0, y4 = 0;
        float oldx = 0, oldy1 = 0, oldy2 = 0, oldy3 = 0;
    };
    float moog_filter(float input, float cutoff, float res, float fold, float drive, MoogFilterState& s) const;
    MoogFilterState filter_left, filter_right;
    void attach_host_parameters(juce::AudioProcessorValueTreeState& new_host_parameters);
    std::string get_host_parameter_name(const char* parameter_id) const;
    float get_host_parameter_value(const char* parameter_id, float fallback_value = 0.0f) const;
    float get_smoothed_host_parameter_value(const char* parameter_id, float fallback_value = 0.0f);
    void reset_parameter_smoothers();
    bool set_host_parameter_value(const char* parameter_id, float value) const;
    float ui_host_parameter_slider(charm::Rect rect, const char* parameter_id, float fallback_value = 0.0f);
    float ui_host_parameter_row(charm::Rect& rect, const char* parameter_id,
                                float fallback_value = 0.0f, float row_height = 32.0f,
                                float label_width = 128.0f);


    // float hann_window[2048];
    // void create_hann_window()
    // {
    //     for (int i = 0; i < 2048; i++) {
    //         double multiplier = 0.5 * (1 - cos(2*M_PI*i/2047));
            
    //     }
    // }
    void manipulate_spectrum(float* signal, int N);
    
    void set_working_file(std::string working_filepath)
    {
        // Copy safely
        std::strncpy(filename_text_state.buffer, working_filepath.c_str(), sizeof(filename_text_state.buffer) - 1);

        // Ensure null-termination
        filename_text_state.buffer[sizeof(filename_text_state.buffer) - 1] = '\0';
        
//        if (strlen(this->filename_text_buffer) > 0) {
//            printf("Open File: %s\n", this->filename_text_buffer);
//            // push_undo(graph);
//            graph.load(this->filename_text_buffer);
//        } else {
//            printf("Filename is empty. Cannot open.\n");
//        }
//        if(AudioGraphSerialization::read_header(std::istream &in))
//        graph.load(this->filename_text_buffer);
    }
    
    // Register platform clipboard functions
    void set_clipboard_callbacks(ClipboardSetFunc setFunc,
                               ClipboardGetFunc getFunc,
                               void* context)
    {
      clipboardSetFunc = setFunc;
      clipboardGetFunc = getFunc;
      clipboardCtx = context;
    }


    // Platform-agnostic "copy"
    void set_clipboard_string(const std::string& str)
    {
        if (clipboardSetFunc)
            clipboardSetFunc(clipboardCtx, str.c_str());
    }
    
    // Platform-agnostic "paste"
    std::string get_clipboard_string()
    {
        if (clipboardGetFunc)
            return clipboardGetFunc(clipboardCtx);
        return {};
    }
    
    // For direct const char* (GLFW style)
    const char* get_clipboard_cstr()
    {
        if (clipboardGetFunc)
            return clipboardGetFunc(clipboardCtx);
        return "";
    }
    
    std::unique_ptr<UICore> ui_ctx;
    
    float settings_theme_panel_scroll_y = 0.0f; // used for settings panel scroll position
    float main_panel_scroll_y = 0.0f;
    int side_panel_size = 200; // used for settings panel width

    float viz_buffer_left[MAX_BUFFER] = {0};
    float viz_buffer_right[MAX_BUFFER] = {0};
    float viz_buffer_mono[MAX_BUFFER] = {0};

    const static int ringbuffer_size = 448000;
    float ringbuffer_track[ringbuffer_size] = {0}; // 10 seconds at 44.1kHz
    int ringbuffer_write_pos = 0;
    float segment_size = 1280*64;

    float main_gain    = 1.0f;
    float phase_invert = 1.0f;
    float param_smooth_coeff = 0.01f; // 0 = instant, closer to 1 = very slow
    
    bool is_settings_open = false;
    
    TextInputState filename_text_state;

    ClipboardSetFunc clipboardSetFunc = nullptr;
    ClipboardGetFunc clipboardGetFunc = nullptr;
    void* clipboardCtx = nullptr;
    
    bool draw_cable_amp = true;
    
    // patcher select rect
    charm::Vec2 select_rect_start = {-1, -1}; // Start with an invalid position
    charm::Rect select_rect = {-1,-1,-1,-1};
    
    // audio
    float sample_rate;
    int block_size;
    //global clock
    double current_ppq = 0;
    double beat_fraction_bpm = 120.0;
    double note_interval_hz = 2.0;
    double note_duration_ms = 500.0;
    double lastKnownBpm = -1.0;

    float signal[1024*16];
    float magnitude_threshold = 8.0f;

private:
    juce::AudioProcessorValueTreeState* host_parameters = nullptr;
    std::unordered_map<std::string, float> smoothed_parameter_values;

};
