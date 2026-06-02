#!/usr/bin/env python3

import os
import json

SESSIONS_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../www/data/sessions.json')

def get_session_id():
    """Extracts the session_id from the HTTP_COOKIE environment variable."""
    cookie_header = os.environ.get('HTTP_COOKIE', '')

    for part in cookie_header.split(';'):
        part = part.strip()
        if part.startswith('session_id='):
            return part[len('session_id='):]

    return None

def load_sessions():
    """Loads sessions.json, returning an empty dict {} if missing or corrupted."""
    try:
        with open(SESSIONS_FILE, 'r') as session_file:
            return json.load(session_file)
    except Exception:
        return {}

def save_sessions(sessions):
    """Writes data to sessions.json, creating parent directories if needed."""
    os.makedirs(os.path.dirname(SESSIONS_FILE), exist_ok=True)
    with open(SESSIONS_FILE, 'w') as session_file:
        json.dump(sessions, session_file)
        session_file.write('\n')

def main():
    """Deletes the server-side session and expires the client-side cookie."""
    session_id = get_session_id()

    if session_id:
        sessions = load_sessions()
        sessions.pop(session_id, None)
        save_sessions(sessions)

    print('Status: 302 Found')
    print('Location: /index.html')
    print('Set-Cookie: session_id=; HttpOnly; Max-Age=0; Path=/')
    print('Content-Type: text/html\n')

if __name__ == "__main__":
    main()
