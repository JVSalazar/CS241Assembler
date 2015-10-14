#include "kind.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>

#include <map>
#include <stdio.h>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::getline;
using ASM::Token;
using ASM::Lexer;

using namespace ASM;
using namespace std;


int main(int argc, char* argv[]){
  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  vector< vector<Token*> > tokLines;
  // Map that stores labels as keys and preceding
  // non-null lines as values
  map <string, int> labels;
  try{
    // Create a MIPS recognizer to tokenize
    // the input lines
    Lexer lexer;
    // Tokenize each line of the input
    string line;

    // Counter for number of non-null lines
    int nonNullLines = 0;
    // Counts number of lines being inputted
    int numLines = 0;

    while(getline(cin, line)) {
      if (line != "") {
        tokLines.push_back(lexer.scan(line));
        numLines++;
      }
    }


    // Array that stores number of lables per line
    int numLabels[numLines];


    // Iterate through vector, finding labels
    // Refactor this to do it in the while loop sometime buddy
    int lineCount = 0; // Counts current line
    vector<vector<Token*> >::iterator itl;
    for(itl = tokLines.begin(); itl != tokLines.end(); ++itl){
      numLabels[lineCount] = 0;
      bool lineNonNull = false;
      vector<Token*>::iterator itl2;
      for(itl2 = itl->begin(); itl2 != itl->end(); ++itl2){
        int tokenCheck = (*itl2)->getKind();
        // Label variables
        string rawLabel; // stores raw label as string if first token is a label
        int labLen = 0; // stores raw label length
        string label; // stores label as string without colon
        if (tokenCheck == LABEL) {
          rawLabel = (*itl2)->getLexeme();
          labLen = rawLabel.size();
          label = rawLabel.substr(0, labLen - 1);
          // Takes care of duplicate labels
          if (labels.find(label) == labels.end()) {
            labels[label] = nonNullLines;
            numLabels[lineCount]++; // Increments number of lables per line
          }
          else {
            cerr << "ERROR: duplicate label" << endl;
            return 1;
          }
        }
        // Increments non-null lines
        if (!lineNonNull && (tokenCheck != LABEL)) {
          nonNullLines += 4;
          lineNonNull = true;
        }
      }

      lineCount++;
    }


    // Processes remaining tokens after labels
    lineCount = 0;
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      vector<Token*>::iterator it2;
      int oper = 0;
      if (it->size() > numLabels[lineCount]) {
        oper = (*(it->begin() + numLabels[lineCount]))->getKind(); // Gets first non-label token in line
      }
      else {
        lineCount++;
        continue;
      }

      int numReqArg = 0; // Will be set to number of args req for first token

      // determining operator with error checking
      switch(oper) {
        case DOTWORD:
          nonNullLines = (nonNullLines + 1) * 4;
          numReqArg = 1;
          break;
        case ERR:
          cerr << "ERROR: invalid token" << endl;
          return 1;
        case LABEL:
          break;
        default:
          cerr << "ERROR: invalid initial token" << endl;
          return 1;
      }

      // # of correct arguments check
      if (((it->size() - numLabels[lineCount] - 1) != numReqArg) && (oper != LABEL)) {
        cerr << "ERROR: invalid number of arguments" << endl;
        return 1;
      }

      // iterating through each token per line
      for(it2 = it->begin() + numLabels[lineCount] + 1; it2 != it->end(); ++it2){
        int arg = (*it2)->getKind(); // gets argument
        int num; // variable to store token to integer conversion
        string strLabel; // variable to store label string
        
        switch(arg) {
          case ID:
            strLabel = (*it2)->getLexeme();
            if (labels.find(strLabel) != labels.end()) {
              num = labels[strLabel];
              putchar((num>>24) & 0xff);
              putchar((num>>16) & 0xff);
              putchar((num>>8) & 0xff);
              putchar(num & 0xff);
            }
            else {
              cerr << "ERROR label not found" << endl;
            }
            break;
          case INT:
            num = (*it2)->toInt();
            if (num >= -2147483648 && num <= 2147483648) {
              putchar((num>>24) & 0xff);
              putchar((num>>16) & 0xff);
              putchar((num>>8) & 0xff);
              putchar(num & 0xff);
            }
            break;
          case HEXINT:
            num = (*it2)->toInt();
            if (num >= -2147483648 && num <= 2147483648) {
              putchar((num>>24) & 0xff);
              putchar((num>>16) & 0xff);
              putchar((num>>8) & 0xff);
              putchar(num & 0xff);
            }
            break;
          case ERR:
            cerr << "ERROR: invalid token" << endl;
            return 1;
          default:
            cerr << "ERROR: invalid argument token" << endl;
            return 1;
        }
      }
      lineCount++;
    }

  } catch(const string& msg){
    // If an exception occurs print the message and end the program
    cerr << msg << endl;
  }
  // Delete the Tokens that have been made
  vector<vector<Token*> >::iterator it;
  for(it = tokLines.begin(); it != tokLines.end(); ++it){
    vector<Token*>::iterator it2;
    for(it2 = it->begin(); it2 != it->end(); ++it2){
      delete *it2;
    }
  }

/*  for(map<string, int >::const_iterator it = labels.begin(); it != labels.end(); ++it) {
      cerr << it->first << " " << it->second << endl;
  }*/
}
