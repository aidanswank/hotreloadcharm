#include "ui_impl.h"
#include "CharmApp.h"

static UITheme theme;

UITheme* get_ui_theme()
{
    return &theme;
}
//
void vg_draw_rect(charm::Rect rect, Color color, void* userdata)
{
    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
    nvgBeginPath(vg);
    nvgRect(vg, rect.x, rect.y, rect.w, rect.h);
    nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
    nvgFill(vg);
}

// void vg_draw_rect(charm::Rect rect, Color color, void* userdata)
// {
//    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
//    float radius = 6.0f; // corner roundness

//    // --- Base glossy gradient ---
//    NVGpaint glossy = nvgLinearGradient(
//        vg,
//        rect.x, rect.y, rect.x, rect.y + rect.h,
//        nvgRGBA(
//            std::min(color.r + 40, 255),
//            std::min(color.g + 40, 255),
//            std::min(color.b + 40, 255),
//            color.a
//        ), // lighter top
//        nvgRGBA(
//            std::max(color.r - 40, 0),
//            std::max(color.g - 40, 0),
//            std::max(color.b - 40, 0),
//            color.a
//        )  // darker bottom
//    );

//    nvgBeginPath(vg);
//    nvgRoundedRect(vg, rect.x, rect.y, rect.w, rect.h, radius);
//    nvgFillPaint(vg, glossy);
//    nvgFill(vg);

//    // --- Top shine (white gradient fade) ---
//    NVGpaint shine = nvgLinearGradient(
//        vg,
//        rect.x, rect.y, rect.x, rect.y + rect.h * 0.4f,
//        nvgRGBA(255, 255, 255, 120),
//        nvgRGBA(255, 255, 255, 0)
//    );

//    nvgBeginPath(vg);
//    nvgRoundedRect(vg, rect.x, rect.y, rect.w, rect.h, radius);
//    nvgFillPaint(vg, shine);
//    nvgFill(vg);

//    // --- Bottom shadow (dark gradient) ---
//    NVGpaint shadow = nvgLinearGradient(
//        vg,
//        rect.x, rect.y + rect.h * 0.6f, rect.x, rect.y + rect.h,
//        nvgRGBA(0, 0, 0, 100),
//        nvgRGBA(0, 0, 0, 0)
//    );

//    nvgBeginPath(vg);
//    nvgRoundedRect(vg, rect.x, rect.y, rect.w, rect.h, radius);
//    nvgFillPaint(vg, shadow);
//    nvgFill(vg);
// //
// //    // --- Outline ---
// //    nvgBeginPath(vg);
// //    nvgRoundedRect(vg, rect.x + 0.5f, rect.y + 0.5f, rect.w - 1.0f, rect.h - 1.0f, radius);
// //    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 160));
// //    nvgStrokeWidth(vg, 1.0f);
// //    nvgStroke(vg);
// }


//void vg_draw_box(charm::Rect rect, Color color, void* userdata)
//{
//    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
//    nvgBeginPath(vg);
//    nvgRect(vg, rect.x, rect.y, rect.w, rect.h);
//    nvgStrokeColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
//    nvgStrokeWidth(vg, 1.0f); // Adjust stroke width as needed
//    nvgStroke(vg);
//}

void vg_draw_box(charm::Rect rect, Color color, void* userdata) {
    NVGcontext* vg = static_cast<NVGcontext*>(userdata);

    float radius = 4.0f; // change this for roundness

    nvgBeginPath(vg);
    nvgRoundedRect(vg, rect.x, rect.y, rect.w, rect.h, radius);
    nvgStrokeColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
    nvgStrokeWidth(vg, 1.0f); // Adjust stroke width as needed
    nvgStroke(vg);
}

static const float vg_font_size = 19.5;
void vg_draw_string(const char* string, charm::Vec2 pos, Color color, void* userdata)
{
    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
    nvgFontSize(vg, vg_font_size); // Adjust font size as needed
    nvgFontFace(vg, "sans-bold"); // Set the font face, ensure the font is loaded
    nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg, pos.x, pos.y, string, NULL);
}

void vg_draw_line(charm::Vec2 start, charm::Vec2 end, Color color, void* userdata)
{
    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
    nvgBeginPath(vg);  // Start a new path
    nvgMoveTo(vg, start.x, start.y);  // Move to the start point
    nvgLineTo(vg, end.x, end.y);  // Draw a line to the end point
    nvgStrokeColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));  // Set the stroke color
    nvgStrokeWidth(vg, 2.0f);  // Adjust stroke width as needed
    nvgStroke(vg);  // Stroke the path
}

int vg_get_text_width(const char* string, void* userdata)
{
    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
     nvgFontSize(vg, vg_font_size); // Adjust font size as needed
    return (int)nvgTextBounds(vg, 0, 0, string, NULL, nullptr);
}

int vg_get_text_height(const char* string, void* userdata)
{
    NVGcontext* vg = static_cast<NVGcontext*>(userdata);
     nvgFontSize(vg, vg_font_size); // Adjust font size as needed

    float ascender, descender, lineh;
    nvgTextMetrics(vg, &ascender, &descender, &lineh);
    
    return static_cast<int>(lineh);
}

int vg_load_font(NVGcontext* vg, const char* name, const char* file_path)
{
    int font = nvgCreateFont(vg, name, file_path);
    if (font == -1) {
        std::cerr << "Could not load font: " << file_path << std::endl;
        return -1;
    }
    return font;
}

void vg_render_clip(void* userdata, charm::Rect* rect)
{
    NVGcontext* vg = (NVGcontext*)userdata;

    if(rect == NULL)
    {
        nvgResetScissor(vg);
    } else {
        // Set scissor area in NanoVG
        nvgScissor(vg, rect->x, rect->y, rect->w, rect->h);
    }
}

// a slider that hops to the place the mouse was when clicked
int ui_slider_rect(charm::Rect rect, int *slider_value, int min, int max)
{
    UICore& ui = *get_ui_state();
    // Create the slider handle widget
    WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

    // Draw the slider background
    ui.draw_box(rect, {100, 255, 100, 255});

    // Calculate the relative position of the slider handle
    float value_normalized = (*slider_value - min) / static_cast<float>(max - min);
    float handle_position = static_cast<int>(value_normalized * rect.w);

    if (slider_handle_widget.dragging)
    {
        // Calculate the new handle position based on mouse position
        float relative_mouse_pos = ui.mouse_pos.x - rect.x;
        
        // Clamp the handle position within the slider bounds
        relative_mouse_pos = std::max(0.0f, std::min(relative_mouse_pos, rect.w));

        // Convert the relative position to the actual slider value
        value_normalized = relative_mouse_pos / static_cast<float>(rect.w);
        *slider_value = static_cast<int>(value_normalized * (max - min)) + min;

        printf("Dragging: slider_value %i\n", *slider_value);
    }

    // Calculate the progress rectangle based on the normalized slider value
    charm::Rect progress_rect = {rect.x, rect.y, handle_position, rect.h};

    // Draw the progress rectangle
    ui.draw_rect(progress_rect, {100, 255, 100, 255});

    // Draw the slider handle
    float handle_width = 10; // Width of the handle
    charm::Rect handle_rect = {rect.x + handle_position - handle_width / 2, rect.y, handle_width, rect.h};
    ui.draw_rect(handle_rect, {255, 255, 255, 255}); // Handle color (white)


    if(slider_handle_widget.hovering)
    {
        ui.draw_rect(rect, {255, 255, 255, 100});
    }
    // Draw the formatted string showing the slider value
    ui.draw_formatted_string({rect.x, rect.y}, {255, 255, 255, 255}, "%i", *slider_value);

    return (slider_handle_widget.dragging);
}

// void draw_line(NVGcontext* vg, float x1, float y1, float x2, float y2, NVGcolor color)
// {
//     nvgBeginPath(vg);          // Start a new path
//     nvgMoveTo(vg, x1, y1);     // Move to the start point of the line
//     nvgLineTo(vg, x2, y2);     // Draw a line to the end point of the line
//     nvgStrokeColor(vg, color); // Set the color for the line
//     nvgStrokeWidth(vg, 2.0); // Set the stroke width
//     nvgStroke(vg);             // Render the line
//     nvgClosePath(vg);
// }

// // void ui_labeled_rect(charm::Rect rect, const char* format, ...)
// // {
// //     // Buffer for formatted text
// //     char label_text[256]; // Adjust size as needed

// //     // Format the text
// //     va_list args;
// //     va_start(args, format);
// //     vsnprintf(label_text, sizeof(label_text), format, args);
// //     va_end(args);

// //     // Calculate text position for centering
// //     int text_width = ui.get_text_width(label_text);
// //     int text_height = ui.get_text_height(label_text);
// //     // int x_pos = rect.x + (rect.w - text_width) / 2;
// //     int x_pos = rect.x + theme.padding;
// //     int y_pos = rect.y + (rect.h - text_height) / 2;

// //     // Draw the text centered within the rectangle
// //     ui.draw_string(label_text, {x_pos, y_pos}, theme.text_color);
// // }

 void ui_labeled_rect(charm::Rect rect, const char* format, ...) {
     UICore &ui = *get_ui_state();
     // Buffer for formatted text
     char label_text[256]; // Adjust size as needed

     // Format the text
     va_list args;
     va_start(args, format);
     vsnprintf(label_text, sizeof(label_text), format, args);
     va_end(args);

     // Calculate available space for text
     int max_width = rect.w - (theme.padding * 2);
     int text_width = ui.get_text_width(label_text);
     int text_height = ui.get_text_height(label_text);

     // Determine if truncation is needed
     if (text_width > max_width) {
         const char* ellipsis = "...";
         int ellipsis_width = ui.get_text_width(ellipsis);

         // Truncate text to fit within max_width
         int len = strlen(label_text);
         while (len > 0) {
             // Create a temporary truncated string
             char temp[256];
             strncpy(temp, label_text, len);
             temp[len] = '\0';  // Ensure null termination
             strncat(temp, ellipsis, sizeof(temp) - len - 1);

             // Check if it fits
             if (ui.get_text_width(temp) <= max_width) {
                 strcpy(label_text, temp);
                 break;
             }

             len--;
         }

         // If everything fails, just use "..."
         if (len == 0) {
             strcpy(label_text, ellipsis);
         }
     }

     // Calculate text position (left-aligned with padding)
     float x_pos = rect.x + theme.padding;
     float y_pos = rect.y + (rect.h - text_height) / 2;

     // Draw the text
     ui.draw_string(label_text, {x_pos, y_pos}, theme.text_color);
 }

// int ui_slider_rect(charm::Rect rect, int *slider_value, int min, int max)
// {
//     UICore &ui = *get_ui_state();
//     // Create the slider handle widget
//     WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);
//
//     // Draw the slider background
//     ui.draw_box(rect, theme.border_color);
//
//     // Calculate the relative position of the slider handle
//     float value_normalized = (*slider_value - min) / static_cast<float>(max - min);
//     float handle_position = value_normalized * rect.w;
//
//     if (slider_handle_widget.dragging)
//     {
//         // Calculate the new handle position based on mouse position
//         float relative_mouse_pos = ui.mouse_pos.x - rect.x;
//
//         // Clamp the handle position within the slider bounds
//         relative_mouse_pos = std::max(0.0f, std::min(relative_mouse_pos, rect.w));
//
//         // Convert the relative position to the actual slider value
//         value_normalized = relative_mouse_pos / static_cast<float>(rect.w);
//         *slider_value = static_cast<int>(value_normalized * (max - min)) + min;
//
//         printf("Dragging: slider_value %i\n", *slider_value);
//     }
//
//     // Calculate the progress rectangle based on the normalized slider value
//     charm::Rect progress_rect = {rect.x, rect.y, handle_position, rect.h};
//
//     // Draw the progress rectangle
//     // ui.draw_rect(progress_rect, {100, 255, 100, 255});
//
//     // Draw the triangular slider handle using NanoVG
//     int handle_height = rect.h / 2; // Height of the triangle
//     float handle_center_x = rect.x + handle_position; // X position of the triangle's center
//     float handle_top_y = rect.y; // Y position of the top of the slider
//     float handle_bottom_y = rect.y + rect.h;         // Bottom Y of the slider
//     // nvgBeginPath(vg);
//     // nvgMoveTo(vg, handle_center_x, handle_top_y);                          // Top vertex of the triangle (point)
//     // nvgLineTo(vg, handle_center_x - handle_height / 2, handle_top_y + handle_height); // Bottom-left vertex
//     // nvgLineTo(vg, handle_center_x + handle_height / 2, handle_top_y + handle_height); // Bottom-right vertex
//     // nvgClosePath(vg);
//     nvgBeginPath(vg);
//     nvgMoveTo(vg, handle_center_x, handle_top_y);     // Top of the line
//     nvgLineTo(vg, handle_center_x, handle_bottom_y);  // Bottom of the line
//     nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255)); // Line color (white)
//     nvgStrokeWidth(vg, 2.0f);                        // Line thickness
//     nvgStroke(vg);
//
//     nvgFillColor(vg, nvgRGBA(255, 255, 255, 255)); // Handle color (white)
//     nvgFill(vg);
//
//     // draw_line(vg, rect.x, handle_top_y, rect.x + rect.w, handle_top_y, nvgRGBA(255, 255, 255, 255));
//
//     // if (slider_handle_widget.hovering)
//     // {
//     //     ui.draw_rect(rect, {255, 255, 255, 100});
//     // }
//
//     // Draw the formatted string showing the slider value
//     ui.draw_formatted_string({rect.x, rect.y}, {255, 255, 255, 255}, "%i", *slider_value);
//
//     return (slider_handle_widget.dragging);
// }

