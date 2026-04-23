# Hot-Reloadable Plugin System

This document explains how to use the hot-reloadable plugin system in HotReloadCharm.

## Overview

The plugin system allows you to:
- Load DSP plugins from dynamic libraries (.dylib files)
- Hot-reload plugins without restarting the application
- Process audio through user-defined algorithm implementations
- Manage plugin state independently from the main app

## Architecture

### Files

- **`plugin_interface.h`** - Defines the plugin contract that all plugins must implement
- **`plugin_loader.h`** - Manages plugin lifecycle (load, unload, reload)
- **`filter_plugin.cpp`** - Example plugin implementation (resonant SVF filter)
- **`CharmApp.h/cpp`** - Main app with plugin loading integration

### Plugin Interface

All plugins must implement these extern "C" functions:

```cpp
// Create plugin state
PluginState* plugin_create();

// Destroy plugin state
void plugin_destroy(PluginState* state);

// Process audio block
void plugin_process_audio(PluginState* state,
                         float* outputs[],
                         int num_channels,
                         int num_samples,
                         const PluginAudioContext* context);

// Optional metadata
const char* plugin_get_name();
const char* plugin_get_version();
```

## Using the Plugin System

### Loading a Plugin

From your JUCE plugin processor or UI code:

```cpp
CharmApp app;

// Load a plugin from a dylib file
bool success = app.load_plugin("/path/to/filter_plugin.dylib");
if (success) {
    printf("Plugin loaded: %s\n", app.get_plugin_loader().get_name());
}
```

### Processing Audio

Once a plugin is loaded, `CharmApp::process_audio()` automatically routes audio through it:

```cpp
void process_audio(float* outputs[], int num_channels, int num_samples)
{
    if (m_plugin_loader.is_loaded())
    {
        PluginAudioContext context;
        context.knob_1 = get_host_parameter_value(knob_1_param_id);
        context.knob_2 = get_host_parameter_value(knob_2_param_id);
        context.num_channels = num_channels;
        context.sample_rate = sample_rate;
        
        m_plugin_loader.process_audio(outputs, num_channels, num_samples, &context);
    }
}
```

### Hot-Reloading

To reload a plugin without stopping audio:

```cpp
bool success = app.reload_plugin();
```

This will:
1. Destroy the current plugin state
2. Unload the dylib
3. Reload the dylib from disk
4. Create a fresh plugin instance

Perfect for iterating on filter designs in real-time.

## Creating Your Own Plugin

### Step 1: Create the Plugin Source File

Create `my_filter_plugin.cpp`:

```cpp
#include "plugin_interface.h"
#include <cmath>

struct MyFilterState
{
    float state_var_left = 0.0f;
    float state_var_right = 0.0f;
};

extern "C" {

PluginState* plugin_create()
{
    return reinterpret_cast<PluginState*>(new MyFilterState());
}

void plugin_destroy(PluginState* state)
{
    delete reinterpret_cast<MyFilterState*>(state);
}

void plugin_process_audio(PluginState* state,
                         float* outputs[],
                         int num_channels,
                         int num_samples,
                         const PluginAudioContext* context)
{
    if (!state || !context) return;
    
    MyFilterState* fs = reinterpret_cast<MyFilterState*>(state);
    
    // Use context->knob_1, knob_2, etc. for parameters
    // Process audio
    for (int ch = 0; ch < num_channels; ++ch)
    {
        float* channel = outputs[ch];
        for (int i = 0; i < num_samples; ++i)
        {
            // Your algorithm here
            channel[i] = channel[i] * 0.5f;
        }
    }
}

const char* plugin_get_name()
{
    return "My Filter";
}

const char* plugin_get_version()
{
    return "1.0.0";
}

} // extern "C"
```

### Step 2: Update CMakeLists.txt

Add to your CMakeLists.txt:

```cmake
# Build your plugin
add_library(my_filter_plugin SHARED
    source/my_filter_plugin.cpp
)

target_include_directories(my_filter_plugin
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

set_target_properties(my_filter_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    PREFIX ""
    SUFFIX ".dylib"
)
```

### Step 3: Build and Load

```bash
cmake --build build
```

Your plugin will be built to `build/plugins/my_filter_plugin.dylib`.

Load it in your app:

```cpp
app.load_plugin("build/plugins/my_filter_plugin.dylib");
```

## Example: The SVF Filter Plugin

The included `filter_plugin.cpp` implements a State Variable Filter (SVF):

- **KNOB_1** controls cutoff frequency (0.0 = low, 1.0 = high)
- **KNOB_2** controls resonance (0.0 = no resonance, 1.0 = high resonance)

Formula:
```
c = pow(0.5, (128.0 - knob_1 * 128.0) / 16.0)
r = pow(0.5, (knob_2 * 24.0 + 24.0) / 16.0)

v0 = (1 - r*c)*v0 - c*v1 + c*input
v1 = (1 - r*c)*v1 + c*v0

output = v1
```

The filter maintains separate state for left and right channels.

## Tips

1. **Keep plugins simple** - Complex state management makes hot-reloading tricky
2. **Always check for null** - Plugins may not be loaded
3. **Use the context** - Access knobs and audio info via `PluginAudioContext`
4. **Version your plugins** - Makes debugging easier
5. **Test reload scenarios** - Ensure plugins handle state properly on reload

## Troubleshooting

**Plugin fails to load**: Check that the dylib has the required function symbols:
```bash
nm build/plugins/filter_plugin.dylib | grep plugin_
```

**Audio doesn't change**: Verify the plugin is actually loaded:
```cpp
if (app.get_plugin_loader().is_loaded()) {
    printf("Plugin: %s\n", app.get_plugin_loader().get_name());
}
```

**Crash on reload**: Ensure `plugin_destroy()` properly cleans up all state
