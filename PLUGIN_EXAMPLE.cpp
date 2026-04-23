#include "plugin_interface.h"
#include "plugin_loader.h"

/**
 * Example: How to integrate plugin loading into your JUCE plugin
 * 
 * This file shows the minimal code needed to load and use hot-reloadable plugins
 * in your JUCE AudioProcessor.
 */

class ExamplePluginProcessor {
private:
    PluginLoader m_plugin;
    std::string m_plugin_path;
    
public:
    // In your plugin constructor
    ExamplePluginProcessor() {
        // Construct path to plugin - adjust to your setup
        const char* plugin_name = "filter_plugin.dylib";
        m_plugin_path = get_plugin_directory() + "/" + plugin_name;
    }
    
    // Load plugin from dylib
    bool load_plugin() {
        if (!m_plugin.load(m_plugin_path)) {
            printf("Error: %s\n", m_plugin.get_error().c_str());
            return false;
        }
        
        if (!m_plugin.create_instance()) {
            printf("Failed to create plugin instance\n");
            return false;
        }
        
        printf("✓ Loaded %s v%s from %s\n", 
               m_plugin.get_name(),
               m_plugin.get_version(),
               m_plugin_path.c_str());
        
        return true;
    }
    
    // Hot-reload plugin
    bool reload_plugin() {
        if (!m_plugin.reload()) {
            printf("Reload failed: %s\n", m_plugin.get_error().c_str());
            return false;
        }
        
        if (!m_plugin.create_instance()) {
            printf("Failed to create plugin instance after reload\n");
            return false;
        }
        
        printf("✓ Reloaded %s v%s\n", 
               m_plugin.get_name(),
               m_plugin.get_version());
        
        return true;
    }
    
    // In your processBlock
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
        if (!m_plugin.is_loaded())
            return;
        
        float* channels[2] = {
            buffer.getWritePointer(0),
            buffer.getWritePointer(1)
        };
        
        PluginAudioContext ctx;
        ctx.knob_1 = 0.5f;  // Your parameter values
        ctx.knob_2 = 0.5f;
        ctx.knob_3 = 0.5f;
        ctx.knob_4 = 0.5f;
        ctx.num_channels = 2;
        ctx.sample_rate = 44100;
        
        m_plugin.process_audio(channels, 2, buffer.getNumSamples(), &ctx);
    }
    
    // Get plugin info
    bool is_plugin_loaded() const {
        return m_plugin.is_loaded();
    }
    
    const char* get_plugin_name() const {
        return m_plugin.get_name();
    }
    
    const char* get_plugin_version() const {
        return m_plugin.get_version();
    }
    
private:
    std::string get_plugin_directory() const {
        // Return path to plugins directory
        // On macOS, this could be:
        // - $(BUILD_DIR)/plugins for development
        // - $(APP_CONTENTS)/plugins for deployment
        return "./plugins";
    }
};

/**
 * Quick Usage Example
 */
void example_usage() {
    ExamplePluginProcessor proc;
    
    // Load plugin
    proc.load_plugin();
    
    // ... run audio ...
    
    // Hot-reload when you modify the plugin source
    proc.reload_plugin();
    
    // ... continue running audio ...
}
