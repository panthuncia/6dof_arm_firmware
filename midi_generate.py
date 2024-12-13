import mido
from mido import Message, MidiFile, MidiTrack


def create_valid_midi(file_name):
    """Creates a MIDI file with two non-overlapping tracks."""
    mid = MidiFile(ticks_per_beat=480)
    track = MidiTrack()
    string_track = MidiTrack()
    mid.tracks.append(track)
    #mid.tracks.append(string_track)
    
    # Define the duration of notes and rest between them (in ticks)
    note_duration = 480  # One beat
    rest_duration = 1440  # Three beats
    time_between_notes = note_duration + rest_duration  # Total time per note

    # Middle C (C4) is MIDI note number 60
    notes_sequence = [
        [60],          # Single note
        [62],          # Single note
        #[],
        #[],
        [64, 67],      # Note pair within one octave
        [65],          # Single note
        [67, 69],      # Note pair within one octave
        [71],          # Single note
    ]

    notes_2 = [
        [],
        [],
        [64],
        [64],
        []
    ]
    
    # Track running time for each track
    current_time_track_1 = 0
    current_time_track_2 = 0

    # Generate notes for track 1
    for notes in notes_sequence:
        if notes:
            # Add the time delay since the last note
            track.append(Message('note_on', note=notes[0], velocity=64, time=current_time_track_1))
            # Add additional notes with no time delay
            for note in notes[1:]:
                track.append(Message('note_on', note=note, velocity=64, time=0))
            
            # Note off events
            for note in notes:
                track.append(Message('note_off', note=note, velocity=64, time=note_duration if note == notes[0] else 0))
            
            # Update the running time for the next event
            current_time_track_1 = rest_duration
        else:
            # If no notes, just add the rest duration
            current_time_track_1 += time_between_notes

    # Generate notes for track 2
    # for notes in notes_2:
    #     if notes:
    #         # Add the time delay since the last note
    #         string_track.append(Message('note_on', note=notes[0], velocity=64, time=current_time_track_2))
    #         # Add additional notes with no time delay
    #         for note in notes[1:]:
    #             string_track.append(Message('note_on', note=note, velocity=64, time=0))
            
    #         # Note off events
    #         for note in notes:
    #             string_track.append(Message('note_off', note=note, velocity=64, time=note_duration if note == notes[0] else 0))
            
    #         # Update the running time for the next event
    #         current_time_track_2 = rest_duration
    #     else:
    #         # If no notes, just add the rest duration
    #         current_time_track_2 += time_between_notes
    
    mid.save(file_name)
    print(f"Valid MIDI file '{file_name}' created.")

def create_invalid_midi(file_name):
    """Creates a MIDI file that violates validation conditions."""
    mid = MidiFile()
    track = MidiTrack()
    mid.tracks.append(track)
    
    note_duration = 480
    time_between_notes = note_duration  # One beat
    
    notes_sequence = [
        [60, 73],       # Note pair exceeding one octave
        [60, 62, 64],   # Three notes at once
        [],             # No notes (rest)
        [65],           # Single note
        [67, 69],       # Valid note pair
        [71],           # Single note
        [60],           # Rapid movement (will violate speed condition)
    ]
    
    current_time = 0
    for notes in notes_sequence:
        if notes:
            for note in notes:
                track.append(Message('note_on', note=note, velocity=64, time=0))
            track.append(Message('note_off', note=notes[0], velocity=64, time=note_duration))
            for note in notes[1:]:
                track.append(Message('note_off', note=note, velocity=64, time=0))
        else:
            track.append(Message('note_on', note=0, velocity=0, time=note_duration))
        track.append(Message('note_on', note=0, velocity=0, time=time_between_notes - note_duration))

    mid.save(file_name)
    print(f"Invalid MIDI file '{file_name}' created.")

def main():
    create_valid_midi('valid_midi.mid')
    create_invalid_midi('invalid_midi.mid')

if __name__ == "__main__":
    main()
