import numpy as np
import MaskingClass

class clipseg(MaskingClass):
    def get_mask(self):
        return np.array([1, 2, 3, 4])