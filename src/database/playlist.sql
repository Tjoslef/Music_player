CREATE TABLE songs (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    path TEXT NOT NULL,
    duration INTEGER
);

CREATE TABLE playlists (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL
);

CREATE TABLE playlist_songs (
    playlist_id INTEGER,
    song_id INTEGER,
    position INTEGER,
    FOREIGN KEY (playlist_id) REFERENCES playlists(id),
    FOREIGN KEY (song_id) REFERENCES songs(id)
);
