import cv2
from ultralytics import YOLO
from yolo_segmentation import YOLOSegmentation
import numpy as np
import torch

cap = cv2.VideoCapture("move_cups.mp4")

# load model
model = YOLOSegmentation("yolov8n-seg.pt") # medium coco dataset

while True: # allows you to click through frames
    ret, frame = cap.read()
    if not ret:
        break # not a normal frame

    W, H, _ = frame.shape
    # pass frame to model
    bboxes, classes, segmentations, scores, masks = model.detect(frame) #use model.detect form yolov8m
    for bbox, class_id, seg, score, mask in zip(bboxes, classes, segmentations, scores, masks):
        (x, y, x2, y2) = bbox
        # if  class_id == 32: show only one thing
        cv2.rectangle(frame, (x, y), (x2, y2), (0, 0, 255), 2)

        # polygonal edges
        cv2.polylines(frame, [seg], True, (255, 0, 0), 2) # True - closed
        cv2.fillPoly(frame, [seg], (255, 0, 0)) # change transparency

        cv2.putText(frame, str(class_id), (x, y - 10), cv2.FONT_HERSHEY_PLAIN, 2, (0, 0, 255))

        # mask = mask * 255
        # mask = cv2.resize(mask, (W, H))
        # cv2.imwrite("img", mask)

        # redImg = np.zeros(frame.shape, frame.dtype)
        # redImg[:,:] = (0, 0, 255)
        # redMask = cv2.bitwise_and(redImg, redImg, mask=mask)
        # cv2.addWeighted(redMask, 1, frame, 1, 0, frame)
    cv2.imshow("img", frame)
    key = cv2.waitKey(0) # key = cv2.waitKey(1) no clicks
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()