from typing import Optional
from argparse import ArgumentParser, Namespace
from Pathlib import Path, PosixPath

import logging

import numpy as np
import cv2

parser = ArgumentParser()
parser.add_argument("--video", type=str, required=True)
parser.add_argument("--threshold", type=int, default=80)
parser.add_argument("--output-dir", type=str, default="./output")


def get_frames(video_path: PosixPath, save_dir: Optional[PosixPath] = None):
    logging.info("Reading Video")
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


def main(args: Namespace):
    frames = get_frames(Path(args.video))
    outdir = Path(args.output_dir)
    outdir.mkdir(exist_ok=True, parents=True)
    for i, frame in enumerate(frames):
        # TODO : compute segmented mask for segments, and zero out pixels
        # segmasks = Net(frame)
        # frame[np.where(segmask == 0)] = 0
        # frame[np.where(segmask == 0)] = 0
        # then do per-segement thresholding
        _, mask = cv2.threshold(
                cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY), args.threshold, 1,
                cv2.THRESH_BINARY)
        frame[mask == 0] = 0
        frame[mask == 1] = 255
        cv2.imwrite(str(outdir / f"{i:04d}.png"))


if __name__ == "__main__":
    args = parser.parse_args()
    main(args)