// lazy copy paste todo clean
bool ui_fslider_rect(charm::Rect rect, float *slider_value, float min, float max)
{
    UICore& ui = *get_ui_state();
    // Create the slider handle widget
    WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

    ui.draw_box(rect, theme.border_color);

    // Parameters for the handle size
    float handle_width = 16;
    float handle_height = rect.h;

    // Calculate the normalized value and handle position
    float value_normalized = (*slider_value - min) / static_cast<float>(max - min);
    float handle_position = static_cast<int>(value_normalized * rect.w);

    if (slider_handle_widget.dragging)
    {
        // Calculate mouse position relative to the slider rect
        float relative_mouse_pos = ui.mouse_pos.x - rect.x;

        // Clamp the mouse position so handle stays fully inside slider
        relative_mouse_pos = std::max(handle_width / 2, std::min(relative_mouse_pos, rect.w - handle_width / 2));

        // Normalize the position to [0, 1] range, considering handle width
        value_normalized = (relative_mouse_pos - handle_width / 2) / static_cast<float>(rect.w - handle_width);
        *slider_value = value_normalized * (max - min) + min;
    }

    // Calculate handle position again with clamping for drawing
    handle_position = (value_normalized * (rect.w - handle_width)) + handle_width / 2;

    // Draw the handle rectangle centered at handle_position
    float handle_x = rect.x + handle_position - handle_width / 2;
    float handle_y = rect.y;
    charm::Rect handle_rect = {handle_x, handle_y, handle_width, handle_height};
    ui.draw_rect(handle_rect, theme.slider_handle_color);  // white handle

    if (slider_handle_widget.hovering)
    {
        ui.draw_rect(rect, {255, 255, 255, 100});
    }

//     Draw the formatted string showing the slider value
//     ui.draw_formatted_string({rect.x, rect.y}, {255, 255, 255, 100}, "%f", *slider_value);

    return (slider_handle_widget.dragging);
}

// // int ui_fslider_rect(charm::Rect rect, float *slider_value, float min, float max)
// // {
// //     // Create the slider handle widget
// //     WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

// //     ui.draw_box(rect, {100, 100, 100, 255});

// //     // Calculate the relative position of the slider handle
// //     float value_normalized = (*slider_value - min) / static_cast<float>(max - min);
// //     int handle_position = static_cast<int>(value_normalized * rect.w);

// //     if (slider_handle_widget.dragging)
// //     {
// //         // Calculate the new handle position based on mouse position
// //         int relative_mouse_pos = ui.mouse_pos.x - rect.x;

// //         // Clamp the handle position within the slider bounds
// //         relative_mouse_pos = std::max(0, std::min(relative_mouse_pos, rect.w));

// //         // Convert the relative position to the actual slider value
// //         value_normalized = relative_mouse_pos / static_cast<float>(rect.w);
// //         *slider_value = value_normalized * (max - min) + min;

// //         // printf("Dragging: slider_value %f\n", *slider_value);
// //     }

// //     // Calculate the progress rectangle based on the normalized slider value
// //     charm::Rect progress_rect = {rect.x, rect.y, handle_position, rect.h};

// //     // Draw the progress rectangle
// //     // ui.draw_rect(progress_rect, {100, 255, 100, 255});

// //     // Draw the triangular slider handle using NanoVG
// //     int handle_height = rect.h / 2; // Height of the triangle
// //     float handle_center_x = rect.x + handle_position; // X position of the triangle's center
// //     float handle_top_y = rect.y + handle_height; // Y position of the top of the slider

// //     nvgBeginPath(vg);
// //     nvgMoveTo(vg, handle_center_x, handle_top_y);                          // Top vertex of the triangle (point)
// //     nvgLineTo(vg, handle_center_x - handle_height / 2, handle_top_y + handle_height); // Bottom-left vertex
// //     nvgLineTo(vg, handle_center_x + handle_height / 2, handle_top_y + handle_height); // Bottom-right vertex
// //     nvgClosePath(vg);


// //     nvgFillColor(vg, nvgRGBA(255, 255, 255, 255)); // Handle color (white)
// //     nvgFill(vg);

// //     draw_line(vg, rect.x, handle_top_y, rect.x + rect.w, handle_top_y, nvgRGBA(255, 255, 255, 255));

// //     if (slider_handle_widget.hovering)
// //     {
// //         ui.draw_rect(rect, {255, 255, 255, 100});
// //     }

// //     // Draw the formatted string showing the slider value
// //     ui.draw_formatted_string({rect.x, rect.y}, {255, 255, 255, 255}, "%f", *slider_value);

// //     return (slider_handle_widget.dragging);
// // }

int ui_vfslider_rect(charm::Rect *rect, float *slider_value, float min, float max)
{
    UICore& ui = *get_ui_state();
    // Create the slider handle widget
    WidgetComm slider_handle_widget = ui.build_widget(*rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);
    
    // Draw the slider background
    ui.draw_box(*rect, theme.border_color);
    
    // Calculate the relative position of the slider handle
    float value_normalized = (*slider_value - min) / (max - min);
    // For vertical slider, we use height instead of width, and invert the value
    float slider_handle_size = 16;
    float handle_position = ((1.0f - value_normalized) * (rect->h - slider_handle_size));

    if (slider_handle_widget.dragging)
    {
        // Calculate the new handle position based on mouse position
        float relative_mouse_pos = ui.mouse_pos.y - rect->y;
        // Clamp the handle position within the slider bounds
        relative_mouse_pos = std::max(0.0f, std::min(relative_mouse_pos, rect->h - slider_handle_size));
        // Convert the relative position to the actual slider value
        // Invert the calculation since we want bottom to be max and top to be min
        value_normalized = 1.0f - (relative_mouse_pos / static_cast<float>(rect->h - slider_handle_size));
        *slider_value = value_normalized * (max - min) + min;
        // printf("Dragging: slider_value %f\n", *slider_value);
    }

    // Calculate the handle rectangle
    charm::Rect handle_rect = {
        rect->x,
        rect->y + handle_position,
        rect->w,
        slider_handle_size
    };
    
    ui.draw_rect(handle_rect, theme.slider_handle_color);
    
    if (slider_handle_widget.hovering)
    {
        ui.draw_rect(*rect, {255, 255, 255, 100});
    }
    
    // Draw the formatted string showing the slider value
    // ui.draw_formatted_string({rect->x, rect->y}, {255, 255, 255, 255}, "%.3f", *slider_value);
       
    // // Draw the formatted string in the middle of the slider
    // // Calculate middle position
    // charm::Vec2 text_pos = {
    //     rect->x,  // x position stays the same
    //     rect->y + (rect->h / 2)  // y position is halfway down the slider
    // };
    // ui.draw_formatted_string(text_pos, {255, 255, 255, 255}, "%i", int(*slider_value));
    

    return (slider_handle_widget.dragging);
}

WidgetComm ui_button_rect_impl(charm::Rect rect)
{
    UICore& ui = *get_ui_state();
    WidgetComm widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation);
    
    ui.draw_rect(rect, theme.button_color);
    ui.draw_box(rect, theme.border_color);
    
    if(widget.hovering)
    {
        ui.draw_rect(rect, theme.hot_color);
    }

    return widget;
}

bool ui_button_rect(charm::Rect rect)
{
    return ui_button_rect_impl(rect).clicked;
}

 bool ui_checkbox_rect(charm::Rect rect, bool *value)
 {
     UICore& ui = *get_ui_state();

     WidgetComm widget = ui_button_rect_impl(rect);

     rect = shrink_rect(rect,6);

     if(widget.clicked)
     {
         *value = !*value;
         // printf("toggle %i\n", *value);
     }
     if(*value)
     {
         // ui.draw_rect(rect, theme.button_color);

         // Draw the "X"
         charm::Vec2 top_left = {rect.x, rect.y};
         charm::Vec2 bottom_right = {rect.x + rect.w, rect.y + rect.h};
         charm::Vec2 top_right = {rect.x + rect.w, rect.y};
         charm::Vec2 bottom_left = {rect.x, rect.y + rect.h};

         ui.draw_line(top_left, bottom_right, theme.border_color);
         ui.draw_line(top_right, bottom_left, theme.border_color);
     }

     // ui.draw_box(rect, theme.border_color);

     return widget.clicked;
 }


 bool ui_slider_rect2(charm::Rect rect, int *slider_value, int min, int max)
 {
     UICore &ui = *get_ui_state();
     // Create the slider handle widget
     static float accumulated_delta = 0.0f; // Accumulate the delta as a float

     WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

     // Draw the slider background
     ui.draw_box(rect, {100, 100, 100, 255});

     // Handle dragging
     if (slider_handle_widget.dragging)
     {
         // Calculate the value change based on the drag delta
         float value_range = max - min;
         float pixel_to_value_ratio = value_range / rect.w;

         // Accumulate the delta
         accumulated_delta += (slider_handle_widget.drag_delta_frame.y*-1) * pixel_to_value_ratio;

         // Apply the accumulated delta when it reaches at least 1 or -1
         int value_delta = static_cast<int>(accumulated_delta);
         if (value_delta != 0)
         {
             *slider_value += value_delta;
             accumulated_delta -= value_delta; // Remove the applied delta from the accumulator

             // Clamp the slider value within the min and max range
             *slider_value = std::max(min, std::min(*slider_value, max));

             // printf("Dragging: slider_value %i, delta %i\n", *slider_value, value_delta);
         }
     }
     else
     {
         // Reset the accumulator when not dragging
         accumulated_delta = 0.0f;
     }

     // Calculate the relative position of the progress rectangle
     float value_normalized = (*slider_value - min) / static_cast<float>(max - min);
     float progress_width = static_cast<int>(value_normalized * rect.w);
     charm::Rect progress_rect = {rect.x, rect.y, progress_width, rect.h};

     // Draw the progress rectangle
     ui.draw_rect(progress_rect, {255, 0, 255, 100});

     // // Draw the formatted string showing the slider value
     // ui.draw_formatted_string({rect.x + rect.w/2, rect.y}, {255, 255, 255, 255}, "%i", *slider_value);

     // if(slider_handle_widget.hovering)
     // {
     //     ui.draw_rect(rect, {255, 255, 255, 100});
     // }

     // ui_labeled_rect(rect, "%i",*slider_value);

     return slider_handle_widget.dragging;
 }

// #include <nanovg.h>
// #include <cmath> // For std::sin and std::cos

// bool ui_knob_slider(charm::Rect rect, float *slider_value, float min, float max)
// {
//     float accumulated_delta = 0.0f; // Accumulate the delta as a float

//     WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

//     // Draw the slider track
//     ui.draw_box(rect, theme.border_color);

//     // Handle dragging
//     if (slider_handle_widget.dragging)
//     {
//         // Calculate the value change based on the drag delta
//         float value_range = max - min;
//         float pixel_to_value_ratio = value_range / (rect.w*2);

//         // Accumulate the delta
//         accumulated_delta += (slider_handle_widget.drag_delta_frame.y*-1) * pixel_to_value_ratio;

//         // Apply the accumulated delta when it reaches at least 1 or -1
//         float value_delta = accumulated_delta;
//         if (value_delta != 0)
//         {
//             *slider_value += value_delta;
//             accumulated_delta -= value_delta; // Remove the applied delta from the accumulator

//             // Clamp the slider value within the min and max range
//             *slider_value = std::max(min, std::min(*slider_value, max));
//         }
//     }
//     else
//     {
//         // Reset the accumulator when not dragging
//         accumulated_delta = 0.0f;
//     }

//     if (slider_handle_widget.hovering)
//     {
//         ui.draw_rect(rect, {100, 100, 100, 100});
//     }

//     if (slider_handle_widget.active)
//     {
//         ui.draw_rect(rect, {100, 100, 100, 150});
//     }

//     // Calculate knob position
//     float value_normalized = (*slider_value - min) / (max - min);
//     float start_angle = M_PI / 2;
//     float end_angle = start_angle + value_normalized * 2 * M_PI; // Full rotation

//     // Calculate knob center
//     float knob_center_x = rect.x + rect.w / 2;
//     float knob_center_y = rect.y + rect.h / 2;
//     float knob_radius = std::min(rect.w, rect.h) / 2;
//     float inner_radius = knob_radius - 1.5f; // Adjust this to control the size of the cut-out

