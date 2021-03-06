/*******************************************************************************
** DISR.c, implements the Double Input Symmetrical Relevance criterion
** from
** 
** "On the Use of Variable Complementarity for Feature Selection in Cancer Classification"
** P. Meyer and G. Bontempi, (2006)
**
** Initial Version - 13/06/2008
** Updated - 23/06/2011
**
** Author - Adam Pocock
** 
** Part of the Feature Selection Toolbox, please reference
** "Conditional Likelihood Maximisation: A Unifying Framework for Mutual
** Information Feature Selection"
** G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
** Journal of Machine Learning Research (JMLR), 2011
**
** Please check www.cs.manchester.ac.uk/~gbrown/fstoolbox for updates.
** 
** Copyright (c) 2010-2011, A. Pocock, G. Brown, The University of Manchester
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
** 
**   - Redistributions of source code must retain the above copyright notice, this 
**     list of conditions and the following disclaimer.
**   - Redistributions in binary form must reproduce the above copyright notice, 
**     this list of conditions and the following disclaimer in the documentation 
**     and/or other materials provided with the distribution.
**   - Neither the name of The University of Manchester nor the names of its 
**     contributors may be used to endorse or promote products derived from this 
**     software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
** ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/
#include "FSAlgorithms.h"
#include "FSToolbox.h"

/* MIToolbox includes */
#include "MutualInformation.h"
#include "Entropy.h"
#include "ArrayOperations.h"

void DISR(int k, int noOfSamples, int noOfFeatures, double *featureMatrix, double *classColumn, double *outputFeatures)
{
  /*holds the class MI values*/
  double *classMI = (double *)CALLOC_FUNC(noOfFeatures,sizeof(double));
  
  char *selectedFeatures = (char *)CALLOC_FUNC(noOfFeatures,sizeof(char));
  
  /*holds the intra feature MI values*/
  int sizeOfMatrix = k*noOfFeatures;
  double *featureMIMatrix = (double *)CALLOC_FUNC(sizeOfMatrix,sizeof(double));
  
  double maxMI = 0.0;
  int maxMICounter = -1;
  
  double **feature2D = (double**) CALLOC_FUNC(noOfFeatures,sizeof(double*));
  
  double score, currentScore, totalFeatureMI;
  int currentHighestFeature;
  
  double *mergedVector = (double *) CALLOC_FUNC(noOfSamples,sizeof(double));
  
  int arrayPosition;
  double mi, tripEntropy;
  
  int i,j,x;
  
  for(j = 0; j < noOfFeatures; j++)
  {
    feature2D[j] = featureMatrix + (int)j*noOfSamples;
  }
  
  for (i = 0; i < sizeOfMatrix;i++)
  {
    featureMIMatrix[i] = -1;
  }/*for featureMIMatrix - blank to -1*/


  for (i = 0; i < noOfFeatures;i++)
  {    
    /*calculate mutual info
    **double calculateMutualInformation(double *firstVector, double *secondVector, int vectorLength);
    */
    classMI[i] = calculateMutualInformation(feature2D[i], classColumn, noOfSamples);
    
    if (classMI[i] > maxMI)
    {
      maxMI = classMI[i];
      maxMICounter = i;
    }/*if bigger than current maximum*/
  }/*for noOfFeatures - filling classMI*/
  
  selectedFeatures[maxMICounter] = 1;
  outputFeatures[0] = maxMICounter;
  
  /*****************************************************************************
  ** We have populated the classMI array, and selected the highest
  ** MI feature as the first output feature
  ** Now we move into the DISR algorithm
  *****************************************************************************/
  
  for (i = 1; i < k; i++)
  {
    score = 0.0;
    currentHighestFeature = 0;
    currentScore = 0.0;
    totalFeatureMI = 0.0;
    
    for (j = 0; j < noOfFeatures; j++)
    {
      /*if we haven't selected j*/
      if (selectedFeatures[j] == 0)
      {
        currentScore = 0.0;
        totalFeatureMI = 0.0;
        
        for (x = 0; x < i; x++)
        {
          arrayPosition = x*noOfFeatures + j;
          if (featureMIMatrix[arrayPosition] == -1)
          {
            /*
            **double calculateMutualInformation(double *firstVector, double *secondVector, int vectorLength);
            **double calculateJointEntropy(double *firstVector, double *secondVector, int vectorLength);
            */
            
            mergeArrays(feature2D[(int) outputFeatures[x]], feature2D[j],mergedVector,noOfSamples);
            mi = calculateMutualInformation(mergedVector, classColumn, noOfSamples);
            tripEntropy = calculateJointEntropy(mergedVector, classColumn, noOfSamples);
            
            featureMIMatrix[arrayPosition] = mi / tripEntropy;
          }/*if not already known*/
          currentScore += featureMIMatrix[arrayPosition];
        }/*for the number of already selected features*/
        
        if (currentScore > score)
		{
		  score = currentScore;
		  currentHighestFeature = j;
		}
	  }/*if j is unselected*/
    }/*for number of features*/
  
    selectedFeatures[currentHighestFeature] = 1;
    outputFeatures[i] = currentHighestFeature;
  
  }/*for the number of features to select*/
  
  for (i = 0; i < k; i++)
  {
    outputFeatures[i] += 1; /*C indexes from 0 not 1*/
  }/*for number of selected features*/
  
  FREE_FUNC(classMI);
  FREE_FUNC(mergedVector);
  FREE_FUNC(feature2D);
  FREE_FUNC(featureMIMatrix);
  FREE_FUNC(selectedFeatures);
  
  classMI = NULL;
  mergedVector = NULL;
  feature2D = NULL;
  featureMIMatrix = NULL;
  selectedFeatures = NULL;
  
}/*DISR(int,int,int,double[][],double[],double[])*/

