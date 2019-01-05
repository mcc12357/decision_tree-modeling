#include<iostream>
using namespace std;
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include <stdlib.h>
#include <time.h>
#include<math.h>
#include<cmath>
int maxheight = 15;
double stopcondition = 0.9;
double eps = 0.001;
double punish = 0.5;
int totalleafnum;
struct node
{
	int attr;//0��ʾx��1��ʾy��2��ʾz
	double val;//��ǰ�ڵ����ֵ
	bool isleaf;//�Ƿ���Ҷ�ӽڵ�
	int sedtype;//0��ʾ������1��ʾ������2��ʾ����
	int num0,num1,num2;//�ֱ�Ϊ���������������ҵĸ���
	int wrongnum;//��ǰ�������������
	int	leafnum;//Ҷ�ӽڵ���
	int leafwrongnum;//��ǰ������Ҷ�ӽڵ��ϵĴ���������ܺ�
	int dataNum;//��ǰ�ڵ����������
	int height;//��ǰ���ĸ߶�
	node *leftchild;//��������ʾС�ڵ��ڵ�ǰ����ֵ
	node *rightchild;//��������ʾ���ڵ�ǰ����ֵ
};
struct data
{
	double x,y,z;
	int sedtype;
};
int max(int a,int b)
{
	if(a>b) return a;
	return b;
}
int min(int a,int b)
{
	if(a<b) return a;
	return b;
}
double max(double a,double b)
{
	if(a>b) return a;
	return b;
}
double min(double a,double b)
{
	if(a<b) return a;
	return b;
}
void init(node *root)
{
	root->isleaf = false;
	root->height = 0;
	root->num0 = 0;
	root->num1 = 0;
	root->num2 = 0;
	root->wrongnum = 0;
	root->leafnum = 0;
	root->leafwrongnum = 0;
	root->dataNum = 0;
	root->leftchild = NULL;
	root->rightchild = NULL;
}
bool sortbyX(data a,data b)
{
	if(a.x==b.x)
	{
		if(a.y==b.y)
		{
			return a.z<b.z;
		}
		return a.y<b.y;
	}
	return a.x<b.x;
}
bool sortbyY(data a,data b)
{
	if(a.y==b.y)
	{
		if(a.x==b.x)
		{
			return a.z<b.z;
		}
		return a.x<b.x;
	}
	return a.y<b.y;
}
bool sortbyZ(data a,data b)
{
	if(a.z==b.z)
	{
		if(a.x==b.x)
		{
			return a.y<b.y;
		}
		return a.x<b.x;
	}
	return a.z<b.z;
}
void dataRandom(data a[],int num)
{
	srand((unsigned) time(NULL)); 
	int n = 50000;//�������50000�Σ���֤ԭ�������
	int i;
	for(i=0;i<n-1;i++)
	{
			int j = rand() % num,k = rand() % num;  //����0 ~ num-1�������j,k
			data tmp;
			tmp = a[j];
			a[j] = a[k];
			a[k] = tmp;
	}
}
void setLeafattr(node *root,bool isleaf,int sedtype,int height)
{
	root->isleaf = isleaf;
	root->sedtype = sedtype;
	root->height = height;
	root->leftchild = NULL;
	root->rightchild = NULL;
	root->leafnum = 1;
}
void setNoneLeafattr(node *root,int attr,double val,bool isleaf,int height,node *leftchild,node *rightchild)
{
	root->attr = attr;
	root->val = val;
	root->isleaf = isleaf;
	root->height = height;
	root->leftchild = leftchild;
	root->rightchild = rightchild;
}
	
