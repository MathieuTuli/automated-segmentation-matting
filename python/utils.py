from pathlib import PosixPath, Path
from datetime import datetime
from typing import Optional

import cv2


def get_frames(video_path: PosixPath, save_dir: Optional[PosixPath] = None):
    cap = cv2.VideoCapture(str(video_path))
    count = 0
    frames = list()
    while True:
        ret, frame = cap.read()
        if ret:
            if save_dir is not None:
                fname = save_dir / f"{count:04d}.png"
                cv2.imwrite(fname, frame)
            frames.append(frame)
        else:
            break
        count += 1
    return frames


def get_time_now():
    return datetime.datetime.now().strftime("%Y_%m_%d_%Y_%M_%S")


def get_video_reading_dir(fname: PosixPath):
    root = Path("~/.cache/asm/videos").expanduser()
    outdir = root / (fname.stem + "_" + get_time_now())
    return outdir
