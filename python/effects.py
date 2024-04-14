from typing import List, Callable, Dict, Any
from pathlib import PosixPath

import numpy as np
import cv2


MODEL = None


def segmentation(frame: np.ndarray):
    if MODEL is None:
        # LOAD MODEL
        ...
    raise NotImplementedError


def get_threshold_mask(frame: np.ndarray, threshold: int = 80) -> np.ndarray:
    _, mask = cv2.threshold(
            cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY), threshold, 1,
            cv2.THRESH_BINARY)
    return mask


def apply_mask(frame: np.ndarray, mask: np.ndarray) -> np.ndarray:
    frame[mask == 0] = 0
    # frame[mask == 1] = 255
    return frame


def process_video(outdir: PosixPath,
                  root: PosixPath,
                  effects: List[Callable],
                  kwags_list: List[Dict[str, Any]]):
    count = 0
    for i, frame in enumerate(root.iterdir()):
        img = cv2.imread(str(frame))
        for effect, kwargs in zip(effects, kwags_list):
            mask = effect(img, **kwargs)
            img = apply_mask(img, mask)
        cv2.imwrite(str(outdir / frame.name), img)
        count += 1
    return count
