#pragma once
#include "EvoNet.h"



EvoNet::EvoNet(
	int population, double mutateRate,
	double Ninputs, double Nhiddens, int SizeHidden, double Noutputs
)
{
	genCount = 0;
	size = population;
	rate = mutateRate;
	for (size_t i = 0; i < population; i++)
	{
		Nnet temp(Ninputs, Nhiddens, SizeHidden, Noutputs);
		pop.push_back(temp);
	}
}

void EvoNet::DoEpoch(vector<double> input,bool testing,bool max)
{
	time_epoch = 0;

	if (!testing)
	{
		genCount++; //= pop.front().getAge();
	}

	bestout.resize(genCount);

	vector<double> output;
	for (size_t i = 0; i < size; i++)
	{
		//pop[i].setScore(
		//	GetTotalDif(truth,
		//	output = pop[i].Propigate(truth))
		//);

		output = pop[i].Propigate(input);

		double score = 0;
		for (size_t j = 0; j < input.size(); j++)
		{
			if ((int)(output[j] * (126 - 32) + 32) == (int)(input[j] * (126 - 32) + 32))
			{
				score++;
			}
			else
			{
				score -= pow(abs(output[j] - input[j]), 2);
			}
		}

		//int score=0;
		//for (size_t j = 0; j < truth.size(); j++)
		//{
		//	score += pow(abs(output[j] - truth[j])*truth.size(),2);
		//}
		pop[i].setScore(score);
	
		time_epoch += pop[i].GetSpeed();
	}
	if (!testing)
	{
		Reorder(max);
	}



}

void EvoNet::Reorder(bool max)
{

	for (size_t i = 0; i < size; i++)
	{
		int spot=-1;
		for (size_t j = 0; j < i; j++)
		{
			if (max)
			{
				if (pop[i].getScore() > pop[j].getScore())//if i is greater than j
								spot = j;//make it the best spot to be
			}
			else
			{
				if (pop[i].getScore() < pop[j].getScore())//if i is smaller than j
					spot = j;//make it the best spot to be
			}
			
		}
		if(spot != -1)//if better spot
			std::swap(pop[i], pop[spot]);//they trade places with best spot
	}
}
//saves a percent of the population to be parents and produces mutated babies
void EvoNet::repopulate(double save)
{
	time_repop = 0;

	int saved = save*size;
	pop.resize((int)(size*save));
	for (size_t i = 0; i < size - saved; i++)
	{
		int parent = (int)(RandNum()*saved);
		pop.push_back(pop[parent]);
		pop.back().Mutate(rate);

		time_repop += pop.back().GetSpeed();
	}
}
//maximize or minimize
void EvoNet::updateStats(bool max)
{
	prevmed = average;
	//reset max to get new best
	if (max)
		best = RAND_MAX;
	else
		best = -RAND_MAX;
	average = 0;

	bestout[genCount-1] = pop.front().getLastLayer();//set an output as a fall back in case of error
	for (size_t i = 0; i < size; i++)
	{
		average += pop[i].getScore();
		if (max)
		{
			if (best > pop[i].getScore())
			{
				best = pop[i].getScore();
				bestout[genCount-1] = pop[i].getLastLayer();
			}
		}
		else
		if (best < pop[i].getScore())
		{
			best = pop[i].getScore();
			bestout[genCount-1] = pop[i].getLastLayer();
		}

	}
	if (best == abs(RAND_MAX))
	{
		best = 0;
	}
	average /= size;
}

//repopulate optimized to save only one chosen parent
void EvoNet::inbreed(Nnet parent)
{
	time_repop = 0;
	double mult = 2.5;
	rate *= mult;//multiply mutation due to inbreeding

	//int saved = 1;
	pop.clear();
	pop.push_back(parent);
	for (size_t i = 0; i < size - 1; i++)
	{
		int parent = (int)(RandNum());
		pop.push_back(pop[i]);
		pop.back().Mutate(rate);

		time_repop += pop.back().GetSpeed();
	}
	rate /= mult;//reset mutation for normal repop
}

void EvoNet::SaveBest(string name)
{
	pop[bestNet].saveNet(name);
}