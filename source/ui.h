#pragma once

#include <algorithm>
#include <string>

#define DOUBLE_CLICK_THRESHOLD 0.3 // seconds (adjust as needed)

typedef uint64_t UIId;

// had generic names so had to wrap in namespace
// was conflicting with MacTypes
namespace charm
{
    struct Vec2 { float x, y; };
    struct Rect {
        float x, y, w, h;
    };
}

float lerp(float a, float b, float f);

void print_rect(charm::Rect rect);

void print_vec2(charm::Vec2 vec2);

struct Color { unsigned char r, g, b, a; };

// https://web.archive.org/web/20210306102303/https://halt.software/dead-simple-layouts/

typedef enum {
    RectCut_Left,
    RectCut_Right,
    RectCut_Top,
    RectCut_Bottom,
    RectAdd_Top,
} RectCutSide;

typedef struct {
    charm::Rect* rect;
    RectCutSide side;
} RectCut;

charm::Rect shrink_rect(const charm::Rect& rect, int amount);

charm::Rect cut_left(charm::Rect* rect, float a);

charm::Rect cut_right(charm::Rect* rect, float a);

charm::Rect cut_top(charm::Rect* rect, float a);

charm::Rect cut_bottom(charm::Rect* rect, float a);

charm::Rect cut_corner(charm::Rect* rect, float width, float height);

// Get functions - same as cut, but keep input rect intact
charm::Rect get_left(const charm::Rect* rect, float a);

charm::Rect get_right(const charm::Rect* rect, float a);

charm::Rect get_top(const charm::Rect* rect, float a);

charm::Rect get_bottom(const charm::Rect* rect, float a);

// Add functions - add rectangle outside of the input rectangle
charm::Rect add_left(const charm::Rect* rect, float a);

charm::Rect add_right(const charm::Rect* rect, float a);

charm::Rect add_top(charm::Rect* rect, float a);

charm::Rect add_bottom(const charm::Rect* rect, float a);

RectCut rectcut(charm::Rect* rect, RectCutSide side);

charm::Rect rectcut_cut(RectCut rectcut, int a);

/*
    hook your drawing functions like this

    UIRenderer renderer = {
        sdl2_renderer,  // userdata
        sdl2_draw_rect,
        sdl2_draw_string,
        sdl2_get_text_width,
    };

    and then pass that to UICore
*/

// this is graphics API agnostic library you have to implement these functions
struct UIRenderer
{
    void* userdata;
    void (*draw_rect)(charm::Rect rect, Color color, void* userdata);
    void (*draw_box)(charm::Rect rect, Color color, void* userdata);
    void (*draw_line)(charm::Vec2 start, charm::Vec2 end, Color color, void* userdata);
    void (*draw_string)(const char* string, charm::Vec2 pos, Color color, void* userdata);
    int (*get_text_width)(const char* string, void* userdata);
    int (*get_text_height)(const char* string, void* userdata);
    void (*render_clip)(void* userdata, charm::Rect *rect);
};

#include <vector>

typedef uint32_t WidgetFlags;

enum
{
  WidgetFlag_Clickable       = (1<<0),
  WidgetFlag_ViewScroll      = (1<<1),
  WidgetFlag_DrawText        = (1<<2),
  WidgetFlag_DrawBorder      = (1<<3),
  WidgetFlag_DrawBackground  = (1<<4),
  WidgetFlag_DrawDropShadow  = (1<<5),
  WidgetFlag_Clip            = (1<<6),
  WidgetFlag_HotAnimation    = (1<<7),//todo rename hovering
  WidgetFlag_ActiveAnimation = (1<<8),
  WidgetFlag_Dragging        = (1<<9),
  WidgetFlag_TextInput       = (1<<10),// can you implement this?
};

/*
inspired by
https://www.rfleury.com/p/ui-part-3-the-widget-building-language

on "Confronting Combinatorics"
*/

// widget communication
struct WidgetComm
{
    UIId id = -1;
    charm::Vec2 mouse = {-1,-1};
    charm::Vec2 drag_delta = {0,0}; // delta since drag
    charm::Vec2 drag_delta_frame = {0,0}; // per frame
    bool clicked = false;
    bool double_clicked = false; // need to implement
    bool right_clicked = false;
    bool pressed = false;
    bool released = false;
    bool dragging = false;
    bool hovering = false;
    bool active = false;
    UIId released_over = 0;

    int keyentered = 0;
    int keychar = 0;
    int typing = 0;

};

/*
much help
https://www.youtube.com/watch?v=V_phHKr0yRY
https://solhsa.com/imgui/
*/

#include <map>

// from CSS
#define REM 16.0f

// data smuggling cross frames..........
struct ScrollArea
{
    charm::Rect* rect; // this is the rect that gets moved around
    charm::Rect rect_copy; // this is BEFORE scroll, "viewport"
    // data passed in from begin
    float* scroll_x;
    float* scroll_y;
    std::string panel_name; // keep track of previous frame overflow
};


