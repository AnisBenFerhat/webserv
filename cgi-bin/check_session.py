#!/usr/bin/env python3

import os
import json
import time

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
    """Validates the session cookie against the database and outputs JSON status."""
    print('Content-Type: application/json\n')

    session_id = get_session_id()
    sessions = load_sessions()

    if session_id and session_id in sessions:
        session = sessions[session_id]

        if int(time.time()) < session.get('expires_at', 0):
            print(json.dumps({'valid': True, 'username': session['username']}))
        else:
            sessions.pop(session_id, None)
            save_sessions(sessions)
            print(json.dumps({'valid': False}))
    else:
        print(json.dumps({'valid': False}))

if __name__ == "__main__":
    main()
