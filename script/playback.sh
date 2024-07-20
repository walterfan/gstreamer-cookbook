
video_path="`pwd`/../video"
video_file="talk.mp4"
loop_count=86400

if [ $# -gt 0 ]; then
    video_file=$1
    echo "video file : $video_path/$video_file"
fi

if [ $# -gt 1 ]; then
    loop_count=$2
    echo "loop_count : $loop_count"
fi
echo "playback $video_path/$video_file $loop_count"
DISPLAY=:0 python3 media_player.py $video_path/$video_file $loop_count