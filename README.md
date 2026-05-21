# Lumina-Flow
Generative light engine for ESP32-C6.

## Description
Lumina-Flow is an interactive, generative light control system. Instead of static patterns, it utilizes a "light engine" that calculates smooth, non-repeating color transitions using linear interpolation, inspired by natural atmospheric shifts like sunsets.

## Features
- **Generative Engine**: Smooth color and brightness transitions using LERP math.
- **Interactive Controls**: Two-button interface for brightness and palette customization.
- **Combo Mode**: Simultaneous button press to boost animation speed.
- **Natural Motion**: Inertial color shifting for an "ethereal" visual experience.

## Hardware
- ESP32-C6-DevKitC-1
- 2x Tactile Buttons (connected to GPIO 9 and 10 with internal PULLUP).

## Usage
- **Button 1**: Cycle brightness (20 -> 100 -> 255).
- **Button 2**: Randomize hue target.
- **Both**: Boost transition speed.

## License
Distributed under the MIT License. See `LICENSE` for more information.
