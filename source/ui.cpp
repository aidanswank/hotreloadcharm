#include "ui.h"

float lerp(float a, float b, float f)
{
    return (a * (1.0 - f)) + (b * f);
}

void print_rect(charm::Rect rect)
{
    printf("x%f y%f w%f h%f\n", rect.x, rect.y, rect.w, rect.h);
}

void print_vec2(charm::Vec2 vec2)
{
    printf("x%f y%f\n", vec2.x, vec2.y);
}

// static bool overflow_vert = false;
// static bool overflow_horz = false;

// bool* get_overflow_vert()
// {
//     return &overflow_vert;
// }

// bool* get_overflow_horz()
// {
//     return &overflow_horz;
// }

charm::Rect rectcut_cut(RectCut rectcut, int a) {
    switch (rectcut.side) {
        case RectCut_Left:   return cut_left(rectcut.rect, a);
        case RectCut_Right:  return cut_right(rectcut.rect, a);
        case RectCut_Top:    return cut_top(rectcut.rect, a);
        case RectCut_Bottom: return cut_bottom(rectcut.rect, a);
        case RectAdd_Top: return add_top(rectcut.rect, a);
        default: abort();
    }
}

void UICore::init(UIRenderer renderer)
{
    this->renderer = renderer;
}

int UICore::get_text_width(const char *string) 
{
    return renderer.get_text_width(string, renderer.userdata);
}

int UICore::get_text_height(const char *string) 
{
    return renderer.get_text_height(string, renderer.userdata);
}

void UICore::draw_string(const char *string, charm::Vec2 pos, Color color) 
{
    renderer.draw_string(string, pos, color, renderer.userdata);
}

void UICore::draw_formatted_string(charm::Vec2 pos, Color color, const char* format, ...) 
{
    char buffer[1024]; // Buffer to hold the formatted string

    va_list args;
    va_start(args, format); // Initialize the va_list with the last known fixed argument

    // Use vsnprintf to format the string safely
    vsnprintf(buffer, sizeof(buffer), format, args); // Format the string and store it in buffer

    va_end(args); // Clean up the va_list

    this->draw_string(buffer, pos, color); // Draw the formatted string
}

void UICore::draw_rect(charm::Rect rect, Color color)
{
    renderer.draw_rect(rect, color, renderer.userdata);
}

void UICore::draw_box(charm::Rect rect, Color color)
{
    renderer.draw_box(rect, color, renderer.userdata);
}

void UICore::draw_line(charm::Vec2 start, charm::Vec2 end, Color color)
{
    renderer.draw_line(start, end, color, renderer.userdata);
}

UIId UICore::next_id()
{
    UIId id = id_counter;
    id_counter += 1;
    return id;
}

int UICore::rect_is_inside(charm::Rect rect1, charm::Rect rect2) {
    if (rect1.x >= rect2.x && // rect1's left side is inside rect2
        rect1.x + rect1.w <= rect2.x + rect2.w && // rect1's right side is inside rect2
        rect1.y >= rect2.y && // rect1's top side is inside rect2
        rect1.y + rect1.h <= rect2.y + rect2.h) { // rect1's bottom side is inside rect2
        return 1; // rect1 is completely inside rect2
    }
    return 0; // rect1 is not completely inside rect2
}

int UICore::rect_intersects(charm::Rect rect1, charm::Rect rect2) {
    if (rect1.x + rect1.w <= rect2.x || // rect1 is to the left of rect2
        rect1.x >= rect2.x + rect2.w || // rect1 is to the right of rect2
        rect1.y + rect1.h <= rect2.y || // rect1 is above rect2
        rect1.y >= rect2.y + rect2.h) { // rect1 is below rect2
        return 0; // No intersection
    }
    return 1; // Intersection
}

int UICore::point_in_rect(charm::Rect rect, charm::Vec2 pos) {
    if (pos.x < rect.x || pos.y < rect.y ||
        pos.x >= rect.x + rect.w || pos.y >= rect.y + rect.h) {
        return 0;
    }
    return 1;
}


