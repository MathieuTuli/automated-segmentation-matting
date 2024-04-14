from pathlib import Path, PosixPath
import sys

import gi
gi.require_version("Gtk", "4.0")  # NOQA
gi.require_version('Adw', '1')  # NOQA

from gi.repository import Gtk, Adw, GLib, Gio, Gdk

from utils import (
        get_frames,
        get_video_reading_dir,
        get_image_fname,
        get_image_from_file
        )
from cache import Cache

CACHE = Cache()


class FrameNavigator(Gtk.Box):
    def __init__(self, label: str = "", spacing: int = 12, *args, **kwargs):
        super().__init__(orientation=Gtk.Orientation.VERTICAL,
                         spacing=spacing,
                         hexpand=True,
                         *args, **kwargs)
        label = Gtk.Label(label=label)
        self.frames_bp = Gtk.Button(label="Previous Frame")
        self.frames_bn = Gtk.Button(label="Next Frame")
        self.frames_bp.connect('clicked', self.previous_frame)
        self.frames_bn.connect('clicked', self.next_frame)
        self.canvas = Gtk.Image(hexpand=True)
        self.frame = 0
        self.root = None
        self.max_frames = None

        self.frames_bp.set_css_classes(["border"])
        self.frames_bn.set_css_classes(["border"])
        self.canvas.set_css_classes(["border"])

        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, halign="center")
        box.set_spacing(12)
        box.append(self.frames_bp)
        box.append(self.frames_bn)
        self.append(label)
        self.append(box)
        self.append(self.canvas)

        self.cwidth = None

    def previous_frame(self, button):
        self.frame = max(0, self.frame - 1)
        self.set_frame(
            get_image_fname(self.root, self.frame))

    def next_frame(self, button):
        self.frame = min(self.max_frames, self.frame + 1)
        self.set_frame(
            get_image_fname(self.root, self.frame))

    def set_frame(self, fname: PosixPath):
        self.canvas.set_from_file(str(fname))
        img = get_image_from_file(fname)
        h, w, c = img.shape
        if self.cwidth is None:
            self.cwidth = self.canvas.get_width()
        self.canvas.set_size_request(self.cwidth, int(self.cwidth * h / w))


class MainWindow(Gtk.ApplicationWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # DEFAULTS
        self.set_default_size(600, 250)
        self.set_title("Main Window")

        # STYLE
        css_provider = Gtk.CssProvider()
        css_provider.load_from_path('style.css')
        Gtk.StyleContext.add_provider_for_display(
                Gdk.Display.get_default(),
                css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

        # LAYOUT
        main = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        upload_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        vis_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        effects_row = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)

        main.set_css_classes(["border"])
        upload_row.set_css_classes(["rows", "border"])
        vis_row.set_css_classes(["rows", "border"])
        effects_row.set_css_classes(["rows", "border"])

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

        self.raw_frame_nav = FrameNavigator("Original Frames")
        self.proc_frame_nav = FrameNavigator("Processed Frames")
        self.raw_frame_nav.set_css_classes(["border"])
        self.proc_frame_nav.set_css_classes(["border"])
        vr_b1.append(self.raw_frame_nav)
        vr_b2.append(self.proc_frame_nav)

        # EFFECTS ROw
        er_b1 = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        er_b2 = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        er_b1.set_spacing(12)
        er_b2.set_spacing(12)
        effects_row.append(er_b1)
        effects_row.append(er_b2)

        label = Gtk.Label(label="run:")
        er_b1.append(label)

        process_button = Gtk.Button(label="process video")
        process_button.connect("clicked", self.process_video)
        er_b1.append(process_button)

        label = Gtk.Label(label="add effects:")
        er_b2.append(label)

        segmentation_button = Gtk.Button(label="thresholding")
        threshold_button = Gtk.Button(label="segmentation")
        segmentation_button.connect("clicked", self.add_segmentation_effect)
        threshold_button.connect("clicked", self.add_thresholding_effect)
        er_b2.append(segmentation_button)
        er_b2.append(threshold_button)

        self.effects = list()

        # ACTION -------------------------------------------------------------
        # action = Gio.SimpleAction.new("something", None)
        # action.connect("activate", self.print_something)
        # self.add_action(action)
        # menu = Gio.Menu.new()
        # menu.append("thresholding", "win.something")
        # popover = Gtk.PopoverMenu()
        # popover.set_menu_model(menu)
        # hamburger = Gtk.MenuButton(direction=Gtk.ArrowType.RIGHT)
        # hamburger.set_popover(popover)
        # hamburger.set_icon_name("list-add-symbolic")
        # er_b1.append(hamburger)
        # --------------------------------------------------------------------

    def print_something(self, action, param):
        print("Something!")

    def process_video(self, button):
        raise NotImplementedError

    def add_thresholding_effect(self, button):
        raise NotImplementedError

    def add_segmentation_effect(self, button):
        raise NotImplementedError

    def show_video_upload_dialog(self, button):
        self.video_upload.open(self, None, self.open_video_upload_callback)

    def open_video_upload_callback(self, dialog, result):
        try:
            file = dialog.open_finish(result)
            if file is not None:
                print(f"File path is {file.get_path()}")
                # TODO THIS KEEPS THE DIALOG OPEN
                # NEED TO ASYNC LOAD THE VIDEO?
                self.spinner.start()
                fname = Path(file.get_path())
                outdir = get_video_reading_dir(fname)
                CACHE.raw_video_dir = outdir
                frames = get_frames(fname, outdir)
                self.raw_frame_nav.root = outdir
                self.raw_frame_nav.max_frames = frames.shape[0] - 1
                self.spinner.stop()
                # TODO DISPLAY FIRST FRAME
                self.raw_frame_nav.set_frame(
                    get_image_fname(outdir, self.raw_frame_nav.frame))
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
