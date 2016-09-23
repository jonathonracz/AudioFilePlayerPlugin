# AudioFilePlayerPlugin
JUCE-powered audio plugin for playing audio files for test environments.

This is a plugin based on the audio file player example from the JUCE demo. This was created because I needed a plugin capable of:
- Playing audio files of various formats
- Lightweight, with none of the frills of a full fledged sampler
- Open source
- Capable of loading/saving parameters including the audio file loaded
- Cross platform.

I couldn't find anything online that met these requirements, so I wrote this. I use it in conjunction with the JUCE plugin host example to create a fast and lightweight environment for testing and debugging new audio plugins. I'm planning to add new features and pretty it up a bit in the future, but overall it will remain a barebones plugin targeted toward this specific purpose for audio development.
