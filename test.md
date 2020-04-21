
gst-launch -v filesrc location=~/Videos/angle.mp4 ! qtdemux name=demux demux.audio_00 ! queue ! rtpmp4apay pt=97 ! udpsink host=localhost port=5001

gst-launch-1.0 -v filesrc location=~/Videos/s.mkv ! matroskademux name=demux demux.audio_0 ! queue ! rtpmp4apay pt=97 ! udpsink host=localhost port=5001