//     // Draw the filled arc representing the current value
//     nvgBeginPath(vg);
//     nvgArc(vg, knob_center_x, knob_center_y, knob_radius, start_angle, end_angle, NVG_CW);
//     // nvgArc(vg, knob_center_x, knob_center_y, knob_radius, M_PI / 2, 2 * M_PI + M_PI /2, NVG_CW);
//     nvgLineTo(vg, knob_center_x, knob_center_y); // Close the path to create a filled shape
    
//     // // nvgArc(vg, knob_center_x, knob_center_y, knob_radius - 15.0, M_PI / 2, 2 * M_PI + M_PI /2, NVG_CW);
//     // nvgRect(vg, knob_center_x-16, knob_center_y-16, 32, 32);
//     // nvgPathWinding(vg, NVG_HOLE); // Set winding direction to subtract shapes

//     nvgFillColor(vg, nvgRGBA(255, 255, 255, 100));
//     nvgFill(vg);

//     // nvgBeginPath(vg);
//     // nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
//     // nvgFill(vg);
//     // // printf("%f %f\n", start_angle, end_angle);

//     // Draw the current value as text in the center
//     ui_labeled_rect(rect, "%f", *slider_value);

//     return slider_handle_widget.dragging;
// }

// // bool ui_knob_slider(charm::Rect rect, int *slider_value, int min, int max)
// // {
// //     static float accumulated_delta = 0.0f; // Accumulate the delta as a float

// //     WidgetComm slider_handle_widget = ui.build_widget(rect, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

// //     // Draw the slider track
// //     ui.draw_box(rect, {100, 255, 100, 255});

// //     // Handle dragging
// //     if (slider_handle_widget.dragging)
// //     {
// //         // Calculate the value change based on the drag delta
// //         float value_range = max - min;
// //         float pixel_to_value_ratio = value_range / rect.w;

// //         // Accumulate the delta
// //         accumulated_delta += (slider_handle_widget.drag_delta_frame.y) * pixel_to_value_ratio;

// //         // Apply the accumulated delta when it reaches at least 1 or -1
// //         int value_delta = static_cast<int>(accumulated_delta);
// //         if (value_delta != 0)
// //         {
// //             *slider_value += value_delta;
// //             accumulated_delta -= value_delta; // Remove the applied delta from the accumulator

// //             // Clamp the slider value within the min and max range
// //             *slider_value = std::max(min, std::min(*slider_value, max));
// //         }
// //     }
// //     else
// //     {
// //         // Reset the accumulator when not dragging
// //         accumulated_delta = 0.0f;
// //     }

// //     if(slider_handle_widget.hovering)
// //     {
// //         ui.draw_rect(rect,{100,100,100,100});
// //     }

// //     if(slider_handle_widget.active)
// //     {
// //         ui.draw_rect(rect,{100,100,100,150});
// //     }

// //     // Calculate knob position
// //     float value_normalized = (*slider_value - min) / static_cast<float>(max - min);
// //     float angle = value_normalized * 2 * M_PI; // Angle in radians
// //     float knob_radius = 10.0f; // Radius of the knob
// //     float knob_center_x = rect.x + rect.w / 2;
// //     float knob_center_y = rect.y + rect.h / 2;
// //     float knob_x = knob_center_x + (rect.w / 2 - knob_radius) * std::cos(angle);
// //     float knob_y = knob_center_y + (rect.h / 2 - knob_radius) * std::sin(angle);

// //     // Draw the knob using NanoVG
// //     nvgBeginPath(vg);
// //     nvgCircle(vg, knob_x, knob_y, knob_radius);
// //     nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
// //     nvgFill(vg);

// //     ui_labeled_rect(rect, "%i", *slider_value);

// //     return slider_handle_widget.dragging;
// // }

// bool ui_combo_rect(charm::Rect *rect, std::vector<std::string> options, int* selected, bool& opened)
// {
//     WidgetComm widget = ui_button_rect_impl(*rect);

//     // close combo if clicked somewhere else
//     if (opened && ui.mouse_clicked) {
//         charm::Rect expanded_rect = *rect;
//         expanded_rect.h += options.size() * 32; // Assuming each option is 32px high
    
//         if (!ui.point_in_rect(expanded_rect, ui.mouse_pos)) {
//             opened = false; // Close the combo box if clicked outside
//         }
//     }
    
//     if(widget.clicked)
//     {
//         opened = !opened;
//         // ui.active = widget.id;
//         // printf("clicked %i\n", opened);
//     }

//     ui_labeled_rect(*rect, options[*selected].c_str());

//     if(opened)
//     {
//         for(int i = 0; i < options.size(); i++)
//         {
//             charm::Rect option_rect = *rect;
//             option_rect.y += 32 * i + 32; // plus extra space for current selected box

//             if(ui_button_rect(option_rect)){
//                 opened = false;
//                 *selected = i;
//                return true;
//             }
//             ui_labeled_rect(option_rect, options[i].c_str());
            
//         }
//     }

//     return false;
// }


// static charm::Rect scrollbar_vert_rect = {0,0,0,0};
// static charm::Rect scrollbar_horz_rect = {0,0,0,0};
// // charm::Rect last_scroll_area = {0,0,0,0};
// static std::vector<ScrollArea> scroll_stack;

// static std::map<std::string, float> panel_overflow_vert_prev;
// static std::map<std::string, float> panel_overflow_horz_prev;

void begin_scroll_area(float *scroll_x, float *scroll_y, charm::Rect* area, std::string panel_name)
{
    // printf("panel begin %s\n", panel_name.c_str());
    // ui.draw_box(*area, theme.border_color);

    UICore &ui = *get_ui_state();
    // bool& overflow_vert = ui.overflow_horz;
    // bool& overflow_horz = ui.overflow_vert;

    ui.scroll_stack.push_back({area, *area, scroll_x, scroll_y, panel_name});

    ui.in_scroll_area = ui.region_hit(*area);
    ui.active_scroll_area = *area;

    // printf("hello%i\n",show_slider);
    // printf("scroll max vert %i\n", scrollmax_vert);
    // printf("at panel %s\n", panel_name.c_str());
    // printf("at begin scrollmax vert %i\n", scrollmax_vert);
    
    if(scroll_y)
    {
        // ScrollArea& scroll_rect = scroll_stack.back();
        // if(scrollmax_vert>0)
        // {
        //     scrollbar_vert_rect = rectcut_cut(rectcut(area, RectCut_Right), 18);
        // }
        // printf("scroll yyy\n");
        // ScrollArea& scroll_rect = scroll_stack.back();
        // scroll_rect.overflow_vert = true;
        ui.overflow_vert = true;
        // charm::Rect a = *area;

        // printf("at begin scrollmax vert %i\n", scrollmax_vert);
        

        // im having a hard time managing state. bascially want to cut out a space the vertical scroolbar but need to know if its overflowing. which i can check for the negative area but first i need to compute the layout so this needs to be one frame behind. i also have mulitple begin and ends so i can just use one glo
        // printf("begin '%s' map %i area h %i\n ", panel_name.c_str(), panel_overflow_vert_prev[panel_name], area->h);
        if(ui.panel_overflow_vert_prev[panel_name] < 0)
        {
            // printf("cut!\n");
            ui.scrollbar_vert_rect = rectcut_cut(rectcut(area, RectCut_Right), 18);
        }
        // scrollbar_vert_rect = get_right(area, 18);
    }

    if(scroll_x)
    {
        ui.overflow_horz = true;
        // charm::Rect a = *area;

        // scrollbar_horz_rect = rectcut_cut(rectcut(area, RectCut_Bottom), 18);
        // scrollbar_horz_rect = get_bottom(area, 18);

        if(ui.panel_overflow_horz_prev[panel_name] < 0)
        {
            // printf("cut!\n");
            ui.scrollbar_horz_rect = rectcut_cut(rectcut(area, RectCut_Bottom), 18);
        }
    }
    // charm::Rect resize_handle = add_right(&scrollbar_vert_rect, 18);
    ui.renderer.render_clip(ui.renderer.userdata, area);

    charm::Rect rect_copy = ui.scroll_stack.back().rect_copy;
    
    float lerp_overflow_factor = 0.3;
    // clamp before adding scroll, causes jitterness
    // want to add lerp here but still causes kinda jitteryness so just clamp for now
    if (scroll_x)
    {
        if (ui.region_hit(rect_copy))
        {
            *scroll_x -= ui.scroll_delta.x;
        }

        float scroll_max = 0.0f;
        if (ui.panel_overflow_horz_prev[panel_name] < 0) {
            scroll_max = -ui.panel_overflow_horz_prev[panel_name];
        }

        *scroll_x = std::clamp(*scroll_x, 0.0f, scroll_max);
//        float target_scroll = std::clamp(*scroll_x, 0.0f, scroll_max);
//        *scroll_x = lerp(*scroll_x, target_scroll, lerp_overflow_factor);
        
        area->x += *scroll_x * -1;
    }
    
    if (scroll_y)
    {
        if (ui.region_hit(rect_copy))
        {
            *scroll_y -= ui.scroll_delta.y;
        }

        // Clamp BEFORE applying scroll to area
        float scroll_max = 0.0f;
        if (ui.panel_overflow_vert_prev[panel_name] < 0) {
            scroll_max = -ui.panel_overflow_vert_prev[panel_name];
        }

        *scroll_y = std::clamp(*scroll_y, 0.0f, scroll_max);
//        float target_scroll = std::clamp(*scroll_y, 0.0f, scroll_max);
//        *scroll_y = lerp(*scroll_y, target_scroll, lerp_overflow_factor);
        
        area->y += *scroll_y * -1;
    }


    // printf("area %i\n", area->h);
    // print_rect(*area);
    // scrollmax_vert = 0;
}

void handle_scroll_axis(
    float* scroll_value,
    float content_size,
    std::map<std::string, float>& overflow_map,
    const std::string& panel_name,
    charm::Rect& scrollbar_rect,
    bool is_vertical)
{
    if (!scroll_value) return;

    float scroll_max = 0.0;
    float lerp_factor = 0.5;

    if (content_size < 0) {
        scroll_max = -content_size;
//        float target_scroll = std::clamp(*scroll_value, 0.0f, scroll_max);
    //    printf("is_vertical %i scroll_max %f\n", is_vertical, scroll_max);
//        *scroll_value = lerp(*scroll_value, target_scroll, lerp_factor);
//        *scroll_value = target_scroll;

        if (is_vertical) {
            ui_vfslider_rect(&scrollbar_rect, scroll_value, scroll_max, 0);
        } else {
            ui_fslider_rect(scrollbar_rect, scroll_value, 0, scroll_max);
        }
    }

    overflow_map[panel_name] = content_size;
}

