import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib

Gst.init(None)
num_loops = 0

def play_media(file_path):
    pipeline_str = "playbin uri=file://" + file_path
    pipeline = Gst.parse_launch(pipeline_str)

    message_bus = pipeline.get_bus()
    loop = GLib.MainLoop()

    def on_message(bus, message):
        t = message.type
        if t == Gst.MessageType.EOS:
            if num_loops > 0:
                print(f"left run count: {num_loops}")
                pipeline.set_state(Gst.State.READY)
                pipeline.set_state(Gst.State.PLAYING)
                num_loops -= 1
            else:
                print(f"quit because left run count is {num_loops}")
                loop.quit()
        elif t == Gst.MessageType.ERROR:
            err, debug = message.parse_error()
            print("Error: %s" % err)
            loop.quit()
    
    message_bus.add_signal_watch()
    message_bus.connect("message", on_message)

    pipeline.set_state(Gst.State.PLAYING)

    try:
        loop.run()
    except KeyboardInterrupt:
        print("Interrupted by user")

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 3:
        print("Usage: python play_media.py <file_path> <num_loops>")
    else:
        print("note: the file_path should use absolute path")
        file_path = sys.argv[1]
        num_loops = int(sys.argv[2])
        play_media(file_path, num_loops)
