
#include "CharmApp.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <dirent.h>
#include <vector>
#include <string>

namespace
{
juce::RangedAudioParameter* get_host_parameter(juce::AudioProcessorValueTreeState* host_parameters,
                                              const char* parameter_id)
{
    if (host_parameters == nullptr)
        return nullptr;

    return host_parameters->getParameter(parameter_id);
}
}

CharmApp::CharmApp()
// :
{
    // Plugin will be loaded dynamically from UI
    // if(this->module_loader.load("/Users/aidan/dev/cpp/juce_cmake/hotreloadcharm/build/plugins/filter_plugin.dylib"))
    // {
    //     this->module_loader.create_instance();
    //     printf("✓ Loaded plugin: %s v%s\n", this->module_loader.get_name(), this->module_loader.get_version());
    // }
    // else
    // {
    //     printf("Failed to load plugin: %s\n", this->module_loader.get_error().c_str());
    // }
}

CharmApp::~CharmApp()
{
    
}

void CharmApp::attach_host_parameters(juce::AudioProcessorValueTreeState& new_host_parameters)
{
    host_parameters = &new_host_parameters;
}

std::string CharmApp::get_host_parameter_name(const char* parameter_id) const
{
    auto* parameter = get_host_parameter(host_parameters, parameter_id);
    if (parameter == nullptr)
        return parameter_id != nullptr ? parameter_id : std::string{};

    return parameter->getName(128).toStdString();
}

float CharmApp::get_host_parameter_value(const char* parameter_id, float fallback_value) const
{
    if (host_parameters == nullptr)
        return fallback_value;

    if (auto* raw_value = host_parameters->getRawParameterValue(parameter_id))
        return raw_value->load();

    return fallback_value;
}

float CharmApp::get_smoothed_host_parameter_value(const char* parameter_id, float fallback_value)
{
    const float target = get_host_parameter_value(parameter_id, fallback_value);

    if (parameter_id == nullptr)
        return target;

    const std::string key(parameter_id);
    auto [it, inserted] = smoothed_parameter_values.emplace(key, target);

    if (inserted)
        return target;

    const float coefficient = juce::jlimit(0.0f, 1.0f, param_smooth_coeff);
    it->second += coefficient * (target - it->second);
    return it->second;
}

void CharmApp::reset_parameter_smoothers()
{
    smoothed_parameter_values.clear();
}

bool CharmApp::set_host_parameter_value(const char* parameter_id, float value) const
{
    auto* parameter = get_host_parameter(host_parameters, parameter_id);
    if (parameter == nullptr)
        return false;

    const auto range = parameter->getNormalisableRange();
    const float clamped_value = juce::jlimit(range.start, range.end, value);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(clamped_value));
    return true;
}

float CharmApp::ui_host_parameter_slider(charm::Rect rect, const char* parameter_id, float fallback_value)
{
    auto* parameter = get_host_parameter(host_parameters, parameter_id);
    float value = get_host_parameter_value(parameter_id, fallback_value);

    float min = 0.0f;
    float max = 1.0f;

    if (parameter != nullptr)
    {
        const auto range = parameter->getNormalisableRange();
        min = range.start;
        max = range.end;
    }

    if (ui_fslider_rect(rect, &value, min, max))
        set_host_parameter_value(parameter_id, value);

    return value;
}

float CharmApp::ui_host_parameter_row(charm::Rect& rect, const char* parameter_id,
                                      float fallback_value, float row_height, float label_width)
{
    charm::Rect new_line = cut_top(&rect, row_height);
    charm::Rect label_column = cut_left(&new_line, label_width);

    const std::string label = get_host_parameter_name(parameter_id);
    ui_labeled_rect(label_column, "%s", label.c_str());

    const float value = ui_host_parameter_slider(new_line, parameter_id, fallback_value);
    ui_labeled_rect(new_line, "%.3f", value);

    return value;
}

void CharmApp::init_ui(UIRenderer ui_renderer)
{
    ui_ctx = std::make_unique<UICore>();
    ui_ctx->init(ui_renderer);
    set_ui_state(ui_ctx.get());
};

void CharmApp::prepare_to_play(double sampleRate, int blockSize)
{
    this->sample_rate = sampleRate;
    this->block_size = blockSize;
    reset_parameter_smoothers();

    // Initialize your audio buffers, oscillators, etc.
    for (int i = 0; i < MAX_BUFFER; ++i)
    {
        viz_buffer_left[i] = 0.0f;
        viz_buffer_right[i] = 0.0f;
        viz_buffer_mono[i] = 0.0f;
    }

    main_gain = 1.0f;
    phase_invert = 1.0f;
    // other synth init...
}