void end_scroll_area() {

    UICore &ui = *get_ui_state();
    ScrollArea& scroll_area = ui.scroll_stack.back();
    charm::Rect *area = scroll_area.rect;
    float *scroll_x = scroll_area.scroll_x;
    float *scroll_y = scroll_area.scroll_y;
    // printf("panel end %s\n", scroll_area.panel_name.c_str());

    // scroll_stack.pop_back();

    // if(scroll_area.overflow_vert)
    // // if(overflow_vert)
    // {
    //     printf("true!?\n");
    //     print_rect(*area);
    //     // scrollbar_vert_rect = rectcut_cut(rectcut(area, RectCut_Right), 18);
    // }

    ui.renderer.render_clip(ui.renderer.userdata, NULL);
    float lerp_factor = 0.5f; // Adjust this value to control the speed of the lerp

    if (scroll_y) {
        handle_scroll_axis(
            scroll_y,
            area->h,
            ui.panel_overflow_vert_prev,
            scroll_area.panel_name,
            ui.scrollbar_vert_rect,
            true
        );
    }

    if (scroll_x) {
        handle_scroll_axis(
            scroll_x,
            area->w,
            ui.panel_overflow_horz_prev,
            scroll_area.panel_name,
            ui.scrollbar_horz_rect,
            false
        );
    }
//    if(scroll_y)
//    {
//         float scrollmax_vert = 0;
//
//
//        if(area->h < 0)
//        {
//            scrollmax_vert = area->h * -1;
//
//            // Calculate the target scroll position
//            float target_scroll = std::min(std::max(*scroll_y, 0.0f), scrollmax_vert);
//            *scroll_y = lerp(*scroll_y, target_scroll, lerp_factor);
//
//            ui_vfslider_rect(&scrollbar_vert_rect, scroll_y, scrollmax_vert, 0);
//
//        } else {
//            // has_overflown_vert = false;
//            // If there's no scrolling needed, lerp back to 0
////            float lerp_factor = 0.1f;
//            *scroll_y = lerp(*scroll_y, 0.0f, lerp_factor);
//        }
//
//        panel_overflow_vert_prev[scroll_area.panel_name] = area->h;
//    }
//
//    if(scroll_x)
//    {
//        // printf("area->w %i\n", area->w);
//        // print_rect(*area);
//        float scrollmax_horz = 0;
//
//        if(area->w < 0)
//        {
//            scrollmax_horz = (area->w * -1);
//            // Calculate the target scroll position
//            float target_scroll = std::min(std::max(*scroll_x, 0.0f), scrollmax_horz);
//            // Lerp the scroll position
//            *scroll_x = lerp(*scroll_x, target_scroll, lerp_factor);
//            ui_fslider_rect(scrollbar_horz_rect, scroll_x, 0, scrollmax_horz);
//
//        } else {
////             *scroll_x = *scroll_x + (0 - *scroll_x) * lerp_factor;
//            *scroll_x = lerp(*scroll_x, 0.0f, lerp_factor);
//
//        }
//
//        panel_overflow_horz_prev[scroll_area.panel_name] = area->w;
//    }

    // if(ui.in_scroll_area)
    // {
    //     // charm::Rect title_bar_rect = cut_top(area, 16);
    //     ui.draw_string(scroll_area.panel_name.c_str(), {scroll_area.rect->x, scroll_area.rect->y}, theme.text_color);
    // }

    if(ui.region_hit(scroll_area.rect_copy))
//     if(ui.in_scroll_area)
    {
    // charm::Rect title_bar_rect = cut_top(area, 16);
        // scroll_area.rect_copy
        charm::Rect bottom_panel_flash = get_bottom(&scroll_area.rect_copy, ui.get_text_height(scroll_area.panel_name.c_str()));
        // charm::Rect bottom_panel_flash = get_bottom(&scroll_area.rect_copy, 16.0);
        ui.draw_string(scroll_area.panel_name.c_str(), {bottom_panel_flash.x, bottom_panel_flash.y}, theme.text_color);
        // ui.draw_rect(scroll_area.rect_copy,{255,0,255,100});
    }

    // finally draw border at end
    // dont use "area" pointer cuz its been so chopped up at this point
    ui.draw_box(scroll_area.rect_copy, theme.border_color);

    ui.active_scroll_area = {0, 0, 0, 0};
    ui.in_scroll_area = false;

    // bool& overflow_vert = *get_overflow_vert();
    // bool& overflow_horz = *get_overflow_horz();
    //NULL turns off render clip
    // ui.renderer.render_clip(ui.renderer.userdata, NULL);
    ui.overflow_vert = false;
    ui.overflow_horz = false;
//    ui.scroll_delta = 0;
    ui.scroll_stack.pop_back();
}

charm::Rect resizable_panel(RectCut layout, int* panel_size) {
    // static bool is_resizing = false;
    // static int last_mouse_pos = 0;

    // not needed 
    // if (*hide_sidepanel) {
    //     return charm::Rect{0, 0, 0, 0}; // Return empty rect if panel is hidden
    // }

    charm::Rect panel_left = rectcut_cut(layout, *panel_size);

    // Print debug info
    // printf("cuttype %i panel_size %i\n", layout.side, *panel_size);

    // Create resize handle
    charm::Rect resize_handle = rectcut_cut(layout, 8);

    // UIId id = ui.next_id();

    // Draw the resize handle
    UICore &ui = *get_ui_state();
    ui.draw_rect(resize_handle, {100, 100, 100, 255});

    WidgetComm resize_handle_widget = ui.build_widget(resize_handle, WidgetFlag_Dragging | WidgetFlag_HotAnimation);

    // quick hack, i wrote this before the widget com system but it still works? gonna probably have to come back
    UIId id = resize_handle_widget.id;

    if (ui.region_hit(resize_handle)) {
        ui.next_hover = id;
        // ui.draw_rect(resize_handle, {100, 0, 100, 255});

        if (!ui.is_resizing) {
            ui.last_mouse_pos = ((layout.side == RectCut_Left) || (layout.side == RectCut_Right)) ? ui.mouse_pos.x : ui.mouse_pos.y;
            ui.is_resizing = true;
            // ui.draw_rect(resize_handle, {180, 180, 180, 255});
            ui.draw_rect(resize_handle, {180, 180, 180, 255});
        }
    }

    if (ui.active == id && ui.is_resizing) {
        ui.next_hover = id;

        int current_mouse_pos = ((layout.side == RectCut_Left) || (layout.side == RectCut_Right)) ? ui.mouse_pos.x : ui.mouse_pos.y;
        int delta = current_mouse_pos - ui.last_mouse_pos;

        if (layout.side == RectCut_Left) {
            *panel_size = std::max(*panel_size + delta, 100); // Minimum width of 100
        } else if (layout.side == (RectCut_Bottom)) {
            *panel_size = std::max(*panel_size - delta, 100); // Minimum height of 100
        } else if (layout.side == (RectCut_Top)) {
            *panel_size = std::max(*panel_size + delta, 100); // Minimum height of 100
        } else if (layout.side == (RectCut_Right)) {
            *panel_size = std::max(*panel_size - delta, 100); // Minimum width of 100
        }

        ui.last_mouse_pos = current_mouse_pos;
    }

    if (!ui.mouse_down) {
        ui.is_resizing = false;
    }

    return panel_left;
}

bool rc_button(RectCut layout, const char* label)
{
    UICore& ui = *get_ui_state();
    float size = ui.get_text_width(label) + (theme.padding*2);
    float text_height = ui.get_text_height(label);
    // charm::Rect rect = cut_left(layout, size);
    charm::Rect rect = rectcut_cut(layout, size);
    
    bool clicked = ui_button_rect(rect);
    ui.draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
    return clicked;
}

// bool rc_combo(RectCut layout, std::vector<std::string> options, int* selected, bool& opened)
// {
//     static std::vector<std::string> option_strings = {"option 1", "option 2", "wowwww"};
//     // static int selected = 0;
//     // static bool is_open = 0;

//     const char* label = options[*selected].c_str();

//     float size = ui.get_text_width(label) + (theme.padding*2);
//     float text_height = ui.get_text_height(label);
//     // charm::Rect rect = cut_left(layout, size);
//     charm::Rect rect = rectcut_cut(layout, size);
    
//     // bool clicked = ui_button_rect(rect);
//     bool clicked = ui_combo_rect(&rect, options, selected, opened);
//     // ui.draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
//     return clicked;
// }

// // void rc_label(RectCut layout, const char* label)
// // {
// //     float size = ui.get_text_width(label) + (theme.padding*2);
// //     float text_height = ui.get_text_height(label);
// //     charm::Rect rect = rectcut_cut(layout, size);
// //     ui.draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
// // }

#include <cstdarg> // For va_list, va_start, va_end
#include <cstdio>  // For vsnprintf

void rc_label(RectCut layout, const char* format, ...) {
    UICore &ui = *get_ui_state();
    // Define a buffer to hold the formatted text
    char formatted_label[256]; // Adjust size as needed
    va_list args;

    // Initialize the variable argument list
    va_start(args, format);

    // Format the string
    vsnprintf(formatted_label, sizeof(formatted_label), format, args);

    // Clean up the variable argument list
    va_end(args);

    // Calculate the full text width
    float text_width = ui.get_text_width(formatted_label);
    float text_height = ui.get_text_height(formatted_label);
    
    // Get a rectangle from the layout
    charm::Rect rect = rectcut_cut(layout, text_width + (theme.padding * 2));

    // Check if the text fits
    if (text_width > rect.w - (theme.padding * 2)) {
        // Truncate and add "..."
        const char* ellipsis = "...";
        float ellipsis_width = ui.get_text_width(ellipsis);

        // Start from the full string and reduce it
        int len = strlen(formatted_label);
        while (len > 0) {
            // Create a temporary truncated version
            char temp[256];
            strncpy(temp, formatted_label, len);
            temp[len] = '\0';  // Ensure null termination
            strncat(temp, ellipsis, sizeof(temp) - len - 1);

            // Check if it fits
            if (ui.get_text_width(temp) <= rect.w - (theme.padding * 2)) {
                // Draw truncated string
                ui.draw_string(temp, 
                               {rect.x + theme.padding, 
                                rect.y + int(rect.h / 2) - int(text_height / 2)}, 
                               theme.text_color);
                return;
            }

            // Shorten the string
            len--;
        }

        // If everything fails, just draw "..."
        ui.draw_string(ellipsis, 
                       {rect.x + theme.padding, 
                        rect.y + int(rect.h / 2) - int(text_height / 2)}, 
                       theme.text_color);
    } else {
        // Use the full string
        ui.draw_string(formatted_label, 
                       {rect.x + theme.padding, 
                        rect.y + int(rect.h / 2) - int(text_height / 2)}, 
                       theme.text_color);
    }
}

void draw_oscilloscope(const charm::Rect& rect,
                       const float* audio_data,
                       size_t sample_count,
                       NVGcontext* vg)
{
    if (!audio_data || sample_count == 0 || rect.w <= 1) return;

    NVGcolor waveform_color = nvgRGB(0, 255, 0); // Green
    float rect_width  = rect.w;
    float rect_height = rect.h;
    float rect_left   = rect.x;
    float rect_top    = rect.y;
    float center_y    = rect_top + rect_height * 0.5f;

    nvgBeginPath(vg);
    nvgMoveTo(vg, rect_left, center_y);

    float y_scale = rect_height * 0.5f;

    // Plot every sample (or skip if too many)
    size_t step_size = std::max<size_t>(1, sample_count / (size_t)rect_width);

    for (size_t i = 0; i < sample_count; i += step_size)
    {
        // Map sample index → [0, rect_width]
        float x = rect_left + (float)i / (float)(sample_count - 1) * rect_width;
        float y = center_y - audio_data[i] * y_scale;
        nvgLineTo(vg, x, y);
    }

    nvgStrokeColor(vg, waveform_color);
    nvgStrokeWidth(vg, 1.5f);
    nvgStroke(vg);
}

#include <algorithm> // std::clamp
#include <cmath>     // sqrtf

