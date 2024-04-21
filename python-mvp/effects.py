from typing import List, Callable, Dict, Any
from collections import defaultdict
from argparse import ArgumentParser
from pathlib import PosixPath

from torchvision import transforms

import numpy as np
import torch
import yaml
import cv2

from models.xmem.utils.range_transform import im_normalization
from models.xmem.inference.inference_core import InferenceCore
from models.sam import SamPredictor, sam_model_registry
from models.xmem.utils.mask_mapper import MaskMapper
from models.xmem.network import XMem


SAM = None
XMEM = None
MASKS = None

parser = ArgumentParser()
parser.add_argument('--model', default='./saves/XMem.pth')

# Data options
parser.add_argument('--d16_path', default='../DAVIS/2016')
parser.add_argument('--d17_path', default='../DAVIS/2017')
parser.add_argument('--y18_path', default='../YouTube2018')
parser.add_argument('--y19_path', default='../YouTube')
parser.add_argument('--lv_path', default='../long_video_set')
# For generic (G) evaluation, point to a folder that contains "JPEGImages" and "Annotations"
parser.add_argument('--generic_path')

parser.add_argument('--dataset', help='D16/D17/Y18/Y19/LV1/LV3/G', default='D17')
parser.add_argument('--split', help='val/test', default='val')
parser.add_argument('--output', default=None)
parser.add_argument('--save_all', action='store_true', 
            help='Save all frames. Useful only in YouTubeVOS/long-time video', )

parser.add_argument('--benchmark', action='store_true', help='enable to disable amp for FPS benchmarking')
        
# Long-term memory options
parser.add_argument('--disable_long_term', action='store_true')
parser.add_argument('--max_mid_term_frames', help='T_max in paper, decrease to save memory', type=int, default=10)
parser.add_argument('--min_mid_term_frames', help='T_min in paper, decrease to save memory', type=int, default=5)
parser.add_argument('--max_long_term_elements', help='LT_max in paper, increase if objects disappear for a long time', 
                                                type=int, default=10000)
parser.add_argument('--num_prototypes', help='P in paper', type=int, default=128)

parser.add_argument('--top_k', type=int, default=30)
parser.add_argument('--mem_every', help='r in paper. Increase to improve running speed.', type=int, default=5)
parser.add_argument('--deep_update_every', help='Leave -1 normally to synchronize with mem_every', type=int, default=-1)

# Multi-scale options
parser.add_argument('--save_scores', action='store_true')
parser.add_argument('--flip', action='store_true')
parser.add_argument('--size', default=480, type=int, 
            help='Resize the shorter side to this size. -1 to use original resolution. ')

args = parser.parse_args()
config = vars(args)
config['enable_long_term'] = not config['disable_long_term']

def segmentation(fid: int, frame: np.ndarray, prompts):
    global SAM
    global XMEM
    global MAPPER
    global MASKS
    global config
    if SAM is None:
        model = sam_model_registry["default"](checkpoint="ckpts/sam_vit_h_4b8939.pth")
        SAM = SamPredictor(model)
    if XMEM is None:
        network = XMem(config, "./ckpts/XMem.pth").cuda().eval()
        model_weights = torch.load("./ckpts/XMem.pth")
        network.load_weights(model_weights, init_as_zero_if_needed=True)
        with open("models/xmem/config.yaml", 'r') as stream:
            config2 = yaml.safe_load(stream)
        XMEM = InferenceCore(network, config2)
        MAPPER = MaskMapper()

    samprompts = defaultdict(list)
    h, w, c = frame.shape
    for p in prompts:
        samprompts[p[0]].append((
            [p[1] * w, p[2] * h],
            1
            ))
    mask = None
    if fid in samprompts:
        prompts = samprompts[fid]
        SAM.set_image(frame, "RGB")
        masks, _, _ = SAM.predict(np.array([x[0] for x in prompts]),
                                  np.array([x[1] for x in prompts]))

        mask = masks.sum(0)
    #     mask = MAPPER.convert_mask(mask)
    #     XMEM.set_all_labels(list(MAPPER.remappings.values()))
    #     # MASK = mask

    # im_transform = transforms.Compose([
    #     transforms.ToTensor(),
    #     im_normalization,
    # ])
    # frame = im_transform(frame).to("cuda")
    # if mask is not None:
    #     probs, _ = XMEM.step(frame, torch.Tensor(mask[0]).to("cuda"), mask[1])
    # else:
    #     probs, _ = XMEM.step(frame, None, None)
    # out_mask = torch.argmax(probs, dim=0)
    # out_mask = (out_mask.detach().cpu().numpy()).astype(np.uint8)

    # mask = np.zeros_like(out_mask)
    # for k, v in MAPPER.remappings.items():
    #     mask[out_mask == v] = k
    # # mask, _, _ = XMEM.track(frame, mask)
    return mask


def get_threshold_mask(fid: int, frame: np.ndarray, threshold: int) -> np.ndarray:
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
    if XMEM is not None:
        XMEM.clear_memory()
    for i, frame in enumerate(root.iterdir()):
        img = cv2.imread(str(frame))
        for effect, kwargs in zip(effects, kwags_list):
            mask = effect(int(frame.stem), img, **kwargs)
            if mask is not None:
                img = apply_mask(img, mask)
                cv2.imwrite(str(outdir / ("mask" + frame.name)),
                            np.full_like(frame, 255, dtype=np.uint8) * mask)
        cv2.imwrite(str(outdir / frame.name), img)
        count += 1
    if XMEM is not None:
        XMEM.clear_memory()
    print("done processing")
    return count