// Helper to extract a contiguous segment from the ringbuffer for drawing
void get_audio_segment(const float* ringbuffer, int write_pos, int buffer_size,
                       float* out_segment, int segment_size)
{
    int start = (write_pos + buffer_size - segment_size) % buffer_size;
    for (int i = 0; i < segment_size; ++i) {
        out_segment[i] = ringbuffer[(start + i) % buffer_size];
    }
}

// #include <fftw3.h>
#include <math.h>

// void CharmApp::manipulate_spectrum(float* signal, int num_samples)
// {
//     for (int i = 0; i < num_samples; i++) {
//         float multiplier = 0.5 * (1 - cos(2*M_PI*i/(num_samples-1)));
//         signal[i] = multiplier * signal[i];
//     }

//     // Allocate memory for FFT result and inverse
//     fftwf_complex* fft_result = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (num_samples / 2 + 1));
//     float* ifft_result = (float*) fftwf_malloc(sizeof(float) * num_samples);

//     // Create FFTW plans (single precision)
//     fftwf_plan forward_plan = fftwf_plan_dft_r2c_1d(num_samples, signal, fft_result, FFTW_ESTIMATE);
//     fftwf_plan inverse_plan = fftwf_plan_dft_c2r_1d(num_samples, fft_result, ifft_result, FFTW_ESTIMATE);

//     // Perform forward FFT
//     fftwf_execute(forward_plan);

//     // Manipulate the spectrum
//     for (int i = 0; i <= num_samples / 2; i++)
//     {
//         float real = fft_result[i][0];
//         float imag = fft_result[i][1];

//         // Cartesian to polar coordinates:
//         float magnitude = sqrtf(real * real + imag * imag);
//         float phase = atan2f(imag, real); // optional

//         if (magnitude < this->magnitude_threshold)
//         {
//             fft_result[i][0] = 0.0f;
//             fft_result[i][1] = 0.0f;
//         }
//     }

//     // Perform inverse FFT
//     fftwf_execute(inverse_plan);

//     // Normalize inverse result and copy back to signal
//     for (int i = 0; i < num_samples; i++)
//     {
//         signal[i] = ifft_result[i] / num_samples;
//     }

//     // Cleanup
//     fftwf_destroy_plan(forward_plan);
//     fftwf_destroy_plan(inverse_plan);
//     fftwf_free(fft_result);
//     fftwf_free(ifft_result);
// }


// Moog VCF — Stilson/Smith CCRMA
float CharmApp::moog_filter(float input, float cutoff, float res, float fold, float drive, MoogFilterState& s) const
{
    const float fs = sample_rate > 0 ? sample_rate : 44100.0f;
    const float fc = expf(2.3f + cutoff * 7.7f);

    const float f     = 2.0f * fc / fs;
    const float k     = 3.6f * f - 1.6f * f * f - 1.0f;
    const float p     = (k + 1.0f) * 0.5f;
    const float scale = expf((1.0f - p) * 1.386249f);
    const float r     = res * scale;

    float x = input - r * s.y4;

    const float limit = powf(10.0f, fold * -30.0f * 0.05f);
    if (x > limit || x < -limit)
    {
        x = fabsf(fabsf(fmodf(input - limit, limit * 4.0f)) - limit * 2.0f) - limit;
    }

    x = tanhf(x * (1.0f + (drive * 2.0f)));

    s.y1 = x    * p + s.oldx  * p - k * s.y1;
    s.y2 = s.y1 * p + s.oldy1 * p - k * s.y2;
    s.y3 = s.y2 * p + s.oldy2 * p - k * s.y3;
    s.y4 = s.y3 * p + s.oldy3 * p - k * s.y4;

    s.y4 = s.y4 - (s.y4 * s.y4 * s.y4) / 6.0f;

    s.oldx  = x;
    s.oldy1 = s.y1;
    s.oldy2 = s.y2;
    s.oldy3 = s.y3;

    return s.y4;
}

void CharmApp::process_audio(float* outputs[], int num_channels, int num_samples)
{
    this->custom_function_state.playhead_pos = this->phasor_clock.phase;
    ui_custom_function_eval(&this->custom_function_state);
    
    set_host_parameter_value(knob_1_param_id, 1.0 - this->custom_function_state.playhead_function_value);

    if (this->module_loader.is_loaded())
    {
        // Build context with current parameter values
        ModuleAudioContext context;
        context.knob_1 = get_host_parameter_value(knob_1_param_id, 0.5f);
        context.knob_2 = get_host_parameter_value(knob_2_param_id, 0.5f);
        context.knob_3 = get_host_parameter_value(knob_3_param_id, 0.5f);
        context.knob_4 = get_host_parameter_value(knob_4_param_id, 0.5f);
        context.num_channels = num_channels;
        context.sample_rate = static_cast<int>(sample_rate);
        
        // Process audio through module
        this->module_loader.process_audio(outputs, num_channels, num_samples, &context);
    }
}

