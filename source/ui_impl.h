#pragma once
#include "ui.h"
#include "nanovg.h"
#include <iostream>
#include <algorithm>

class CharmApp;

struct UITheme
{
	Color button_color = {65,57,68,200};
    Color hot_color = {255,255,255,100};
    Color flash_color = {255,255,255,180};
    Color text_color = {255,255,255,255};
    Color panel_color = {27,27,28,255};
    Color label_bg_color = {180,24,20,255};
    Color slider_handle_color = {140,163,123,199};
    Color slider_color = {255,255,255,14};
    Color bg_color = {80,80,80,80};
    Color border_color = {150,150,150,200};
    Color node_color = {96,138,132,100};
    int padding = 8;
};

UITheme* get_ui_theme();

void vg_draw_rect(charm::Rect rect, Color color, void* userdata);

void vg_draw_box(charm::Rect rect, Color color, void* userdata);

void vg_draw_string(const char* string, charm::Vec2 pos, Color color, void* userdata);

void vg_draw_line(charm::Vec2 start, charm::Vec2 end, Color color, void* userdata);

int vg_get_text_width(const char* string, void* userdata);

int vg_get_text_height(const char* string, void* userdata);

int vg_load_font(NVGcontext* vg, const char* name, const char* file_path);

void vg_render_clip(void* userdata, charm::Rect* rect);

void draw_line(NVGcontext* vg, float x1, float y1, float x2, float y2, NVGcolor color);

void ui_labeled_rect(charm::Rect rect, const char* format, ...);

int ui_slider_rect(charm::Rect rect, int *slider_value, int min, int max);

// lazy copy paste todo clean
bool ui_fslider_rect(charm::Rect rect, float *slider_value, float min, float max);

int ui_vfslider_rect(charm::Rect *rect, float *slider_value, float min, float max);

WidgetComm ui_button_rect_impl(charm::Rect rect);

bool ui_button_rect(charm::Rect rect);

bool ui_checkbox_rect(charm::Rect rect, bool *value);


bool ui_slider_rect2(charm::Rect rect, int *slider_value, int min, int max);

#include <nanovg.h>
#include <cmath> // For std::sin and std::cos

bool ui_knob_slider(charm::Rect rect, float *slider_value, float min, float max);

bool ui_combo_rect(charm::Rect *rect, std::vector<std::string> options, int* selected, bool& opened);


// static charm::Rect scrollbar_vert_rect = {0,0,0,0};
// static charm::Rect scrollbar_horz_rect = {0,0,0,0};

// // data smuggling cross frames..........
// struct ScrollArea
// {
//     charm::Rect* rect; // this is the rect that gets moved around
//     charm::Rect rect_copy; // this is BEFORE scroll, "viewport"
//     // data passed in from begin
//     float* scroll_x;
//     float* scroll_y;
//     std::string panel_name; // keep track of previous frame overflow
// };
// charm::Rect last_scroll_area = {0,0,0,0};
// std::vector<ScrollArea> scroll_stack;

// static std::map<std::string, float> panel_overflow_vert_prev;
// static std::map<std::string, float> panel_overflow_horz_prev;

void begin_scroll_area(float *scroll_x, float *scroll_y, charm::Rect* area, std::string panel_name);

void handle_scroll_axis(
    float* scroll_value,
    float content_size,
    std::map<std::string, float>& overflow_map,
    const std::string& panel_name,
    charm::Rect& scrollbar_rect,
    bool is_vertical);

void end_scroll_area();

charm::Rect resizable_panel(RectCut layout, int* panel_size);

bool rc_button(RectCut layout, const char* label);

bool rc_combo(RectCut layout, std::vector<std::string> options, int* selected, bool& opened);

#include <cstdarg> // For va_list, va_start, va_end
#include <cstdio>  // For vsnprintf

void rc_label(RectCut layout, const char* format, ...);

void draw_oscilloscope(const charm::Rect& rect, const float* audio_data, size_t sample_count, NVGcontext* vg);

void draw_waveform(const charm::Rect& rect, const float* audio_data, size_t sample_count, NVGcontext* vg);

float draw_waveform_heatmap(const charm::Rect& rect, const float* audio_data,
                           size_t sample_count, NVGcontext* vg);

void draw_vu_meter(const charm::Rect& rect, float level, NVGcontext* vg);

void draw_lissajous(const charm::Rect& rect, const float* audio_data_x, const float* audio_data_y, size_t sample_count, NVGcontext* vg);

// #include <cmath>
// #include <vector>
// #include <complex>
#include <fftw3.h>

// #include <fftw3.h>

// void compute_fft_magnitude(const float* input, float* output, size_t sample_count);

void draw_fft_visualizer(const charm::Rect& rect, const float* audio_data, size_t sample_count, NVGcontext* vg);


void ui_color_picker(charm::Rect* panel, Color* color, const char* label);

WidgetComm ui_text_input(charm::Rect rect, char* buffer, int* cursor_pos, WidgetFlags extra_flags = 0);

struct TextInputState {
    char buffer[256] = {};   // moved text buffer here
    int cursor_pos = 0;
    int selection_start = -1;  // -1 = no selection
    int selection_end = -1;
};

WidgetComm ui_text_input2(charm::Rect rect, TextInputState* state, CharmApp* charm_app, WidgetFlags extra_flags = 0);

// todo remove
struct TextBoxState {
    char buffer[256];
    int cursor_pos = 0;
    bool is_editing = false;
    
    int scroll_x = 0;

    TextBoxState(const char* initial = "0.0") {
        snprintf(buffer, sizeof(buffer), "%s", initial);
        cursor_pos = strlen(buffer);
    }
};

bool ui_text_box(charm::Rect rect, TextBoxState& state);

void rc_text_input(RectCut layout, char* buffer, int* cursor_pos);

bool ui_drag_button(charm::Rect &rect);

struct CustomFunctionState
{
    std::vector<charm::Vec2> points;
    // int num_grid_x = 4;
    // int num_grid_y = 2;
    charm::Vec2 num_grid_lines = {4, 2};
    bool snap_to_grid = true;
    float playhead_pos = 0.0f; // 0.0 to 1.0
    float playhead_function_value = 0.0f; // value at playhead position, between 0 and 1 based on grid
};

WidgetComm ui_custom_function_widget(charm::Rect rect, CustomFunctionState* state);
