#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* pow */

using namespace cv;
using namespace std;


// Initializing grid variables
Size arSize(0,0);
Point START = Point(0,0);
Point DEST = Point(0,0);
Point POS = Point(-1,-1);
Point LAST_POS = Point(-1,-1);
Point SA = Point(-1,-1);
Point SA2 = Point(-1,-1);

// Current Position
int XPOS = 0;
int YPOS = 0;

int NUMACT = 8;

// Q Matrix and R Matrix
Mat Q, R;

int iter = 0;
vector<Point> states;
Mat pathIm;

// Display Matrix
void PRINT(Mat& im)
{

	for(int i=0; i<im.rows; i++)
	{
		for(int j=0; j<im.cols; j++)
		{
			cout<< im.at<float>(i,j)<<" ";
		}
		printf("\n");
	}
	cout<<"\n\n\n\n";
}

// Apply end Limits
void LIMIT(Mat& im, Point& pt)
{
	pt.x = pt.x>0?pt.x:0;
	pt.y = pt.y>0?pt.y:0;
	pt.x = pt.x<im.cols?pt.x:im.cols-1;
	pt.y = pt.y<im.rows?pt.y:im.rows-1;	
}

// Collect reward for the move
float GET_R(Point& pt)
{
	float Rval = sqrt((DEST.x-pt.x)*(DEST.x-pt.x) + (DEST.y-pt.y)*(DEST.y-pt.y));
	float Rmax = sqrt((DEST.x-0)*(DEST.x-0) + (DEST.y-0)*(DEST.y-0));

	return float(Rmax-Rval);
}

// Estimate max Q
Point GET_QMAX(Point& pt, float& qmax)
{
	qmax = Q.at<float>(pt.y,0);
	int index = 0;
	if(Q.at<float>(pt.y,1)>qmax)
	{
		qmax = Q.at<float>(pt.y,1);
		index = 1;
	}
	if(Q.at<float>(pt.y,2)>qmax)
	{
		qmax = Q.at<float>(pt.y,2);
		index = 2;
	}
	if(Q.at<float>(pt.y,3)>qmax)
	{
		qmax = Q.at<float>(pt.y,3);
		index = 3;
	}
	if(Q.at<float>(pt.y,4)>qmax)
	{
		qmax = Q.at<float>(pt.y,4);
		index = 4;
	}
	if(Q.at<float>(pt.y,5)>qmax)
	{
		qmax = Q.at<float>(pt.y,5);
		index = 5;
	}
	if(Q.at<float>(pt.y,6)>qmax)
	{
		qmax = Q.at<float>(pt.y,6);
		index = 6;
	}
	if(Q.at<float>(pt.y,7)>qmax)
	{
		qmax = Q.at<float>(pt.y,7);
		index = 7;
	}

	SA2 = Point(index,SA.y);
	return SA2;
}

// Initiate again with new starting point
void GET_NEW_START(Mat& arena, Point& POS)
{
	vector<Mat> planes;
	split(arena, planes);
	int X = -1;
	int Y = -1;
	Point OLDPOS = POS;
	bool GOTIT = false;
	while((X==-1) || (!GOTIT) )
	{
		X = rand()%(arena.cols-1)+1;
		Y = rand()%(arena.rows-1)+1;

		if(planes[1].at<uchar>(Y,X)==255)
			GOTIT = true;
	}

	cout<<"X:Y - "<<X<<" "<<Y<<endl;
	POS = Point(X,Y);
}

// Highlight current position on arena
void DRAW_POS(Mat& arena, Point POS)
{
	circle(arena, POS, 1, Scalar(250,50,5),-1);
}

// Check if the move is valid or not
bool CHECK_MOVE(Point pt)
{
	if((std::abs(pt.y-pt.x)<5)||(std::abs((arSize.height-pt.x-pt.y)<=3)))
	{
		states.push_back(pt);
		return true;
	}

	return false;
}

