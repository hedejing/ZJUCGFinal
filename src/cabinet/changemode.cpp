#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include<time.h>
using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Need filepath argv." << endl;
		return -1;
	}
	string videoname(argv[1]);
	if ((int)videoname.find(".avi")<0)
	{
		cout << "Need .avi filepath argv." << endl;
		return -1;
	}
	CvCapture *capture = cvCreateFileCapture(videoname.c_str());//读取avi格式的影片
	if (capture == NULL)
	{
		cout << "Open video fail." << endl;
		return -1;
	}
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);//取得视频大小，来放缩图片
	int numFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	int mbi = 0;
	string newvideoname;
	newvideoname = videoname.substr(0, videoname.find(".avi")) + ".rgb";
	FILE *fp = fopen(newvideoname.c_str(), "wb");
	if (fp==NULL)
	{
		cout << "Create file fail." << endl;
		return -1;
	}
	fwrite(&frameW, sizeof(int), 1, fp);
	fwrite(&frameH, sizeof(int), 1, fp);
	fwrite(&numFrames, sizeof(int), 1, fp);
	fwrite(&mbi, sizeof(int), 1, fp);
	
	Mat pic;
	Mat pic2;
	unsigned char *temppic = new unsigned char[frameH*frameW*3];
	int count2 = 0;
	//Mat maindata = Mat(numFrames, frameH*frameW, CV_8UC3);//查分矩阵
	IplImage* frame;//读取下一个影片帧率
	while (1){//播放视频
		frame = cvQueryFrame(capture);//读取下一个影片帧率
		if (!frame) break;
		pic = Mat(frame);
		pic.convertTo(pic2,CV_8UC3);
		count2 = 0;
		memset(temppic, 0, sizeof(temppic));
		for (int i = 0; i < frameH; i++)
		{
			for (int j = 0; j < frameW; j++)
			{
				temppic[count2++] = pic2.at<Vec3b>(i, j)[0];
				temppic[count2++] = pic2.at<Vec3b>(i, j)[1];
				temppic[count2++] = pic2.at<Vec3b>(i, j)[2];
			}
		}
		fwrite(temppic, 1, frameH*frameW * 3, fp);
	}
	fclose(fp);
	cvReleaseCapture(&capture);//释放视频
	system("pause");
	return 0;
}