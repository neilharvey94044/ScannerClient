

# Cross-Platform Uniden Scanner Client

### Build Steps
- To generate for Debug build:
```bash
cmake -S ./src -B ./build -DCMAKE_BUILD_TYPE=Debug
```
- To generate for Release build (default):
```bash
cmake -S ./src -B ./build
```
- To build:
```bash
cmake --build ./build
```
Note: do the above from the top level ScannerClient directory.

### RFCs and Protocols

#### Real-Time Streaming Protocol
https://www.rfc-editor.org/rfc/rfc7826#page-25
https://www.w3.org/2008/WebVideo/Fragments/wiki/UA_Server_RTSP_Communication
https://www.cs.columbia.edu/~hgs/rtsp/draft/draft-ietf-mmusic-rtsp-03.html#SECTION00020000000000000000

#### Real-Time Transport Protocol
https://www.rfc-editor.org/rfc/rfc3550#page-4

#### G.711 Codec
https://en.wikipedia.org/wiki/G.711

#### ITU Software Tools Library on Github
https://github.com/openitu/STL


 