// Collect next moves 
Point NEXT_MOVE(int Qp, Point& POS)
{
	Point SA;
	vector<int> valid;
	
	if((POS.x-1 > 0 && POS.y-1 > 0) && CHECK_MOVE(Point(POS.x-1, POS.y-1)))
				{ valid.push_back(0);} 
	if((POS.x > 0 && POS.y-1 > 0)&& CHECK_MOVE(Point(POS.x, POS.y-1)))
				{ valid.push_back(1);} 
	if((POS.x+1 < arSize.width && POS.y-1 > 0)&& CHECK_MOVE(Point(POS.x+1, POS.y-1)))
				{ valid.push_back(2);} 
	if((POS.x-1 > 0 && POS.y > 0)&& CHECK_MOVE(Point(POS.x-1, POS.y)))
				{ valid.push_back(3);} 
	if((POS.x+1 < arSize.width && POS.y > 0)&& CHECK_MOVE(Point(POS.x+1, POS.y)))
				{ valid.push_back(4);} 
	if((POS.x-1 > 0 && POS.y+1 < arSize.height)&& CHECK_MOVE(Point(POS.x-1, POS.y+1)))
				{ valid.push_back(5);} 
	if((POS.x > 0 && POS.y+1 < arSize.height)&& CHECK_MOVE(Point(POS.x, POS.y+1)))
				{ valid.push_back(6);} 
	if((POS.x+1 < arSize.width && POS.y+1 < arSize.height)&& CHECK_MOVE(Point(POS.x+1, POS.y+1)))
				{ valid.push_back(7);} 
		
	int val;	

	int ch = rand()%valid.size();
	val = valid[ch];

	switch(val)
	{
		case 0: POS.x--; POS.y--; SA = Point(0,POS.y*arSize.width+POS.x);break;
		case 1: POS.x; POS.y--; SA = Point(1,POS.y*arSize.width+POS.x);break;
		case 2: POS.x++; POS.y--; SA = Point(2,POS.y*arSize.width+POS.x);break;
		case 3: POS.x--; POS.y; SA = Point(3,POS.y*arSize.width+POS.x);break;
		case 4: POS.x++; POS.y; SA = Point(4,POS.y*arSize.width+POS.x);break;
		case 5: POS.x--; POS.y++; SA = Point(5,POS.y*arSize.width+POS.x);break;
		case 6: POS.x; POS.y++; SA = Point(6,POS.y*arSize.width+POS.x);break;
		case 7: POS.x++; POS.y++; SA = Point(7,POS.y*arSize.width+POS.x);break;
	}

	return SA;
}

// Fill arena with colors to highlight movable and restricted areas
Mat FILL_ARENA(Mat& arena)
{
	for(int i=0; i<arena.cols; i++)
	{
		for(int j=0; j<arena.rows; j++)
		{
			if(CHECK_MOVE(Point(i,j)))
				arena.at<Vec3b>(j,i)[1] = 255;

		}
	}

	line(arena, Point(0,0), Point(0,arena.rows), Scalar(0,0,0), 1);
	line(arena, Point(0,arena.rows-1), Point(arena.cols-1,arena.rows-1), Scalar(0,0,0), 1);
	line(arena, Point(arena.cols-1,arena.rows-1), Point(arena.cols-1,0), Scalar(0,0,0), 1);
	line(arena, Point(arena.cols-1,0), Point(0,0), Scalar(0,0,0), 1);
}

// Intialize arena with required colors
Mat INIT_ARENA(Size arSize, Point START, Point DEST)
{
	Mat arena = Mat::zeros(arSize, CV_8UC3);
	FILL_ARENA(arena);

	circle(arena, START, 1, Scalar(0,255,255),-1);
	circle(arena, DEST, 1, Scalar(0,50,255),-1);
	
	return arena;
}

// Fill R Matrix
void FILL_R(Mat& arena, Mat& R)
{
	vector<Mat> planes;
	split(arena, planes);
    Mat dist;
    distanceTransform(planes[1], dist, CV_DIST_L2, 3);
    normalize(dist, dist, 0, 1., NORM_MINMAX);
    
    R = dist.clone();
}


//--------------------------------------TEST------------------------------------------//

// Get best possible move
void GET_TEST_MOVE(Point& TQPOS)
{
	float tqmax = Q.at<float>(TQPOS.y, 0);
	TQPOS.x = 0;

	if(Q.at<float>(TQPOS.y, 1)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 1);
		TQPOS.x = 1;
	}
	if(Q.at<float>(TQPOS.y, 2)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 2);
		TQPOS.x = 2;
	}
	if(Q.at<float>(TQPOS.y, 3)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 3);
		TQPOS.x = 3;
	}
	if(Q.at<float>(TQPOS.y, 4)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 4);
		TQPOS.x = 4;
	}
	if(Q.at<float>(TQPOS.y, 5)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 5);
		TQPOS.x = 5;
	}
	if(Q.at<float>(TQPOS.y, 6)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 6);
		TQPOS.x = 6;
	}
	if(Q.at<float>(TQPOS.y, 7)> tqmax)
	{
		tqmax = Q.at<float>(TQPOS.y, 7);
		TQPOS.x = 7;
	}

	if(tqmax <= 0)
	{
		TQPOS.x = -1;
	}
}

