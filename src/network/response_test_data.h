#include <string>


using namespace std;

string options_response {
"RTSP/1.0 200 OK\r\n"
"Cache-Control: no-cache\r\n"
"CSeq: 2\r\n"
"Public: DESCRIBE, SETUP, TEARDOWN, PLAY, OPTIONS, GET_PARAMETER\r\n"
"Server: Scanner Audio Server 0.0.1\r\n"
"Supported: play.basic, con.persistent\r\n"
"Content-Length: 0\r\n\r\n"
};
string describe_response {
"RTSP/1.0 200 OK\r\n"
"Cache-Control: no-cache\r\n"
"CSeq: 3\r\n"
"Server: Scanner Audio Server 0.0.1\r\n"
"Content-Base: rtsp://192.168.0.173/au:scanner.au/\r\n"
"Content-Length: 157\r\n"
"Content-Type: application/sdp\r\n\r\n"
"v=0\r\n"
"o=- 0000000000 0000000000 IN IP4 127.0.0.1\r\n"
"s=scanner.au\r\n"
"c=IN IP4 0.0.0.0\r\n"
"t=0 0\r\n"
"a=sdplang:en\r\n"
"a=control:*\r\n"
"m=audio 0 RTP/AVP 0\r\n"
"a=control:trackID=1\r\n\r\n"
};

string setup_response {
"RTSP/1.0 200 OK\r\n"
"Cache-Control: no-cache\r\n"
"CSeq: 4\r\n"
"Server: Scanner Audio Server 0.0.1\r\n"
"Session: 66622000\r\n"
"Transport: RTP/AVP;unicast;client_port=54352;source=192.168.0.173;server_port=56002;ssrc=1449463210\r\n"
"Content-Length: 0    \r\n\r\n"
};

string play_response {
"RTSP/1.0 200 OK\r\n"
"Cache-Control: no-cache\r\n"
"CSeq: 5\r\n"
"RTP-Info: url=rtsp://192.168.0.173/au:scanner.au/trackID=1,seq=1,rtptime=0\r\n"
"Server: Scanner Audio Server 0.0.1\r\n"
"Session: 66622000\r\n"
"Content-Length: 0\r\n\r\n"
};