bool CharmApp::load_plugin(const std::string& dylib_path)
{
    if (!this->module_loader.load(dylib_path))
    {
        printf("Failed to load module: %s\n", this->module_loader.get_error().c_str());
        return false;
    }
    
    if (!this->module_loader.create_instance())
    {
        printf("Failed to create module instance\n");
        return false;
    }
    
    printf("Loaded module: %s v%s\n", this->module_loader.get_name(), this->module_loader.get_version());
    return true;
}

bool CharmApp::reload_plugin()
{
    if (!this->module_loader.reload())
    {
        printf("Failed to reload module: %s\n", this->module_loader.get_error().c_str());
        return false;
    }
    
    if (!this->module_loader.create_instance())
    {
        printf("Failed to create module instance after reload\n");
        return false;
    }
    
    printf("Reloaded module: %s v%s\n", this->module_loader.get_name(), this->module_loader.get_version());
    return true;
}

std::vector<std::string> CharmApp::scan_available_modules()
{
    std::vector<std::string> modules;
    
    // Scan the modules directory
    std::string modules_dir = "/Users/aidan/dev/cpp/juce_cmake/hotreloadcharm/build/plugins";
    
    DIR* dir = opendir(modules_dir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;
            // Look for .dylib files
            if (filename.size() > 6 && filename.substr(filename.size() - 6) == ".dylib") {
                // Remove .dylib extension for display
                std::string module_name = filename.substr(0, filename.size() - 6);
                modules.push_back(module_name);
            }
        }
        closedir(dir);
    }
    
    return modules;
}

#include <unordered_set>

void ui_draw_toolbar(charm::Rect& window_rect, CharmApp *app)
{
    UICore& ui = *get_ui_state();
    charm::Rect toolbar = window_rect;
    
//    begin_scroll_area(NULL, NULL, &window_rect, "toolbar2");
    ui.in_scroll_area = true;
    ui.draw_rect(toolbar, get_ui_theme()->panel_color);
    // ui.draw_box(win)

    if (rc_button(rectcut(&toolbar, RectCut_Left), "Reload")) {
        app->reload_plugin();
    }

    if (rc_button(rectcut(&toolbar, RectCut_Left), "Help")) {
   
    }
//
    if (rc_button(rectcut(&toolbar, RectCut_Right), "Settings")) {
        app->is_settings_open = !app->is_settings_open;
    }
    
    if(ui_text_input2(toolbar, &app->filename_text_state, app).keyentered == CHARM_KEY_RETURN)
    {
        if (strlen(app->filename_text_state.buffer) > 0) {
            printf("Open File: %s\n", app->filename_text_state.buffer);
            // push_undo(graph);
            // app->graph.load(app->filename_text_state.buffer);
        } else {
            printf("Filename is empty. Cannot open.\n");
        }
    }
        ui.in_scroll_area = false;
}

void ui_app_settings(charm::Rect& window_rect, CharmApp *app)
{
    UICore& ui = *get_ui_state();
        // charm::Rect side_panel = rectcut_cut(rectcut(&window_rect, RectCut_Right), 200);  // 200px wide panel 
    charm::Rect side_panel = resizable_panel(rectcut(&window_rect, RectCut_Right), &app->side_panel_size);
    UITheme &theme = *get_ui_theme();
    // app->settings_theme_panel_scroll_y = 0.0f;
    ui.draw_rect(side_panel, get_ui_theme()->panel_color);
    ui.draw_box(side_panel, get_ui_theme()->border_color);

    begin_scroll_area(NULL, &app->settings_theme_panel_scroll_y, &side_panel, "theme settings");

        // Label for the color picker
    charm::Rect new_line = cut_top(&side_panel, 32);
    rc_label(rectcut(&new_line, RectCut_Left), "test button");
    if(ui_button_rect_impl(new_line).clicked)
    {
        printf("Button clicked!\n");
    }

    // color picker already does new line
    ui_color_picker(&side_panel, &theme.button_color, "button");
    ui_color_picker(&side_panel, &theme.text_color, "text");
    ui_color_picker(&side_panel, &theme.panel_color, "panel");
    ui_color_picker(&side_panel, &theme.bg_color, "bg");
    ui_color_picker(&side_panel, &theme.border_color, "border");
    ui_color_picker(&side_panel, &theme.slider_handle_color, "slider handle");
    ui_color_picker(&side_panel, &theme.node_color, "node color");

    new_line = cut_top(&side_panel, 32);
    rc_label(rectcut(&new_line, RectCut_Left), "padding");
    ui_slider_rect(new_line, &theme.padding, 0, 16);

    // new_line = cut_top(&side_panel, 32);
    // ui_fslider_rect(new_line, &app->vuMeter.attackCoeff, 0, 1.0);
    // new_line = cut_top(&side_panel, 32);
    // ui_fslider_rect(new_line, &app->vuMeter.releaseCoeff, 0, 1.0);

    end_scroll_area();
}

