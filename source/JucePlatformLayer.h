/*
  ==============================================================================

    JucePlatformLayer.h
    Created: 6 Mar 2024 7:33:44pm
    Author:  aidan

  ==============================================================================
*/

#pragma once

#include <juce_opengl/juce_opengl.h>
#include "PluginProcessor.h"

using namespace juce;
using namespace ::juce::gl;

// #include "ui.h"

// #include <nanovg.h>

// #include "ui_impl.h"

// #include "AudioGraph.h"
#include "CharmApp.h"

#if defined NANOVG_GL2_IMPLEMENTATION
  #define NANOVG_GL_IMPLEMENTATION 1
  #define nvgCreateContext(flags) nvgCreateGL2(flags)
  #define nvgDeleteContext(context) nvgDeleteGL2(context)
#elif defined NANOVG_GLES2_IMPLEMENTATION
  #define NANOVG_GL_IMPLEMENTATION 1
  #define nvgCreateContext(flags) nvgCreateGLES2(flags)
  #define nvgDeleteContext(context) nvgDeleteGLES2(context)
#elif defined NANOVG_GL3_IMPLEMENTATION
  #define NANOVG_GL_IMPLEMENTATION 1
  #define nvgCreateContext(flags) nvgCreateGL3(flags)
  #define nvgDeleteContext(context) nvgDeleteGL3(context)
#elif defined NANOVG_GLES3_IMPLEMENTATION
  #define NANOVG_GL_IMPLEMENTATION 1
  #define nvgCreateContext(flags) nvgCreateGLES3(flags)
  #define nvgDeleteContext(context) nvgDeleteGLES3(context)
#elif defined NANOVG_METAL_IMPLEMENTATION
  #define nvgCreateContext(layer, flags, w, h) mnvgCreateContext(layer, flags, w, h)
  #define nvgDeleteContext(context) nvgDeleteMTL(context)
  #define nvgBindFramebuffer(fb) mnvgBindFramebuffer(fb)
  #define nvgCreateFramebuffer(ctx, w, h, flags) mnvgCreateFramebuffer(ctx, w, h, flags)
  #define nvgDeleteFramebuffer(fb) mnvgDeleteFramebuffer(fb)
#endif

inline CharmKey juce_to_charm_key (int keycode)
{
    using juce::KeyPress;
    
    // control
    if (keycode == KeyPress::backspaceKey) return CHARM_KEY_BACKSPACE;
    if (keycode == KeyPress::returnKey)    return CHARM_KEY_RETURN;
    if (keycode == KeyPress::escapeKey)    return CHARM_KEY_ESCAPE;
    if (keycode == KeyPress::tabKey)       return CHARM_KEY_TAB;
    if (keycode == KeyPress::spaceKey)     return CHARM_KEY_SPACE;
    if (keycode == KeyPress::deleteKey)    return CHARM_KEY_DELETE;
    if (keycode == KeyPress::insertKey)    return CHARM_KEY_INSERT;

    // arrows
    if (keycode == KeyPress::leftKey)      return CHARM_KEY_LEFT;
    if (keycode == KeyPress::rightKey)     return CHARM_KEY_RIGHT;
    if (keycode == KeyPress::upKey)        return CHARM_KEY_UP;
    if (keycode == KeyPress::downKey)      return CHARM_KEY_DOWN;
    
    return CHARM_KEY_NONE;
}

