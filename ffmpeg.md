# ffmpeg 命令

##　-re

 -re (input)
Read input at native frame rate. Mainly used to simulate a grab device, or live input stream (e.g. when reading from a file). Should not be used with actual grab devices or live input streams(where it can cause packet loss).  By default ffmpeg attempts to read the input(s) as fast as possible.  This option will slow down the reading of the input(s) to the native frame rate of the input(s). It is useful for real-time output (e.g. live streaming)