// Helper: HSV -> NVGcolor
static NVGcolor hsv_to_rgb(float h, float s, float v)
{
    float r, g, b;

    int i = int(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return nvgRGBf(r, g, b);
}

// Heatmap-style waveform
float draw_waveform_heatmap(const charm::Rect& rect, const float* audio_data,
                           size_t sample_count, NVGcontext* vg)
{
    if (!audio_data || sample_count == 0) return 0;

    float rect_width  = rect.w;
    float rect_height = rect.h;
    float rect_left   = rect.x;
    float rect_top    = rect.y;

    size_t pixels = (size_t)rect_width;
    size_t samples_per_pixel = sample_count / pixels;
    if (samples_per_pixel == 0) samples_per_pixel = 1;

    // Static for continuity between frames
    static float smooth_rms = 0.0f;
    float intensity = 0;

    for (size_t px = 0; px < pixels; ++px)
    {
        // === Compute RMS for this column ===
        float sum = 0.0f;
        for (size_t s = 0; s < samples_per_pixel; ++s) {
            size_t idx = px * samples_per_pixel + s;
            if (idx >= sample_count) break;
            float v = audio_data[idx];
            sum += v * v;
        }
        float rms = sqrtf(sum / (float)samples_per_pixel);

        // === Smooth (EMA) ===
        const float alpha = 0.25f; // smaller = smoother, 0.25 works well
        smooth_rms = alpha * rms + (1.0f - alpha) * smooth_rms;

        // Normalize intensity
        intensity = std::clamp(smooth_rms * 4.0f, 0.0f, 1.0f);

        // === Continuous HSV gradient (Green → Yellow → Red) ===
        // hue 120° (green) -> 0° (red)
        float hue = (1.0f - intensity) * (120.0f / 360.0f); 
        NVGcolor color = hsv_to_rgb(hue, 1.0f, 1.0f);

        // === Bar height proportional to RMS ===
        float bar_height = rms * rect_height;
        float x = rect_left + px;
        float y = rect_top + rect_height/2 - bar_height/2;

        // Draw filled vertical bar
        nvgBeginPath(vg);
        nvgRect(vg, x, y, 1.0f, bar_height);
        nvgFillColor(vg, color);
        nvgFill(vg);
    }

    return intensity;
}


// void draw_waveform(const charm::Rect& rect, const float* audio_data, size_t sample_count, NVGcontext* vg)
// {
//     if (!audio_data || sample_count == 0) return;

//     float rect_width  = rect.w;
//     float rect_height = rect.h;
//     float rect_left   = rect.x;
//     float rect_top    = rect.y;

//     size_t pixels = (size_t)rect_width;
//     size_t samples_per_pixel = sample_count / pixels;
//     if (samples_per_pixel == 0) samples_per_pixel = 1;

//     for (size_t px = 0; px < pixels; ++px)
//     {
//         // === Compute RMS (average amplitude) over this block ===
//         float sum = 0.0f;
//         for (size_t s = 0; s < samples_per_pixel; ++s)
//         {
//             size_t idx = px * samples_per_pixel + s;
//             if (idx >= sample_count) break;
//             float v = audio_data[idx];
//             sum += v * v;
//         }
//         float rms = sqrtf(sum / (float)samples_per_pixel);

//         // Normalize 0..1 for color mapping
//         float intensity = std::clamp(rms * 4.0f, 0.0f, 1.0f); // scale factor to taste

//         // === Map intensity to color (green → yellow → red) ===
//         NVGcolor color;
//         if (intensity < 0.5f) {
//             // green → yellow
//             float t = intensity / 0.5f;
//             color = nvgRGBf(t, 1.0f, 0.0f);
//         } else {
//             // yellow → red
//             float t = (intensity - 0.5f) / 0.5f;
//             color = nvgRGBf(1.0f, 1.0f - t, 0.0f);
//         }

//         // === Bar height proportional to RMS ===
//         float bar_height = rms * rect_height;
//         float x = rect_left + px;
//         float y = rect_top + rect_height/2 - bar_height/2;

//         // Draw filled vertical bar
//         nvgBeginPath(vg);
//         nvgRect(vg, x, y, 1.0f, bar_height);
//         nvgFillColor(vg, color);
//         nvgFill(vg);
//     }
// }

void draw_waveform(const charm::Rect& rect, const float* audio_data, size_t sample_count, NVGcontext* vg)
{
    if (!audio_data || sample_count == 0) return;

    NVGcolor waveform_color = nvgRGB(0, 255, 0);
    float rect_width  = rect.w;
    float rect_height = rect.h;
    float rect_left   = rect.x;
    float rect_top    = rect.y;

    nvgBeginPath(vg);

    size_t pixels = (size_t)rect_width;
    size_t samples_per_pixel = sample_count / pixels;
    if (samples_per_pixel == 0) samples_per_pixel = 1;

    for (size_t px = 0; px < pixels; ++px)
    {
        float min_val = 1.0f;
        float max_val = -1.0f;

        // Find min/max in this block of samples
        for (size_t s = 0; s < samples_per_pixel; ++s)
        {
            size_t idx = px * samples_per_pixel + s;
            if (idx >= sample_count) break;
            float v = audio_data[idx];
            if (v < min_val) min_val = v;
            if (v > max_val) max_val = v;
        }

        float x = rect_left + px;
        float y1 = rect_top + rect_height / 2 - max_val * rect_height / 2;
        float y2 = rect_top + rect_height / 2 - min_val * rect_height / 2;

        // Draw vertical line for min/max
        nvgMoveTo(vg, x, y1);
        nvgLineTo(vg, x, y2);
    }

    nvgStrokeColor(vg, waveform_color);
    nvgStrokeWidth(vg, 1.0f);
    nvgStroke(vg);
}

#include <algorithm>
#include <cmath>

// Vertical VU Meter (Ableton-style)
void draw_vu_meter(const charm::Rect& rect, float level, NVGcontext* vg)
{
    // Clamp level 0.0 → 1.0
    level = std::clamp(level, 0.0f, 1.0f);

    float rect_left   = rect.x;
    float rect_top    = rect.y;
    float rect_width  = rect.w;
    float rect_height = rect.h;

    // Height of filled bar
    float filled_height = rect_height * level;

    // === Background ===
    nvgBeginPath(vg);
    nvgRect(vg, rect_left, rect_top, rect_width, rect_height);
    nvgFillColor(vg, nvgRGB(30, 30, 30)); // dark grey
    nvgFill(vg);

    // === Foreground bar ===
    float y = rect_top + rect_height - filled_height;

    // Choose color based on level (green → yellow → red)
    float hue = (1.0f - level) * (120.0f / 360.0f); // 120° = green → 0° = red
    NVGcolor color = hsv_to_rgb(hue, 1.0f, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, rect_left, y, rect_width, filled_height);
    nvgFillColor(vg, color);
    nvgFill(vg);

    // === Border ===
    nvgBeginPath(vg);
    nvgRect(vg, rect_left, rect_top, rect_width, rect_height);
    nvgStrokeColor(vg, nvgRGB(200, 200, 200));
    nvgStrokeWidth(vg, 1.0f);
    nvgStroke(vg);
}


void draw_lissajous(const charm::Rect& rect, const float* audio_data_x, const float* audio_data_y, size_t sample_count, NVGcontext* vg) {
    // Ensure there is valid audio data for both channels
    if (!audio_data_x || !audio_data_y || sample_count == 0) return;

    // Define visual properties
    NVGcolor pattern_color = nvgRGB(0, 255, 0); // Green pattern color
    float rect_width = rect.w;
    float rect_height = rect.h;
    float rect_left = rect.x;
    float rect_top = rect.y;

    // Scale audio samples to fit the rectangle dimensions
    float x_scale = rect_width / 2.0f;  // X-axis scaling for float -1.0 to 1.0 range
    float y_scale = rect_height / 2.0f; // Y-axis scaling for float -1.0 to 1.0 range
    float center_x = rect_left + rect_width / 2.0f; // Center of the rectangle (X-axis)
    float center_y = rect_top + rect_height / 2.0f; // Center of the rectangle (Y-axis)

    // Begin a path for the Lissajous pattern
    nvgBeginPath(vg);

    // Move to the starting point
    float x = center_x + audio_data_x[0] * x_scale;
    float y = center_y - audio_data_y[0] * y_scale;
    nvgMoveTo(vg, x, y);

    // Plot the Lissajous pattern
    size_t step_size = 1; // Adjust step size for performance or detail
    for (size_t i = 1; i < sample_count; i += step_size) {
        x = center_x + audio_data_x[i] * x_scale;
        y = center_y - audio_data_y[i] * y_scale;
        nvgLineTo(vg, x, y);
    }

    // End the path and apply the stroke
    nvgStrokeColor(vg, pattern_color);
    nvgStrokeWidth(vg, 1.5f);
    nvgStroke(vg);
}

#include <cmath>
#include <vector>
#include <complex>
#include <fftw3.h>

// void compute_fft_magnitude(const float* input, float* output, size_t sample_count) {
//     if ((sample_count & (sample_count - 1)) != 0) {
//         // Sample count must be a power of two
//         return;
//     }

//     using Complex = std::complex<float>;
//     std::vector<Complex> buffer(sample_count);

//     // Copy input into complex buffer
//     for (size_t i = 0; i < sample_count; ++i) {
//         buffer[i] = Complex(input[i], 0.0f);
//     }

//     // Bit reversal
//     size_t n = sample_count;
//     size_t j = 0;
//     for (size_t i = 0; i < n; ++i) {
//         if (i < j) std::swap(buffer[i], buffer[j]);
//         size_t bit = n >> 1;
//         while (j & bit) {
//             j ^= bit;
//             bit >>= 1;
//         }
//         j ^= bit;
//     }

//     // Cooley-Tukey
//     for (size_t len = 2; len <= n; len <<= 1) {
//         float angle = -2.0f * M_PI / len;
//         Complex wlen(cosf(angle), sinf(angle));

//         for (size_t i = 0; i < n; i += len) {
//             Complex w(1);
//             for (size_t j = 0; j < len / 2; ++j) {
//                 Complex u = buffer[i + j];
//                 Complex v = buffer[i + j + len / 2] * w;
//                 buffer[i + j] = u + v;
//                 buffer[i + j + len / 2] = u - v;
//                 w *= wlen;
//             }
//         }
//     }

//     // Output magnitudes (only first N/2 are useful for real FFT)
//     for (size_t i = 0; i < n / 2; ++i) {
//         output[i] = std::abs(buffer[i]) / (float)n; // Normalize magnitude
//     }
// }

// void draw_fft_visualizer(const charm::Rect& rect, const float* audio_data, size_t sample_count, NVGcontext* vg) {
//     if (!audio_data || sample_count == 0) return;

//     std::vector<float> fft_magnitudes(sample_count / 2);
//     // compute_fft_magnitude(audio_data, fft_magnitudes.data(), sample_count);

//     float rect_width = rect.w;
//     float rect_height = rect.h;
//     float rect_left = rect.x;
//     float rect_top = rect.y;

//     size_t bin_count = fft_magnitudes.size();

//     float log_min = std::log10f(1.0f);
//     float log_max = std::log10f((float)bin_count);

//     // === Draw Heat Below Line ===
//     for (size_t i = 1; i < bin_count; ++i) {
//         float magnitude = fft_magnitudes[i];
//         magnitude = 20.0f * log10f(magnitude + 1e-6f);
//         magnitude = (magnitude + 60.0f) / 60.0f;
//         magnitude = std::clamp(magnitude, 0.0f, 1.0f);

//         float log_index = std::log10f((float)i);
//         float t = (log_index - log_min) / (log_max - log_min);
//         float x = rect_left + t * rect_width;
//         float y = rect_top + rect_height - magnitude * rect_height;

//         // Draw heat column
//         NVGpaint heat_paint = nvgLinearGradient(vg,
//             x, y,
//             x, rect_top + rect_height,
//             nvgRGBAf(1.0f, 0.4f, 0.0f, magnitude * 0.6f), // Top (orange/red, semi-transparent)
//             nvgRGBAf(0.0f, 0.0f, 0.0f, 0.0f));           // Bottom (transparent)

//         float column_width = 2.0f;
//         nvgBeginPath(vg);
//         nvgRect(vg, x - column_width / 2.0f, y, column_width, rect_top + rect_height - y);
//         nvgFillPaint(vg, heat_paint);
//         nvgFill(vg);
//     }

//     // === Draw FFT Line on Top ===
//     NVGcolor spectrum_color = nvgRGB(0, 150, 255); // Cyan
//     nvgBeginPath(vg);
//     for (size_t i = 1; i < bin_count; ++i) {
//         float magnitude = fft_magnitudes[i];
//         magnitude = 20.0f * log10f(magnitude + 1e-6f);
//         magnitude = (magnitude + 60.0f) / 60.0f;
//         magnitude = std::clamp(magnitude, 0.0f, 1.0f);

//         float log_index = std::log10f((float)i);
//         float t = (log_index - log_min) / (log_max - log_min);
//         float x = rect_left + t * rect_width;
//         float y = rect_top + rect_height - magnitude * rect_height;

//         if (i == 1) nvgMoveTo(vg, x, y);
//         else nvgLineTo(vg, x, y);
//     }
//     nvgStrokeColor(vg, spectrum_color);
//     nvgStrokeWidth(vg, 2.0f);
//     nvgStroke(vg);
// }


void ui_color_picker(charm::Rect* panel, Color* color, const char* label)
{
    const char* labels[] = { "R", "G", "B", "A" };
    unsigned char* channels[] = { &color->r, &color->g, &color->b, &color->a };

    UICore &ui = *get_ui_state();

    // Label for the color picker
    charm::Rect new_line = cut_top(panel, 32);
    rc_label(rectcut(&new_line, RectCut_Left), label);

    for (int i = 0; i < 4; i++)
    {
        new_line = cut_top(panel, 32);

        // Small color preview square
        charm::Rect color_square = cut_left(&new_line, 32);
        ui.draw_rect(color_square, *color);

        // Compute perceived brightness
        float brightness = 0.299f * color->r + 0.587f * color->g + 0.114f * color->b;
        
        // Choose black or white for contrast
        Color text_color = (brightness < 128) ? Color{255, 255, 255, 255} : Color{0, 0, 0, 255};

        ui.draw_string(labels[i], { color_square.x, color_square.y }, text_color);

        // Slider for the current color channel
        float slider_value = *channels[i];  // Copy current channel value
        ui_fslider_rect(new_line, &slider_value, 0, 255);
        *channels[i] = static_cast<unsigned char>(slider_value);  // Store back the modified value
    }
}

// // void ui_text_input(charm::Rect rect)
// // {
// //     WidgetComm widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation);
// //     if(widget.clicked)
// //     {
// //         printf("ui text input");
// //     }
// // }
// // #include <algorithm> // for std::max

// // void rc_text_input(RectCut layout, char* buffer)
// // {
// //     int len = strlen(buffer);
// //     float size = ui.get_text_width(buffer) + (theme.padding*2);
// //     // size = std::max(size, 32.0f);
// //     float text_height = ui.get_text_height(buffer);
// //     charm::Rect rect = rectcut_cut(layout, size);
// //     ui.draw_box(rect, theme.border_color);

// //     WidgetComm widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_TextInput);
// //     // if(widget.clicked)
// //     // {
// //     //     printf("ui text input\n");
// //     // }

// //     if(ui.kbditem == widget.id)
// //     {
// //         // printf("keyentered %i\n",widget.keyentered);

// //         switch (ui.keyentered)
// //         {
// //             case GLFW_KEY_BACKSPACE:
// //                 printf("backspace\n");
// //                 if (len > 0)
// //                 {
// //                     len--;
// //                     buffer[len] = 0;
// //                     // changed = 1;
// //                 }
// //                 ui.keyentered = 0;
// //             break;
// //         }

// //         if (ui.keyentered >= 32 && ui.keyentered < 127)
// //         {
// //             printf("typing %c\n", ui.keychar);
// //             // typine
// //             // result.keychar = keychar;
// //             // keyentered = 0;
// //             buffer[len] = ui.keychar;
// //             len++;
// //             buffer[len] = 0;
// //             ui.keyentered = 0;
// //         }

// //     }

// //     ui.draw_string(buffer, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);

// // }

// WidgetComm ui_text_input(charm::Rect rect, char* buffer, int* cursor_pos, WidgetFlags extra_flags)
// {
//     UICore &ui = *get_ui_state();
//
//     int len = strlen(buffer);
//     float text_height = ui.get_text_height("A");
//     float available_width = rect.w - (theme.padding * 2);
//     float ellipsis_width = ui.get_text_width("...");
//
//     WidgetComm widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_TextInput | extra_flags);
//
//     if (ui.kbditem == widget.id)
//     {
//
////         printf("last widget %i widget id %i\n", ui.lastwidget, widget.id);
//
// ////         Set cursor to end on first focus
// // works for file input but not module widgets?????????????
// //        printf("st kbditem %i widget id %i\n", ui.last_kbditem, widget.id);
//
//         // If clicked inside this widget, update cursor position based on mouse x
//         if (ui.mouse_clicked)
//         {
//             float click_x = ui.mouse_pos.x;
//             float start_x = rect.x + (theme.padding);
//
//             int new_cursor = 0;
//             float current_width = start_x;
//
//             for (int i = 0; i < len; i++)
//             {
//                 char temp = buffer[i + 1];
//                 buffer[i + 1] = 0;
//                 float char_right = start_x + ui.get_text_width(buffer);
//                 buffer[i + 1] = temp;
//
//                 if (click_x < char_right)
//                 {
//                     new_cursor = i;
//                     break;
//                 }
//                 new_cursor = i + 1;
//             }
//
//             *cursor_pos = new_cursor;
//         }
//
//         if (ui.last_kbditem != widget.id)
//         {
//             printf("initial focus, last kbditem %i widget id %i\n", ui.last_kbditem, widget.id);
//             *cursor_pos = len;
//         }
//
//         switch (ui.keyentered)
//         {
//             case CHARM_KEY_BACKSPACE:
//             printf("cursor%i\n",*cursor_pos);
//                 if (*cursor_pos > 0)
//                 {
//
//                     memmove(buffer + *cursor_pos - 1, buffer + *cursor_pos, len - *cursor_pos + 1);
//                     (*cursor_pos)--;
//                 }
//                 ui.keyentered = 0;
//                 break;
//
//             case CHARM_KEY_LEFT:
//                 if (*cursor_pos > 0) (*cursor_pos)--;
//                 ui.keyentered = 0;
//                 break;
//
//             case CHARM_KEY_RIGHT:
//                 if (*cursor_pos < len) (*cursor_pos)++;
//                 ui.keyentered = 0;
//                 break;
//         }
//
//         if (ui.keyentered >= 32 && ui.keyentered < 127)
////         if (ui.keyentered)
//         {
//             if (len < 255) // assuming buffer is at least 256 bytes
//             {
//                 memmove(buffer + *cursor_pos + 1, buffer + *cursor_pos, len - *cursor_pos + 1);
//                 buffer[*cursor_pos] = ui.keychar;
////                 printf("buf%c keychar%c\n", buffer[*cursor_pos], ui.keychar);
//                 (*cursor_pos)++;
//             }
//             ui.keyentered = 0;
//         }
//     }
//
//     // Check if text fits in available space
//     float full_text_width = ui.get_text_width(buffer);
//     bool needs_ellipsis = full_text_width > available_width;
//
//     char display_buffer[256];
//     const char* display_text = buffer;
//
//     // ai slop edit blahhh blah blahhhhhh
//     if (needs_ellipsis)
//     {
//         // Find how much text we can fit with ellipsis
//         int fit_chars = 0;
//         float current_width = 0;
//
//         for (int i = 0; i < len; i++)
//         {
//             char temp = buffer[i + 1];
//             buffer[i + 1] = 0;
//             float test_width = ui.get_text_width(buffer) + ellipsis_width;
//             buffer[i + 1] = temp;
//
//             if (test_width > available_width)
//                 break;
//
//             fit_chars = i + 1;
//         }
//
//         // Create display string with ellipsis
//         if (fit_chars > 0)
//         {
//             strncpy(display_buffer, buffer, fit_chars);
//             strcpy(display_buffer + fit_chars, "...");
//             display_text = display_buffer;
//         }
//         else
//         {
//             // If even one character doesn't fit, just show ellipsis
//             strcpy(display_buffer, "...");
//             display_text = display_buffer;
//         }
//     }
//
//     // Calculate cursor position for display
//     float cursor_x = rect.x + theme.padding;
//     if (*cursor_pos > 0 && !needs_ellipsis)
//     {
//         char temp = buffer[*cursor_pos];
//         buffer[*cursor_pos] = 0;
//         cursor_x += ui.get_text_width(buffer);
//         buffer[*cursor_pos] = temp;
//     }
//     else if (*cursor_pos > 0 && needs_ellipsis)
//     {
//         // For ellipsis case, position cursor at end of visible text
//         cursor_x += ui.get_text_width(display_text);
//     }
//
//     // Draw text (either full text or truncated with ellipsis)
//     ui.draw_string(display_text, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
//
//     // Draw cursor if focused
//     if (ui.kbditem == widget.id)
//     {
//         float cursor_top = rect.y + (rect.h - text_height) / 2;
//         float t = 0.5f * (1.0f + sinf(ui.time * 2.0f * 3.14159f)); // 1Hz sine wave
//
//         Color cursor_color = {
//             (unsigned char)(theme.text_color.r + (theme.border_color.r - theme.text_color.r) * t),
//             (unsigned char)(theme.text_color.g + (theme.border_color.g - theme.text_color.g) * t),
//             (unsigned char)(theme.text_color.b + (theme.border_color.b - theme.text_color.b) * t),
//             (unsigned char)(theme.text_color.a + (theme.border_color.a - theme.text_color.a) * t),
//         };
//
//         ui.draw_line(
//             {(cursor_x), (cursor_top)},
//             {(cursor_x), (cursor_top + text_height)},
//             cursor_color
//         );
//     }
//
//     return widget;
// }

void ui_draw_text_clipped(charm::Rect rect, const char* buffer, Color color)
{
    UICore &ui = *get_ui_state();

    float text_height = ui.get_text_height("A");
    float available_width = rect.w - (theme.padding * 2);
    float ellipsis_width = ui.get_text_width("...");

    int len = strlen(buffer);

    // If full text fits, draw it normally
    float full_text_width = ui.get_text_width(buffer);
    if (full_text_width <= available_width)
    {
        ui.draw_string(
            buffer,
            {rect.x + theme.padding, rect.y + int(rect.h / 2) - int(text_height / 2)},
            color
        );
        return;
    }

    // Otherwise, find how many characters can fit before ellipsis
    char display_buffer[256];
    int fit_chars = 0;

    for (int i = 0; i < len; i++)
    {
        char temp[256];
        strncpy(temp, buffer, i + 1);
        temp[i + 1] = '\0';

        float test_width = ui.get_text_width(temp) + ellipsis_width;
        if (test_width > available_width)
            break;

        fit_chars = i + 1;
    }

    if (fit_chars > 0)
    {
        strncpy(display_buffer, buffer, fit_chars);
        display_buffer[fit_chars] = '\0';
        strcat(display_buffer, "...");
    }
    else
    {
        strcpy(display_buffer, "...");
    }

    // Draw clipped text
    ui.draw_string(
        display_buffer,
        {rect.x + theme.padding, rect.y + int(rect.h / 2) - int(text_height / 2)},
        color
    );
}


WidgetComm ui_text_input(charm::Rect rect, char* buffer, int* cursor_pos, WidgetFlags extra_flags)
{
    UICore &ui = *get_ui_state();

//    ui.renderer.render_clip(ui.renderer.userdata, &rect);

    int len = strlen(buffer);
    float text_height = ui.get_text_height("A");

    WidgetComm widget = ui.build_widget(
        rect,
        WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_TextInput | extra_flags
    );

    if (ui.kbditem == widget.id)
    {
        // Handle mouse click → update cursor position
        if (ui.mouse_clicked)
        {
            float click_x = ui.mouse_pos.x;
            float start_x = rect.x + theme.padding;

            int new_cursor = 0;
            for (int i = 0; i < len; i++)
            {
                char temp = buffer[i + 1];
                buffer[i + 1] = 0;
                float char_right = start_x + ui.get_text_width(buffer);
                buffer[i + 1] = temp;

                if (click_x < char_right)
                {
                    new_cursor = i;
                    break;
                }
                new_cursor = i + 1;
            }
            *cursor_pos = new_cursor;
        }
        
        if(widget.double_clicked)
        {
            printf("selected all text\n");
        }

        // If widget just got focus, set cursor at end
        if (ui.last_kbditem != widget.id)
            *cursor_pos = len;

        // Handle key input
        switch (ui.keyentered)
        {
            case CHARM_KEY_BACKSPACE:
                if (*cursor_pos > 0)
                {
                    memmove(buffer + *cursor_pos - 1, buffer + *cursor_pos, len - *cursor_pos + 1);
                    (*cursor_pos)--;
                }
                ui.keyentered = 0;
                break;

            case CHARM_KEY_LEFT:
                if (*cursor_pos > 0) (*cursor_pos)--;
                ui.keyentered = 0;
                break;

            case CHARM_KEY_RIGHT:
                if (*cursor_pos < len) (*cursor_pos)++;
                ui.keyentered = 0;
                break;
        }

        // Handle printable characters
        if (ui.keyentered >= 32 && ui.keyentered < 127)
        {
            if (len < 255) // assuming buffer is at least 256 bytes
            {
                memmove(buffer + *cursor_pos + 1, buffer + *cursor_pos, len - *cursor_pos + 1);
                buffer[*cursor_pos] = ui.keychar;
                (*cursor_pos)++;
            }
            ui.keyentered = 0;
        }
    }

    // --- Scrolling logic ---
    float available_width = rect.w - theme.padding * 2;
    float full_text_width = ui.get_text_width(buffer);

    // cursor_x = position of cursor in unshifted text
    float cursor_x = rect.x + theme.padding;
    if (*cursor_pos > 0)
    {
        char temp = buffer[*cursor_pos];
        buffer[*cursor_pos] = 0;
        cursor_x += ui.get_text_width(buffer);
        buffer[*cursor_pos] = temp;
    }

    // If text is too wide, shift so the end is visible
    float shift_x = 0.0f;
    if (full_text_width > available_width)
    {
        shift_x = full_text_width - available_width;
    }

    // Draw shifted text
    ui.draw_string(
        buffer,
        {rect.x + theme.padding - shift_x, rect.y + int(rect.h / 2) - int(text_height / 2)},
        theme.text_color
    );

    // Draw cursor if focused
    if (ui.kbditem == widget.id)
    {
        float cursor_top = rect.y + (rect.h - text_height) / 2;
        float t = 0.5f * (1.0f + sinf(ui.time * 2.0f * 3.14159f)); // 1Hz sine wave

        Color cursor_color = {
            (unsigned char)(theme.text_color.r + (theme.border_color.r - theme.text_color.r) * t),
            (unsigned char)(theme.text_color.g + (theme.border_color.g - theme.text_color.g) * t),
            (unsigned char)(theme.text_color.b + (theme.border_color.b - theme.text_color.b) * t),
            (unsigned char)(theme.text_color.a + (theme.border_color.a - theme.text_color.a) * t),
        };

        // Apply same shift to cursor
        ui.draw_line(
            {cursor_x - shift_x, cursor_top},
            {cursor_x - shift_x, cursor_top + text_height},
            cursor_color
        );
    }
    
//    ui.renderer.render_clip(ui.renderer.userdata, NULL);

    return widget;
}

// copy paste working
WidgetComm ui_text_input2(charm::Rect rect, TextInputState* state, CharmApp* charm_app, WidgetFlags extra_flags)
{
    UICore &ui = *get_ui_state();
    ui.renderer.render_clip(ui.renderer.userdata, &rect);

    int len = strlen(state->buffer);
    float text_height = ui.get_text_height("A");

    WidgetComm widget = ui.build_widget(
        rect,
        WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_TextInput | WidgetFlag_Dragging | extra_flags
    );

    auto delete_selection = [&]() {
        if (state->selection_start != -1 && state->selection_start != state->selection_end)
        {
            int sel_min = std::min(state->selection_start, state->selection_end);
            int sel_max = std::max(state->selection_start, state->selection_end);
            memmove(state->buffer + sel_min, state->buffer + sel_max, len - sel_max + 1);
            state->cursor_pos = sel_min;
            state->selection_start = state->selection_end = -1;
            len = strlen(state->buffer);
            return true;
        }
        return false;
    };

    if (ui.kbditem == widget.id)
    {
        // --- Mouse click handling ---
        if (ui.mouse_clicked)
        {
            float click_x = ui.mouse_pos.x;
            int clicked_pos = 0;
            for (int i = 0; i <= len; i++)
            {
                char temp = state->buffer[i];
                state->buffer[i] = 0;
                float char_right = rect.x + theme.padding + ui.get_text_width(state->buffer);
                state->buffer[i] = temp;

                if (click_x < char_right)
                {
                    clicked_pos = i;
                    break;
                }
                clicked_pos = i;
            }

            state->cursor_pos = clicked_pos;
            state->selection_start = clicked_pos;
            state->selection_end = clicked_pos;
        }

        // Double-click selects all
        if (widget.double_clicked)
        {
            state->selection_start = 0;
            state->selection_end = len;
            state->cursor_pos = len;
        }

        // Drag selection
        else if (widget.dragging && widget.drag_delta.x != 0)
        {
            float drag_x = ui.mouse_pos.x;
            int drag_pos = 0;
            for (int i = 0; i <= len; i++)
            {
                char temp = state->buffer[i];
                state->buffer[i] = 0;
                float char_right = rect.x + theme.padding + ui.get_text_width(state->buffer);
                state->buffer[i] = temp;

                if (drag_x < char_right)
                {
                    drag_pos = i;
                    break;
                }
                drag_pos = i;
            }

            state->cursor_pos = drag_pos;
            state->selection_end = drag_pos;
        }

        // Focus handling
        if (ui.last_kbditem != widget.id)
        {
            state->cursor_pos = len;
            state->selection_start = state->selection_end = -1;
        }

            // --- Copy (Command/Ctrl + C) ---

        if ((ui.keymod & CHARM_KEYMOD_SUPER) && (ui.keyentered == 'C')) {
            if (state->selection_start != -1 && state->selection_start != state->selection_end) {
                int sel_min = std::min(state->selection_start, state->selection_end);
                int sel_max = std::max(state->selection_start, state->selection_end);
                char temp = state->buffer[sel_max];
                state->buffer[sel_max] = 0;
                charm_app->set_clipboard_string(std::string(state->buffer + sel_min));
                state->buffer[sel_max] = temp;
            } else {
                charm_app->set_clipboard_string(std::string(state->buffer));
            }
            ui.keyentered = 0; // reset key state to prevent further processing
            return widget;
        }
            // --- Paste (Command/Ctrl + V) ---
        if ((ui.keymod & CHARM_KEYMOD_SUPER) && (ui.keyentered == 'V')) {
            const char* clipboard = charm_app->get_clipboard_cstr();
            if (clipboard) {
                std::string paste_text = clipboard;
                delete_selection();
                if (len + paste_text.length() < sizeof(state->buffer) - 1) {
                    memmove(state->buffer + state->cursor_pos + paste_text.length(), state->buffer + state->cursor_pos, len - state->cursor_pos + 1);
                    memcpy(state->buffer + state->cursor_pos, paste_text.c_str(), paste_text.length());
                    state->cursor_pos += paste_text.length();
                    len += paste_text.length();
                }
            }
            ui.keyentered = 0;  // reset key state to prevent further processing
            return widget;
        }


        // --- Key input ---
        switch (ui.keyentered)
        {
            case CHARM_KEY_BACKSPACE:
                if (!delete_selection() && state->cursor_pos > 0)
                {
                    memmove(state->buffer + state->cursor_pos - 1, state->buffer + state->cursor_pos, len - state->cursor_pos + 1);
                    state->cursor_pos--;
                }
                ui.keyentered = 0;
                break;

            case CHARM_KEY_LEFT:
                if (state->cursor_pos > 0) state->cursor_pos--;
                state->selection_start = state->selection_end = -1;
                ui.keyentered = 0;
                break;

            case CHARM_KEY_RIGHT:
                if (state->cursor_pos < len) state->cursor_pos++;
                state->selection_start = state->selection_end = -1;
                ui.keyentered = 0;
                break;
        }

        // Printable characters
        if (ui.keyentered >= 32 && ui.keyentered < 127)
        {
            delete_selection();
            if (len < sizeof(state->buffer) - 1)
            {
                memmove(state->buffer + state->cursor_pos + 1, state->buffer + state->cursor_pos, len - state->cursor_pos + 1);
                state->buffer[state->cursor_pos] = ui.keychar;
                state->cursor_pos++;
            }
            ui.keyentered = 0;
        }

        // --- Draw selection highlight ---
        if (state->selection_start != -1 && state->selection_start != state->selection_end)
        {
            int sel_min = std::min(state->selection_start, state->selection_end);
            int sel_max = std::max(state->selection_start, state->selection_end);

            float sel_start_x = 0.0f;
            if (sel_min > 0)
            {
                char temp = state->buffer[sel_min];
                state->buffer[sel_min] = 0;
                sel_start_x = ui.get_text_width(state->buffer);
                state->buffer[sel_min] = temp;
            }

            float sel_end_x = 0.0f;
            if (sel_max > 0)
            {
                char temp = state->buffer[sel_max];
                state->buffer[sel_max] = 0;
                sel_end_x = ui.get_text_width(state->buffer);
                state->buffer[sel_max] = temp;
            }

            charm::Rect highlight = {
                rect.x + theme.padding + sel_start_x,
                rect.y + (rect.h - text_height) / 2,
                sel_end_x - sel_start_x,
                text_height
            };

            ui.draw_rect(highlight, {100, 100, 100, 255});
        }
    }

    // --- Draw text ---
    ui.draw_string(
        state->buffer,
        {rect.x + theme.padding, rect.y + (rect.h - text_height) / 2},
        theme.text_color
    );

    // --- Draw cursor ---
    if (ui.kbditem == widget.id)
    {
        float cursor_draw_x = rect.x + theme.padding;
        if (state->cursor_pos > 0)
        {
            char temp = state->buffer[state->cursor_pos];
            state->buffer[state->cursor_pos] = 0;
            cursor_draw_x += ui.get_text_width(state->buffer);
            state->buffer[state->cursor_pos] = temp;
        }

        float cursor_top = rect.y + (rect.h - text_height) / 2;
        float t = 0.5f * (1.0f + sinf(ui.time * 2.0f * 3.14159f));
        Color cursor_color = {
            (unsigned char)(theme.text_color.r + (theme.border_color.r - theme.text_color.r) * t),
            (unsigned char)(theme.text_color.g + (theme.border_color.g - theme.text_color.g) * t),
            (unsigned char)(theme.text_color.b + (theme.border_color.b - theme.text_color.b) * t),
            (unsigned char)(theme.text_color.a + (theme.border_color.a - theme.text_color.a) * t),
        };

        ui.draw_line(
            {cursor_draw_x, cursor_top},
            {cursor_draw_x, cursor_top + text_height},
            cursor_color
        );
    }

    ui.renderer.render_clip(ui.renderer.userdata, NULL);
    return widget;
}

// easier to use text widget
// Handles editing/interaction only, not parsing
// Returns true if the user pressed Enter (submitted)
bool ui_text_box(charm::Rect rect, TextBoxState& state) {
    UICore &ui = *get_ui_state();

    if (!state.is_editing) {
        WidgetComm text_widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation);
        if (text_widget.clicked) {
            state.is_editing = true;
        }

        // Just draw the label
        ui_labeled_rect(rect, "%s", state.buffer);
        return false;
    } else {
        WidgetComm text_widget = ui_text_input(rect, state.buffer, &state.cursor_pos);
        if (text_widget.keyentered == CHARM_KEY_RETURN) {
            state.is_editing = false;
            return true; // "submitted"
        }
        return false;
    }
}