double entropyCal(int start,int end,int a[])
{
	int i;
	int num0 = 0,num1 = 0,num2 = 0;//�ֱ��ʾentropy����Ϊ0,1,2�Ľ�����ִ���
	double sum = 0;
	for(i=start;i<=end;i++)
	{
		if(a[i]==0) num0++;
		if(a[i]==1) num1++;
		if(a[i]==2) num2++;
	}
	if(num0)
	{
		double p0 = num0 * 1.0 / (end - start + 1);
		sum += -p0 * log10(p0) / log10(2);
	}
	if(num1)
	{
		double p1 = num1 * 1.0 / (end - start + 1);
		sum += - p1 * log10(p1) / log10(2);
	}
	if(num2)
	{
		double p2 = num2 * 1.0 / (end - start + 1);
		sum += - p2 * log10(p2) / log10(2);
	}
	return -sum;
}
double giniCal(int start,int end,int a[])
{
	int i;
	int num0 = 0,num1 = 0,num2 = 0;//�ֱ��ʾentropy����Ϊ0,1,2�Ľ�����ִ���
	double sum = 0;
	for(i=start;i<=end;i++)
	{
		if(a[i]==0) num0++;
		if(a[i]==1) num1++;
		if(a[i]==2) num2++;
	}
	double p0 = num0 * 1.0 / (end - start + 1);
	double p1 = num1 * 1.0 / (end - start + 1);
	double p2 = num2 * 1.0 / (end - start + 1);
	sum = 1 - p0 * p0 - p1 * p1 - p2 * p2;
	return -sum;
}
void maxGaincal(double *maxData,int *index,int a[],int dataNum)
{
	int i = 0,j = 1;
	while(j<dataNum)
	{
		while(a[j]==a[i]) j++;//jָ����һ�ֻ��ֵ����
		if(j>=dataNum) break;
		double tmp = j * giniCal(0,j-1,a) + (dataNum-j) * giniCal(j,dataNum-1,a);//gini
		//double tmp = j * entropyCal(0,j-1,a) + (dataNum-j) * entropyCal(j,dataNum-1,a);//entropy
		if(*maxData<tmp)
		{
			*maxData = tmp;
			*index = j - 1;
		}
		i = j;
		j++;
	}
}

