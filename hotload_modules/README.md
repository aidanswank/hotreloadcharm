# Hotload Modules Directory

This directory contains hot-reloadable DSP module implementations using **static polymorphism** via CRTP (Curiously Recurring Template Pattern). Each module is a class that inherits from `ModuleBase<YourModule>`, where state variables are member variables instead of being passed around as parameters.

## Architecture

The new system uses:
- **CRTP Base Class**: `ModuleBase<Derived>` provides the interface without virtual functions
- **Singleton Pattern**: Each module has a static instance accessible via `YourModule::instance()`
- **Member State**: Module state variables are class members (`this->variable`) instead of struct fields
- **No Virtual Polymorphism**: Static dispatch at compile time for better performance

## Current Modules

### `filter_plugin.cpp`
- **Class**: `FilterModule : public ModuleBase<FilterModule>`
- **State Members**: `filter_states[2]` array with `v0`, `v1` per channel
- **Parameters**:
  - `KNOB_1`: Cutoff frequency (0.0 = low, 1.0 = high)
  - `KNOB_2`: Resonance (0.0 = no resonance, 1.0 = high Q)
- **Formula**: `this->v0 = (1-r*c)*this->v0 - c*this->v1 + c*input; this->v1 = (1-r*c)*this->v1 + c*this->v0; output = this->v1`

### `gain_plugin.cpp`
- **Class**: `GainModule : public ModuleBase<GainModule>`
- **State Members**: None needed (simple multiplier)
- **Parameters**:
  - `KNOB_1`: Gain (0.0 = silence, 1.0 = unity, 2.0 = 2x gain)
- **Formula**: `output = input * gain`

## How to Add a New Module

1. Create a new `.cpp` file in this directory (e.g., `my_module.cpp`)
2. Define your module class inheriting from `ModuleBase`:
   ```cpp
   #include "module_interface.h"
   
   class MyModule : public ModuleBase<MyModule>
   {
   public:
       // Your state variables as member variables
       float my_state_variable = 0.0f;
       
       // Implement the module methods
       void process_audio(float* outputs[], int num_channels, int num_samples, const ModuleAudioContext* context)
       {
           // Use this->my_state_variable directly!
           // No more passing state pointers around
       }
       
       const char* get_name() const { return "My Module"; }
       const char* get_version() const { return "1.0.0"; }
   };
   
   // Export the extern "C" interface
   extern "C" {
   ModuleState* module_create() { return MyModule::instance().get_state(); }
   void module_destroy(ModuleState* state) { /* singleton handles it */ }
   void module_process_audio(ModuleState* state, float* outputs[], int num_channels, int num_samples, const ModuleAudioContext* context) {
       MyModule::instance().process_audio(outputs, num_channels, num_samples, context);
   }
   const char* module_get_name() { return MyModule::instance().get_name(); }
   const char* module_get_version() { return MyModule::instance().get_version(); }
   }
   ```

3. Rebuild the project - your module will be automatically discovered and built!

## Benefits of This Approach

- **Class-like**: State is `this->member` instead of function parameters
- **Static Polymorphism**: No virtual function overhead, resolved at compile time
- **Clean Code**: No more casting `reinterpret_cast<YourState*>(state)` everywhere
- **Type Safety**: Member variables are properly typed and scoped
- **Performance**: Direct member access, no indirection through pointers

## Plugin Interface

All plugins must implement the interface defined in `source/plugin_interface.h`:

- **PluginState**: Opaque pointer for plugin-specific state
- **PluginAudioContext**: Contains knob values (0.0-1.0), channel count, sample rate
- **Audio Processing**: Process `num_samples` per channel, modify `outputs[]` in-place

## Building

Plugins are automatically discovered and built by CMake:

```cmake
file(GLOB PLUGIN_SOURCES plugin_modules/*.cpp)
foreach(PLUGIN_SRC ${PLUGIN_SOURCES})
    # Build each as a shared library
endforeach()
```

Output location: `build/plugins/[plugin_name].dylib`

## Hot-Reloading

- Modify plugin source code
- Rebuild: `cmake --build build` (only rebuilds changed plugins)
- Click "Reload" in the UI to swap the running plugin
- No need to restart audio or the application!

## Development Tips

1. **Keep state minimal**: Complex state makes hot-reloading tricky
2. **Handle all channels**: Process both stereo channels independently
3. **Use the context**: Access parameters via `context->knob_1`, etc.
4. **Test reload scenarios**: Ensure state is properly cleaned up
5. **Version your plugins**: Helps with debugging

## Example Plugin Template (CRTP Style)

```cpp
#include "plugin_interface.h"

class MyPlugin : public PluginBase<MyPlugin>
{
public:
    // Your state variables as member variables!
    float my_filter_state = 0.0f;
    float another_variable = 1.0f;
    
    void process_audio(float* outputs[], int num_channels, int num_samples, const PluginAudioContext* context)
    {
        // Use this->my_filter_state directly - no casting needed!
        for (int ch = 0; ch < num_channels; ++ch) {
            for (int i = 0; i < num_samples; ++i) {
                // Process outputs[ch][i] using context->knob_1, this->my_filter_state, etc.
                outputs[ch][i] *= this->another_variable;
            }
        }
    }
    
    const char* get_name() const { return "My Plugin"; }
    const char* get_version() const { return "1.0.0"; }
};

// Export the extern "C" interface for dynamic loading
extern "C" {
PluginState* plugin_create() { return MyPlugin::instance().get_state(); }
void plugin_destroy(PluginState* state) { /* singleton handles cleanup */ }
void plugin_process_audio(PluginState* state, float* outputs[], int num_channels, int num_samples, const PluginAudioContext* context) {
    MyPlugin::instance().process_audio(outputs, num_channels, num_samples, context);
}
const char* plugin_get_name() { return MyPlugin::instance().get_name(); }
const char* plugin_get_version() { return MyPlugin::instance().get_version(); }
}
```