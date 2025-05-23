# ScreenUploader

ScreenUploader is a tool that periodically uploads the client's screen to the server. The uploaded images are encoded in JPEG format and stored on the server organized by time.

The client is implemented in C++, while the server uses the JavaScript Express framework. It supports both Windows and Linux (X11).

Primarily built for practicing client/server architecture and remote device coordination.



## Usage
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/yuzujr/ScreenUploader)
1. Run server

   **Dependencies**

   Install [node.js](https://nodejs.org/).

   `cd server`

   `npm install`

   `node server.js`

   Server will run on port `4000` by default.

2. Edit `config.json`:

- `server_url` is the address where the server running. (`http://127.0.0.1:4000` by default)
- `interval_seconds` is the interval that client send upload request. (60s by default)
- `max_retries` is the number of retry attempts if upload fails. (3 by default)
- `retry_delay_ms` is the delay between retry attempts in milliseconds. (1000ms by default)
- `add_to_startup` determines whether to add the application to system startup. (false by default)
- `client_id` is the unique identifier for the client. (automatically filled on first run)

   Example `config.json`:
   ```json
   {
     "api": {
       "server_url": "http://127.0.0.1:4000",
       "interval_seconds": 60,
       "max_retries": 3,
       "retry_delay_ms": 1000,
       "add_to_startup": false,
       "client_id": ""
     }
   }
   ```

3. Place this `config.json` next to the executable.

4. Run `ScreenUploader` executable.

5. Check the server terminal, you will see the upload logs.
   - Screenshots is saved in `uploads` folder
   - Logs are saved in `logs` folder.
   - Client's config is saved as `uploads/client_id or alias(alias first)/config.json`.

6. Access the server on port 4000 to enter the control panel，you can send commands to clients and view their screenshots.
   ![image](https://github.com/user-attachments/assets/f273fe8f-3650-4c3a-b564-bc78b01b77ca)


8. (Optional) Edit `clients.json` to alias the client ID to a name. This is useful for identifying clients in the server logs.

   Example `clients.json`:
   ```json
   {
     "client_id": "client_name"
   }
   ```



## Build from source

### Client

For windows, msvc compiler is recommended.

For Linux, gcc compiler is recommended.

**Dependencies**

1. Install [cmake](https://cmake.org/download/) (version `3.16` minimum required).

2. For linux, install `libx11`, `libssl`, `libcurl`.

   Ubuntu:

   `sudo apt install libx11-dev libssl-dev libcurl4-openssl-dev`

**Build**

In this step, a proper network connection to github is required.

Set `http_proxy` and `https_proxy` if possible.

Thirdparty libraries `cpr`, `libjpeg-turbo`, `nlohmann_json`, `spdlog` depend on this.

1. `mkdir build && cd build`

2. `cmake .. -DCMAKE_BUILD_TYPE=Release`
   
   > For windows, you can pass '-DUSE_WIN32_GUI=ON' to avoid console window.
   
3. `cmake --build . --config Release`