// struct CustomFunctionState
// {
//     std::vector<charm::Vec2> points;
// };

void ui_custom_function_eval(CustomFunctionState* state)
{
        // calculate playhead value
    if(state->points.size() >= 2)
    {
        // find the two points surrounding the playhead
        charm::Vec2 left_point  = {0.0f, 0.0f};
        charm::Vec2 right_point = {1.0f, 0.0f};

        for(int i = 0; i < state->points.size(); i++)
        {
            if(state->points[i].x <= state->playhead_pos)
            {
                left_point = state->points[i];
            }
            else
            {
                right_point = state->points[i];
                break;
            }
        }

        // interpolate between the two surrounding points
        float t = 0.0f;
        float dx = right_point.x - left_point.x;
        if(dx > 0.0001f) // avoid divide by zero
        {
            t = (state->playhead_pos - left_point.x) / dx;
        }

        state->playhead_function_value = lerp(left_point.y, right_point.y, t);
    }
    else if(state->points.size() == 1)
    {
        state->playhead_function_value = state->points[0].y;
    }
    else
    {
        state->playhead_function_value = 0.0f;
    }
}

WidgetComm ui_custom_function_widget(charm::Rect rect, CustomFunctionState* state)
{
    UICore& ui = *get_ui_state();

    charm::Rect new_line = cut_top(&rect, 32);
    // ui.draw_box(toggle_box, {255,0,255,255});
    // float old_padding = theme.padding;
    // theme.padding = 0;
    if(rc_button(rectcut(&new_line, RectCut_Left), "Delete Point"))
    {
        // remove vector at state->selected_point_idx if its not -1
        if(state->selected_point_idx != -1)
        {
            state->points.erase(state->points.begin() + state->selected_point_idx);
            state->selected_point_idx = -1;
        }

    }

    rc_label(rectcut(&new_line, RectCut_Left), "Snap");
    // theme.padding = old_padding;
    charm::Rect toggle_box = cut_left(&new_line, 32);
    ui_checkbox_rect(toggle_box, &state->snap_to_grid);

    rc_label(rectcut(&new_line, RectCut_Left), "Playhead Pos: %f", state->playhead_pos);
    rc_label(rectcut(&new_line, RectCut_Left), "Function Value: %f", state->playhead_function_value);
    rc_label(rectcut(&new_line, RectCut_Left), "Num Points: %i", state->points.size());

    new_line = cut_top(&rect, 32);
    rc_label(rectcut(&new_line, RectCut_Left), "Grid X");
    if(rc_button(rectcut(&new_line, RectCut_Left), "2"))
        state->num_grid_lines.x = 2;
    if(rc_button(rectcut(&new_line, RectCut_Left), "4"))
        state->num_grid_lines.x = 4;
    if(rc_button(rectcut(&new_line, RectCut_Left), "8"))
        state->num_grid_lines.x = 8;

    rc_label(rectcut(&new_line, RectCut_Left), "Grid Y");
    if(rc_button(rectcut(&new_line, RectCut_Left), "2"))
        state->num_grid_lines.y = 2;
    if(rc_button(rectcut(&new_line, RectCut_Left), "4"))
        state->num_grid_lines.y = 4;
    if(rc_button(rectcut(&new_line, RectCut_Left), "8"))
        state->num_grid_lines.y = 8;

    rc_label(rectcut(&new_line, RectCut_Left), "Bars");
    if(rc_button(rectcut(&new_line, RectCut_Left), "1/2"))
        state->phasor_clock->osc_time_bars = 0.5;
    if(rc_button(rectcut(&new_line, RectCut_Left), "1/3"))
        state->phasor_clock->osc_time_bars = 1.0 / 3.0;
    if(rc_button(rectcut(&new_line, RectCut_Left), "1"))
        state->phasor_clock->osc_time_bars = 1;
    if(rc_button(rectcut(&new_line, RectCut_Left), "1.5"))
        state->phasor_clock->osc_time_bars = 1.5;
    if(rc_button(rectcut(&new_line, RectCut_Left), "2"))
        state->phasor_clock->osc_time_bars = 2;
    if(rc_button(rectcut(&new_line, RectCut_Left), "3"))
        state->phasor_clock->osc_time_bars = 3;
    if(rc_button(rectcut(&new_line, RectCut_Left), "4"))
        state->phasor_clock->osc_time_bars = 4;
    if(rc_button(rectcut(&new_line, RectCut_Left), "8"))
        state->phasor_clock->osc_time_bars = 8;

    WidgetComm main_widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_TextInput);
    // bool snap_to_grid = ui_checkbox(toggle_box, "Snap to grid", state->snap
    
    // ui.draw_rect(rect, theme.button_color);
    ui.draw_box(rect, {255,0,255,255});

    // int num_grid_x = 4;
    // int num_grid_y = 2;
    // draw grid lines
    for(int i = 1; i < state->num_grid_lines.x; i++)
    {
        for(int j = 1; j < state->num_grid_lines.y; j++)
        {
            ui.draw_line(
                {rect.x + i * rect.w / state->num_grid_lines.x, rect.y},
                {rect.x + i * rect.w / state->num_grid_lines.x, rect.y + rect.h},
                {100,100,100,50}
            );
            ui.draw_line(
                {rect.x, rect.y + j * rect.h / state->num_grid_lines.y},
                {rect.x + rect.w, rect.y + j * rect.h / state->num_grid_lines.y},
                {100,100,100,50}
            );
        }
    }
    // charm::Vec2 mouse_pos = ui.mouse_pos;

    // printf("HEY!\n");

    
    if(main_widget.hovering)
    {
        // ui.draw_rect(rect, theme.hot_color);
        charm::Vec2 mouse_pos = ui.mouse_pos;
        charm::Vec2 relative_pos = {0.0, 0.0};
        relative_pos.x = ui.mouse_pos.x - rect.x;
        relative_pos.y = ui.mouse_pos.y - rect.y;

        // WidgetComm point_widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation);
        ui.draw_box({mouse_pos.x-8, mouse_pos.y-8, 16, 16}, {255,0,255,255});

        // printf("ui keymod state %i\n", ui.keyentered);
        // printf("ui mouse right held %i\n", ui.right_mouse_down);
        // printf("mouse pos %f %f\n", relative_pos.x, relative_pos.y);
        // if(main_widget.keyentered)
        // if ((ui.keymod & CHARM_KEYMOD_SUPER)) {
        //     printf("holding super\n");
        //     // ui.keyentered = 0; // consume the key event to prevent other processing
        // }
    }

    for(int i = 0; i < state->points.size(); i++)
    {
        charm::Vec2 normalized_point = state->points[i];
        charm::Vec2 projected_point = {
            rect.x + normalized_point.x * rect.w,
            rect.y + normalized_point.y * rect.h
        };

        
        charm::Rect point_hitbox = {projected_point.x-8, projected_point.y-8, 16, 16};
        WidgetComm point_widget = ui.build_widget(point_hitbox, WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_Dragging);

        if(point_widget.hovering)
        {
            ui.draw_rect(point_hitbox, {255,0,255,100});
        } else {
            ui.draw_rect(point_hitbox, {255,255,255,100});
        }

        if (ui.kbditem == point_widget.id)
        {
            state->selected_point_idx = i;
        }

        if(point_widget.dragging)
        {
            float normalized_drag_delta_frame_x = point_widget.drag_delta_frame.x / rect.w;
            float normalized_drag_delta_frame_y = point_widget.drag_delta_frame.y / rect.h;
            state->points[i].x += normalized_drag_delta_frame_x;
            state->points[i].y += normalized_drag_delta_frame_y;
        }

        if(point_widget.released)
        {
            printf("end of drag\n");
            // snap to grid if enabled
            if(state->snap_to_grid)
            {
                charm::Vec2 old_point = state->points[i];
                charm::Vec2 snapped_point = old_point;
                float grid_size_x = 1.0f / state->num_grid_lines.x;
                float grid_size_y = 1.0f / state->num_grid_lines.y;
                snapped_point.x = roundf(old_point.x / grid_size_x) * grid_size_x;
                snapped_point.y = roundf(old_point.y / grid_size_y) * grid_size_y;
                state->points[i] = snapped_point;
            }


        }
        
        // draw line to next point
        if(i < state->points.size() - 1)
        {
            charm::Vec2 next_normalized_point = state->points[i + 1];
            charm::Vec2 next_projected_point = {
                rect.x + next_normalized_point.x * rect.w,
                rect.y + next_normalized_point.y * rect.h
            };
             ui.draw_line(
                {projected_point.x, projected_point.y},
                {next_projected_point.x, next_projected_point.y},
                {255,255,255,255}
            );
        }
    }

    // // calculate playhead value
    // if(state->points.size() >= 2)
    // {
    //     // find the two points surrounding the playhead
    //     charm::Vec2 left_point  = {0.0f, 0.0f};
    //     charm::Vec2 right_point = {1.0f, 0.0f};

    //     for(int i = 0; i < state->points.size(); i++)
    //     {
    //         if(state->points[i].x <= state->playhead_pos)
    //         {
    //             left_point = state->points[i];
    //         }
    //         else
    //         {
    //             right_point = state->points[i];
    //             break;
    //         }
    //     }

    //     // interpolate between the two surrounding points
    //     float t = 0.0f;
    //     float dx = right_point.x - left_point.x;
    //     if(dx > 0.0001f) // avoid divide by zero
    //     {
    //         t = (state->playhead_pos - left_point.x) / dx;
    //     }

    //     state->playhead_function_value = lerp(left_point.y, right_point.y, t);
    // }
    // else if(state->points.size() == 1)
    // {
    //     state->playhead_function_value = state->points[0].y;
    // }
    // else
    // {
    //     state->playhead_function_value = 0.0f;
    // }

    // custom_function_eval(state);

    if(main_widget.clicked)
    {
        charm::Vec2 mouse_pos = ui.mouse_pos;
        charm::Vec2 relative_pos = {0.0, 0.0};
        relative_pos.x = ui.mouse_pos.x - rect.x;
        relative_pos.y = ui.mouse_pos.y - rect.y;
        charm::Vec2 normalized_pos = {0.0, 0.0}; // between 0 and 1
        normalized_pos.x = relative_pos.x / rect.w;
        normalized_pos.y = relative_pos.y / rect.h;
        printf("mouse pos %f %f\n", relative_pos.x, relative_pos.y);
        printf("normalized pos %f %f\n", normalized_pos.x, normalized_pos.y);
        // printf("ui mouse right held %i\n", ui.right_mouse_down);
        // printf("snap to grid %i\n", state->snap_to_grid);
        if(state->snap_to_grid) // snap to grid
        {
            printf("snapping to grid\n");
            float grid_size_x = rect.w / state->num_grid_lines.x;
            float grid_size_y = rect.h / state->num_grid_lines.y;
            relative_pos.x = roundf(relative_pos.x / grid_size_x) * grid_size_x;
            relative_pos.y = roundf(relative_pos.y / grid_size_y) * grid_size_y;
            normalized_pos.x = relative_pos.x / rect.w;
            normalized_pos.y = relative_pos.y / rect.h;
             printf("snapped relative pos %f %f\n", relative_pos.x, relative_pos.y);
             printf("snapped normalized pos %f %f\n", normalized_pos.x, normalized_pos.y);
        }
        state->points.push_back(normalized_pos);
    }

    // draw playhead line
    float playhead_x = rect.x + state->playhead_pos * rect.w;
    ui.draw_line(
        {playhead_x, rect.y},
        {playhead_x, rect.y + rect.h},
        {255,0,0,255}
    );

    //  calculate playhead value 

    return main_widget;
}

