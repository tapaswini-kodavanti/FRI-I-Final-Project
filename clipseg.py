from transformers import CLIPSegProcessor, CLIPSegForImageSegmentation
from PIL import Image
import torch
import numpy as np
import cv2
import matplotlib.pyplot as plt

processor = CLIPSegProcessor.from_pretrained("CIDAS/clipseg-rd64-refined")
model = CLIPSegForImageSegmentation.from_pretrained("CIDAS/clipseg-rd64-refined")

url = "sbux.jpg"
# image = Image.open(requests.get(url, stream=True).raw)
image = Image.open(url)

prompts = ["cup"] # define things we want to identify
inputs = processor(text=prompts, images=[image] * len(prompts), padding="max_length", return_tensors="pt")
# predict
with torch.no_grad():
  outputs = model(**inputs)
preds = outputs.logits.unsqueeze(1)

# visualize
# convert to numpy array 
if len(prompts) <= 1:
    probs = [torch.flatten(i) for i in preds]
    temp = np.zeros((preds.shape[0], preds.shape[0], 3), dtype="uint8")
    for idx, i in enumerate(probs):
        a = np.array(torch.sigmoid(i)) # this is slow
        temp[idx] = np.array([(255, 255, 255) if x > 0.1 else (0, 0, 0) for x in a ])
# else:
#     [ax[i+1].imshow(torch.sigmoid(preds[i][0])) for i in range(len(prompts))]
#     [ax[i+1].text(0, -15, prompt) for i, prompt in enumerate(prompts)]
img = cv2.cvtColor(temp,cv2.COLOR_RGB2BGR)

cv2.imshow("mask", img)
cv2.waitKey()

#plt.show(block=True)


