/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <filesystem>

juce::AudioProcessorValueTreeState::ParameterLayout CharmAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(CharmApp::knob_1_param_id, 1),
        "knob 1",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(CharmApp::knob_2_param_id, 1),
        "knob 2",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(CharmApp::knob_3_param_id, 1),
        "knob 3",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(CharmApp::knob_4_param_id, 1),
        "knob 4",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID(CharmApp::mainGainParamID, 1),
    //     "Main Gain",
    //     juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
    //     1.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID(CharmApp::phaseInvertParamID, 1),
    //     "Phase Invert",
    //     juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f),
    //     1.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID(CharmApp::filterCutoffParamID, 1),
    //     "Filter Cutoff",
    //     juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
    //     0.5f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID(CharmApp::filterResParamID, 1),
    //     "Filter Resonance",
    //     juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
    //     0.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID(CharmApp::filterFoldParamID, 1),
    //     "Filter Fold",
    //     juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
    //     0.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID(CharmApp::filterDriveParamID, 1),
    //     "Filter Drive",
    //     juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
    //     0.0f));

    return { params.begin(), params.end() };
}

std::string getAssetsFolder()
{
    // Get the folder where the app executable lives
    juce::File exeFolder = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                                .getParentDirectory();

    // Assets folder inside executable folder
    juce::File assetsFolder = exeFolder.getChildFile("Assets");
    
    // Ensure it exists (optional)
    assetsFolder.createDirectory();

    // Return full path as std::string
    return assetsFolder.getFullPathName().toStdString();
}

std::vector<std::string> listFilesInFolder(const std::string& folderPath)
{
    std::vector<std::string> files;

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file())
                files.push_back(entry.path().string());
                std::cout << entry.path().string() << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error accessing folder: " << e.what() << std::endl;
    }

    return files;
}



void saveOrReadTestFile(CharmApp* app)
{
    // Construct the file path in the user application data directory
    juce::File file = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory
    ).getChildFile("MyPlugin/untitled.ark");

    // Make sure the directory exists
    file.getParentDirectory().createDirectory();

    // Print the full path so you know where the file is
    printf("File path: %s\n", file.getFullPathName().toRawUTF8());
    
    printf("asset folder: %s\n", getAssetsFolder().c_str());
    listFilesInFolder(getAssetsFolder() + "/cat-eyes-882179871");
    
    app->set_working_file(file.getFullPathName().toRawUTF8());

//    FILE* f = fopen(file.getFullPathName().toRawUTF8(), "r");
//    if (f)
//    {
//        // File exists, read it
//        printf("File exists. Contents:\n");
//
//        char buffer[256];
//        while (fgets(buffer, sizeof(buffer), f))
//            printf("%s", buffer);
//
//        fclose(f);
//    }
//    else
//    {
//        // File doesn't exist, create and write
//        f = fopen(file.getFullPathName().toRawUTF8(), "w");
//        if (!f)
//        {
//            perror("Failed to create file");
//            return 1;
//        }
//
//        fprintf(f, "This is a test file.\nHello from C/printf!\n");
//        fclose(f);
//
//        printf("File did not exist. Created it at: %s\n", file.getFullPathName().toRawUTF8());
//    }
}

//==============================================================================
CharmAudioProcessor::CharmAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
         :
#endif
             parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    app = std::make_unique<CharmApp>();  // <-- must happen first
    app->attach_host_parameters(parameters);
    app->main_gain = app->get_host_parameter_value(CharmApp::knob_1_param_id, 1.0f);
    app->phase_invert = app->get_host_parameter_value(CharmApp::knob_2_param_id, 1.0f);

    saveOrReadTestFile(app.get());
}

CharmAudioProcessor::~CharmAudioProcessor()
{
}

//==============================================================================
const juce::String CharmAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CharmAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CharmAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CharmAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CharmAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CharmAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CharmAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CharmAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CharmAudioProcessor::getProgramName (int index)
{
    return {};
}

void CharmAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CharmAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    if (app)
        app->prepare_to_play(sampleRate, samplesPerBlock);
}

void CharmAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CharmAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// void CharmAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
// {
//     juce::ScopedNoDenormals noDenormals;
//     auto totalNumInputChannels  = getTotalNumInputChannels();
//     auto totalNumOutputChannels = getTotalNumOutputChannels();

