# Hot-Reloadable Plugin System Implementation

## Summary

I've implemented a complete hot-reloadable plugin system for your JUCE application. This allows you to:

✅ **Load DSP plugins from dylib files** - Plugins are compiled as separate shared libraries
✅ **Hot-reload plugins without restarting** - Swap plugins in real-time
✅ **Implement custom filters** - Build your own signal processing algorithms
✅ **Example SVF filter** - Includes your specified resonant filter formula
✅ **Parameter passing** - 4 knobs (KNOB_1 to KNOB_4) pass values to plugins
✅ **Multi-channel support** - Processes stereo (and beyond) with independent state per channel

## What Was Created

### Core Files

1. **`source/plugin_interface.h`**
   - Defines the contract all plugins must implement
   - Includes `PluginState` struct and required function signatures
   - Parameter structure `PluginAudioContext` with knob values

2. **`source/plugin_loader.h`**
   - Manages plugin lifecycle (load, create, destroy, reload)
   - Handles dynamic library loading via dlopen/dlsym
   - Error reporting and metadata retrieval

3. **`source/filter_plugin.cpp`** - Example Plugin
   - Implements your SVF filter formula
   - State variables per channel: v0_left, v1_left, v0_right, v1_right
   - Coefficients calculated from KNOB_1 (cutoff) and KNOB_2 (resonance)
   - Formula: `v0 = (1-r*c)*v0 - c*v1 + c*input` and `v1 = (1-r*c)*v1 + c*v0`

4. **`CharmApp.h/cpp`** - Integration
   - Added `PluginLoader m_plugin_loader` member
   - Updated `process_audio()` to route through loaded plugin
   - New methods: `load_plugin(path)` and `reload_plugin()`
   - Automatically passes knob values to plugin via `PluginAudioContext`

### Build Setup

5. **`CMakeLists.txt`** - Updated
   - Added target to build `filter_plugin.dylib`
   - Output directory: `build/plugins/`
   - Plugin runs as dependency of main app

### Documentation

6. **`PLUGIN_SYSTEM.md`** - Complete guide
   - Architecture overview
   - How to use plugins in your code
   - Instructions for creating custom plugins
   - Tips and troubleshooting

7. **`PLUGIN_EXAMPLE.cpp`** - Usage examples
   - Minimal code snippets
   - How to integrate with JUCE AudioProcessor

8. **`FILTER_FORMULA.md`** - Formula documentation
   - Your exact formula implementation
   - Parameter ranges and meanings
   - How to extend with more knobs

## Quick Start

### Building

```bash
cd /Users/aidan/dev/cpp/juce_cmake/hotreloadcharm
rm -rf build && mkdir build && cd build
cmake ..
cmake --build .
```

The plugin will be built to: `build/plugins/filter_plugin.dylib`

### Using in Your Code

```cpp
// In your JUCE plugin processor constructor or method
CharmApp app;

// Load the plugin
if (app.load_plugin("build/plugins/filter_plugin.dylib")) {
    printf("Plugin loaded: %s\n", 
           app.get_plugin_loader().get_name());
}

// In your processBlock/process_audio
void process_audio(float* outputs[], int num_channels, int num_samples) {
    if (m_plugin_loader.is_loaded()) {
        PluginAudioContext context;
        context.knob_1 = get_host_parameter_value(knob_1_param_id, 0.5f);
        context.knob_2 = get_host_parameter_value(knob_2_param_id, 0.5f);
        context.knob_3 = get_host_parameter_value(knob_3_param_id, 0.5f);
        context.knob_4 = get_host_parameter_value(knob_4_param_id, 0.5f);
        context.num_channels = num_channels;
        context.sample_rate = static_cast<int>(sample_rate);
        
        m_plugin_loader.process_audio(outputs, num_channels, 
                                     num_samples, &context);
    }
}

// Hot-reload on demand
app.reload_plugin();  // Unload, reload dylib, create new instance
```

## Plugin Binary

✅ Successfully built and verified:

```
build/plugins/filter_plugin.dylib (33 KB)
Exported symbols:
  - plugin_create
  - plugin_destroy
  - plugin_process_audio
  - plugin_get_name
  - plugin_get_version
```

## Your SVF Filter

The included plugin implements exactly your formula:

**Parameters:**
- `KNOB_1` → Cutoff frequency (0.0 = very low, 1.0 = very high)
- `KNOB_2` → Resonance (0.0 = none, 1.0 = high Q)

**State Variables:**
- Per-channel: `v0`, `v1`
- Maintains filter memory across samples

**Processing:**
```
c = pow(0.5, (128 - knob_1*128) / 16.0)
r = pow(0.5, (knob_2*24 + 24) / 16.0)

v0 = (1-r*c)*v0 - c*v1 + c*input
v1 = (1-r*c)*v1 + c*v0

output = v1
```

## Creating Your Own Plugins

To create a new plugin:

1. Create `source/my_plugin.cpp` with the required functions
2. Add to CMakeLists.txt:
   ```cmake
   add_library(my_plugin SHARED source/my_plugin.cpp)
   set_target_properties(my_plugin PROPERTIES
       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
       PREFIX "" SUFFIX ".dylib"
   )
   ```
3. Build: `cmake --build build`
4. Load: `app.load_plugin("build/plugins/my_plugin.dylib")`

## Workflow: Iterative Development

1. Edit `source/filter_plugin.cpp` with your algorithm
2. Build: `cmake --build build` (only rebuilds plugin dylib)
3. While audio is running: `app.reload_plugin()`
4. Instant feedback on audio changes!

No need to restart the entire plugin/DAW.

## Files Modified

- ✏️ `source/CharmApp.h` - Added plugin loader member and methods
- ✏️ `source/CharmApp.cpp` - Integrated plugin processing
- ✏️ `source/PluginProcessor.cpp` - Fixed parameter ID references
- ✏️ `CMakeLists.txt` - Added plugin dylib build target

## Files Created

- 📄 `source/plugin_interface.h` - Plugin contract
- 📄 `source/plugin_loader.h` - Plugin manager
- 📄 `source/filter_plugin.cpp` - Example SVF filter
- 📄 `PLUGIN_SYSTEM.md` - Full documentation
- 📄 `PLUGIN_EXAMPLE.cpp` - Usage examples
- 📄 `FILTER_FORMULA.md` - Formula reference

## Next Steps

1. **Test the filter**: Adjust KNOB_1 and KNOB_2 to hear cutoff and resonance changes
2. **Modify the formula**: Edit `source/filter_plugin.cpp` and reload
3. **Add more filters**: Create new plugin files following the interface
4. **Integrate with UI**: Add buttons to load/reload plugins from your plugin's GUI

Enjoy hot-reloadable DSP development! 🎵
