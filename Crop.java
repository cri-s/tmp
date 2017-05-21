package org.openpnp.vision.pipeline.stages;

import java.awt.Color;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.openpnp.vision.pipeline.CvPipeline;
import org.openpnp.vision.pipeline.CvStage;

/**
 * Mask everything in the working image outside of a circle centered at the center of the image with
 * the specified diameter.
 */
public class Crop extends CvStage {
    private int width = 600;
    private int height = 400;


    public int getWidth() {
        return width;
    }


    public void setWidth(int width) {
        this.width = width;
    }


    public int getHeight() {
        return height;
    }


    public void setHeight(int height) {
        this.height = height;
    }


    @Override
    public Result process(CvPipeline pipeline) throws Exception {
        Mat mat = pipeline.getWorkingImage();
	if(width>(mat.cols()/2-8)||height>(mat.rows()/2-8)) {
		throw new Exception("Arguments out of bound");
	}
	mat=mat.submat(	(mat.rows()-height)/2,(mat.rows()-height)/2+height
			,(mat.cols()-width)/2,(mat.cols()-width)/2+width 
		   );
        return new Result(mat.clone());
    }
}
  
