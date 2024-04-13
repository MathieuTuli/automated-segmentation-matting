from typing import Optional
from Pathlib import PosixPath

import numpy as np
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