// // WidgetComm ui_text_input(charm::Rect rect, char* buffer, int* cursor_pos, WidgetFlags extra_flags = 0)
// // {

// //     int len = strlen(buffer);
// //     float text_height = ui.get_text_height("A");

// //     // Compute visible text width and layout
// //     // float text_width = ui.get_text_width(buffer);
// //     // float size = std::max(text_width + (theme.padding * 2), 32.0f);
// //     // float size = text_width + (theme.padding * 2);
// //     // charm::Rect rect = rectcut_cut(layout, size);
// //     // ui.draw_box(rect, theme.border_color);

// //     WidgetComm widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_TextInput | extra_flags);

// //     // // this probably be apart of build widget...
// //     // if (ui.mouse_clicked) {
// //     //     // charm::Rect expanded_rect = *rect;
// //     //     // expanded_rect.h += options.size() * 32; // Assuming each option is 32px high
    
// //     //     if (!ui.point_in_rect(rect, ui.mouse_pos)) {
// //     //         // opened = false; // Close the combo box if clicked outside
// //     //         ui.kbditem = -1;
// //     //     }
// //     // }

// //     if (ui.kbditem == widget.id)
// //     {
// //         switch (ui.keyentered)
// //         {
// //             case GLFW_KEY_BACKSPACE:
// //                 if (*cursor_pos > 0)
// //                 {
// //                     memmove(buffer + *cursor_pos - 1, buffer + *cursor_pos, len - *cursor_pos + 1);
// //                     (*cursor_pos)--;
// //                 }
// //                 ui.keyentered = 0;
// //                 break;

