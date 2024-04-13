import sys

import gi
gi.require_version("Gtk", "4.0")
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw, GLib  # NOQA


class MainWindow(Gtk.ApplicationWindow):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.set_default_size(600, 250)
        self.set_title("Main Window")

        self.main = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        self.upload_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        self.vis_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        self.effects_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        self.set_child(self.main)
        self.main.append(self.upload_row)
        self.main.append(self.vis_row)
        self.main.append(self.effects_row)
        self.main.set_spacing(12)
        self.upload_row.set_spacing(12)
        self.vis_row.set_spacing(12)
        self.effects_row.set_spacing(12)

        self.video_upload = Gtk.FileDialog.new()
        self.video_upload.set_title("Select a File")
        self.button = Gtk.Button(label="upload video")
        self.button.connect('clicked', self.show_video_upload_dialog)
        self.spinner = Gtk.Spinner()
        self.upload_row.append(self.button)
        self.upload_row.append(self.spinner)

        frames_label = Gtk.Label(label="Frames")
        self.vis_row.append(frames_label)

    def show_video_upload_dialog(self, button):
        self.video_upload.open(self, None, self.open_video_upload_callback)

    def open_video_upload_callback(self, dialog, result):
        try:
            file = dialog.open_finish(result)
            if file is not None:
                print(f"File path is {file.get_path()}")
                self.spinner.start()
                # TODO Handle loading file from here
                self.spinner.stop()
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
    app = ASMApp(application_id="test")
    app.run(sys.argv)
