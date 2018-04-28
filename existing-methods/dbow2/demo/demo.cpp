/**
 * File: Demo.cpp
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DBoW2
 * License: see the LICENSE.txt file
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <fstream>

// DBoW2
#include "DBoW2.h" // defines OrbVocabulary and OrbDatabase

#include <DUtils/DUtils.h>
#include <DVision/DVision.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>


using namespace DBoW2;
using namespace DUtils;
using namespace std;

typedef vector<string> stringvec;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void loadFeatures(vector<vector<cv::Mat > > &features);

void loadTrainingFeatures(string name, vector<vector<cv::Mat > > &trainFeatures, vector<string> &trainLabels);
void loadTestingFeatures(string name, vector<vector<cv::Mat > > &testFeatures, vector<string> &testLabels);
void testDatabaseFromDrive(const vector<vector<cv::Mat > > &trainFeatures, const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels);
void testTrainedDatabase(const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels);
void saveLabels(vector<string> &trainLabels);
void loadLabels(vector<string> &trainLabels);


void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out);
void testVocCreation(const vector<vector<cv::Mat > > &features);
void testDatabase(const vector<vector<cv::Mat > > &features);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// number of training images
const int NTRAIN = 1400;

// number of testing images
const int NTEST = 350;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void wait()
{
  cout << endl << "Press enter to continue" << endl;
  getchar();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void read_directory(const std::string& name, stringvec& v)
{
  DIR* dirp = opendir(name.c_str());
  struct dirent * dp;
  while ((dp = readdir(dirp)) != NULL) 
  {
    v.push_back(dp->d_name);
  }
  closedir(dirp);
}

// ----------------------------------------------------------------------------

int main()
{
  //vector<vector<cv::Mat > > featurestrain; //training images
  vector<vector<cv::Mat > > featurestest; //testing images
  vector<string> labelstrain; // training image labels
  vector<string> labelstest; // testing image labels
  //loadTrainingFeatures("images/train",featurestrain,labelstrain);
  loadLabels(labelstrain);
  loadTestingFeatures("images/test/evening/resized", featurestest, labelstest);
  //testDatabaseFromDrive(featurestrain, featurestest, labelstrain, labelstest);
  testTrainedDatabase(featurestest, labelstrain, labelstest);
  //loadFeatures(features);

  //testVocCreation(features);

  wait();

  //testDatabase(features);

  return 0;
}

// ----------------------------------------------------------------------------

void loadFeatures(vector<vector<cv::Mat > > &features)
{
  features.clear();
  features.reserve(NTRAIN);

  cv::Ptr<cv::ORB> orb = cv::ORB::create();

  cout << "Extracting ORB features..." << endl;
  for(int i = 0; i < NTRAIN; ++i)
  {
    stringstream ss;
    ss << "images/image" << i << ".png";

    cv::Mat image = cv::imread(ss.str(), 0);
    cv::Mat mask;
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    orb->detectAndCompute(image, mask, keypoints, descriptors);

    features.push_back(vector<cv::Mat >());
    changeStructure(descriptors, features.back());
  }
}


/* files are named as <LABEL>_<NUMBER> */
string getLabel(string filename)
{
  string label = "";
  for(int i=0;i<int(filename.length());i++)
  {
    if(filename[i]!='_')
    {
      label += filename[i];
    }
    else
    {
      break;
    }
  }
  return label;
}


// ----------------------------------------------------------------------------

void loadTrainingFeatures(string name, vector<vector<cv::Mat > > &trainFeatures, vector<string> &trainLabels)
{
  trainFeatures.clear();
  trainFeatures.reserve(NTRAIN);
  trainLabels.clear();
  trainLabels.reserve(NTRAIN);

  // ORB-SLAM extracts 1000 features for mono kitti example that we used for GTA V
  cv::Ptr<cv::ORB> orb = cv::ORB::create(1000);

  cout << "Extracting ORB features..." << endl;

  // get all images in training directory and get their features
  DIR* dirp = opendir(name.c_str());
  struct dirent * dp;
  while((dp = readdir(dirp)) != NULL) 
  {
    if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0)
    {
      stringstream ss;
      ss<<name<<"/"<<dp->d_name;
      cout<<"file is:"<<ss.str()<<endl;
      cv::Mat image = cv::imread(ss.str(), 0);
      cv::Mat mask;
      vector<cv::KeyPoint> keypoints;
      cv::Mat descriptors;

      orb->detectAndCompute(image, mask, keypoints, descriptors);

      trainLabels.push_back(getLabel(dp->d_name));
      trainFeatures.push_back(vector<cv::Mat >());
      changeStructure(descriptors, trainFeatures.back());
    }
  }
  closedir(dirp);
}