enum CharmKey {
    CHARM_KEY_NONE = 0,

    // control keys
    CHARM_KEY_BACKSPACE, // or delete?
    CHARM_KEY_RETURN,
    CHARM_KEY_ESCAPE,
    CHARM_KEY_TAB,
    CHARM_KEY_SPACE = 32, // hack to keep text input working
    CHARM_KEY_DELETE,
    CHARM_KEY_INSERT,

    // arrows
    CHARM_KEY_LEFT,
    CHARM_KEY_RIGHT,
    CHARM_KEY_UP,
    CHARM_KEY_DOWN,

//    // modifiers
//    CHARM_KEY_SHIFT,
//    CHARM_KEY_CTRL,
//    CHARM_KEY_ALT,
//    CHARM_KEY_COMMAND, // meta / cmd on mac

};

enum CharmKeyMod
{
    CHARM_KEYMOD_NONE   = 0,
    CHARM_KEYMOD_SHIFT  = 1 << 0,
    CHARM_KEYMOD_CTRL   = 1 << 1,
    CHARM_KEYMOD_ALT    = 1 << 2,
    CHARM_KEYMOD_SUPER  = 1 << 3  // Command on Mac, Windows key on PC
};

struct UICore
{
    // std::map<int, bool> keymap;
    const uint8_t *keystates = nullptr;
	// bool previous_keystates[SDL_NUM_SCANCODES] = { false }; // Array to store the previous states of keys

    bool overflow_vert = false;
    bool overflow_horz = false;
    charm::Rect scrollbar_vert_rect = {0,0,0,0};
    charm::Rect scrollbar_horz_rect = {0,0,0,0};
    // charm::Rect last_scroll_area = {0,0,0,0};
    std::vector<ScrollArea> scroll_stack;

    std::map<std::string, float> panel_overflow_vert_prev;
    std::map<std::string, float> panel_overflow_horz_prev;

    charm::Vec2 mouse_pos = {0,0};
    charm::Vec2 drag_start_pos = {0,0};
    charm::Vec2 mouse_frame_delta = {0,0};
    charm::Vec2 last_mouse = {0,0};
    // charm::Vec2 mouse_frame_delta = {0,0};
    bool mouse_down = 0;
    bool mouse_released = 0;
    bool mouse_clicked = 0;

    // resizable panel
    bool is_resizing = false;
    int last_mouse_pos = 0;// should be its own thing

    bool right_mouse_clicked = 0;
    bool right_mouse_released = 0;
    bool right_mouse_down = 0;

    bool undo_pressed = 0; // control z
    bool save_pressed = 0; // control s
    bool copy_pressed = 0; // control c
    bool paste_pressed = 0; // control v

    double last_click_time = 0.0;
    int click_count = 0;

    bool double_click = 0;
//    float scroll_delta_y = 0;
    charm::Vec2 scroll_delta = {0.0f,0.0f};

    float scroll_input = 0;
    float target_scroll_delta = 0;
    UIId id_counter = 0;
    UIId hover = 0;
    UIId next_hover = 0; // this is needed so we can detect a wiget above another one
    UIId mouse_down_hover = 0;
    UIId clicked = 0;
    UIId active = 0;

    // text widget input
    int kbditem = -1;
    int keyentered;
    int keymod;
    int keychar;

    // used for tab cycling through widgets
    int last_widget;

    // ID of the previously focused keyboard input widget — used to detect when a new text input gains focus
    int last_kbditem;

    float time = 0.0f;     // in seconds
    float dt = 0.016f;     // todo update this with real frame delta time

    UIRenderer renderer;

    // not using this really anymore :/ kinda fucked up everything
    charm::Rect active_scroll_area = {0, 0, 0, 0}; // Active scroll area
    bool in_scroll_area = false;           // Whether mouse is in the scroll area

    void init(UIRenderer renderer);

    int get_text_width(const char *string);

    int get_text_height(const char *string);

    void draw_string(const char *string, charm::Vec2 pos, Color color);

    void draw_formatted_string(charm::Vec2 pos, Color color, const char* format, ...);

    void draw_rect(charm::Rect rect, Color color);

    void draw_box(charm::Rect rect, Color color);

    void draw_line(charm::Vec2 start, charm::Vec2 end, Color color);

    UIId next_id();

    int rect_is_inside(charm::Rect rect1, charm::Rect rect2);

    int rect_intersects(charm::Rect rect1, charm::Rect rect2);

    int point_in_rect(charm::Rect rect, charm::Vec2 pos);

    bool region_hit(charm::Rect rect);

    void new_frame();

    void end_frame();

    WidgetComm build_widget(charm::Rect rect, WidgetFlags flags);

};

UICore* get_ui_state();

void set_ui_state(UICore* ctx);
