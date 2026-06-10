#!/usr/bin/env python3

import os
import json

# Path to the uploads directory, relative to cgi-bin/, one level up
UPLOADS_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), '../www/uploads'
)

def main():
    """Lists files in the uploads directory and returns their name and size as JSON."""
    print('Content-Type: application/json\n')

    files = []

    try:
        for filename in os.listdir(UPLOADS_DIR):
            filepath = os.path.join(UPLOADS_DIR, filename)
            if os.path.isfile(filepath) and not filename.startswith('.'):
                files.append({
                    'name': filename,
                    'size': os.path.getsize(filepath)
                })
    except Exception:
        pass

    print(json.dumps({'files': files}))

if __name__ == '__main__':
    main()
