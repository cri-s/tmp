#include <stdio.h>
#include <stdlib.h>

#include <iostream>


// for OpenCV2
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

  Mat light;
Mat correct(Mat img) { Mat tmp;
	if(img.data==NULL) return img;
	if(light.data==NULL) return img;
	if(light.size()!=img.size()) return img;
	if(img.type()!=CV_8U) return img;
	img.convertTo(tmp, CV_32F,1./255.);
	multiply(tmp,light,tmp);
	tmp.convertTo(img,CV_8U,255.,30);
	return img;
}

int
main (int argc, char *argv[])
{
  Mat src_img, template_img;
  Mat frame;
  Mat result_mat;
  Mat debug_img;
  Rect roi = Rect (0, 0, 1, 1);
  CvCapture *capture = 0;

redo:
  if (argc < 3)
    {
      fprintf (stderr, "\n");
      fprintf (stderr, "Usage: %s <search-img/camera-id> <template-img/-> ", argv[0]);
      fprintf (stderr, "\n");
      exit (-1);

    }
  if(argc>3&&!strcmp(argv[1],"-w")) {
	light=imread (argv[2], 0);
	argv[2]=argv[0]; argc-=2; argv+=2; goto redo;
	}

  cvNamedWindow ("result", 1);
  if (atoi (argv[1]) || strlen (argv[1]) < 3)
    {
      capture = cvCaptureFromCAM (atoi (argv[1]));
      if (!capture)
	cout << "No camera detected" << endl;
      frame = cvQueryFrame (capture);
      imshow ("result", frame);
      waitKey (50);
      frame = cvQueryFrame (capture);
      imshow ("result", frame);
      waitKey (50);
      frame = cvQueryFrame (capture);
      imshow ("result", frame);
      waitKey (50);
    }
  else
    frame = imread (argv[1], 1);

  if (strlen(argv[2])>1)
    {
      template_img = imread (argv[2], CV_LOAD_IMAGE_GRAYSCALE);
      if (template_img.data == NULL)
	{
	  printf ("imread() failed...\n");
	  return -1;
	}
    }
  else
      cvtColor (frame(roi), template_img, CV_BGR2GRAY);


  if (frame.data == NULL)
    {
      printf ("imread() failed...\n");
      return -1;
    }

      if (light.data != NULL) { Mat tmp,tmp1;
      GaussianBlur (light, light, cvSize (17, 17), 0);
      GaussianBlur (light, light, cvSize (23, 23), 0);
      GaussianBlur (light, light, cvSize (23, 23), 0);
	light.convertTo(tmp, CV_32F,1./255.);
printf("pixel value at 10:10 from tmp is %f\n",tmp.at<float>(10,10));
	light=Mat::ones(tmp.size(),CV_32F);
printf("pixel value at 10:10 from light is %f\n",light.at<float>(10,10));
	light-=tmp;
printf("pixel value at 10:10 from light is %f\n",light.at<float>(10,10));
	}

  while (true)
    {
      if (capture)
	frame = cvQueryFrame (capture);
      cvtColor (frame, src_img, CV_BGR2GRAY);
	src_img=correct(src_img);
      cvtColor (src_img, debug_img, CV_GRAY2BGR);
      if (frame.data == NULL)
	break;

      GaussianBlur (src_img, src_img, cvSize (7, 7), 0);
      if (template_img.data != NULL)
	{

// method: CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM _CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED
	  int match_method = CV_TM_CCORR_NORMED;
	  matchTemplate (src_img, template_img, result_mat, match_method);
	  //normalize (result_mat, result_mat, 0, 1, NORM_MINMAX, -1, Mat ());

	  double minVal;
	  double maxVal;
	  Point minLoc, maxLoc, matchLoc;
	  minMaxLoc (result_mat, &minVal, &maxVal, &minLoc, &maxLoc, Mat ());
	  if (match_method == CV_TM_SQDIFF
	      || match_method == CV_TM_SQDIFF_NORMED)
	    maxVal = minVal, matchLoc = minLoc;
	  else
	    matchLoc = maxLoc;

	  if (maxVal > 0.92)
	    {
	      rectangle (debug_img,
			 matchLoc,
			 Point (matchLoc.x + template_img.cols,
				matchLoc.y + template_img.rows), CV_RGB (255,
									 0,
									 0),
			 3);

	      char buff[100];
	      sprintf (buff, "X %d  Y %d %.2f", (int) matchLoc.x,
		       (int) matchLoc.y, maxVal);
	      putText (debug_img, buff, Point (50, 100), FONT_HERSHEY_SIMPLEX,
		       1, Scalar (255, 255, 255), 8);
	      putText (debug_img, buff, Point (50, 100), FONT_HERSHEY_SIMPLEX,
		       1, Scalar (0, 200, 200), 4);
	    }
	  circle (debug_img, Point (debug_img.cols / 2, debug_img.rows / 2),
		  3, Scalar (0, 200, 200), -1);
	}
      imshow ("result", debug_img);

      int i;
      int c = waitKey (20);
      i = c + 1 - '0';
      if (i == 1)
	i = 11;
      i *= 30;
      if (c >= '0' && c <= '9')
	{
	  roi =
	    Rect (debug_img.cols / 2 - i / 2, debug_img.rows / 2 - i / 2, i,
		  i);
	  rectangle (debug_img, roi.tl (), roi.br (), Scalar (0, 200, 200),
		     2);
	  imshow ("result", debug_img);
	  waitKey (10);
	}
      if (c == 't')
	{
	  template_img = src_img (roi).clone();
	}
      if (c == 's')
	imwrite ("image.png", frame);
      if (c == 'p')
	imwrite ("templ.png", template_img);
      if (c == 'q')
	break;
      if (c == '?'||c=='h') {
	cout << "Help: 	commands" << endl;
	cout << "	s	Save image to image.png" << endl;
	cout << "	p	Save template to templ.png" << endl;
	cout << "	t	use selected Roi as Template" << endl;
	cout << "	0-9	set Roi size" << endl;
	cout << "	h	this help " << endl;
	cout << "	?	this help " << endl;
	cout << "	ESC	quit" << endl;
	cout << "	q	quit" << endl;
	cout << "	" << endl;
		
	}
      if ((c & 0xff) == 27)
	break;
    }

  return 0;
}
