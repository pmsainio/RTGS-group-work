# Granular Synthesizer Plugin

This JUCE-based granular synthesizer plugin breaks audio into small grains and recombines them in real-time, enabling granular time-stretching, pitch-shifting, and texture synthesis.

---

## 🔁 Signal Flow

1. **Audio file loaded** via `GrainAudioProcessor::readFile()`
2. File is stored in `fileBuffer` and passed to the `GrainSynthVoice`
3. A note-on event calls `startNote()`:
    - Envelope and buffer are initialized
    - Grain triggering logic begins
4. `renderNextBlock()` generates audio block-by-block:
    - Grains are launched periodically
    - Each grain reads from the buffer and applies an envelope
5. Envelope shapes grain amplitude (attack, sustain, release)
6. Final output is mixed into the output buffer

---

## 🧩 Main Components

### `GrainAudioProcessor`

- Top-level audio processor
- Manages parameters, audio file loading, and MIDI
- Initializes and connects:
  - `GranSynth` (grain processor)
  - `GrainSynthVoice` (voice logic)

### `GranSynth`

- Holds an array of grain slots
- Each grain contains:
  - Start/end position
  - Envelope (attack, sustain, release)
  - Pitch ratio
- Handles:
  - `trigger()` to launch grains
  - `processBlock()` to render active grains into a buffer

### `GrainSynthVoice`

- Handles MIDI note logic
- Owns `filePositionInSamples`, `grainSize`, `density`, and envelope values
- On each block:
  - Launches grains at interval
  - Passes grain start/end to `GranSynth::trigger()`

---

## 🧠 Key Functions

### `GrainAudioProcessor::readFile(path)`
Loads an audio file into memory and assigns it to the active voice.

### `GrainSynthVoice::startNote(...)`
- Called when a MIDI note is triggered
- Sets pitch and prepares grain system

### `GrainSynthVoice::renderNextBlock(...)`
- Called by the audio thread
- Launches new grains based on interval and position
- Converts `grainSize` from ms → randomized grain length in samples

### `GranSynth::trigger(start, end, pitch)`
- Finds an available grain slot
- Sets pitch, start/end positions
- Calculates envelope durations and starts envelope

### `GrainEnvelope`
- Contains attack, sustain, release times
- Generates envelope shape via cosine ramps
- Called every block for active grains

---

## 🎛 Parameters

| Parameter     | ID          | Description                        |
|---------------|-------------|------------------------------------|
| `filePos`     | filePos     | Position in file (0.0 – 1.0)       |
| `grainLen`    | grainLen    | Max grain length (30–100 ms)       |
| `density`     | density     | Grains per second                  |
| `attack`      | attack      | Envelope attack time (ms)          |
| `sustain`     | sustain     | Sustain level (0–1)                |
| `release`     | release     | Envelope release time (ms)         |

Parameter callbacks live in `PluginProcessor.cpp` and update the voice in real-time.

---

