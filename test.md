
gst-launch -v filesrc location=~/Videos/angle.mp4 ! qtdemux name=demux demux.audio_00 ! queue ! rtpmp4apay pt=97 ! udpsink host=localhost port=5001

gst-launch-1.0 -v filesrc location=~/Videos/s.mkv ! matroskademux name=demux demux.audio_0 ! queue ! rtpmp4apay pt=97 ! udpsink host=localhost port=5001

gst-lauch-1.0 udpsrc udpsink host=localhost port=5001 ! autoaudiosink


gst-launch -v udpsrc port=5000 ! "application/x-rtp,media=(string)audio, clock-rate=(int)44100, width=16, height=16, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, channel-positions=(int)1, payload=(int)96" ! rtpL16depay ! audioconvert ! alsasink device=hw:2,0 sync=false


gst-launch-1.0 -v udpsrc uri=udp://localhost:5001 ! "application/x-rtp,media=(string)audio, clock-rate=(int)44100, width=16, height=16, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, channel-positions=(int)1, payload=(int)96" ! rtpL16depay ! audioconvert ! autoaudiosink

gst-launch-1.0 -v udpsrc uri=udp://localhost:5001 ! "application/x-rtp,media=(string)audio, clock-rate=(int)44100, width=16, height=16, encoding-name=(string)L16, encoding-params=(string)1, channels=(int)1, channel-positions=(int)1, payload=(int)96" ! rtpL16depay ! audioconvert ! alsasink device=hw:2,0 sync=false