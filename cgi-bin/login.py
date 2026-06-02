#!/usr/bin/env python3

import os
import sys
import json
import time

SESSIONS_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../www/data/sessions.json')
CREDENTIALS = {'admin': 'webserv42'}
SESSION_EXPIRATION = 3600

def load_sessions():
    """Loads sessions.json. Returns an empty dict {} if missing or corrupted."""
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

def parse_post_body():
    """Reads the POST body from stdin and returns a dictionary of the submitted form data."""
    content_length = int(os.environ.get('CONTENT_LENGTH', 0) or 0)
    body_content = sys.stdin.read(content_length) if content_length > 0 else ''

    form_data = {}
    for pair in body_content.split('&'):
        if '=' in pair:
            key, value = pair.split('=', 1)
            form_data[key] = value.replace('+', ' ')

    return form_data

def main():
    """Parses credentials, validates them, creates a session, and redirects."""
    form_data = parse_post_body()
    username = form_data.get('username', '')
    password = form_data.get('password', '')

    if CREDENTIALS.get(username) == password:
        session_id = os.urandom(16).hex()
        sessions = load_sessions()
        current_timestamp = int(time.time())

        sessions[session_id] = {
            'username': username,
            'created_at': current_timestamp,
            'expires_at': current_timestamp + SESSION_EXPIRATION
        }

        save_sessions(sessions)

        print('Status: 302 Found')
        print('Location: /admin/index.html')
        print(f'Set-Cookie: session_id={session_id}; HttpOnly; Max-Age={SESSION_EXPIRATION}; Path=/')
        print('Content-Type: text/html\n')

    else:
        print('Status: 302 Found')
        print('Location: /?login_error=1')
        print('Content-Type: text/html\n')

if __name__ == "__main__":
    main()
