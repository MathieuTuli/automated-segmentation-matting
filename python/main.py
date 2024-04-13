from argparse import ArgumentParser, Namespace
from Pathlib import Path

import logging

import cv2

from .utils import get_frames

parser = ArgumentParser()
parser.add_argument("--video", type=str, required=True)
parser.add_argument("--threshold", type=int, default=80)
parser.add_argument("--output-dir", type=str, default="./output")


def main(args: Namespace):
    logging.info("Reading video.")
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
