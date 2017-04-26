//#pragma once
#ifndef WORDQEFEATURE_H
#define WORDQEFEATURE_H

//#include <string>
#include <vector>
#include <set>
#include "StatefulFeatureFunction.h"
#include "FFState.h"
//#include "moses/Word.h"
//#include <crfsuite.hpp>

namespace CRFSuite
{
  class Tagger;
//  class Attribute;
//  class Item;
//  class ItemSequence;
//  typedef std::vector<Attribute> Item;
//  typedef std::vector<Item> ItemSequence;
}


namespace Moses
{

  class Extractor;
//typedef std::vector< std::string > feature_vector;

//class Extractor {
//private:
//  // vector that contains:
//  //   -- set of all unigrams
//  //   -- set of all bigrams
//  //   -- set of all trigrams
//  //   -- ...
//  //   -- set of all ngrams (n -- LM order)
//  std::vector< std::set< std::string > > m_ngram_list;
//  std::set<std::string> m_stopwords;
//  std::string m_punctuation;
//  size_t m_order;
//
//  CRFSuite::Item find_lm_features(std::vector<std::string>& line, size_t index);
//  CRFSuite::Item find_source_lm_features(std::vector<std::string>& line, size_t index);
//  CRFSuite::Attribute check_backoff(std::vector<std::string>& ngram);
//
//public:
//  Extractor(std::string ngram_file, std::string stopword_file, size_t order);
//  ~Extractor() {}
//  CRFSuite::Item extract_features(std::vector<std::string>& source,
//                                  std::vector<std::string>& target,
//                                  std::vector<std::string>& source_pos,
//                                  std::vector<std::string>& target_pos,
//                                  std::vector<size_t>& alignments,
//                                  size_t index);
//};

//class WordQEState : public FFState
//{
//public:
//  WordQEState(int targetLen)
//    :m_targetLen(targetLen) {
//  }
////  WordQEState(std::vector<Word> &words, 
////			  CRFSuite::ItemSequence &features,
////			  std::vector<size_t> &alignments): m_words(words), m_features(features), m_alignments(alignments)
////  {}
//  WordQEState(std::vector<Word> words, 
//			  CRFSuite::ItemSequence features,
//			  std::vector<size_t> alignments);
//  ~WordQEState() {}
//
////  const std::vector<Word> GetWords() const {
////    return m_words;
////  }
////  const CRFSuite::ItemSequence GetFeatures() const {
////    return m_features;
////  }
////  const std::vector<size_t> GetAlignments() const {
////    return m_alignments;
////  }
//
//  virtual size_t hash() const {
//    return (size_t) m_targetLen;
//  }
//  const std::vector<Word> GetWords() const;
//  const CRFSuite::ItemSequence GetFeatures() const;
//  const std::vector<size_t> GetAlignments() const;
////
////  virtual size_t hash() const;
//  virtual bool operator==(const FFState& o) const;
//
//private:
//  int m_targetLen;
//  std::vector<Word> m_words;
//  CRFSuite::ItemSequence m_features;
//  std::vector<size_t> m_alignments;
//
//};

class WordQEFeature : public StatefulFeatureFunction
{
public:
  WordQEFeature(const std::string &line);
  ~WordQEFeature();

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }
  void Load();

//  virtual const FFState* EmptyHypothesisState(const InputType &input) const {
//    return new WordQEState(0);
//  }
  virtual const FFState* EmptyHypothesisState(const InputType &input) const;

  void EvaluateInIsolation(const Phrase &source
                           , const TargetPhrase &targetPhrase
                           , ScoreComponentCollection &scoreBreakdown
                           , ScoreComponentCollection &estimatedScores) const;
  void EvaluateWithSourceContext(const InputType &input
                                 , const InputPath &inputPath
                                 , const TargetPhrase &targetPhrase
                                 , const StackVec *stackVec
                                 , ScoreComponentCollection &scoreBreakdown
                                 , ScoreComponentCollection *estimatedScores = NULL) const;

  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
      , const TranslationOptionList &translationOptionList) const;

  FFState* EvaluateWhenApplied(
    const Hypothesis& cur_hypo,
    const FFState* prev_state,
    ScoreComponentCollection* accumulator) const;
  FFState* EvaluateWhenApplied(
    const ChartHypothesis& /* cur_hypo */,
    int /* featureID - used to index the state in the previous hypotheses */,
    ScoreComponentCollection* accumulator) const;

  void SetParameter(const std::string& key, const std::string& value);

private:
  size_t m_probability;
  size_t m_use_pos;
//  std::string m_ngram_file_target;
//  std::string m_ngram_file_source;
  std::string m_stopword_file;
  size_t m_numScoreComponents;
  size_t m_order;
  Extractor* m_extractor;

  // model file
  std::string m_model_file;

  CRFSuite::Tagger* m_tagger;
};


}

#endif
