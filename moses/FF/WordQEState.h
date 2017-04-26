#ifndef WORDQESTATE_H
#define WORDQESTATE_H

#include "moses/Word.h"
#include "FFState.h"
#include <crfsuite.hpp>

//namespace CRFSuite
//{
//  class Attribute;
//  class Item;
//  class ItemSequence;
//}

namespace Moses {

class WordQEState : public FFState
{
public:
  WordQEState(int targetLen)
    :m_targetLen(targetLen) {
  }
  WordQEState(std::vector<Word> words,
              CRFSuite::ItemSequence features,
              std::vector<size_t> alignments);
  ~WordQEState() {}

  virtual size_t hash() const {
    return (size_t) m_targetLen;
  }
  const std::vector<Word> GetWords() const;
  const CRFSuite::ItemSequence GetFeatures() const;
  const std::vector<size_t> GetAlignments() const;
  virtual bool operator==(const FFState& o) const;

private:
  int m_targetLen;
  std::vector<Word> m_words;
  CRFSuite::ItemSequence m_features;
  std::vector<size_t> m_alignments;

};

class Extractor {
private:
  // vector that contains:
  //   -- set of all unigrams
  //   -- set of all bigrams
  //   -- set of all trigrams
  //   -- ...
  //   -- set of all ngrams (n -- LM order)
//  std::vector< std::set< std::string > > m_ngram_list_target;
//  std::vector< std::set< std::string > > m_ngram_list_source;
  std::set<std::string> m_stopwords;
  std::string m_punctuation;
  size_t m_order;

//  CRFSuite::Item find_lm_features(std::vector<std::string>& line, size_t index);
//  CRFSuite::Item find_source_lm_features(std::vector<std::string>& line, size_t index);
//  CRFSuite::Attribute check_backoff(std::vector<std::string>& ngram);

public:
  Extractor(std::string stopword_file, size_t order);
  ~Extractor() {}
  CRFSuite::Item extract_features(std::vector<std::string>& source,
                                  std::vector<std::string>& target,
                                  std::vector<std::string>& source_pos,
                                  std::vector<std::string>& target_pos,
                                  std::vector<size_t>& alignments,
                                  size_t index);
};

}

#endif