// Test the Q-learned matrix
void TEST(Mat arena, Point START)
{
	Mat im = arena.clone();
	Point TPOS = START;
	int numstep = 0;
	while(sqrt((DEST.y-TPOS.y)*(DEST.y-TPOS.y)+(DEST.x-TPOS.x)*(DEST.x-TPOS.x))>0&&waitKey(1)<0&&++numstep<arena.cols*arena.rows/4)
	{
		//im = arena.clone();
		Point TQPOS = Point(-1, int(TPOS.y*arena.cols+TPOS.x));
		GET_TEST_MOVE(TQPOS);

		switch(TQPOS.x)
		{
			case 0: TPOS.x--; TPOS.y--; break; 
			case 1: TPOS.x; TPOS.y--; break; 
			case 2: TPOS.x++; TPOS.y--; break; 
			case 3: TPOS.x--; TPOS.y; break; 
			case 4: TPOS.x++; TPOS.y; break; 
			case 5: TPOS.x--; TPOS.y++; break; 
			case 6: TPOS.x; TPOS.y++; break; 
			case 7: TPOS.x++; TPOS.y++; break; 
			default: cout<<"Invalid move\n";
		}

		
		circle(im, START, 1, Scalar(0,255,255),-1);
		circle(im, DEST, 1, Scalar(0,50,255),-1);
		circle(im, TPOS, 1, Scalar(124,47,32),-1);

		
		Mat temp = im.clone();
		resize(temp, temp, Size(), 5, 5);
		// imshow("test", temp);
		pathIm = temp.clone();
		
	}
	// Save the arena 
	// imwrite("arena.jpg", im);
}


//--------------------------------------MAIN------------------------------------------//

int main(int argc, char** argv)
{
	if(argc!=7)
	{
		printf("Usage: ./ql arenaWidth arenaHeight X_start Y_start X_dest Y_dest");
	}

	arSize = Size(atoi(argv[1]), atoi(argv[2]));
	START = Point(atoi(argv[3]), atoi(argv[4]));
	DEST = Point(atoi(argv[5]), atoi(argv[6]));
	
	XPOS = START.x;
	YPOS = START.y;
	POS = Point(XPOS, YPOS);
	LAST_POS = Point(XPOS, YPOS);
	vector<Point> tempStates;
	while(waitKey(1)<0)
	{
		Mat arena = INIT_ARENA(arSize, START, DEST);
		Mat initArena = arena.clone();

		if(Q.empty())
		{
			Q=Mat::zeros(arSize.width*arSize.height, 8, CV_32FC1);
			R=Mat::zeros(arSize.height, arSize.width, CV_32FC1);
			FILL_R(arena, R);
		    printf("Initialized 'Q' matrix \n");
		}


		Point origPOS = POS;
		Point QPOS = Point(-1, int(POS.y*arSize.width+POS.x));

		SA = NEXT_MOVE(int(POS.y*arSize.width+POS.x), POS);
		QPOS.x = SA.x;

		float R_VAL = (GET_R(POS)-GET_R(LAST_POS))*R.at<float>(POS.y, POS.x)*R.at<float>(POS.y, POS.x);

		// GIVE NEGAIVE REWARDS TO CENTER OF ARENA
		// if(std::abs(POS.x-arena.cols/2)<3 && std::abs(POS.y-arena.rows/2)<3 )
		// {
		// 	R_VAL = -100;
		// }
		
		float Q_MAX = 0.0;
		SA2 = GET_QMAX(SA, Q_MAX);

		Q.at<float>(QPOS.y, QPOS.x) = Q.at<float>(QPOS.y, QPOS.x) +0.99*(R_VAL + 0.75*(Q_MAX) - Q.at<float>(QPOS.y, QPOS.x));

		LAST_POS = POS;

		POS = Point(SA2.y%arena.cols, SA2.y/arena.cols);		
		DRAW_POS(arena, POS);

		if(POS.x == DEST.x && POS.y == DEST.y)
		{

			if(iter==0)
				Q.at<float>(DEST.y*arena.cols+DEST.x, QPOS.x) += 100;

			++iter;
			if(iter>1)
				TEST(arena, START);
			cout<<"Iteration Number : "<<iter<<endl;
			GET_NEW_START(arena, POS);
			LAST_POS = POS;
		}
		resize(arena, arena, Size(), 5, 5);

		if(!pathIm.empty())
		{
			Mat result;
			hconcat(arena, pathIm, result);
			string text = "Iter #"+to_string(iter);
			putText(result, text, Point(20,result.rows-10),  FONT_HERSHEY_PLAIN, 1.0, Scalar(250,0,0), 1);
			
			imshow("Result", result);
		}
		else
		{
			resize(initArena, initArena, Size(), 5, 5);
			pathIm = initArena.clone();
			Mat result;
			hconcat(arena, pathIm, result);
			// imshow("Result", result);
		}
	}
}