void maxGainratiocal(node *root,double *maxData,int *index,int a[],int dataNum)
{
	double rootEntropy = 0;
	
	if(root->num0) 
	{
		double p0 = root->num0 * 1.0 / dataNum;
		rootEntropy += -p0 * log10(p0) / log10(2);
	}
	if(root->num1)
	{
		double p1 = root->num1 * 1.0 / dataNum;
		rootEntropy += -p1 * log10(p1) / log10(2);
	}
	if(root->num2)
	{
		double p2 = root->num2 * 1.0 / dataNum;
		rootEntropy += -p2 * log10(p2) / log10(2);
	}
	//rootEntropy = -entropyCal(0,dataNum-1,a);
	double *gain = new double[dataNum];//����ÿһ�ֻ��ֵ���Ϣ����
	int *gainNextpos = new int[dataNum];//��¼���ֵ�λ��
	int k = 0;//kΪ������
	int i = 0,j = 1;
	while(j<dataNum)
	{
		while(a[j]==a[i]) j++;//jָ����һ�ֻ��ֵ����
		if(j>=dataNum) break;
		double tmp = (j * entropyCal(0,j-1,a) + (dataNum-j) * entropyCal(j,dataNum-1,a) ) / dataNum;//entropy
		gain[k] = tmp;
		gainNextpos[k] = j;
		k++;
		i = j;
		j++;
	}
	for(i=0;i<k;i++) gain[i] += rootEntropy;
	double sum = 0;
	for(i=0;i<k;i++)
	{
		sum += gain[i];
	}
	double avg = sum / k;
	for(i=0;i<k;i++)
	{
		if(gain[i]>avg)
		{
			double splitInfo = 0;
			if(gainNextpos[i])
			{
				double pleft = gainNextpos[i] * 1.0 / dataNum;
				splitInfo += -pleft * log10(pleft) / log10(2);
			}
			if(dataNum-gainNextpos[i])
			{
				double pright = (dataNum-gainNextpos[i]) * 1.0 / dataNum;
				splitInfo += -pright * log10(pright) / log10(2);
			}
			if(splitInfo)
			{
				if(*maxData<gain[i]/splitInfo)
				{
					*maxData = gain[i]/splitInfo;
					*index = gainNextpos[i] - 1;
				}
			}
		}
	}
}
void build(node *root,data dataX[],int dataNum,int height,int *leafnum,int *leafwrongnum)
{	
	int i;
	root->dataNum = dataNum;//ÿ�α��浱ǰ�ڵ����������
	int num0 = 0,num1 = 0,num2 = 0;
	//����ÿ�ֳ�������ִ���
	for(i=0;i<dataNum;i++)
	{
		if(dataX[i].sedtype==0) num0++;
		if(dataX[i].sedtype==1) num1++;
		if(dataX[i].sedtype==2) num2++;
	}
	root->num0 = num0;
	root->num1 = num1;
	root->num2 = num2;
	int numMax = max(num0,max(num1,num2));
	if(num0==numMax) root->wrongnum = num1 + num2;
	else if(num1==numMax) root->wrongnum = num0 + num2;
	else root->wrongnum = num0 + num1;
	if(dataNum<=1 || numMax / dataNum >= stopcondition || height>=maxheight)//�������һ���Դﵽ��ֵ��ﵽ���߶ȣ�ֹͣ����������
	{
		//����Ҷ�ӽڵ�Ĳ���ֵ
		if(num0==numMax) setLeafattr(root,true,0,height);
		else if(num1==numMax) setLeafattr(root,true,1,height);
		else setLeafattr(root,true,2,height);
		totalleafnum++;
		return ;
	}
	data *dataY = new data[dataNum];
	data *dataZ = new data[dataNum];
	for(i=0;i<dataNum;i++) 
	{
		dataY[i] = dataX[i];
		dataZ[i] = dataX[i];
	}
	sort(dataX,dataX+dataNum,sortbyX);
	sort(dataY,dataY+dataNum,sortbyY);
	sort(dataZ,dataZ+dataNum,sortbyZ);
	int *tmp = new int[dataNum];
	double maxX = -10000,maxY = -10000,maxZ = -10000;
	int indexX = 0,indexY = 0,indexZ = 0;
	node *leftchild,*rightchild;
	leftchild = (node*)malloc(sizeof(node));
	rightchild = (node*)malloc(sizeof(node));
	init(leftchild);
	init(rightchild);
	for(i=0;i<dataNum;i++)
	{
		tmp[i] = dataX[i].sedtype;
	}
	maxGaincal(&maxX,&indexX,tmp,dataNum);
	//maxGainratiocal(root,&maxX,&indexX,tmp,dataNum);
	for(i=0;i<dataNum;i++)
	{
		tmp[i] = dataY[i].sedtype;
	}
	maxGaincal(&maxY,&indexY,tmp,dataNum);
	//maxGainratiocal(root,&maxY,&indexY,tmp,dataNum);
	for(i=0;i<dataNum;i++)
	{
		tmp[i] = dataZ[i].sedtype;
	}
	maxGaincal(&maxZ,&indexZ,tmp,dataNum);
	//maxGainratiocal(root,&maxZ,&indexZ,tmp,dataNum);
	double maxGain = max(maxX,max(maxY,maxZ));
	if(fabs(maxGain-maxX) < eps)
	{
		int leftNum = indexX + 1,rightNum = dataNum - indexX - 1;
		data *leftData = new data[leftNum];
		for(i=0;i<leftNum;i++) leftData[i] = dataX[i];
		data *rightData = new data[rightNum];
		for(i=0;i<rightNum;i++) rightData[i] = dataX[i+leftNum];
		setNoneLeafattr(root,0,dataX[indexX].x,false,height,leftchild,rightchild);
		build(leftchild,leftData,leftNum,height+1,&leftchild->leafnum,&leftchild->leafwrongnum);
		build(rightchild,rightData,rightNum,height+1,&rightchild->leafnum,&rightchild->leafwrongnum);
	}
	else if(fabs(maxGain-maxY) < eps)
	{
		int leftNum = indexY + 1,rightNum = dataNum - indexY - 1;
		data *leftData = new data[leftNum];
		for(i=0;i<leftNum;i++) leftData[i] = dataY[i];
		data *rightData = new data[rightNum];
		for(i=0;i<rightNum;i++) rightData[i] = dataY[i+leftNum];
		setNoneLeafattr(root,1,dataY[indexY].y,false,height,leftchild,rightchild);
		build(leftchild,leftData,leftNum,height+1,&leftchild->leafnum,&leftchild->leafwrongnum);
		build(rightchild,rightData,rightNum,height+1,&rightchild->leafnum,&rightchild->leafwrongnum);
	}
	else
	{
		int leftNum = indexZ + 1,rightNum = dataNum - indexZ - 1;
		data *leftData = new data[leftNum];
		for(i=0;i<leftNum;i++) leftData[i] = dataZ[i];
		data *rightData = new data[rightNum];
		for(i=0;i<rightNum;i++) rightData[i] = dataZ[i+leftNum];
		setNoneLeafattr(root,2,dataZ[indexZ].z,false,height,leftchild,rightchild);
		build(leftchild,leftData,leftNum,height+1,&leftchild->leafnum,&leftchild->leafwrongnum);
		build(rightchild,rightData,rightNum,height+1,&rightchild->leafnum,&rightchild->leafwrongnum);
	}
	*leafnum = leftchild->leafnum + rightchild->leafnum;
	*leafwrongnum = leftchild->leafwrongnum + rightchild->leafwrongnum;

}
void showDecisionTree(node *root)
{
	int i;
	if(root && root->isleaf==true)
	{
		for(i=1;i<=root->height;i++) printf("   ");
		printf("sedtype:%d height:%d\n",root->sedtype,root->height);
		return ;
	}
	for(i=1;i<=root->height;i++) printf("   ");
	printf("attr:%d val:%lf height:%d\n",root->attr,root->val,root->height);
	if(root->leftchild) showDecisionTree(root->leftchild);
	if(root->rightchild) showDecisionTree(root->rightchild);
}
void release(node *root)
{
	if(root->isleaf) 
	{
		free(root);
		return ;
	}
	if(root->leftchild) release(root->leftchild);
	if(root->rightchild) release(root->rightchild);
	free(root);
}
void prune(node *root)
{
	//if(root->leftchild) release(root->leftchild);
	//if(root->rightchild) release(root->rightchild);
	root->isleaf = true;
	root->leafnum = 1;
	int sedtype = max(root->num0,(root->num1,root->num2));
	if(root->num0==sedtype) root->sedtype = 0;
	else if(root->num1==sedtype) root->sedtype = 1;
	else root->sedtype = 2;
	root->leftchild = NULL;
	root->rightchild = NULL;
}
void pepPrune(node *root)
{
	if(root->isleaf) return;//�����Ҷ�ӽڵ�ֱ��return
	if(!root->isleaf)//����Ƿ�Ҷ�ڵ㳢�Խ��м�֦
	{
		double tmpwrong = root->leafwrongnum + punish * root->leafnum;
		double e = tmpwrong / root->dataNum;
		double standardError = sqrt(root->dataNum * e * (1-e));
		double eSub = root->wrongnum + punish;
		if(tmpwrong+standardError>eSub) 
		{
			prune(root);
			return ;
		}
	}
	if(root->leftchild) pepPrune(root->leftchild);
	if(root->rightchild) pepPrune(root->rightchild);
}
int main()
{
	int x;
	printf("��ѡ��ָ����ԣ�\n");
	printf("1:����ָ�� 2:��Ϣ�� 3:��Ϣ������\n");
	while(~scanf("%d",&x))
	//while(~scanf("%d",&maxheight))
	{
		printf("ѵ�����þ���������:\n");
		totalleafnum = 0;
		data origin[2000];
		int originnum = 0;
		FILE * fp=fopen("C:\\origin.txt","r");
		//���ļ� 
		int num=0;
		while(fscanf(fp,"%d %lf %lf %lf",&origin[num].sedtype,&origin[num].x,&origin[num].y,&origin[num].z)!=EOF)
		{
			 //printf("%d %lf %lf %lf\n",origin[num].sedtype,origin[num].x,origin[num].y,origin[num].z);//�������ʾ����Ļ 
			 num++;
		}
		originnum = num;
		fclose(fp);
		//printf("%d\n",num);//num=575
		//dataRandom(origin,num);

		//Ԥ�������׼ȷ��ģ��
		/*
		int trainnum = 450;
		int predictnum = num - trainnum;
		data *trainData = new data[trainnum];
		data *predictData = new data[predictnum];
		int i = 0;

		
		//FILE *fpwtrain = fopen("C:\\trainC.txt","w");
		//FILE *fpwpredict = fopen("C:\\predictC.txt","w");
		for(i=0;i<trainnum;i++)
		{
			trainData[i] = origin[i];
			//fprintf(fpwtrain,"%d %lf %lf %lf\n",trainData[i].sedtype,trainData[i].x,trainData[i].y,trainData[i].z);
		}
		for(i=trainnum;i<num;i++)
		{
			predictData[i-trainnum] = origin[i];
			//fprintf(fpwpredict,"%d %lf %lf %lf\n",origin[i].sedtype,origin[i].x,origin[i].y,origin[i].z);
		}
		//fclose(fpwtrain);
		//fclose(fpwpredict);
		

		//����ѵ��������
		i = 0;
		FILE *fprtrain = fopen("C:\\trainC.txt","r");
		while(fscanf(fprtrain,"%d %lf %lf %lf",&trainData[i].sedtype,&trainData[i].x,&trainData[i].y,&trainData[i].z)!=EOF)
		{
			//printf("%d %lf %lf %lf\n",trainData[i].sedtype,trainData[i].x,trainData[i].y,trainData[i].z);//�������ʾ����Ļ 
			i++;
		}
		fclose(fprtrain);

		//������Լ�����
		i = 0;
		FILE *fprpredict = fopen("C:\\predictC.txt","r");
		while(fscanf(fprpredict,"%d %lf %lf %lf",&predictData[i].sedtype,&predictData[i].x,&predictData[i].y,&predictData[i].z)!=EOF)
		{
			//printf("%d %lf %lf %lf\n",predictData[i].sedtype,predictData[i].x,predictData[i].y,predictData[i].z);//�������ʾ����Ļ 
			i++;
		}
		*/

		node *root;
		root = (node*)malloc(sizeof(node));
		init(root);
		//build(root,trainData,trainnum,0,&root->leafnum,&root->leafwrongnum);//���ڼ�����ȷ��ʱ�Ľ���
		build(root,origin,originnum,0,&root->leafnum,&root->leafwrongnum);
		//pepPrune(root);
		showDecisionTree(root);

		
		//Ԥ��׼ȷ��ģ��
		/*
		int rightpredict = 0;//׼ȷԤ������
		for(i=0;i<predictnum;i++)
		{
			node *tmp = root;
			while(true)
			{
				if(tmp->isleaf==true)
				{
					if(tmp->sedtype==predictData[i].sedtype) rightpredict++;
					break;
				}
				if(tmp->attr==0)
				{
					if(predictData[i].x<=tmp->val) tmp = tmp->leftchild;
					else tmp = tmp->rightchild;
					continue;
				}
				if(tmp->attr==1)
				{
					if(predictData[i].y<=tmp->val) tmp = tmp->leftchild;
					else tmp = tmp->rightchild;
					continue;
				}
				if(tmp->attr==2)
				{
					if(predictData[i].z<=tmp->val) tmp = tmp->leftchild;
					else tmp = tmp->rightchild;
				}
			}
		}
		//printf("totalleafnum = %d\n",totalleafnum);
		printf("rightpredict = %d\n",rightpredict);
		printf("rightratio = %.3f\n",rightpredict / 125.0);
		*/

		//ʵ�ʽ�ģģ��
		
		double maxX = 504469.182, maxY = 4528505.923, maxZ = 1111.9, minX = 495560.744, minY = 4512757.568, minZ = 910.5;
        double stepX = 200, stepY = 200,stepZ = 2;
		double posX = minX-10*stepX, posY = minY-10*stepY, posZ = minZ-10*stepZ;
		FILE *fpwmodel = fopen("C:\\modelC.txt","w");
		fprintf(fpwmodel,"result\n");
		fprintf(fpwmodel,"4\n");
		fprintf(fpwmodel,"X\n");
		fprintf(fpwmodel,"Y\n");
		fprintf(fpwmodel,"Z\n");
		fprintf(fpwmodel,"Sedtype\n");
		while(posX<=maxX+10*stepX)
		{
			posY = minY;
			while(posY<=maxY+10*stepY)
			{
				posZ = minZ;
				while(posZ<=maxZ+10*stepZ)
				{
					node *tmp = root;
					int predictresult;
					while(true)
					{
						if(tmp->isleaf==true)
						{
							predictresult = tmp->sedtype;
							break;
						}
						if(tmp->attr==0)
						{
							if(posX<=tmp->val) tmp = tmp->leftchild;
							else tmp = tmp->rightchild;
							continue;
						}
						if(tmp->attr==1)
						{
							if(posY<=tmp->val) tmp = tmp->leftchild;
							else tmp = tmp->rightchild;
							continue;
						}
						if(tmp->attr==2)
						{
							if(posZ<=tmp->val) tmp = tmp->leftchild;
							else tmp = tmp->rightchild;
						}
					}
					fprintf(fpwmodel,"%lf %lf %lf %d\n",posX,posY,posZ,predictresult);
					posZ += stepZ;
				}
				posY += stepY;
			}
			posX += stepX;
		}
		fclose(fpwmodel);
	}
	
}
