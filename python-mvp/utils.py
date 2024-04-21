from pathlib import PosixPath, Path
from datetime import datetime
from typing import Optional

import numpy as np
import cv2


def get_image_from_file(fname: PosixPath) -> np.ndarray:
    return cv2.imread(str(fname))


def get_image_fname(root: PosixPath, id: int) -> PosixPath:
    return root / f"{id:04d}.png"


def get_frames(video_path: PosixPath,
               save_dir: Optional[PosixPath] = None) -> np.ndarray:
    cap = cv2.VideoCapture(str(video_path))
    count = 0
    frames = list()
    while True:
        ret, frame = cap.read()
        if ret:
            if save_dir is not None:
                fname = get_image_fname(save_dir, count)
                cv2.imwrite(str(fname), frame)
            frames.append(frame)
        else:
            break
        count += 1
    return np.array(frames)


def get_time_now():
    return datetime.now().strftime("%Y_%m_%d_%Y_%M_%S")


def get_video_reading_dir(fname: PosixPath):
    root = Path("~/.cache/asm/videos").expanduser()
    # outdir = root / (fname.stem + "_" + get_time_now())
    outdir = root / fname.stem
    outdir.mkdir(exist_ok=True, parents=True)
    return outdir
