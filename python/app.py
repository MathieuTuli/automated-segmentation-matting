from pathlib import Path
import sys

import gi
gi.require_version("Gtk", "4.0")  # NOQA
gi.require_version('Adw', '1')  # NOQA

from gi.repository import Gtk, Adw, GLib, Gio

from utils import get_frames, get_video_reading_dir
from cache import Cache


class FrameNavigator(Gtk.Box):
    def __init__(self, label: str = "", spacing: int = 12, *args, **kwargs):
        super().__init__(orientation=Gtk.Orientation.VERTICAL,
                         spacing=spacing,
                         *args, **kwargs)
        label = Gtk.Label(label=label)
        self.frames_bp = Gtk.Button(label="Previous Frame")
        self.frames_bn = Gtk.Button(label="Next Frame")
        self.canvas = Gtk.Image()

        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        box.set_spacing(12)
        box.append(self.frames_bp)
        box.append(self.frames_bn)
        self.append(label)
        self.append(box)
        self.append(self.canvas)


class MainWindow(Gtk.ApplicationWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # DEFAULTS
        self.set_default_size(600, 250)
        self.set_title("Main Window")

        # LAYOUT
        main = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        upload_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        vis_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        effects_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)

        main.set_spacing(12)
        upload_row.set_spacing(12)
        vis_row.set_spacing(12)
        effects_row.set_spacing(12)

        self.set_child(main)
        main.append(upload_row)
        main.append(vis_row)
        main.append(effects_row)

        # WINDOW
        # self.win_ctrls = Gtk.WindowControls()
        # main.append(self.win_ctrls)

        # UPLOAD ROW
        self.video_upload = Gtk.FileDialog.new()
        self.video_upload.set_title("Select a File")
        button = Gtk.Button(label="upload video")
        button.connect('clicked', self.show_video_upload_dialog)
        self.spinner = Gtk.Spinner()
        upload_row.append(button)
        upload_row.append(self.spinner)

        # VIS ROW
        vr_b1 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        vr_b2 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        vr_b1.set_spacing(12)
        vr_b1.set_spacing(12)
        vis_row.append(vr_b1)
        vis_row.append(vr_b2)

        raw_frame_nav = FrameNavigator("Original Frames")
        proc_frame_nav = FrameNavigator("Processed Frames")
        vr_b1.append(raw_frame_nav)
        vr_b2.append(proc_frame_nav)

        # EFFECTS ROw
        er_b1 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        er_b1.set_spacing(12)
        effects_row.append(er_b1)

        process_button = Gtk.Button(label="process")
        # add_effect_button = Gtk.MenuButton(label="add effect")
        er_b1.append(process_button)
        # er_b1.append(add_effect_button)

        # ACTION -------------------------------------------------------------
        action = Gio.SimpleAction.new("something", None)
        action.connect("activate", self.print_something)
        self.add_action(action)
        menu = Gio.Menu.new()
        menu.append("Do Something", "win.something")
        popover = Gtk.PopoverMenu()
        popover.set_menu_model(menu)
        hamburger = Gtk.MenuButton(direction=Gtk.ArrowType.RIGHT)
        hamburger.set_popover(popover)
        hamburger.set_icon_name("list-add-symbolic")
        er_b1.append(hamburger)
        # --------------------------------------------------------------------
        self.cache = Cache()

    def print_something(self, action, param):
        print("Something!")

    def show_video_upload_dialog(self, button):
        self.video_upload.open(self, None, self.open_video_upload_callback)

    def open_video_upload_callback(self, dialog, result):
        try:
            file = dialog.open_finish(result)
            if file is not None:
                print(f"File path is {file.get_path()}")
                self.spinner.start()
                fname = Path(file.get_path())
                outdir = get_video_reading_dir(fname)
                self.cache.video_dir = outdir
                outdir.mkdir(parents=True)
                get_frames(fname, outdir)
                self.spinner.stop()
                # TODO DISPLAY FIRST FRAME
        except GLib.Error as error:
            print(f"Error opening file: {error.message}")


class ASMApp(Adw.Application):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.connect('activate', self.on_activate)

    def on_activate(self, app):
        self.win = MainWindow(application=app)
        self.win.present()


if __name__ == "__main__":
    app = ASMApp()
    app.run(sys.argv)
