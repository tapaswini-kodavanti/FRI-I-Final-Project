import cv2
from ultralytics import YOLO
import numpy as np
import torch
from YOLOSegmentation import YOLOSegmentation

cap = cv2.VideoCapture(0)

# load model
model = YOLOSegmentation("yolov8n-seg.pt") # medium coco dataset
    
while True: # allows you to click through frames
    ret, frame = cap.read()
    if not ret:
        break # not a normal frame
    prc = model.segment_image(frame)
    cv2.imshow("img", prc)
    key = cv2.waitKey(0) # key = cv2.waitKey(1) no clicks
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()