// just reduces gain by 0.5
void simple_plugin(float* outputs[], int num_channels, int num_samples)
{
    printf("Hello from plugin code! num channels: %d, num samples: %d\n", num_channels, num_samples);
    for (int i = 0; i < num_samples; ++i)
    {
        float left  = outputs[0][i];
        float right = outputs[1][i];
        outputs[0][i] = left  * 0.5;
        outputs[1][i] = right * 0.5;
    }
}

void CharmApp::on_render(charm::Rect window_rect, float scale)
{
    set_ui_state(ui_ctx.get());
    
    // nvgBeginFrame(vg, app_state.window_size.x, app_state.window_size.y, 2.0f);
    UICore& ui = *get_ui_state();
    NVGcontext* vg = (NVGcontext*)ui.renderer.userdata;
    
    ui.new_frame();
    
    // render_stuff(window_rect);
    ui.draw_rect(window_rect, get_ui_theme()->bg_color);
    
    charm::Rect toolbar_rect = cut_top(&window_rect, 32);
    ui_draw_toolbar(toolbar_rect, this);

    if(this->is_settings_open)
    {
        ui_app_settings(window_rect, this);
    }

    // ui.in_scroll_area = true;

    // charm::Rect viz_area = cut_top(&window_rect, 128);
    // draw_visualizers(viz_area);

    // charm::Rect side_panel = resizable_panel(rectcut(&window_rect, RectCut_Right), &app->side_panel_size);
    // UITheme &theme = *get_ui_theme();
    // // app->settings_theme_panel_scroll_y = 0.0f;
    // ui.draw_rect(side_panel, get_ui_theme()->panel_color);
    // ui.draw_box(side_panel, get_ui_theme()->border_color);

    begin_scroll_area(NULL, &this->main_panel_scroll_y, &window_rect, "main");

    // charm::Rect new_line = cut_top(&window_rect, 32);
    // if(rc_button(rectcut(&new_line, RectCut_Left), "reload")) {
    //     printf("Button clicked!\n");
    // }

    charm::Rect new_line = cut_top(&window_rect, 32.0);
    // just a label
    rc_label(rectcut(&new_line, RectCut_Left), "dylibs folder");

    // list modules from dylibs folder
    auto available_modules = scan_available_modules();
    for (const auto& module_name : available_modules) {
        if (rc_button(rectcut(&new_line, RectCut_Left), module_name.c_str())) {
            // Load the module
            std::string module_path = "/Users/aidan/dev/cpp/juce_cmake/hotreloadcharm/build/plugins/" + module_name + ".dylib";
            load_plugin(module_path);
        }
    }


    ui_host_parameter_row(window_rect, knob_1_param_id, 0.0);
    ui_host_parameter_row(window_rect, knob_2_param_id, 0.0);
    ui_host_parameter_row(window_rect, knob_3_param_id, 0.5f);
    ui_host_parameter_row(window_rect, knob_4_param_id, 0.0f);
    
    end_scroll_area();
    
    begin_scroll_area(NULL, NULL, &window_rect, ""); // there is no scroll area just turns the "area" active todo fix
    charm::Rect leftover_shrunk_rect = shrink_rect(window_rect, 16.0);
    // static CustomFunctionState custom_function_state = {};
    // custom_function_state.playhead_pos += ui.dt * 0.1f; // animate playhead for demo
    // this->custom_function_state.playhead_pos = this->phasor_clock.phase;
    // if (custom_function_state.playhead_pos > 1.0f)
    //     custom_function_state.playhead_pos -= 1.0f;
    ui_custom_function_widget(leftover_shrunk_rect, &this->custom_function_state);
    end_scroll_area();

    // printf("HEY!!!!\n");

    ui.end_frame();
};
