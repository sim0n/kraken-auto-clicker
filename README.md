## Kraken Clicker
Kraken is an open-source automatic clicker with an abstraction layer that will make it very easy for any software developer to tailor it for their needs

### Introduction
1. Features
    1. Double Click Prevention
    1. Randomly generated delays on every startup
    1. Supports most Minecraft clients and versions out of the box
    
## Usage
1. Consumer
    1. Run the *kraken.exe* application
    1. Press the toggle key "*GRAVE*"
    1. Hold down the left mouse button

1. Developer
    1. Adding recorded input
        1. First record your clicks (both press and release)
        1. Serialize the click data you have recorded to match a sequence container with an element of ```std::pair<int, int>```
        1. Add the serialized click data in [recorded_clicks.h](/recorded_clicks.h)
    1. Adding your randomization engine or randomization algorithm to the randomly generated click delays
        1. The randomly generated click delays are defined in [kraken.cpp#L30](/kraken.cpp#L30) so that's where you'll have to go
    1. Installing your own double click prevention callback
        1. Due to Kraken's simple abstraction layer it will make it very easy for you to install your double click prevention which is defined in [kraken.cpp#L65](/kraken.cpp#L65)
        1. Simply switch out the ```std::nullopt``` with your double click prevention function
    1. Changing toggle key bind
        1. The toggle keybind is defined in [clicker.cpp#L130](/clicker.cpp#L130)