//==============================================================================
/*
*/
class JucePlatformLayer  : public juce::Component,
                         public OpenGLRenderer,
                         private juce::KeyListener
{
public:
    // JucePlatformLayer();
    JucePlatformLayer(CharmApp* app_ptr);
    ~JucePlatformLayer() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;
    bool keyPressed(const juce::KeyPress& kp, juce::Component*) override {
        int kc = kp.getKeyCode();
        printf("juce kc%c\n",kc);
        app->ui_ctx->keyentered = juce_to_charm_key(kc);
        if(app->ui_ctx->keyentered==CHARM_KEY_NONE)
        {
            printf("WARNING: PASSING UNIMPLEMENTED KEYCODE %i\n", kc);
            app->ui_ctx->keyentered = kc;
        }
        bool isRepeat = kp.getTextCharacter() == 0 ? false : false; // JUCE provides isKeyRepeat() too in some versions
//        printf("key char %c \n", kp.getTextCharacter());
//        app->ui_ctx->keychar = kp.getTextCharacter();
        
        // --- fill keymod ---
        app->ui_ctx->keymod = CHARM_KEYMOD_NONE;
        auto mods = kp.getModifiers();

        if (mods.isShiftDown())   app->ui_ctx->keymod |= CHARM_KEYMOD_SHIFT;
        if (mods.isCtrlDown())    app->ui_ctx->keymod |= CHARM_KEYMOD_CTRL;
        if (mods.isAltDown())     app->ui_ctx->keymod |= CHARM_KEYMOD_ALT;
        if (mods.isCommandDown()) app->ui_ctx->keymod |= CHARM_KEYMOD_SUPER;

        app->ui_ctx->keychar = kp.getTextCharacter(); // lower is higher case, for text input
        
//        // --- Detect Command+C (or Ctrl+C on Windows/Linux) ---
//        if ((app->ui_ctx->keymod & CHARM_KEYMOD_SUPER)   // Command/Windows key
//            && (app->ui_ctx->keyentered == 'C'))
//        {
//            printf("Copy detected!\n");
//            // trigger your copy logic here
//            return true;
//        }
        
        // plat::Mod mods = plat::juce_mods_to_mod(kp.getModifiers().isShiftDown(),
        //                                        kp.getModifiers().isCtrlDown(),
        //                                        kp.getModifiers().isAltDown(),
        //                                        kp.getModifiers().isCommandDown());
        // plat::KeyEvent ev = plat::make_key_event_from_juce(kc, true, kp.isKeyDown(), mods, 0, static_cast<char32_t>(kp.getTextCharacter()));
        // auto cb = plat::get_key_callback();
        // if (cb) cb(ev);
        return true; // return true if you consumed it
    }

    
    void mouseMove(const MouseEvent& event) override
    {
        // set_ui_state(ui_ctx.get());
        app->ui_ctx->mouse_pos = { event.position.x, event.position.y };
    }

    void mouseDrag(const MouseEvent& event) override
    {
        // set_ui_state(app->ui_ctx.get());
        app->ui_ctx->mouse_pos = { event.position.x, event.position.y };
    }

    void mouseDown(const MouseEvent& event) override {
        // set_ui_state(app->ui_ctx.get());
        app->ui_ctx->mouse_pos = { event.position.x, event.position.y };
        app->ui_ctx->mouse_down = true;
        app->ui_ctx->mouse_clicked = true;

        double current_time = juce::Time::getMillisecondCounterHiRes() / 1000.0;

        if ((current_time - app->ui_ctx->last_click_time) <= 0.25) {
            app->ui_ctx->click_count++;
            if (app->ui_ctx->click_count == 2) {
                app->ui_ctx->double_click = true;
//                printf("double click\n");
                app->ui_ctx->click_count = 0;
            }
        } else {
            app->ui_ctx->click_count = 1;
        }

        app->ui_ctx->last_click_time = current_time;

        // if (event.mods.isRightButtonDown()) {
        //     app->ui_ctx->right_mouse_down = true;
        //     app->ui_ctx->right_mouse_clicked = true;
        // }

        if(event.mods.isRightButtonDown())
        {
          printf("right button down!!!!\n");
          app->ui_ctx->right_mouse_released = true;
        }
    }

    void mouseUp(const MouseEvent& event) override
    {
        // set_ui_state(app->ui_ctx.get());
        app->ui_ctx->mouse_down = false;
        app->ui_ctx->mouse_released = true;

        // if (!event.mods.isRightButtonDown())
        // {
        //     app->ui_ctx->right_mouse_down = false;
        //     app->ui_ctx->right_mouse_released = true;
        // }
    }
    
    // In your JUCE component implementation (.cpp file):
    void mouseWheelMove(const juce::MouseEvent& event,
                                      const juce::MouseWheelDetails& wheel)
    {
        // JUCE provides wheel deltas in the MouseWheelDetails struct
        // wheel.deltaX corresponds to your xoffset
        // wheel.deltaY corresponds to your yoffset
        
        app->ui_ctx->scroll_delta.y += wheel.deltaY * scrollSensitivity;
        app->ui_ctx->scroll_delta.x += wheel.deltaX * scrollSensitivity;
        
        // Optional: You might want to scale the values since JUCE's wheel deltas
        // are typically smaller than GLFW's
        // ui.scroll_delta.y += wheel.deltaY * scrollSensitivity;
        // ui.scroll_delta.x += wheel.deltaX * scrollSensitivity;
        
        // If you need to trigger a repaint or update
//        repaint();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePlatformLayer)
    
    OpenGLContext openGLContext;
    
    float scrollSensitivity = 48.0f;
    // CharmApp app;
    CharmApp* app; // pointer, not owned
    NVGcontext* vg = nullptr;
};
