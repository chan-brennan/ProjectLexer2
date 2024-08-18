/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <set>
#include <unordered_set>
#include <unordered_map>

using namespace std;
vector<string> LHSVector;
vector<vector<string>> VectorRHS;

unordered_set<string> uniqueRHSItems;
unordered_set<string> uniqueLHSItems;

vector<string> ItemLibrary;

unordered_map<int, unordered_set<int>> firstSets;

unordered_map<int, unordered_set<int>> followSets;


struct GrammarRule {
    int LHS;
    vector<int> RHS;
};

vector<GrammarRule> grammarRules;

bool hasString(vector<string> vec, string str) {
    for (string element : vec) {
        if (element == str) {
            return true;
        }
    }
    return false;
}

bool sethasString(const unordered_set<string>& set, const string& str) {
    return set.find(str) != set.end();
}

void addLHSItems() {
    for (int i = 0; i < LHSVector.size(); i++) {
        if (uniqueLHSItems.insert(LHSVector.at(i)).second) {
            ItemLibrary.push_back(LHSVector.at(i));
        }
        for(string s : VectorRHS.at(i)){
            if(hasString(LHSVector, s)){
                if (uniqueLHSItems.insert(s).second) {
                    ItemLibrary.push_back(s);
                }
            }
        }
    }
}

void addRHSItems() {
    vector<vector<string>> dupedRHS = VectorRHS;

    for (auto iteration = dupedRHS.begin(); iteration != dupedRHS.end(); ++iteration) {
        for (auto sIteration = iteration->begin(); sIteration != iteration->end();) {
            if (hasString(LHSVector, *sIteration) || *sIteration == "#") {
                sIteration = iteration->erase(sIteration);
            } else {
                ++sIteration;
            }
        }
    }
    
    for (int i = 0; i < dupedRHS.size(); i++) {
        for (string item : dupedRHS.at(i)) {
            if (uniqueRHSItems.insert(item).second) {
                ItemLibrary.push_back(item);
            }
        }
    }
}

int retrieveIndex(const vector<string>& vec, const string& target) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == target) {
            return i;
        }
    }
    return -1;
}

void createGrammarRules(){
    for(int i = 0; i < LHSVector.size(); i++){
        GrammarRule r;
        r.LHS = retrieveIndex(ItemLibrary, LHSVector.at(i));
        vector<int> tempVec;
        for(int j = 0; j < VectorRHS.at(i).size(); j++){
            tempVec.push_back(retrieveIndex(ItemLibrary, VectorRHS.at(i).at(j)));
        }
        r.RHS = tempVec;
        grammarRules.push_back(r);
    }
}

void printGrammarRules(vector<GrammarRule> mGrammarRules) {
    cout << "Grammar Rules:" << endl;
    for (const auto& rule : mGrammarRules) {
        cout << "LHS: " << rule.LHS << ", RHS: ";
        for (int RHSItem : rule.RHS) {
            cout << RHSItem << " ";
        }
        cout << endl;
    }
}