// ----------------------------------------------------------------------------

void loadTestingFeatures(string name, vector<vector<cv::Mat > > &testFeatures, vector<string> &testLabels)
{
  testFeatures.clear();
  testFeatures.reserve(NTEST);
  testLabels.clear();
  testLabels.reserve(NTEST);

  // ORB-SLAM extracts 1000 features for mono kitti example that we used for GTA V
  cv::Ptr<cv::ORB> orb = cv::ORB::create(1000);

  cout << "Extracting testing ORB features..." << endl;

  // get all images in testing directory and get their features
  DIR* dirp = opendir(name.c_str());
  struct dirent * dp;
  while((dp = readdir(dirp)) != NULL) 
  {
    if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0)
    {
      stringstream ss;
      ss<<name<<"/"<<dp->d_name;
      cout<<"file is:"<<ss.str()<<endl;
      cv::Mat image = cv::imread(ss.str(), 0);
      cv::Mat mask;
      vector<cv::KeyPoint> keypoints;
      cv::Mat descriptors;

      orb->detectAndCompute(image, mask, keypoints, descriptors);

      testLabels.push_back(getLabel(dp->d_name));
      testFeatures.push_back(vector<cv::Mat >());
      changeStructure(descriptors, testFeatures.back());
    }
  }
  closedir(dirp);

}

void saveLabels(vector<string> &trainLabels){
  cout << "Saving labels..." <<endl;
  fstream f;
  f.open("labels",fstream::out);
  for(size_t i = 0; i < trainLabels.size(); i++)
  {
    f<<trainLabels[i]<<endl;
  }
  f.close();
}

void loadLabels(vector<string> &trainLabels)
{
  trainLabels.clear();
  trainLabels.reserve(NTRAIN);
  fstream f;
  f.open("labels",fstream::in);
  string label = "";
  for(int i = 0; i < 1400; i++)
  {
    getline(f,label);
    trainLabels.push_back(label);
  }
}

// ----------------------------------------------------------------------------