bool UICore::region_hit(charm::Rect rect) {
    // // printf("active scroll area ");
    // // print_rect(active_scroll_area);
    // // printf("\n");
    // // printf("in scroll area %i\n", in_scroll_area);
    // // if (!in_scroll_area) return 0; // Ignore hits outside scroll areas
    // if (mouse_pos.x < rect.x || mouse_pos.y < rect.y ||
    //     mouse_pos.x >= rect.x + rect.w || mouse_pos.y >= rect.y + rect.h) {
    //     return 0;
    // }
    // return 1;

    return point_in_rect(rect, mouse_pos);
}

void UICore::new_frame()
{
    overflow_vert = false;

    // // Update previous mouse position
    // prev_mouse_pos = mouse_pos;

    // reset counts
    id_counter = 1;
    hover = next_hover;
    next_hover = 0;
    // reset clicked
    clicked = 0;
    // if(mouse_released)
    // {
    //     // active = 0;  // Reset active when mouse is released
    // }

    if(mouse_clicked)
    {
        drag_start_pos = mouse_pos;
    }

    if(mouse_released) // if mouse up
    {
        // this runs one frame behind
        // this is so if the user pulls away the mouse they can exit the click
        if(mouse_down_hover == hover)
        {
            clicked = mouse_down_hover;
            // printf("released\n");
        }

        // clicked = mouse_down_hover;

        mouse_down_hover = 0;
        // active = 0;  // Reset active when mouse is released // widget turns it off itself
    }

    if(mouse_clicked) // if mouse down
    {
        if(hover != 0) // if something is being hovered
        {
            mouse_down_hover = hover;
        }

        if(active == 0 && hover != 0)
        {
            active = hover;
        }
    }

}

void UICore::end_frame()
{
    // // Update previous mouse position
    // prev_mouse_pos = mouse_pos;
    last_mouse = mouse_pos;
    
    if(mouse_released)
    {
        mouse_released = false;
    }

    if(mouse_clicked)
    {
        mouse_clicked = false;
    }

    keyentered = 0;
    last_kbditem = kbditem;
    
    scroll_delta.y = 0;
    scroll_delta.x = 0;

     mouse_clicked = false;
     mouse_released = false;
    // right_mouse_clicked = false;
    right_mouse_released = false;
    
    double_click = false;
    
    time += dt; // for time based effects, e.g cursor blink
    
    // used by ui_jack
    // drag_alive_frame++;
}

