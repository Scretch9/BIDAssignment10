#include "pch.h"
#include "BID08.h"

int means[3];


//YOUR CODE
double euclid_distance(int x, int y) {
	return std::sqrt((x - y)*(x - y));
}



int initializeKMeans(int data[], int data_len, int k) {

	int min = INT_MAX;int max = 0;
	for (int i = 0; i < data_len; i++)
	{
		if (data[i] < min)min = data[i];
		if (data[i] > max)max = data[i];
	}

	return min + rand() % max;
}

int updateMean(int n, int mean, int neighbour) {
	return (int) round((mean*(n - 1) + neighbour) / float(n));
}


int classify(int means[], int k, int neighbour) {
	int min = INT_MAX;
	int index = -1;

	for (int i = 0; i < k; i++)
	{
		int distance = (int) (euclid_distance(neighbour, means[i]));

		if (distance < min) {
			min = distance; 
			index = i;
		}
	}

	return index;

}

//input:
// int[] data : array of data including previous data so far, with length of num_prev_data.
// num_prev_data : number of data above
int predict(int* data, int num_prev_data) {
	//your new data should be data[num_prev_data-1];

	//NEW///////////////////////////////////////////////////////////
	int value = data[num_prev_data - 1];
	std::vector<int> clusters[3];	

	std::vector<int> belongsTo;

	for (int i = 0; i < num_prev_data; i++)
	{
		belongsTo.push_back(5);
	}

	int maxIteration = 3;

	for (int j = 0; j < maxIteration; j++)
	{
		int noChange = 0;
		for (int i = 0; i < num_prev_data; i++)
		{
			int item = data[i];
			

			int index = classify(means,3,item);
			//printf("item: %d index: %d\n", item,index);
			clusters[index].push_back(item);
			int cSize = clusters[index].size();
			means[index] = updateMean(cSize, means[index],item);

			//printf("index: %i belongsto: %i\n ", index, belongsTo[i]);
			if (index != belongsTo[i]) {				
				noChange = 1;
			}

			belongsTo[i] = index;
		}
		

		if (noChange<1) {
			//printf("No change after %i iterations.\n", j);
			break; 
		}
	}

	float avgClusterValue[3];

	for (int i = 0; i < 3; i++)
	{
		//printf("cluster %d", i);
		int sum = 0;
		for (std::vector<int>::iterator it = clusters[i].begin(); it != clusters[i].end(); ++it) {
			sum += *it;
			//printf("%d\t", sum);
		}
		avgClusterValue[i] = (float) (sum / (clusters[i].size()+1));
		//printf("\navg: %2f\n", avgClusterValue[i]);
	}

	//NEW///////////////////////////////////////////////////////////

	//output: 0 for black, 1 for gray and 2 for white.
	/*
	if (value > 300)return 2;
	if (value > 200)return 1;
	else return 0;
	*/

	//order the clusters by their avg value to label them
	int min = INT_MAX, max = 0;
	for (int i = 0; i < 3; i++)
	{
		if (avgClusterValue[i] <= min)min = avgClusterValue[i];
		if (avgClusterValue[i] >= max)max = avgClusterValue[i];
		//printf("%i. val: %f\n", i, avgClusterValue[i]);
	}
	//printf("was: %i",data[num_prev_data-1]);

	int label;

	//return the label of the cluster to whom the current item belongs
	if (avgClusterValue[belongsTo[num_prev_data - 1]] == min) label =  0;
	else if (avgClusterValue[belongsTo[num_prev_data - 1]] == max) label = 2;
	else label = 1;

	//printf("value: %i, avgCluster: %f, label: %i\n", data[num_prev_data-1], avgClusterValue[belongsTo[num_prev_data - 1]], label);
	//printf("%i", belongsTo[num_prev_data - 1]);
	//printf(", label: %i\n\n", label);

	return label;

}



//////////////////////////////////////////////////////////
//////////////////DO NOT MODIFY BELOW/////////////////////
//////////////////////////////////////////////////////////
int main(void) {
	for (size_t i = 0; i < 3; i++)
	{
		means[i] = rand() % 800;
		//printf("%i. %i", i, means[i]);
	}

	float average_score = 0;
	for (int f = 1; f <= 4; f++) {
		char filename[10];
		sprintf(filename, "data%d.txt", f);
		FILE* fp = fopen(filename, "r");

		int N;
		fscanf(fp, "%d", &N);
		//int data[N];
		int data[300];
		int score = 0;
		for (int i = 0; i < N; i++) {
			int d, label;
			fscanf(fp, "%d %d", &d, &label);
			data[i] = d;
			int pred = predict(data, i + 1);
			if (pred == label)score++;
		}
		printf("round %d: your score: %f\n", f, score / float(N));
		average_score += score / float(N);
		//score the prediction
		fclose(fp);
	}
	average_score /= 4;
	printf("average score: %f\n", average_score);
}