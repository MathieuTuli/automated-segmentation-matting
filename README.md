# Automated Segmentation Masking

This repo is a tool for automated semgentation masking. 

It's born out of a recent [post by James Lee](https://x.com/JamesLee03/status/1777684860938928225) where he needed help segmenting a very specific workflow.
This is obviously a common problem in the world of animation, and often requires custom solutions on a case-by-case basis.
That being said, I attempted to do something for this and found a workflow I think could be generalizable, so I'm making a tool for it.

I'm aware that [Track Anything](https://github.com/gaomingqi/Track-Anything) exists, but I don't find it has all the features I want nor is it user-friendly. It still requires things like a python installation and a basic understanding of code and tuning of the parameters. My goal is to abstract all that away and (a) make something anyone can use, (b) make something with a more complete feature set, and (c) hopefully also improve on some of the computational burdens (I have a few ideas).

My very quick PoC is here:


https://github.com/MathieuTuli/automated-segmentation-matting/assets/20361597/59b2d6a2-df77-4c97-a42b-514a05c932be

## updates
- ~I've decided to build this out in GTK: could've gone with Qt but for now I just want something simple and easy to get started.~
  - ~For now, I will make a mvp in Python just to get things working and not have to deal with ONNX just yet. Once it's in a good state, I will convert to C++ and build out the app that can actually run without a need for Python as a dependency. And do proper design and control flow etc. etc.~
- Okay the plan is still to make a python mvp, but I how performant the sam web demo was and decided to make this a web-app
  - for segmentation, I'm still going to use [Segment Anything](https://github.com/facebookresearch/segment-anything)
  - for segmentation tracking, I plan on using [XMem](https://github.com/hkchengrex/XMem) - I just need to test if it's web compatible
    - if not, I'll probably need to find an alternative


## the idea

The workflow is simple

1. Use something like [Segment Anything](https://github.com/facebookresearch/segment-anything) to get a segmentation mask of different parts of the video
2. Select the masks you want
3. Here is where you have a choice:
  - You could use those masks out of the box. This was done by Jordan in his [post](https://x.com/JordanPizza1/status/1778050334528553283)
  - You could use threshold the image and then use masks you care about to refine the prediction
4. Iterate on the thresholding

It's a dead simple workflow, but getting things working together requires some tuning and proper tooling would greatly help the process, so that's what this repo aims to do.
You can see from my PoC that it works quite well for a very specific and detailed thing like teeth even.

The tooling should also be able to:

1. Apply different types of thresholds to different masked areas
2. Apply different stages of thresholding (threshold the thresholding)
3. Apply other basic computer vision effects a user might want

There's many ideas and ways you can take this, so I'm going to start simple. I have the code that generates the original PoC obviously but I'm not gonna upload it yet cause it's messy, so stay tuned.
If you're curious, the code basically looks like this:

```Python
import cv2

def main():
    frames = list()
    cap = cv2.VideoCapture("video.mp4")
    # populate frames
    model = SegmentAnything()
    for frame in frames:
        masks = SegmentAnything(frame)
        glasses_mask = masks[0]  # id selected visually in tool
        mouth_mask = masks[17]  # id selected visually in tool
        mask = glasses_mask + mouth_mask
        _, thresh = cv2.threshold(cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY), THRESH, 255, cv2.THRESH_BINARY)
        final_matte = thresh * mask
        #  1. here you could do more thresholding to remove other artifacts
        #  2. you could also threshold different THRESH based on different masks
        #  3. ...
```

## the final product

~I'm not sure what format the tool will be. Currently, I'm thinking a desktop tool to avoid web latencies with large/high resolution videos. Plus, I can use ONNX Runtime or something to simplify the model stuff and improve performance perhaps and avoid having artists/end users deal with python or gradio god forbid.~

Okay I've decided it will be a web app for maximum cross-compatibility, and given how performant the models can be, this should be fine to run on any modern device. Plus I like web, and GTK kinda sucks to work with.