void printStringGrammarRules(vector<GrammarRule> grammarRules) {
    for (const auto& rule : grammarRules) {
        cout << ItemLibrary[rule.LHS] << " -> ";
        for (size_t i = 0; i < rule.RHS.size(); ++i) {
            cout << ItemLibrary[rule.RHS[i]];
            if (i < rule.RHS.size() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
}

void readGrammarRules()
{
    LexicalAnalyzer lexer;
    Token token;

    try {

    token = lexer.GetToken();

    LHSVector.push_back(token.lexeme);

    while (token.token_type != END_OF_FILE) {
        token = lexer.GetToken();
        
        if (lexer.peek(1).token_type == ARROW) {
            LHSVector.push_back(token.lexeme);
            continue;
        }

        if (token.token_type == ARROW) {
            if(lexer.peek(1).token_type == STAR){
                vector<string> RHSVectorTemp;
                RHSVectorTemp.push_back("#");
                VectorRHS.push_back(RHSVectorTemp);
            }
            continue;
        }

        if (token.token_type != STAR && token.token_type != END_OF_FILE && token.token_type != HASH) {
            vector<string> RHSVectorTemp;
            RHSVectorTemp.push_back(token.lexeme);
            while (lexer.peek(1).token_type != STAR && lexer.peek(1).token_type != END_OF_FILE) {
                token = lexer.GetToken();
                RHSVectorTemp.push_back(token.lexeme);
            }
            VectorRHS.push_back(RHSVectorTemp);
        }

    }

    addRHSItems();
    addLHSItems();
    createGrammarRules();
    } catch (exception& e) {
        cout << "Syntax error!" << endl;
    }
}

void printUniverseItems(){
    if(ItemLibrary.size() >= 2){
        for(int i = 2; i < ItemLibrary.size(); i++){
            cout << ItemLibrary.at(i);
            if(i != ItemLibrary.size() - 1){
                cout << " ";
            }
        }
    }
}

void printTerminalsAndNoneTerminals()
{
    printUniverseItems();
}

void removeUselessSymbols() {
    vector<bool> vecGeneration(ItemLibrary.size(), false);
    vecGeneration.at(0) = true;

    for (int i = 2; i < uniqueRHSItems.size() + 2; i++) {
        if (ItemLibrary.size() > i) {
            vecGeneration.at(i) = true;
        }
    }    

    bool changed = true;

    while (changed) {
        changed = false;
        for (const auto& rule : grammarRules) {
            if (vecGeneration.at(rule.LHS)) {
                continue;
            }
            bool allGenerating = true;
            for (int RHSItem : rule.RHS) {
                if (!vecGeneration.at(RHSItem)) {
                    allGenerating = false;
                    break;
                }
            }
            if (allGenerating) {
                vecGeneration.at(rule.LHS) = true;
                changed = true;
            }
        }
    }

    vector<GrammarRule> generatingGrammarRules;

    for (const auto& rule : grammarRules) {
        bool allGeneratingRHS = true;
        for(int item : rule.RHS){
            if(!vecGeneration.at(item)){
                allGeneratingRHS = false;
                break;
            }
        }

        if (vecGeneration.at(rule.LHS) && allGeneratingRHS) {
            generatingGrammarRules.push_back(rule);
        }
    }

    vector<bool> reachableVector(ItemLibrary.size(), false);
    reachableVector.at(0) = true;

    if (!uniqueLHSItems.empty()) {
        reachableVector.at(2 + uniqueRHSItems.size()) = true;
    }

    bool c = true;

    while (c) {
        c = false;
        for (const auto& rule : generatingGrammarRules) {
            if (!reachableVector.at(rule.LHS)) {
                continue;
            }
            bool changeMade = false;
            for (int RHSItem : rule.RHS) {
                if (!reachableVector.at(RHSItem)) {
                    reachableVector.at(RHSItem) = true;
                    changeMade = true;
                }
            }
            if (changeMade) {
                c = true;
            }
        }
    }

    vector<GrammarRule> usefulGrammarRules;

    for (const auto& rule : generatingGrammarRules) {
        bool allReachableRHS = true;
        for (int RHSItem : rule.RHS) {
            if (!reachableVector.at(RHSItem)) {
                allReachableRHS = false;
                break;
            }
        }
        if (reachableVector.at(rule.LHS) && allReachableRHS) {
            usefulGrammarRules.push_back(rule);
        }
    }

    printStringGrammarRules(usefulGrammarRules);
}

void printMap(unordered_map<int, unordered_set<int>>& map, int type){
    set<int> keys;

    string typeString = type == 0 ? "FIRST(" : "FOLLOW("; 

    for (const auto& pair : map) {
        keys.insert(pair.first);
    }

    for (int key : keys) {
        if (key >= 2 + uniqueRHSItems.size()) {
            cout << typeString << ItemLibrary[key] << ") = { ";
            set<int> values;
            for (int value : map.at(key)) {
                values.insert(value);
            }
            bool firstValue = true;
            for (int value : values) {
                if (!firstValue) {
                    cout << ", ";
                } else {
                    firstValue = false;
                }
                cout << ItemLibrary[value];
            }
            cout << " }" << endl;
        }
    }
}

bool addItemsWithoutEps(unordered_set<int>& destSet, const unordered_set<int>& sourceSet) {
    bool changed = false;
    for (const auto& item : sourceSet) {
        if (item != 0 && destSet.insert(item).second) {
            changed = true;
        }
    }
    return changed;
}

bool containsValue(const unordered_set<int>& mySet, int value) {
    return mySet.find(value) != mySet.end();
}

void FirstSetsHelperFunction()
{
    for(GrammarRule r : grammarRules){
        firstSets[r.LHS] = unordered_set<int>();
    }

    firstSets[0].insert(0);

    for (int i = 2; i < uniqueRHSItems.size() + 2; i++) {
        firstSets[i].insert(i);
    }
    
    bool changed = 1;
    while(changed){
        changed = 0;
        for(GrammarRule r : grammarRules){
            for (int i = 0; i < r.RHS.size();) {
                bool allPreviousContainEpsilon = true;
                for (int j = 0; j < i; ++j) {
                    if (!containsValue(firstSets[r.RHS.at(j)], 0)) {
                        allPreviousContainEpsilon = false;
                        break;
                    }
                }
                if (i == 0 || allPreviousContainEpsilon) {
                    if (addItemsWithoutEps(firstSets[r.LHS], firstSets[r.RHS.at(i)])) {
                        changed = true;
                    }
                }
                if(allPreviousContainEpsilon && (i == r.RHS.size() - 1) && (containsValue(firstSets[r.RHS.at(i)], 0))){
                    if (firstSets[r.LHS].insert(0).second) {
                    changed = true;
                    }
                }
                ++i;
            }            
        }
    }
}

void calculateFirstSets(){
    FirstSetsHelperFunction();
    printMap(firstSets, 0);
}

void FollowSetsHelperFunction()
{
    FirstSetsHelperFunction();

    for(GrammarRule r : grammarRules){
        followSets[r.LHS] = unordered_set<int>();
    }

    followSets[uniqueRHSItems.size() + 2].insert(1);

    bool changed = true;
    while (changed) {
        changed = false;
        for (const GrammarRule rule : grammarRules) {
            const int currentLHS = rule.LHS;
            const vector<int> currentRHS = rule.RHS;
            
            for (int i = currentRHS.size() - 1; i >= 0;) {
                bool allFollowingContainEpsilon = true;
                int symbol = currentRHS[i];

                for (int j = currentRHS.size() - 1; j > i; j--) {
                    if (!containsValue(firstSets[currentRHS.at(j)], 0)) {
                        allFollowingContainEpsilon = false;
                        break;
                    }
                }

                if ((i == currentRHS.size() - 1) || allFollowingContainEpsilon) {
                    if (addItemsWithoutEps(followSets[symbol], followSets[currentLHS])) {
                        changed = true;
                    }
                }
                
                if(i + 1 < currentRHS.size()){
                    if (addItemsWithoutEps(followSets[symbol], firstSets[currentRHS[i+1]])) {
                        changed = true;
                    }
                    int allFollowingContainEpsilonUptoX = i + 1;

                    for(int x = i + 1; x < currentRHS.size(); x++){
                        if (!containsValue(firstSets[currentRHS.at(x)], 0)) {
                            allFollowingContainEpsilonUptoX = x;
                            break;
                        }                   
                    }

                    if (allFollowingContainEpsilonUptoX < currentRHS.size()) {
                        if (addItemsWithoutEps(followSets[symbol], firstSets[currentRHS.at(allFollowingContainEpsilonUptoX)])) {
                            changed = true;
                        }
                    }

                }

                i--;

            }
        }
    }
}

void calculateFollowSets(){
    FollowSetsHelperFunction();
    printMap(followSets, 1);
}

bool compareGrammarRules(vector<GrammarRule> rules1, vector<GrammarRule> rules2) {
    if (rules1.size() != rules2.size()) {
        return false;
    }

    for (int i = 0; i < rules1.size(); i++) {
        if (rules1[i].LHS != rules2[i].LHS) {
            return false;
        }

        if (!equal(rules1[i].RHS.begin(), rules1[i].RHS.end(), rules2[i].RHS.begin())) {          
            return false;
        }
    }

    return true;
}

bool doSetsHaveCommonItems(unordered_set<int> set1, unordered_set<int> set2) {
    for (int element : set1) {
        if (set2.find(element) != set2.end())
            return true;
    }

    return false;
}

void checkIfGrammarHasPredictiveParser()
{
    FirstSetsHelperFunction();
    FollowSetsHelperFunction();

    bool predictiveParser = true;

    vector<bool> vecGeneration(ItemLibrary.size(), false);

    vecGeneration.at(0) = true;

    for (int i = 2; i < uniqueRHSItems.size() + 2; i++) {
        if (ItemLibrary.size() > i) {
            vecGeneration.at(i) = true;
        }
    }    

    bool changed = true;

    while (changed) {
        changed = false;
        for (const auto& rule : grammarRules) {
            if (vecGeneration.at(rule.LHS)) {
                continue;
            }
            bool allGenerating = true;
            for (int RHSItem : rule.RHS) {
                if (!vecGeneration.at(RHSItem)) {
                    allGenerating = false;
                    break;
                }
            }
            if (allGenerating) {
                vecGeneration.at(rule.LHS) = true;
                changed = true;
            }
        }
    }

    vector<GrammarRule> generatingGrammarRules;

    for (const auto& rule : grammarRules) {
        bool allGeneratingRHS = true;
        for(int item : rule.RHS){
            if(!vecGeneration.at(item)){
                allGeneratingRHS = false;
                break;
            }
        }

        if (vecGeneration.at(rule.LHS) && allGeneratingRHS) {
            generatingGrammarRules.push_back(rule);
        }
    }

    vector<bool> reachableVector(ItemLibrary.size(), false);

    reachableVector.at(0) = true;

    if (!uniqueLHSItems.empty()) {
        reachableVector.at(2 + uniqueRHSItems.size()) = true;
    }

    bool c = true;

    while (c) {
        c = false;
        for (const auto& rule : generatingGrammarRules) {
            if (!reachableVector.at(rule.LHS)) {
                continue;
            }
            bool changeMade = false;
            for (int RHSItem : rule.RHS) {
                if (!reachableVector.at(RHSItem)) {
                    reachableVector.at(RHSItem) = true;
                    changeMade = true;
                }
            }
            if (changeMade) {
                c = true;
            }
        }
    }

    vector<GrammarRule> usefulGrammarRules;

    for (const auto& rule : generatingGrammarRules) {
        bool allReachableRHS = true;
        for (int RHSItem : rule.RHS) {
            if (!reachableVector.at(RHSItem)) {
                allReachableRHS = false;
                break;
            }
        }
        if (reachableVector.at(rule.LHS) && allReachableRHS) {
            usefulGrammarRules.push_back(rule);
        }
    }

    if(!compareGrammarRules(usefulGrammarRules, grammarRules)){
        cout << "NO";
        return;
    }
    
    if(!usefulGrammarRules.empty()){
        unordered_set<int> repeatedNonTerm;
        for(int i = 0; i < grammarRules.size(); i++){
            int currentLHS = grammarRules[i].LHS;
            if(i + 1 < grammarRules.size()){
                for(int j = i + 1; j < grammarRules.size(); j++){
                    if(grammarRules[j].LHS == currentLHS){
                        repeatedNonTerm.insert(currentLHS);
                    }
                }
            }
        }

        for(int currentLHS : repeatedNonTerm){
            for(int i = 0; i < grammarRules.size(); i++){
                if(grammarRules[i].LHS == currentLHS && (i + 1 < grammarRules.size())){
                    for(int j = i + 1; j < grammarRules.size(); j++){
                        if((grammarRules[j].LHS == currentLHS) && doSetsHaveCommonItems(firstSets[grammarRules[i].RHS[0]], firstSets[grammarRules[j].RHS[0]])){
                            cout << "NO";
                            return;
                        };
                        
                    }
                }
            }
        }

        unordered_set<int> epsilonNonTerm;

        for(int i = 0; i < grammarRules.size(); i++){
            int currentLHS = grammarRules[i].LHS;
            if(containsValue(firstSets[currentLHS], 0)){
                epsilonNonTerm.insert(currentLHS);
            }
        }

        for(int currentLHS : epsilonNonTerm){
            if(doSetsHaveCommonItems(firstSets[currentLHS], followSets[currentLHS])){
                cout << "NO";
                return;
            };        
        }
    }

    cout << "YES";
}
    
int main (int argc, char* argv[])
{
    ItemLibrary.push_back("#");
    ItemLibrary.push_back("$");

    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    task = atoi(argv[1]);
    
    readGrammarRules();  

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: removeUselessSymbols();
            break;

        case 3: calculateFirstSets();
            break;

        case 4: calculateFollowSets();
            break;

        case 5: checkIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }

    return 0;
}
