#include "moses/ScoreComponentCollection.h"
#include "moses/Hypothesis.h"
#include "moses/Util.h"
//#include "FFState.h"
#include "WordQEFeature.h"
#include "WordQEState.h"

//#include <vector>
#include <iterator>
#include <string>
#include <utility>
#include <cstdlib>

#include <crfsuite.hpp>

//#include <boost/python.hpp>
//#include <Python.h>
//#include <boost/python/stl_iterator.hpp>
//#include <boost/timer.hpp>

//namespace py = boost::python;
namespace crf = CRFSuite;
using namespace std;

namespace Moses
{

const std::vector<Word> WordQEState::GetWords() const {
    return m_words;
}

const CRFSuite::ItemSequence WordQEState::GetFeatures() const {
  return m_features;
}

const std::vector<size_t> WordQEState::GetAlignments() const {
  return m_alignments;
}

WordQEState::WordQEState(vector<Word> words, crf::ItemSequence features, vector<size_t> alignments) { 
  m_words = words;
  m_features = features;
  m_alignments = alignments;
}

const FFState* WordQEFeature::EmptyHypothesisState(const InputType &input) const {
  vector<Word> words;
  words.clear();
  crf::ItemSequence features;
  features.clear();
  vector<size_t> alignments;
  alignments.clear();
  return new WordQEState(words, features, alignments);
}

Extractor::Extractor(string stopword_file, size_t order) {
  m_order = order;
  //cerr << "Extractor constructor" << endl;
  // list of stopwords
  ifstream inFile(stopword_file.c_str());
  string line;
  if (!inFile) {
    UTIL_THROW(util::Exception, "Couldn't open file" << stopword_file);
  }
  while (getline(inFile, line)) {
    m_stopwords.insert(line);
  }
  inFile.close();
  //cerr << "Loaded stopwords" << endl;
}


////////////////////////////////////////////////////////////////
// compares only words 
// (no features comparison, because features of previous words
// don't matter for the current word)
bool WordQEState::operator==(const FFState& o) const
{
  const WordQEState& other = dynamic_cast<const WordQEState&>(o);
  if (m_targetLen != other.m_targetLen) return false;
  if (m_words.size() == other.m_words.size()) {
  	int result;
    for (size_t i = 0; i < m_words.size(); ++i) {
      result = Word::Compare(m_words[i], other.m_words[i]);
      if (result != 0) return false;
    }
    return true;
  } 
  else return false;
}


WordQEFeature::WordQEFeature(const std::string &line)
  :StatefulFeatureFunction(1, line)
{
  ReadParameters();
  m_tagger = new crf::Tagger();
}

WordQEFeature::~WordQEFeature(){
  delete m_extractor;
  delete m_tagger;
}

void WordQEFeature::EvaluateInIsolation(const Phrase &source
    , const TargetPhrase &targetPhrase
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection &estimatedScores) const
{}

void WordQEFeature::EvaluateWithSourceContext(const InputType &input
    , const InputPath &inputPath
    , const TargetPhrase &targetPhrase
    , const StackVec *stackVec
    , ScoreComponentCollection &scoreBreakdown
    , ScoreComponentCollection *estimatedScores) const
{}

void WordQEFeature::EvaluateTranslationOptionListWithSourceContext(const InputType &input
    , const TranslationOptionList &translationOptionList) const
{}

string left_context(vector<string> token_list, size_t index){
	if (index == 0) {
	  //cerr << "left context: index=0" << endl;
	  return "<s>";
	}
	else {
	  //cerr << "left context: index=" << index;
	  //cerr << ", word=" << token_list[index-1] << endl;
      return token_list[index-1];
	}
}

string right_context(vector<string> token_list, size_t index){
	if (index + 1 >= token_list.size()) return "</s>";
	else return token_list[index+1];
}


//crf::Attribute Extractor::check_backoff(vector<string>& ngram){
//  bool bigram_1 = 0;
//  bool bigram_2 = 0;
//  bool unigram_1 = 0;
//  bool unigram_2 = 0;
//  // trigram (1, 2, 3)
//  if (m_ngram_list_target[2].find(ngram[0] + " " + ngram[1] + " " + ngram[2]) != m_ngram_list_target[2].end()) {
//	  crf::Attribute c_attr("backoff_behavior_left=1.0");
//      return c_attr;
//  }
//  if (m_ngram_list_target[1].find(ngram[0] + " " + ngram[1]) != m_ngram_list_target[1].end())
//	bigram_1 = 1;
//  if (m_ngram_list_target[1].find(ngram[1] + " " + ngram[2]) != m_ngram_list_target[1].end())
//	bigram_2 = 1;
//  // two bigrams (1, 2) and (2, 3)
//  if (bigram_1 && bigram_2)
//      return crf::Attribute("backoff_behavior_left=0.8");
//  // bigram (2, 3)
//  if (bigram_2)
//      return crf::Attribute("backoff_behavior_left=0.6");
//  if (m_ngram_list_target[0].find(ngram[2]) != m_ngram_list_target[0].end())
//	  unigram_2 = 1;
//  // bigram (1, 2) and unigram (3)
//  if (bigram_1 && unigram_2)
//      return crf::Attribute("backoff_behavior_left=0.4");
//  if (m_ngram_list_target[0].find(ngram[1]) != m_ngram_list_target[0].end())
//	  unigram_1 = 1;
//  // unigrams (2) and (3)
//  if (unigram_1 && unigram_2)
//      return crf::Attribute("backoff_behavior_left=0.3");
//  // unigram (3)
//  if (unigram_2)
//      return crf::Attribute("backoff_behavior_left=0.2");
//  // all words unknown
//  return crf::Attribute("backoff_behavior_left=0.1");
//}

//crf::Item Extractor::find_lm_features(std::vector<std::string>& line, size_t index){
//  crf::Item features;
// 
//  // vector of left ngrams
//  vector<string> left_ngrams;
//  // vector of left words
//  vector<string> back_left;
//  // form left contexts
//  left_ngrams.push_back(line[index]);
//  back_left.push_back(line[index]);
//  for (size_t i = 1; i < m_order; ++i){
//    string cur_word;
//	if (i > index)
//	  cur_word = "<s>";
//    else 
//	  cur_word = line[index-i];
//	left_ngrams.push_back(cur_word + " " + left_ngrams[i-1]);
//	if (i < 3)
//	  back_left.push_back(cur_word);
//  }
//
//  // highest order ngram left
//  for (size_t i = m_order-1; i >= 0; --i){
//	// find ngram
//	if (m_ngram_list_target[i].find(left_ngrams[i]) != m_ngram_list_target[i].end()){
//	  string attr;
//	  attr = i + 1;
//	  attr = "highest_order_ngram_left=" + attr;
//      features.push_back(crf::Attribute(attr));
//	  break;
//	}
//  }
//  // left ngrams not found
//  if (features.size() == 0)
//    features.push_back(crf::Attribute("highest_order_ngram_left=0"));
//  // left trigram backoff
//  features.push_back(check_backoff(back_left));
//  return features;
//}

//crf::Item Extractor::find_source_lm_features(std::vector<std::string>& ngram, size_t index){
////  crf::Item features = vector<crf::Attribute>(2, crf::Attribute("0"));
//  crf::Item features;
//  features.reserve(2);
//
//  // form left ngrams
//  vector<string> left_ngrams;
//  left_ngrams.push_back(ngram[index]);
//  for (size_t i = 1; i < m_order; ++i){
//    string cur_word;
//	if (i > index)
//	  cur_word = "<s>";
//    else 
//	  cur_word = ngram[index-i];
//	left_ngrams.push_back(cur_word + " " + left_ngrams[i-1]);
//  }
//
//  // form right ngrams
//  vector<string> right_ngrams;
//  right_ngrams.push_back(ngram[index]);
//  for (size_t i = 1; i < m_order; ++i){
//    string cur_word;
//    if(ngram.size() <= i)
//	  cur_word = "</s>";
//	else
//	  cur_word = ngram[index+1];
//	right_ngrams[i] = right_ngrams[i-1] + " " + cur_word;
//  }
//
//  // highest order ngram left
//  for (size_t i = m_order-1; i >= 0; --i){
//	// find ngram
//	if (m_ngram_list_source[i].find(left_ngrams[i]) != m_ngram_list_source[i].end()){
//	  string attr;
//	  attr = i + 1;
//	  attr = "source_highest_order_ngram_left=" + attr;
//      features.push_back(crf::Attribute(attr));
//	  break;
//	}
//  }
//  if (features.size() == 0)
//	features.push_back(crf::Attribute("source_highest_order_ngram_left=0"));
//  // highest order ngram right
//  for (size_t i = m_order-1; i >= 0; --i){
//	// find ngram
//	if (m_ngram_list_source[i].find(right_ngrams[i]) != m_ngram_list_source[i].end()){
//	  string attr;
//	  attr = i + 1;
//	  attr = "source_highest_order_ngram_right=" + attr;
//	  features.push_back(crf::Attribute(attr));
//	  break;
//	}
//  }
//  if (features.size() == 1)
//	features.push_back(crf::Attribute("source_highest_order_ngram_right=0"));
//  return features;
//}


// source, target
crf::Item Extractor::extract_features(vector<string>& source, 
				                vector<string>& target,
								vector<string>& source_pos,
								vector<string>& target_pos,
								vector<size_t>& alignments,
								size_t index) {
    crf::Item word_features;
	int src_idx = alignments[index];
	// token
	//cerr << "token" << endl;
	string token = target[index];
	word_features.push_back(crf::Attribute("token=" + token));
	// left_context
	//cerr << "left context" << endl;
	string tg_left_cont = left_context(target, index);
	word_features.push_back(crf::Attribute("left_context=" + tg_left_cont));

	string src_token;
	string src_left_cont;
	string src_right_cont;
	string src_pos;
	// no aligned word
	if (src_idx == -1) {
		src_token = "__unaligned__";
		src_left_cont = "__unaligned__";
		src_right_cont = "__unaligned__";
		src_pos = "__unaligned__";
	}
	// aligned word exists
	else {
		src_token = source[src_idx];
		src_left_cont = left_context(source, src_idx);
		src_right_cont = right_context(source, src_idx);
		src_pos = source_pos[src_idx];
	}

	// aligned_token
	//cerr << "aligned token" << endl;
	word_features.push_back(crf::Attribute("aligned_token=" + src_token));
	// src_left_context
	//cerr << "src left context" << endl;
	word_features.push_back(crf::Attribute("src_left_context=" + src_left_cont));
	// src_right_context
	//cerr << "src right context" << endl;
	word_features.push_back(crf::Attribute("src_right_context=" + src_right_cont));
	// is_stopword
	//cerr << "stopword" << endl;
	if (m_stopwords.find(token) != m_stopwords.end()) {
		word_features.push_back(crf::Attribute("is_stopword=1"));
	} else {
		word_features.push_back(crf::Attribute("is_stopword=0"));
	}
	// is_punctuation
	//cerr << "punctuation" << endl;
	if (m_punctuation.find(token) != string::npos) {
		word_features.push_back(crf::Attribute("is_punctuation=1"));
	} else {
		word_features.push_back(crf::Attribute("is_punctuation=0"));
	}
	// is_proper_noun == starts with an uppercase letter
	//cerr << "proper noun" << endl;
	if (token[0] <= 'Z' && token[0] >= 'A') {
		word_features.push_back(crf::Attribute("is_proper_noun=1"));
	} else {
		word_features.push_back(crf::Attribute("is_proper_noun=0"));
	}
	// is_digit
	//cerr << "digit" << endl;
	try {
		float token_fl = atof(token.c_str());
		word_features.push_back(crf::Attribute("is_digit=1"));
	} catch (int e) {
		word_features.push_back(crf::Attribute("is_digit=0"));
	}
	// highest_order_ngram_left
	// backoff_behavior_left
	//crf::Item lm_features = find_lm_features(target, index);
	//word_features.insert(word_features.end(), lm_features.begin(), lm_features.end());
	// source_highest_order_ngram_left
	// source_highest_order_ngram_right
	//crf::Item source_lm_features = find_source_lm_features(source, index);
	//word_features.insert(word_features.end(), source_lm_features.begin(), source_lm_features.end());
	// target_pos
	//cerr << "target pos" << endl;
	string tg_pos = target_pos[index];
	word_features.push_back(crf::Attribute("target_pos=" + tg_pos));
	// aligned_source_pos
	//cerr << "source pos" << endl;
	word_features.push_back(crf::Attribute("aligned_source_pos=" + src_pos));
	// token+left
	//cerr << "token+left" << endl;
	word_features.push_back(crf::Attribute("token+left=" + token + "|" + tg_left_cont));
	// token+source
	//cerr << "token+source" << endl;
	word_features.push_back(crf::Attribute("token+source=" + token + "|" + src_token));
	// POS+sourcePOS
	//cerr << "pos+source pos" << endl;
	word_features.push_back(crf::Attribute("POS+sourcePOS=" + tg_pos + "|" + src_pos));
	// target+left+source
	//cerr << "token+left+source" << endl;
	word_features.push_back(crf::Attribute("target+left+source=" + token + "|" + tg_left_cont + "|" + src_token));

	return word_features;
}

FFState* WordQEFeature::EvaluateWhenApplied(
  const Hypothesis& cur_hypo,
  const FFState* prev_state,
  ScoreComponentCollection* accumulator) const
{
  //cerr << "START EVALUATION" << endl;
  const WordQEState* tnState = static_cast<const WordQEState*>(prev_state);
  assert(tnState);

  // current hypothesis target phrase
  const TargetPhrase& targetPhrase = cur_hypo.GetCurrTargetPhrase();
  if (targetPhrase.GetSize() == 0) return new WordQEState(*tnState);

  // create representations that are common for the whole phrase
  vector<string> source;
  vector<string> target;
  vector<string> source_pos;
  vector<string> target_pos;
  vector<size_t> alignments;

  //cerr << "Getting information about previous phrase" << endl;
  crf::ItemSequence prev_features = tnState->GetFeatures();
  vector<Word> prev_words = tnState->GetWords();
  vector<size_t> prev_align = tnState->GetAlignments();

  // add data from the previous phrase
  //cerr << "Size of previous phrase: " << prev_words.size() << endl;
  for (size_t i = 0; i < prev_words.size(); i++) {
    //cerr << "Starting with word " << i << endl;
    target.push_back(prev_words[i].GetString(0).as_string().c_str());
    //cerr << "Word: '" << prev_words[i].GetString(0).as_string() << "' added to target" << endl;
    if (m_use_pos == 1) {
      target_pos.push_back(prev_words[i].GetString(1).as_string().c_str());
      //cerr << "Word's POS: '" << prev_words[i].GetString(1).as_string() << "' added to target POS list" << endl;
    }
    alignments.push_back(prev_align[i]);
    //cerr << "Alignments " << prev_align[i] << " added" << endl;;
    //cerr << "Word " << i << " added" << endl;
  }

  const InputType& all_source = cur_hypo.GetInput();
  size_t source_start = cur_hypo.GetCurrSourceWordsRange().GetStartPos();
  set<pair<size_t,size_t> > all_alignments = targetPhrase.GetAlignTerm().GetAlignments();
  vector<size_t> align_to_save;
  align_to_save.reserve(targetPhrase.GetSize());
  //cerr << "Information about current phrase" << endl;
  for (size_t i = 0; i < targetPhrase.GetSize(); ++i){
    //cerr << "Cur phrase: starting with word " << i << endl;
    target.push_back(targetPhrase.GetWord(i).GetString(0).as_string());
    //cerr << "Cur phrase: Word: '" << targetPhrase.GetWord(i).GetString(0).as_string() << "' added to target" << endl;
    // target POS-tags
    if (m_use_pos == 1) {
	  //cerr << "USE POS!!!" << endl;
	  //cerr << "Target word: " << targetPhrase.GetWord(i) << endl;
	  //cerr << "Number of factors: " << targetPhrase.GetWord(i)[0]->GetString() << endl;
	  //cerr << "Number of factors: " << targetPhrase.GetWord(i)[1]->GetString() << endl;
	  //cerr << "Target word POS representation: " << targetPhrase.GetWord(i).GetString(1) << endl;
      target_pos.push_back(targetPhrase.GetWord(i).GetString(1).as_string());
      //cerr << "Cur phrase: Word's POS: '" << targetPhrase.GetWord(i).GetString(1).as_string() << "' added to target POS list" << endl;
    }
    // alignments
	int cur_align = -1;
    for (set<pair<size_t,size_t> >::iterator it = all_alignments.begin(); it != all_alignments.end(); it++){
      if (it->second == i) {
        cur_align = it->first + source_start;
		break;
      }
    }
    alignments.push_back(cur_align);
    //cerr << "Cur phrase: Alignments " << cur_align << " added" << endl;;
    align_to_save.push_back(cur_align);
  }

  // source words and POS-tags
  //  TODO: this can be done once and stored in the WordQEState
  for (size_t i = 0; i < all_source.GetSize(); i++){
    source.push_back(all_source.GetWord(i).GetString(0).as_string());
    if (m_use_pos == 1) {
      source_pos.push_back(all_source.GetWord(i).GetString(1).as_string());
    }
  }

  //cerr << "Source features loaded" << endl;
  //------------------WORD-LEVEL------------------
  // extract features for the current phrase
  crf::ItemSequence phrase_features;  // all word-level features for the current phrase -- already tagged
  // add features from the previous phrase

  phrase_features.reserve(targetPhrase.GetSize());
  for (size_t i = 0; i < targetPhrase.GetSize(); ++i) {
	phrase_features.push_back(m_extractor->extract_features(source, target, source_pos, target_pos, alignments, i + prev_words.size()));
  }
  //cerr << "Phrase features extracted" << endl;

  // form the sequence for the current word
  crf::ItemSequence sequence;
  // insert features for the previous phrase
  sequence.insert(sequence.end(), prev_features.begin(), prev_features.end());
  // insert features for the current phrase
  sequence.insert(sequence.end(), phrase_features.begin(), phrase_features.end());
  //cerr << "All features joined" << endl;

  // tag the whole sequence (prev.phrase + cur.phrase)
  crf::StringList prediction = m_tagger->tag(sequence);
  //cerr << "Predition made" << endl;
  string good_label = "OK";
  //string bad_label = "BAD";
  for (size_t i = prev_features.size(); i < sequence.size(); i++){
    //cerr << "Word " << i - prev_features.size() << " prediction" << endl;
	double score;
	if (m_probability)
	  score = m_tagger->marginal(good_label, i);
    else {
	  if (prediction[i] == "OK")
		score = 1.0;
	  else if (prediction[i] == "BAD")
		score = 0.0;
	  else
		cerr << "Unknown prediction: " << prediction[i] << endl;
	}
	//cerr << "Individual prediction made" << endl;
    accumulator->PlusEquals(this, score);
	//cerr << "Individual prediction added to accumulator" << endl;
  }
  
  //--------------END WORD-LEVEL----------------- 
  vector<Word> new_words;
  for (size_t i = 0; i < targetPhrase.GetSize(); ++i){
    new_words.push_back(targetPhrase.GetWord(i));
  }
  //cerr << "New state created" << endl;
  return new WordQEState(new_words, phrase_features, align_to_save);
}

FFState* WordQEFeature::EvaluateWhenApplied(
  const ChartHypothesis& /* cur_hypo */,
  int /* featureID - used to index the state in the previous hypotheses */,
  ScoreComponentCollection* accumulator) const
{
  vector<Word> words;
  words.clear();
  crf::ItemSequence features;
  features.clear();
  vector<size_t> alignments;
  alignments.clear();
 
  return new WordQEState(words, features, alignments);
}

void WordQEFeature::SetParameter(const std::string& key, const std::string& value)
{
//  if (key == "config") {
//    m_config = Scan<string>(value);
//  }
  if (key == "order") {
    m_order = Scan<size_t>(value);
  }
  //else if (key == "ngrams_target") {
  //  m_ngram_file_target = Scan<string>(value);
  //}
  //else if (key == "ngrams_source") {
  //  m_ngram_file_source = Scan<string>(value);
  //}
  else if (key == "stopwords") {
    m_stopword_file = Scan<string>(value);
  }
  else if (key == "use_pos") {
    m_use_pos = Scan<size_t>(value);
    //cerr << "POS FEATURE = " << m_use_pos << endl;
  }
  else if (key == "probability") {
    m_probability = Scan<size_t>(value);
    //cerr << "POS FEATURE = " << m_use_pos << endl;
  }
  else if (key == "model") {
    m_model_file = Scan<string>(value);
  }
  else if (key == "num_features") {
	m_numScoreComponents = Scan<size_t>(value);
  }
  else {
    StatefulFeatureFunction::SetParameter(key, value);
  }
}

void WordQEFeature::Load()
{
  if (m_model_file == "") return; //allow all, for now

  if (m_model_file == "*") return; //allow all

//  if (m_config == "") return;
//
//  if (m_config == "*") return;


  //-----------Extractor-------------
  m_extractor = new Extractor(m_stopword_file, m_order);
  //------------load model-----------
  m_tagger->open(m_model_file);
  //cerr << "Loading completed" << endl;
  return;
}

}

