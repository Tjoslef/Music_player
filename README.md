# Music Player

A lightweight music player for WAV files with playlist management capabilities,
built using SQLite for data storage and OpenAL for audio playback.

## Features

- **WAV File Playback**
  - Supports playback of WAV audio files through OpenAL
  - Basic playback controls (play, pause, stop)

- **Playlist Management**
  - Create and manage multiple playlists using SQLite
  - Add/remove songs from playlists
  - Persistent storage of playlist data

- **Technical Stack**
  - **SQLite3**: Embedded database for playlist storage
  - **OpenAL**: Cross-platform audio API for low-latency playback
  - **C99**: Core implementation language for performance