WidgetComm UICore::build_widget(charm::Rect rect, WidgetFlags flags)
{
    WidgetComm result;

    UIId id = next_id();
    // WidgetComm result;
    result.id = id;


    // if(region_hit(rect) && (active == 0))
    // printf("active id %i my id %i\n", active, id);
    // if(region_hit(rect) && rect_intersects(rect, active_scroll_area))
    // {
    //     // draw_rect(rect, {255,0,0,200});
    //     next_hover = id;
    // }
    
//        draw_box(active_scroll_area, {255,255,0,255});

//         if(region_hit(rect) && rect_intersects(rect, active_scroll_area))
//        if(region_hit(rect))
    if(in_scroll_area && region_hit(rect)) // allow in scroll area to overridden (toolbar) needs better api
    {
        // draw_rect(rect, {255,0,0,200});
        next_hover = id;
    }

        // If no widget has keyboard focus, take it
    if (kbditem == 0)
        kbditem = id;

    // // If we have keyboard focus, show it
    // if (kbditem == id)
    
    if (kbditem == id)
    {
        if(flags & WidgetFlag_HotAnimation) // todo rename WidgetFlag_Hovering or be on by default
        {
            draw_rect(rect, {0, 200, 0, 100});
        }

        result.keyentered = keyentered;

         switch (keyentered)
         {
         case CHARM_KEY_TAB: // TAB
             // If tab is pressed, lose keyboard focus.
             // Next widget will grab the focus.
             kbditem = 0;
             // // If shift was also pressed, we want to move focus
             // // to the previous widget instead.
              if (keymod & CHARM_KEYMOD_SHIFT) //old sdl
                 kbditem = last_widget;
             // Also clear the key so that next widget
             // won't process it
             keyentered = 0;
             break;
         case CHARM_KEY_RETURN : // RETURN or ENTER
             // Had keyboard focus, received return,
             // so we'll act as if we were clicked.
             // printf("key entered... %c\n",keyentered);
             // result.clicked = (clicked == id);
             result.clicked = 1;
             keyentered = 0; // Clear keyentered to prevent continuous clicks
             return result;
             break;
         default:
             // keyentered = 0;
             break;
         }

        // Dont use fucked up everything after 

        // // printf("hey keyentered %i\n", keyentered);
        // // prevent keys strokes from non text input widgets to fire accidently
        // // maybe this is ugly idk
        // if (!(flags & WidgetFlag_TextInput))
        // {
        //     // result.keyentered = keyentered;
        //     if(keyentered)
        //     {
        //         keyentered = 0;
        //     }
        // }

        if (mouse_clicked) {            
            if (!point_in_rect(rect, mouse_pos)) {
                // opened = false; // Close the combo box if clicked outside
                kbditem = -1;
            }
        }

    }

    last_widget = id;

    if(flags & WidgetFlag_Clickable)
    {
        result.clicked = (clicked == id);
        
//            if(result.clicked)
//            {
//                draw_rect(rect, {255,255,255,255});
//            }
        // set the keyboard focus to widget we just clicked
        // result.double_clicked = (double_click && (fid==active))
            // if(ui.double_click && (widget.id==ui.active))

        if(result.clicked)
            kbditem = id;

    }

    if(flags & WidgetFlag_HotAnimation) // todo rename WidgetFlag_Hovering or be on by default
    {
        result.hovering = ( (hover == id) && (active == false) );
        // result.hovering = ( (hover == id) );
    }
    
    if(mouse_released && (active == id)) // if mouse release and the this widget is the current id
    {
//         printf("releaseee id %i\n", id);
        active = 0;
        result.released = true;
        result.drag_delta.x = mouse_pos.x - drag_start_pos.x;
        result.drag_delta.y = mouse_pos.y - drag_start_pos.y;
    }
    
    if (flags & WidgetFlag_Dragging)
    {
        if (active == id && (mouse_down_hover == id))
        {
            // printf("many\n"); // this prints until widget is no longer active, how can i only get one event 
            // print_vec2(drag_start_pos);

            // if(mouse_released)
            // {
            //     printf("released\n");
            // }
            result.drag_delta.x = mouse_pos.x - drag_start_pos.x;
            result.drag_delta.y = mouse_pos.y - drag_start_pos.y;
            
            result.drag_delta_frame.x = mouse_pos.x - last_mouse.x;
            result.drag_delta_frame.y = mouse_pos.y - last_mouse.y;
            
            
            result.dragging = true;
            // Check if drag delta per frame is greater than 0
            // if (result.drag_delta_frame.x != 0 || result.drag_delta_frame.y != 0)
            // {
            //     result.dragging = true;
            // }
            // else
            // {
            //     result.dragging = false;
            // }
            
        }
        else
        {
            // result.drag_delta = {0, 0};
            result.dragging = false;

        }

        if(active == id)
        {
            result.active = true;
        }

    }

    if(result.hovering)
    {
        draw_rect(rect, {255,255,255,50});
    }


    if(flags & WidgetFlag_Clickable) // or double clickabled?? idk
    {
        result.double_clicked = (double_click && (id==mouse_down_hover));
        // result.right_clicked = (right_mouse_clicked && (id==mouse_down_hover)); doesnt work 
    }

    return result;
}

thread_local UICore* current_ui_context = nullptr;

UICore* get_ui_state()
{
    return current_ui_context;
}