void testDatabaseFromDrive(const vector<vector<cv::Mat > > &trainFeatures, const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels)
{
  
  cout << "Creating trained database..." << endl;

  // load the vocabulary from disk

  OrbVocabulary  voc;

  voc.loadFromTextFile("ORBvoc.txt");
  
  OrbDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that 
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add images to the database
  for(int i = 0; i < NTRAIN; i++)
  {
    db.add(trainFeatures[i]);
  }

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;

  // and query the database
  cout << "Querying the database: " << endl;

  int relevantImages = 0; // number of images in results that have the expected label of query
  int currRecall = 0; // recall per query
  //int sumRecall = 0; // sum of the recall values of all the queries for a specific K 
  //double sumPrecision = 0; // sum of precision values of all queries for a specific K
  //double precision = 0; // precision for a specific K
  //double recall = 0; // recall for a specific K
  QueryResults ret;

  double data[4][2]; // data[][0] is recall, data[][1] is precision
  int K[4] = {1,2,20,30};

  // initialize array
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 2; j++)
    {
      data[i][j] = 0;
    }
  }


  
  for(int i = 0; i < NTEST; i++)
  {
    db.query(testFeatures[i],ret,K[3]+1);
    
    currRecall = 0;
    relevantImages = 0;
    for(int j = 0; j < K[3]+1; j++)
    {
      if(j==K[0])
      {
        data[0][0] += currRecall;
        data[0][1] += (double(relevantImages))/double(K[0]); 
      }
      else if(j==K[1])
      {
        data[1][0] += currRecall;
        data[1][1] += (double(relevantImages))/double(K[1]); 
      }
      else if(j==K[2])
      {
        data[2][0] += currRecall;
        data[2][1] += (double(relevantImages))/double(K[2]); 
      }
      else if(j==K[3])
      {
        data[3][0] += currRecall;
        data[3][1] += (double(relevantImages))/double(K[3]); 
      }
      if(testLabels[i]==trainLabels[ret[j].Id])
      {
        relevantImages++;
        if(currRecall==0)
        {
          currRecall++;
        }
      }
    }
  }
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 2; j++)
    {
      data[i][j] /= NTEST;
    }

  }
  cout<<"For K="<<K[0]<<" Recall="<<data[0][0]<<endl;
  cout<<"For K="<<K[0]<<" Precision="<<data[0][1]<<endl;
  cout<<endl;
  cout<<"For K="<<K[1]<<" Recall="<<data[1][0]<<endl;
  cout<<"For K="<<K[1]<<" Precision="<<data[1][1]<<endl;
  cout<<endl;
  cout<<"For K="<<K[2]<<" Recall="<<data[2][0]<<endl;
  cout<<"For K="<<K[2]<<" Precision="<<data[2][1]<<endl;
  cout<<endl;
  cout<<"For K="<<K[3]<<" Recall="<<data[3][0]<<endl;
  cout<<"For K="<<K[3]<<" Precision="<<data[3][1]<<endl;
  
  
  /*
  // top K results are returned from the database
  for(int K = 1;K < 3; K++)
  {
    sumRecall = 0;
    sumPrecision = 0;
    for(int i = 0; i < NTEST; i++)
    {
      // query database with testing images
      db.query(testFeatures[i], ret, K); //retrieves top 4 results

      // check all results to see if expected label exists in the set
      currRecall = 0;
      relevantImages = 0;
      for(int j=0;j<K;j++)
      {
        if(testLabels[i]==trainLabels[ret[j].Id])
        {
          relevantImages++;

          // set the recall to 1 if any label in the result set matches the expected label
          if(currRecall==0)
          {
            currRecall++;
          }
        }
      }
      sumRecall += currRecall;
      sumPrecision += (double(relevantImages))/double(K);

      //cout << "Searching for Image: " << i << " with label:"<<testLabels[i]<< ". Label retrieved is:" << trainLabels[ret[0].Id] <<endl;

    }

    recall = double(sumRecall)/NTEST;
    precision = sumPrecision/NTEST;
    cout<<"For K="<<K<<" Recall="<<recall<<endl;
    cout<<"For K="<<K<<" Precision="<<precision<<endl;

  }
    
  */

  cout << endl;

  // we can save the database. The created file includes the vocabulary
  // and the entries added
  //cout << "Saving database..." << endl;
  //db.save("db.yml.gz");

  //saveLabels(trainLabels);

  
  //cout << "... done!" << endl;
  
  // once saved, we can load it again  
  //cout << "Retrieving database once again..." << endl;
  //OrbDatabase db2("small_db.yml.gz");
  //cout << "... done! This is: " << endl << db2 << endl;
}

int findInArray(int index, int a[], int size)
{
  for(int i = 0; i < size; i++)
  {
    if(index == a[i])
    {
      return i;
    }
  }
  return -1;
}


void testTrainedDatabase(const vector<vector<cv::Mat > > &testFeatures, vector<string> &trainLabels, vector<string> &testLabels)
{

  // load the vocabulary from disk

  cout << "Loading database..." << endl;
  
  OrbDatabase db("db.yml.gz");

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;

  // and query the database
  cout << "Querying the database: " << endl;

  int relevantImages = 0; // number of images in results that have the expected label of query
  int currRecall = 0; // recall per query

  int numK = 6;

  double data[6][2]; // data[][0] is recall, data[][1] is precision
  int K[6] = {1,2,20,30,40,50};

  // initialize array
  for(int i = 0; i < numK; i++)
  {
    for(int j = 0; j < 2; j++)
    {
      data[i][j] = 0;
    }
  }


  QueryResults ret;
  int kIndex = 0;
  for(int i = 0; i < NTEST; i++)
  {
    db.query(testFeatures[i],ret,K[numK-1]+1);
    
    currRecall = 0;
    relevantImages = 0;
    for(int j = 0; j < K[numK-1]+1; j++)
    {
      /*
      if(j==K[0])
      {
        data[0][0] += currRecall;
        data[0][1] += (double(relevantImages))/double(K[0]); 
      }
      else if(j==K[1])
      {
        data[1][0] += currRecall;
        data[1][1] += (double(relevantImages))/double(K[1]); 
      }
      else if(j==K[2])
      {
        data[2][0] += currRecall;
        data[2][1] += (double(relevantImages))/double(K[2]); 
      }
      else if(j==K[3])
      {
        data[3][0] += currRecall;
        data[3][1] += (double(relevantImages))/double(K[3]); 
      }
      */
      kIndex = findInArray(j,K,numK);
      if(kIndex != -1)
      {
        data[kIndex][0] += currRecall;
        data[kIndex][1] += (double(relevantImages))/double(K[kIndex]);
      }
      if(testLabels[i]==trainLabels[ret[j].Id])
      {
        relevantImages++;
        if(currRecall==0)
        {
          currRecall++;
        }
      }
    }
  }
  for(int i = 0; i < numK; i++)
  {
    for(int j = 0; j < 2; j++)
    {
      data[i][j] /= NTEST;
    }
    cout<<"For K="<<K[i]<<" Recall="<<data[i][0]<<endl;
    cout<<"For K="<<K[i]<<" Precision="<<data[i][1]<<endl;
    cout<<endl;

  }

  cout << endl;

}

