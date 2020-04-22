# test cmd

## aac over rtp

### gst

gst-launch-1.0 -v filesrc location=~/Videos/angle.mp4 ! qtdemux name=demux demux.audio_00 ! queue ! rtpmp4apay pt=97 ! udpsink host=192.168.1.140 port=5001

gst-launch-1.0 -v filesrc location=~/Videos/hunter.mkv ! matroskademux name=demux demux.audio_0 ! queue ! rtpmp4apay pt=97 ! udpsink host=localhost port=5001

gst-launch-1.0 udpsrc udpsink host=localhost port=5001 ! autoaudiosink

gst-launch -v udpsrc port=5000 ! "application/x-rtp,media=(string)audio, clock-rate=(int)44100, width=16, height=16, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, channel-positions=(int)1, payload=(int)96" ! rtpL16depay ! audioconvert ! alsasink device=hw:2,0 sync=false

gst-launch-1.0 -v udpsrc uri=udp://localhost:5001 ! "application/x-rtp,media=(string)audio, clock-rate=(int)44100, width=16, height=16, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, channel-positions=(int)1, payload=(int)96" ! rtpL16depay ! audioconvert ! autoaudiosink

gst-launch-1.0 -v udpsrc uri=udp://localhost:5001 ! "application/x-rtp,media=(string)audio, clock-rate=(int)44100, width=16, height=16, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, channel-positions=(int)1, payload=(int)96" ! rtpL16depay ! audioconvert ! alsasink device=hw:2,0 sync=false

## ffmpeg

推到广播:`ffmpeg -re -i Videos/hunter.mkv -vn -ac 1 -ar 16000 -acodec aac  -f rtp rtp://233.233.233.223:6666>test.sdp`
播放:`ffplay -v 64 -protocol_whitelist "file,http,https,rtp,udp,tcp,tls" ./test.sdp`