// //             case GLFW_KEY_LEFT:
// //                 if (*cursor_pos > 0) (*cursor_pos)--;
// //                 ui.keyentered = 0;
// //                 break;

// //             case GLFW_KEY_RIGHT:
// //                 if (*cursor_pos < len) (*cursor_pos)++;
// //                 ui.keyentered = 0;
// //                 break;
// //         }

// //         if (ui.keyentered >= 32 && ui.keyentered < 127)
// //         {
// //             if (len < 255) // assuming buffer is at least 256 bytes
// //             {
// //                 memmove(buffer + *cursor_pos + 1, buffer + *cursor_pos, len - *cursor_pos + 1);
// //                 buffer[*cursor_pos] = ui.keychar;
// //                 (*cursor_pos)++;
// //             }
// //             ui.keyentered = 0;
// //         }
// //     }

// //     // Draw text up to cursor
// //     float cursor_x = rect.x + theme.padding;
// //     if (*cursor_pos > 0)
// //     {
// //         char temp = buffer[*cursor_pos];
// //         buffer[*cursor_pos] = 0;
// //         cursor_x += ui.get_text_width(buffer);
// //         buffer[*cursor_pos] = temp;
// //     }

// //     // Draw text
// //     ui.draw_string(buffer, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);

// //     // Draw cursor if focused
// //     // if (ui.kbditem == widget.id)
// //     // {
// //     //     float cursor_top = rect.y + (rect.h - text_height) / 2;
// //     //     ui.draw_line({int(cursor_x), int(cursor_top)}, {int(cursor_x), int(cursor_top + text_height)}, theme.text_color);
// //     // }

// //     // Draw cursor if focused and blinking
// //     // if (ui.kbditem == widget.id)
// //     // {
// //     //     // Blink every 1 second: on for 0.5s, off for 0.5s
// //     //     bool show_cursor = fmod(ui.time, 1.0f) < 0.5f;
// //     //     if (show_cursor)
// //     //     {
// //     //         float cursor_top = rect.y + (rect.h - text_height) / 2;
// //     //         ui.draw_line(
// //     //             {int(cursor_x), int(cursor_top)},
// //     //             {int(cursor_x), int(cursor_top + text_height)},
// //     //             theme.text_color
// //     //         );
// //     //     }
// //     // }

// //     if (ui.kbditem == widget.id)
// //     {
// //         float cursor_top = rect.y + (rect.h - text_height) / 2;
// //         float t = 0.5f * (1.0f + sinf(ui.time * 2.0f * 3.14159f)); // 1Hz sine wave

// //         Color cursor_color = {
// //             (unsigned char)(theme.text_color.r + (theme.border_color.r - theme.text_color.r) * t),
// //             (unsigned char)(theme.text_color.g + (theme.border_color.g - theme.text_color.g) * t),
// //             (unsigned char)(theme.text_color.b + (theme.border_color.b - theme.text_color.b) * t),
// //             (unsigned char)(theme.text_color.a + (theme.border_color.a - theme.text_color.a) * t),
// //         };

// //         ui.draw_line(
// //             {int(cursor_x), int(cursor_top)},
// //             {int(cursor_x), int(cursor_top + text_height)},
// //             cursor_color
// //         );
// //     }

// //     // //too lazy to do highlighting so double click clears line
// //     // if(ui.double_click && (widget.id==ui.active))
// //     // {
// //     //     // printf("wasup\n");
// //     //     if(len>0)
// //     //     {
// //     //         for(int i=0; i<len; i++)
// //     //         {
// //     //             buffer[i] = 0;
// //     //             *cursor_pos = 0;
// //     //         }
// //     //     }
// //     // }

// //     return widget;
// // }

// void rc_text_input(RectCut layout, char* buffer, int* cursor_pos)
// {
//     // Compute visible text width and layout
//     float text_width = ui.get_text_width(buffer);
//     float size = std::max(text_width + (theme.padding * 2), 32.0f);
//     charm::Rect rect = rectcut_cut(layout, size);
    
//     ui_text_input(rect, buffer, cursor_pos);
// }

// bool ui_drag_button(charm::Rect &rect)
// {
//     WidgetComm widget = ui.build_widget(rect, WidgetFlag_Clickable | WidgetFlag_HotAnimation | WidgetFlag_Dragging);

//     bool val = false;
//     // if(widget.hovering)
//     // {
//     //     ui.draw_rect(rect, {255, 0, 255, 100});
//     // }

//     if (widget.dragging)
//     {
//         rect.x += widget.drag_delta_frame.x;
//         rect.y += widget.drag_delta_frame.y;
//         // printf("%i %i\n", widget.drag_delta.x, widget.drag_delta.y);
//         val = true;
//     }

//     ui.draw_rect(rect, theme.button_color);
//     ui.draw_box(rect, theme.border_color);
//     // ui_labeled_rect(rect, "k");

//     // charm::Rect line_rect = get_top(&rect, 32);
//     // static char buff[256];
//     // static int cursor = 0;
//     // ui_text_input(line_rect, buff, &cursor);


//     return widget.clicked;
// }