// ----------------------------------------------------------------------------

void changeStructure(const cv::Mat &plain, vector<cv::Mat> &out)
{
  out.resize(plain.rows);

  for(int i = 0; i < plain.rows; ++i)
  {
    out[i] = plain.row(i);
  }
}

// ----------------------------------------------------------------------------

void testVocCreation(const vector<vector<cv::Mat > > &features)
{
  // branching factor and depth levels 
  const int k = 9;
  const int L = 3;
  const WeightingType weight = TF_IDF;
  const ScoringType score = L1_NORM;

  OrbVocabulary voc(k, L, weight, score);

  cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
  voc.create(features);
  cout << "... done!" << endl;

  cout << "Vocabulary information: " << endl
  << voc << endl << endl;

  // lets do something with this vocabulary
  cout << "Matching images against themselves (0 low, 1 high): " << endl;
  BowVector v1, v2;
  for(int i = 0; i < NTRAIN; i++)
  {
    voc.transform(features[i], v1);
    for(int j = 0; j < NTRAIN; j++)
    {
      voc.transform(features[j], v2);
      
      double score = voc.score(v1, v2);
      cout << "Image " << i << " vs Image " << j << ": " << score << endl;
    }
  }

  // save the vocabulary to disk
  cout << endl << "Saving vocabulary..." << endl;
  voc.save("small_voc.yml.gz");
  cout << "Done" << endl;
}

// ----------------------------------------------------------------------------

void testDatabase(const vector<vector<cv::Mat > > &features)
{
  cout << "Creating a small database..." << endl;

  // load the vocabulary from disk
  //OrbVocabulary voc("small_voc.yml.gz");

  OrbVocabulary  voc("ORBvoc.yml.gz");

  // using the ORB SLAM vocabulary which works well with cityscapes
  //loads binary ORBSLAM vocabulary
  //voc.loadFromTextFile("ORBvoc.txt");
  //voc.save("ORBvoc.yml.gz");


  OrbDatabase db(voc, false, 0); // false = do not use direct index
  // (so ignore the last param)
  // The direct index is useful if we want to retrieve the features that 
  // belong to some vocabulary node.
  // db creates a copy of the vocabulary, we may get rid of "voc" now

  // add images to the database
  for(int i = 0; i < NTRAIN; i++)
  {
    db.add(features[i]);
  }

  cout << "... done!" << endl;

  cout << "Database information: " << endl << db << endl;

  // and query the database
  cout << "Querying the database: " << endl;

  QueryResults ret;
  for(int i = 0; i < NTRAIN; i++)
  {
    db.query(features[i], ret, 4); //retrieves top 4 results?

    // ret[0] is always the same image in this case, because we added it to the 
    // database. ret[1] is the second best match.

    cout << "Searching for Image " << i << ". " << ret << endl;
  }

  cout << endl;

  // we can save the database. The created file includes the vocabulary
  // and the entries added
  cout << "Saving database..." << endl;
  db.save("small_db.yml.gz");
  cout << "... done!" << endl;
  
  // once saved, we can load it again  
  cout << "Retrieving database once again..." << endl;
  OrbDatabase db2("small_db.yml.gz");
  cout << "... done! This is: " << endl << db2 << endl;
}

// ----------------------------------------------------------------------------


