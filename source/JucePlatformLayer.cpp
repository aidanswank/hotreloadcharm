/*
  ==============================================================================

    JucePlatformLayer.cpp
    Created: 6 Mar 2024 7:33:44pm
    Author:  aidan

  ==============================================================================
*/

#include "JucePlatformLayer.h"

#if NANOVG_METAL_IMPLEMENTATION
#include <nanovg_mtl.h>
#else
#include <nanovg_gl.h>
#endif

// This is the JUCE-specific function
void juceClipboardCallback(void* ctx, const char* text)
{
    juce::SystemClipboard::copyTextToClipboard(text);
}

// Paste handler
const char* juceClipboardGet(void* ctx)
{
    // Careful: we need to keep the string alive
    static std::string buffer;
    buffer = juce::SystemClipboard::getTextFromClipboard().toStdString();
    return buffer.c_str();
}

// Copy handler
void juceClipboardSet(void* ctx, const char* text)
{
    juce::SystemClipboard::copyTextToClipboard(text);
}

//==============================================================================
JucePlatformLayer::JucePlatformLayer(CharmApp* app_ptr) : app(app_ptr)
{
    // openGLContext.setOpenGLVersionRequired (OpenGLContext::openGL3_2);

    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    // Indicates that no part of this Component is transparent.
    setOpaque(true);

    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);

    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);

    addKeyListener(this);
    setWantsKeyboardFocus(true);
    
    // Bind the JUCE callback
//    app->set_clipboard_callback(&juceClipboardCallback, nullptr);
    
    // Bind JUCE clipboard
    app->set_clipboard_callbacks(&juceClipboardSet, &juceClipboardGet, nullptr);
}

JucePlatformLayer::~JucePlatformLayer()
{
    openGLContext.detach();
}

void JucePlatformLayer::paint (juce::Graphics& g)
{
}

void JucePlatformLayer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

#include "BinaryData.h"
extern unsigned char* BinaryData_Roboto_Light_ttf;
extern int BinaryData_Roboto_Light_ttfSize;

void JucePlatformLayer::newOpenGLContextCreated()
{
    const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    printf("OpenGL Version: %s\n", glVersion);

    #if NANOVG_METAL_IMPLEMENTATION
        vg = nvgCreateContext(nativeHandle, NVG_ANTIALIAS | NVG_TRIPLE_BUFFER, width, height);
    #else
        vg = nvgCreateContext(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    #endif

    if (!vg)
    {
        printf("Failed to create NanoVG context!\n");
    }

//     std::string font_path = get_resource_path("fonts/Roboto-Light.ttf");
// //    std::string font_path = get_resource_path("fonts/unifont-14.0.01.ttf");

//     if (vg_load_font(vg, "sans-bold", font_path.c_str()) == -1) {
//         printf("failed to load font: %s\n", font_path.c_str());
//     }

    int fontId = nvgCreateFontMem(vg, "sans-bold",
        (unsigned char*)BinaryData::RobotoLight_ttf,
        BinaryData::RobotoLight_ttfSize, 0);

    if (fontId == -1)
        printf("Failed to load font\n");
    else
        nvgFontFace(vg, "sans-bold");

    //agnostic
    UIRenderer ui_renderer;
    ui_renderer.userdata = vg; // Pass the NanoVG context
    ui_renderer.draw_rect = vg_draw_rect;
    ui_renderer.draw_box = vg_draw_box;
    ui_renderer.draw_string = vg_draw_string;
    ui_renderer.get_text_width = vg_get_text_width;
    ui_renderer.get_text_height = vg_get_text_height;
    ui_renderer.draw_line = vg_draw_line;
    ui_renderer.render_clip = vg_render_clip;

    app->init_ui(ui_renderer);
    // app->register_modules();
}

void JucePlatformLayer::openGLContextClosing()
{
    if (vg)
    {
        nvgDeleteContext(vg);
        vg = nullptr;
    }
}

void JucePlatformLayer::renderOpenGL()
{
    // Clear the screen
    // juce::OpenGLHelpers::clear(juce::Colours::darkslategrey);

    if (!vg)
    {
        printf("no vg!\n");
        return;
    }

    float scale = (float)openGLContext.getRenderingScale();
    int fbWidth  = roundToInt(getWidth() * scale);
    int fbHeight = roundToInt(getHeight() * scale);

    glViewport(0, 0, fbWidth, fbHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, getWidth(), getHeight(), scale);

    // float ascender, descender, lineh;
    // nvgTextMetrics(vg, &ascender, &descender, &lineh);
    // printf("asc=%f desc=%f lineh=%f\n",ascender,descender,lineh);

    // Example NanoVG drawing
    charm::Rect window_rect = {0,0, (float)getWidth(), (float)getHeight()};

    app->on_render(window_rect, scale);

    nvgEndFrame(vg);
}
