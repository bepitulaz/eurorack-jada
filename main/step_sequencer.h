#ifndef STEP_SEQUENCER_H
#define STEP_SEQUENCER_H

typedef struct {
    int step;
    double pitch;
    int gate;
} StepData;

// Initializes the step sequencer with the specified number of steps, CV outputs, and gate outputs.
void step_sequencer_init();

/* For steps data management */
// Sets the pitch value (in Hertz) for a specific step.
void set_step_pitch(int step, double pitch);
// Retrieves the pitch value for a specific step.
double get_step_pitch(int step);
// Sets the gate value (on/off) for a specific step.
void set_step_gate(int step, int gate_value);
// Retrieves the gate value for a specific step.
int get_step_gate(int step);

/* For playback control */
// Starts playback of the sequence.
void step_sequencer_play();
// Stops playback of the sequence.
void step_sequencer_stop();
// Retrieves the currently playing step index (0 to num_steps-1).
int get_current_step();
// Sets the loop mode for playback (e.g., forward, backward, ping-pong).
void set_loop_mode(int loop_mode);
// Clears all pitch and gate values in the sequence.
void clear_sequence();

/* Clock integration */
// Sets the clock source (internal or external)
void set_clock_source(int clock_source);
// Sets the desired tempo (beats per minute) for internal clock generation.
void set_tempo(int bpm);

#endif // STEP_SEQUENCER_H