void set_ui_state(UICore* ctx)
{
    // printf("set_ui_state %p\n", ctx);
    current_ui_context = ctx;
}

charm::Rect shrink_rect(const charm::Rect& rect, int amount) {
    return {
        rect.x + amount,
        rect.y + amount,
        rect.w - 2 * amount,
        rect.h - 2 * amount
    };
}

charm::Rect cut_left(charm::Rect* rect, float a) {
    UICore& ui = *get_ui_state();

    float x = rect->x;
    float w;
    
    if(ui.overflow_horz) {
        w = a;
    } else {
        w = std::min(a, rect->w);
    }
    
    rect->x += w;
    rect->w -= w;
    return {x, rect->y, w, rect->h};
}

charm::Rect cut_right(charm::Rect* rect, float a) {
    UICore& ui = *get_ui_state();
    float w;
    
    if(ui.overflow_horz) {
        w = a;
        // Allow width to become negative if content exceeds visible area
        // rect->w -= w;
    } else {
        w = std::min(a, rect->w);
    }
    rect->w -= w;
    
    return {rect->x + rect->w, rect->y, w, rect->h};
}

charm::Rect cut_top(charm::Rect* rect, float a) {
    UICore& ui = *get_ui_state();
    float y = rect->y;
    float h;
    
    if(ui.overflow_vert)
    {
        h = a;
    } else {
        h = std::min(a, rect->h);
    }

    rect->y += h;
    rect->h -= h;
    return {rect->x, y, rect->w, h};
}

charm::Rect cut_bottom(charm::Rect* rect, float a) {
    float h = std::min(a, rect->h);
    rect->h -= h;
    return {rect->x, rect->y + rect->h, rect->w, h};
}

charm::Rect cut_corner(charm::Rect* rect, float width, float height) {
    UICore& ui = *get_ui_state();

    float w, h;
    float original_x = rect->x;
    float original_y = rect->y;

    // Handle horizontal cutting
    if (ui.overflow_horz) {
        w = width;
    } else {
        w = std::min(width, rect->w);
    }
    rect->x += w;
    rect->w -= w;

    // Handle vertical cutting
    if (ui.overflow_vert) {
        h = height;
    } else {
        h = std::min(height, rect->h);
    }
    rect->y += h;
    rect->h -= h;

    // Return the cut corner rectangle
    return {original_x, original_y, w, h};
}


// Get functions - same as cut, but keep input rect intact
charm::Rect get_left(const charm::Rect* rect, float a) {
    float w = (a < rect->w) ? a : rect->w;
    return { rect->x, rect->y, w, rect->h };
}

charm::Rect get_right(const charm::Rect* rect, float a) {
    float w = (a < rect->w) ? a : rect->w;
    return { rect->x + rect->w - w, rect->y, w, rect->h };
}

charm::Rect get_top(const charm::Rect* rect, float a) {
    float h = (a < rect->h) ? a : rect->h;
    return { rect->x, rect->y, rect->w, h };
}

charm::Rect get_bottom(const charm::Rect* rect, float a) {
    float h = (a < rect->h) ? a : rect->h;
    return { rect->x, rect->y + rect->h - h, rect->w, h };
}

// Add functions - add rectangle outside of the input rectangle
charm::Rect add_left(const charm::Rect* rect, float a) {
    return { rect->x - a, rect->y, a, rect->h };
}

charm::Rect add_right(const charm::Rect* rect, float a) {
    return { rect->x + rect->w, rect->y, a, rect->h };
}

charm::Rect add_top(charm::Rect* rect, float a) {
    // rect->y += a;
    // rect->h = a;
    return { rect->x, rect->y + a, rect->w, a };
}

charm::Rect add_bottom(const charm::Rect* rect, float a) {
    return { rect->x, rect->y + rect->h, rect->w, a };
}

RectCut rectcut(charm::Rect* rect, RectCutSide side) {
    return { rect, side };
}
