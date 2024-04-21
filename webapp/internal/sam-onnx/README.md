# what do

## one
```console
pip install -r requirements.txt
```

## two
download weights

largest model
```bash
wget https://dl.fbaipublicfiles.com/segment_anything/sam_vit_h_4b8939.pth
```

second largest model
```bash
wget https://dl.fbaipublicfiles.com/segment_anything/sam_vit_l_0b3195.pth
```

third largest model
```bash
wget https://dl.fbaipublicfiles.com/segment_anything/sam_vit_b_01ec64.pth
```

## three
```bash
python export_onnx_model.py \
    --checkpoint "sam_vit_h_4b8939.pth" \
    --output "sam_onnx_h.onnx" \
    --model-type "vit_h" \ # see model/build_sam.py:47 (the sam_model_registry)
    --return-single-mask \
    --quantize-out "sam_onnx_h_q.onnx"
```
