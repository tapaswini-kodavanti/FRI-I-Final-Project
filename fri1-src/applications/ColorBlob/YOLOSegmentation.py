from ultralytics import YOLO
import numpy as np
from PIL import Image
import cv2
import torch
import cv2

class YOLOSegmentation:
    def __init__(self, model_path):
        self.model = YOLO(model_path)

    def detect(self, img):
        # Get img shape
        height, width, channels = img.shape

        results = self.model.predict(source=img.copy(), save=False, save_txt=False)
        result = results[0]
        segmentation_contours_idx = []
        for seg in result.masks.segments:
            # contours
            seg[:, 0] *= width
            seg[:, 1] *= height
            segment = np.array(seg, dtype=np.int32)
            segmentation_contours_idx.append(segment)

        bboxes = np.array(result.boxes.xyxy.cpu(), dtype="int")
        # Get class ids
        class_ids = np.array(result.boxes.cls.cpu(), dtype="int")
        # Get scores
        scores = np.array(result.boxes.conf.cpu(), dtype="float").round(2)
        #get masks
        masks = np.array(result.masks.data.cpu(), dtype="float")
        return bboxes, class_ids, segmentation_contours_idx, scores, masks
    
    def segment_image(self, frame):
        W, H, _ = frame.shape
        # pass frame to model
        try:
            bboxes, classes, segmentations, scores, masks = self.detect(frame) #use model.detect form yolov8m
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
                return masked_img # segmented image
            else: 
                return frame
            
    def get_mask(self, frame):
        np_arr = self.segment_image(frame)
        m = cv2.Mat.from_arr(np_arr)
        return m

