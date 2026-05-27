#!/usr/bin/env python3
import signal
import socket
import subprocess
import time
import http.client
import sys

# Color codes (Fixed the typo in RED)
PURPLE = "\033[1;35m[TEST]\033[0m"
YELLOW = "\033[1;33m[TEST]\033[0m"
RED = "\033[1;31m[TEST]\033[0m"

def run_tests():
    print(f"{PURPLE} Starting Tests...")
    
    # 1. Launch your server as a background process
    server_process = subprocess.Popen(
        ["./webserv", "conf/default.conf"], 
        cwd="..",  # Forces the binary to execute from the root directory context!
        stdout=None, 
        stderr=None
    )
    
    # Dynamic wait loop (up to 3 seconds)
    server_ready = False
    for _ in range(30): # 30 attempts * 0.1s = 3 seconds max
        if server_process.poll() is not None:
            print(f"{RED} ❌ Server crashed immediately on startup!")
            break
        try:
            # Try a quick handshake connection
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(0.1)
            s.connect(("localhost", 8080))
            s.close()
            server_ready = True
            break
        except socket.error:
            time.sleep(0.1)

    if not server_ready:
        print(f"{RED} ❌ Could not connect to the server.")
        server_process.terminate()
        sys.exit(1)
    
    success = True
    try:
        # 2. Define and send your custom requests
        print(f"{PURPLE} 🛰️  Sending test request: GET /index.html on port 8080")
        conn = http.client.HTTPConnection("localhost", 8080)
        
        headers = {"Host": "localhost", "User-Agent": "TestAutomationScript"}
        conn.request("GET", "/index.html", headers=headers)
        
        response = conn.getresponse()
        body = response.read().decode()

        # 3. Assert your expectations
        if "Test" not in body:
            print(f"{RED} ❌ Test Failed: Response body didn't match expected content")
        else:
            print(f"{PURPLE} ✅ Test Passed!")
            print(f"{PURPLE} 📥 Body: {body}")
        if "there" not in body:
            print(f"{PURPLE} ⚠️  End part missing, long messages aren't supported for now.")

        print(f"\n{PURPLE} 🛰️  Sending test request: GET /index.html on port 8000")
        conn = http.client.HTTPConnection("localhost", 8000)
        
        headers = {"Host": "localhost", "User-Agent": "TestAutomationScript"}
        conn.request("GET", "/index.html", headers=headers)
        
        response = conn.getresponse()
        body = response.read().decode()

        # 3. Assert your expectations
        if "Test" not in body:
            print(f"{RED} ❌ Test Failed: Response body didn't match expected content")
        else:
            print(f"{PURPLE} ✅ Test Passed!")
            print(f"{PURPLE} 📥 Body: {body}")
        if "there" not in body:
            print(f"{PURPLE} ⚠️  End part missing, long messages aren't supported for now.")


    except Exception as e:
        print(f"{RED} 💥 Test script encountered an error: {e}")
        success = False

    finally:
        # 4. Safely check if the process is still running before killing it
        if server_process.poll() is None:
            print(f"\n{PURPLE} 🛑 Sending SIGINT (Ctrl + C) to C++ Web Server...")
            
            # This sends the exact same signal as pressing Ctrl+C in the terminal
            server_process.send_signal(signal.SIGINT)
            
            try:
                # Give it up to 3 seconds to execute its cleanup blocks and exit cleanly
                server_process.wait(timeout=3)
                print(f"{PURPLE} ✅ Server shut down gracefully.")
            except subprocess.TimeoutExpired:
                # Fallback if your C++ server gets stuck in an infinite loop and ignores SIGINT
                print(f"{YELLOW} ⚠️ Server did not respond to SIGINT, forcing shutdown...")
                server_process.kill()
        
    if not success:
        sys.exit(0)

if __name__ == "__main__":
    run_tests()