//     // In case we have more outputs than inputs, this code clears any output
//     // channels that didn't contain input data, (because these aren't
//     // guaranteed to be empty - they may contain garbage).
//     // This is here to avoid people getting screaming feedback
//     // when they first compile a plugin, but obviously you don't need to keep
//     // this code if your algorithm always overwrites all the output channels.
//     for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//         buffer.clear (i, 0, buffer.getNumSamples());

//     // This is the place where you'd normally do the guts of your plugin's
//     // audio processing...
//     // Make sure to reset the state if your inner loop is processing
//     // the samples and the outer loop is handling the channels.
//     // Alternatively, you can process the samples with the channels
//     // interleaved by keeping the same state.
//     for (int channel = 0; channel < totalNumInputChannels; ++channel)
//     {
//         auto* channelData = buffer.getWritePointer (channel);

//         // ..do something to the data...
//     }
// }

void CharmAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    float* outputs[2] = { buffer.getWritePointer(0),
                            numChannels > 1 ? buffer.getWritePointer(1) : nullptr };
        
    // Get playhead info from the host
    auto playHead = getPlayHead();
    juce::AudioPlayHead::CurrentPositionInfo positionInfo;
    
    if (playHead != nullptr && playHead->getCurrentPosition(positionInfo))
    {
        // Beat fragment (0.25 = 1/4 beat = 16th note)
        double beatFraction = 0.25;
        
        // Get host BPM and PPQ position
        double hostBPM = positionInfo.bpm;
        double ppqPosition = positionInfo.ppqPosition;
        app->current_ppq = ppqPosition;
//        printf("ppq%f\n", app->current_ppq);
        
        // --- Update BPM if changed ---
        if (hostBPM > 0.0 && hostBPM != app->lastKnownBpm)
        {
            app->lastKnownBpm = hostBPM;
            // Calculate outputs
            printf("bpm changed %f\n", hostBPM);

            // // Tell PhasorClock its new rate
            //  for (AudioNode* module : app->graph.nodes)
            //  {
            //      if (module->name == "PhasorClock")
            //      {
            //          auto* phasor = static_cast<PhasorClock*>(module);
            //          phasor->setBPM(hostBPM, getSampleRate());
            //      }
            //  }
            
            // B: Beats per minute for the beat fraction
            // If beat fraction is 0.25 (16th note), and host is 120 BPM,
            // then 16th notes occur at 120 * 4 = 480 BPM
            double beatFractionBPM = hostBPM / beatFraction;
            
            // Hz: Frequency of the note interval
            // Convert BPM to Hz: BPM / 60 = beats per second
            double noteIntervalHz = beatFractionBPM / 60.0;
            
            // ms: Duration of one beat fragment in milliseconds
            // Duration = (60 / BPM) * 1000 ms
            double noteDurationMs = (60.0 / beatFractionBPM) * 1000.0;
            
            // Store values in app class
            app->beat_fraction_bpm = beatFractionBPM;
            app->note_interval_hz = noteIntervalHz;
            app->note_duration_ms = noteDurationMs;
            
//            printf("")
            // Example: Print values for debugging (remove in production)
            DBG("Beat Fraction: " << beatFraction);
            DBG("Host BPM: " << hostBPM);
            DBG("PPQ Position: " << ppqPosition);
            DBG("B (BPM): " << beatFractionBPM);
            DBG("Hz: " << noteIntervalHz);
            DBG("ms: " << noteDurationMs);
        }
        
        // // --- Resync to bar grid (0–1 phasor per bar) ---
        // for (AudioNode* module : app->graph.nodes)
        // {
        //    if (module->name == "PhasorClock")
        //    {
            //    auto* phasor = static_cast<PhasorClock*>(module);
            //    phasor->resyncFromPPQ(ppqPosition);
        //    }
        // }

        app->phasor_clock.set_bpm(hostBPM, getSampleRate());
        app->phasor_clock.resync_from_ppq(ppqPosition);
    }
    app->process_audio(outputs, numChannels, numSamples);
}

//==============================================================================
bool CharmAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CharmAudioProcessor::createEditor()
{
    return new CharmAudioProcessorEditor (*this);
}

//==============================================================================
void CharmAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void CharmAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CharmAudioProcessor();
}
