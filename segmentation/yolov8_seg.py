import cv2
from ultralytics import YOLO
from yolo_segmentation import YOLOSegmentation
import numpy as np
import torch

cap = cv2.VideoCapture(0)

# load model
model = YOLOSegmentation("yolov8n-seg.pt") # medium coco dataset

def segment_image(frame):
    W, H, _ = frame.shape
    # pass frame to model
    try:
        bboxes, classes, segmentations, scores, masks = model.detect(frame) #use model.detect form yolov8m
    except:
        return frame
    for bbox, class_id, seg, score, mask in zip(bboxes, classes, segmentations, scores, masks):
        if  class_id == 41: # show only one thing
            (x, y, x2, y2) = bbox
            # cv2.rectangle(frame, (x, y), (x2, y2), (0, 0, 255), 2)

            # polygonal edges
        
            cv2.putText(frame, str(class_id), (x, y - 10), cv2.FONT_HERSHEY_PLAIN, 2, (0, 0, 255))

            mask = np.zeros(frame.shape[:2], dtype="uint8")
            # cv2.polylines(mask, [seg], True, (255, 0, 0), 2) # True - closed
            cv2.fillPoly(mask, [seg], (255, 255, 255)) # change transparency


            masked_img = cv2.bitwise_and(frame, frame, mask=mask)
            # cv2.addWeighted(redMask, 1, frame, 1, 0, frame)
            print(masked_img.shape)
            return masked_img # segmented image
        else: 
            return frame
    
while True: # allows you to click through frames
    ret, frame = cap.read()
    if not ret:
        break # not a normal frame
    prc = segment_image(frame)
    cv2.imshow("img", prc)
    key = cv2.waitKey(0) # key = cv2.waitKey(1) no clicks
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()
