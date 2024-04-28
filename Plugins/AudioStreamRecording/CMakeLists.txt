set(audioStreamRecordingFiles
    audio-stream-recording.cpp
    audio-stream-recording.hpp
)
add_library(audioStreamRecording ${audioStreamRecordingFiles})

target_link_libraries(audioStreamRecording PRIVATE Winmm)